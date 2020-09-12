/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 *
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 *
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 *
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <NinEmu/Core/EmulatorWorker.h>
#include <QtCore>
#include <stdio.h>

EmulatorWorker::EmulatorWorker(QObject* parent)
: QObject(parent)
, _info{}
, _workerState(WorkerState::Idle)
, _input(0)
, _audioFrequency(48000)
, _state(nullptr)
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
    QString    saveFile;
    QString    biosPath;
    NinError   err;
    NinInt32   frameCycles;
    NinInt32   frameDelay;
    NinInt32   system;
    NinInt32   diskSideCount;
    bool       success;

    std::unique_lock<std::mutex> lock(_mutex);

    closeRomRaw();
    raw = path.toUtf8();
    err = ninCreateState(&_state, raw.data());
    if (err != NIN_OK)
    {
        printf("Error loading rom (%d)\n", (int)err);
        fflush(stdout);
        _workerState = WorkerState::Idle;
        success      = false;
    }
    else
    {
        ninInfoQueryInteger(_state, &frameCycles, NIN_INFO_FRAME_CYCLES);
        ninInfoQueryInteger(_state, &frameDelay, NIN_INFO_FRAME_DELAY);
        ninInfoQueryInteger(_state, &system, NIN_INFO_SYSTEM);
        ninInfoQueryInteger(_state, &diskSideCount, NIN_INFO_DISK_SIDE_COUNT);

        if (system == NIN_SYSTEM_FDS)
        {
            biosPath = QCoreApplication::applicationDirPath() + "/bios/disksys.rom";
            raw      = biosPath.toUtf8();
            ninLoadBiosFDS(_state, raw.data());
        }

        _frameCycles        = frameCycles;
        _frameDelay         = frameDelay;
        _info.diskSideCount = diskSideCount;
        _cyc                = 0;
        _accumulator        = 0;

        raw = getSaveLocation("nes", path).toUtf8();
        ninSetSaveFile(_state, raw.data());
        ninAudioSetCallback(_state, &audioCallback, this);
        ninAudioSetFrequency(_state, _audioFrequency);
        _workerState = WorkerState::Starting;
        success      = true;
        emit reset(_info);
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

void EmulatorWorker::insertDisk(int diskSide)
{
    std::unique_lock<std::mutex> lock(_mutex);

    ninInsertDisk(_state, diskSide);
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
    emit                         audio(_audioBuffer);
}

void EmulatorWorker::workerMain()
{
    TimePoint prev;
    TimePoint now;
    uint64_t  dt;
    uint64_t  delay;

    std::unique_lock<std::mutex> lock(_mutex);

    prev         = Clock::now();
    _accumulator = 0;

    for (;;)
    {
        now  = Clock::now();
        dt   = (now - prev).count();
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
            now          = Clock::now();
            prev         = now;
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
            break;
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
    QString   ninDir;
    QString   saveDir;
    QString   saveName;
    QString   savePath;

    ninDir   = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    saveName = info.completeBaseName() + ".sav";
    saveDir  = ninDir + "/saves/" + prefix;
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
