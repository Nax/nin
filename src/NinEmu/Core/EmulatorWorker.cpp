/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <QtCore>
#include <NinEmu/Core/EmulatorWorker.h>

EmulatorWorker::EmulatorWorker(QObject* parent)
: QObject(parent)
, _state(nullptr)
, _workerState(WorkerState::Idle)
, _input(0)
, _audioFrequency(48000)
{
    connect(this, &EmulatorWorker::audioEvent, this, &EmulatorWorker::syncAudio, Qt::QueuedConnection);
    _thread = std::thread(&EmulatorWorker::workerMain, this);
}

EmulatorWorker::~EmulatorWorker()
{
    std::unique_lock<std::mutex> lock(_mutex);

    _workerState = WorkerState::Stopping;
    lock.unlock();
    _cv.notify_one();

    _thread.join();
}

bool EmulatorWorker::loadRom(const QString& path)
{
    QByteArray raw;
    QString saveFile;
    QString biosPath;
    NinError err;
    NinInt32 frameCycles;
    NinInt32 frameDelay;
    NinInt32 system;
    bool success;

    std::unique_lock<std::mutex> lock(_mutex);

    closeRomRaw();
    raw = path.toUtf8();
    err = ninCreateState(&_state, raw.data());
    if (err != NIN_OK)
    {
        printf("Error loading rom (%d)\n", (int)err);
        fflush(stdout);
        _workerState = WorkerState::Idle;
        success = false;
    }
    else
    {
        ninInfoQueryInteger(_state, &frameCycles, NIN_INFO_FRAME_CYCLES);
        ninInfoQueryInteger(_state, &frameDelay, NIN_INFO_FRAME_DELAY);
        ninInfoQueryInteger(_state, &system, NIN_INFO_SYSTEM);

        if (system == NIN_SYSTEM_FDS) {
            biosPath = QCoreApplication::applicationDirPath() + "/bios/disksys.rom";
            raw = biosPath.toUtf8();
            ninLoadBiosFDS(_state, raw.data());
        }

        _frameCycles = frameCycles;
        _frameDelay = frameDelay;
        _cyc = 0;
        _accumulator = 0;

        raw = getSaveLocation("nes", path).toUtf8();
        ninSetSaveFile(_state, raw.data());
        ninAudioSetCallback(_state, &audioCallback, this);
        ninAudioSetFrequency(_state, _audioFrequency);
        _workerState = WorkerState::Starting;
        success = true;
        emit reset();
    }

    lock.unlock();
    _cv.notify_one();

    return success;
}

void EmulatorWorker::closeRom()
{
    std::unique_lock<std::mutex> lock(_mutex);

    closeRomRaw();
    _workerState = WorkerState::Idle;

    lock.unlock();
    _cv.notify_one();
}

void EmulatorWorker::pause()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (_workerState == WorkerState::Running)
    {
        _workerState = WorkerState::Paused;
        emit update(_state);
        lock.unlock();
        _cv.notify_one();
    }
}

void EmulatorWorker::resume()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (_workerState == WorkerState::Paused)
    {
        _workerState = WorkerState::Starting;
        lock.unlock();
        _cv.notify_one();
    }
}

void EmulatorWorker::stepFrame()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (_workerState == WorkerState::Paused || _workerState == WorkerState::Running)
    {
        _workerState = WorkerState::StepFrame;
        lock.unlock();
        _cv.notify_one();
    }
}

void EmulatorWorker::stepSingle()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (_workerState == WorkerState::Paused || _workerState == WorkerState::Running)
    {
        _workerState = WorkerState::StepSingle;
        lock.unlock();
        _cv.notify_one();
    }
}

void EmulatorWorker::inputKeyPress(uint8_t key)
{
    _input |= key;
}

void EmulatorWorker::inputKeyRelease(uint8_t key)
{
    _input &= ~key;
}

void EmulatorWorker::setAudioFrequency(uint32_t freq)
{
    _audioFrequency = freq;
}

void EmulatorWorker::syncAudio()
{
    std::unique_lock<std::mutex> lock(_audioMutex);
    emit audio(_audioBuffer);
}

void EmulatorWorker::workerMain()
{
    TimePoint prev;
    TimePoint now;
    uint64_t dt;
    uint64_t delay;

    std::unique_lock<std::mutex> lock(_mutex);

    prev = Clock::now();
    _accumulator = 0;

    for (;;)
    {
        now = Clock::now();
        dt = (now - prev).count();
        prev = now;

        switch (_workerState)
        {
        case WorkerState::Idle:
        case WorkerState::Paused:
            _cv.wait(lock);
            break;
        case WorkerState::Starting:
            workerUpdate();
            _accumulator = 0;
            now = Clock::now();
            prev = now;
            _workerState = WorkerState::Running;
            break;
        case WorkerState::Running:
            _accumulator += dt;
            delay = _frameDelay / 4;
            while (_accumulator >= delay)
            {
                _accumulator -= delay;
                workerUpdate();
            }
            _cv.wait_for(lock, std::chrono::nanoseconds(delay - _accumulator));
            break;
        case WorkerState::StepFrame:
            workerStepFrame();
            _workerState = WorkerState::Paused;
        case WorkerState::StepSingle:
            workerStepSingle();
            _workerState = WorkerState::Paused;
            break;
        case WorkerState::Stopping:
            return;
        }
    }
}

void EmulatorWorker::workerUpdate()
{
    size_t cyc;

    cyc = _cyc;
    ninSetInput(_state, _input);
    if (ninRunCycles(_state, _frameCycles / 4 - cyc, &cyc))
    {
        emit frame((const char*)ninGetScreenBuffer(_state));
        emit update(_state);
    }
    _cyc = cyc;
}

void EmulatorWorker::workerStepFrame()
{
    size_t cyc;

    ninSetInput(_state, _input);
    for (;;)
    {
        if (ninRunCycles(_state, 1, &cyc))
        {
            emit frame((const char*)ninGetScreenBuffer(_state));
            emit update(_state);
            return;
        }
    }
}

void EmulatorWorker::workerStepSingle()
{
    size_t cyc;

    ninSetInput(_state, _input);
    if (ninStepInstruction(_state))
    {
        emit frame((const char*)ninGetScreenBuffer(_state));
    }
    emit update(_state);
}

void EmulatorWorker::closeRomRaw()
{
    if (_state)
    {
        ninDestroyState(_state);
        _state = nullptr;
    }
}

QString EmulatorWorker::getSaveLocation(const QString& prefix, const QString& name)
{
    QFileInfo info(name);
    QString ninDir;
    QString saveDir;
    QString saveName;
    QString savePath;

    ninDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    saveName = info.completeBaseName() + ".sav";
    saveDir = ninDir + "/saves/" + prefix;
    savePath = saveDir + "/" + saveName;

    QDir dir(saveDir);
    if (!dir.exists())
        dir.mkpath(".");

    return savePath;
}

void EmulatorWorker::audioCallback(void* arg, const float* samples)
{
    EmulatorWorker* emu;

    emu = (EmulatorWorker*)arg;
    {
        std::unique_lock<std::mutex> lock(emu->_audioMutex);
        memcpy(emu->_audioBuffer, samples, sizeof(emu->_audioBuffer));
    }
    emit emu->audioEvent();
}
