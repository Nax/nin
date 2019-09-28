#include <stdio.h>
#include <QFileInfo>
#include <QApplication>
#include "EmulatorWorker.h"

EmulatorWorker::EmulatorWorker(QObject* parent)
: QObject(parent)
, _state(nullptr)
, _workerState(WorkerState::Idle)
, _input(0)
{
    _thread = std::thread(&EmulatorWorker::workerMain, this);
}

EmulatorWorker::~EmulatorWorker()
{
    std::unique_lock lock(_mutex);

    _workerState = WorkerState::Stopping;
    lock.unlock();
    _cv.notify_one();

    _thread.join();
}

bool EmulatorWorker::loadRom(const QString& path)
{
    QByteArray raw;
    QString saveFile;
    NinError err;
    NinInt32 frameCycles;
    NinInt32 frameDelay;
    bool success;

    std::unique_lock lock(_mutex);

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

        _frameCycles = frameCycles;
        _frameDelay = frameDelay;
        _cyc = 0;
        _accumulator = 0;

        QFileInfo info(path);
        saveFile = info.path() + "/" + info.completeBaseName() + ".sav";
        raw = saveFile.toUtf8();
        ninSetSaveFile(_state, raw.data());
        ninAudioSetCallback(_state, &audioCallback, this);
        _workerState = WorkerState::Starting;
        success = true;
    }

    lock.unlock();
    _cv.notify_one();

    return success;
}

void EmulatorWorker::closeRom()
{
    std::unique_lock lock(_mutex);

    closeRomRaw();
    _workerState = WorkerState::Idle;

    lock.unlock();
    _cv.notify_one();
}

void EmulatorWorker::pause()
{
    std::unique_lock lock(_mutex);

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
    std::unique_lock lock(_mutex);

    if (_workerState == WorkerState::Paused)
    {
        _workerState = WorkerState::Starting;
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

void EmulatorWorker::workerMain()
{
    TimePoint prev;
    TimePoint now;
    uint64_t dt;
    uint64_t delay;

    std::unique_lock lock(_mutex);

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

void EmulatorWorker::closeRomRaw()
{
    if (_state)
    {
        ninDestroyState(_state);
        _state = nullptr;
    }
}

void EmulatorWorker::audioCallback(void* emu, const float* samples)
{
    emit ((EmulatorWorker*)emu)->audio(samples);
}
