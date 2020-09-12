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

#ifndef EMULATOR_WORKER_H
#define EMULATOR_WORKER_H

#include <NinEmu/Core/EmulatorInfo.h>
#include <QObject>
#include <QString>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <nin/nin.h>
#include <thread>

class EmulatorWorker : public QObject
{
    Q_OBJECT;

public:
    explicit EmulatorWorker(QObject* parent = nullptr);
    virtual ~EmulatorWorker();

    bool loadRom(const QString& path);
    void closeRom();

    void pause();
    void resume();
    void stepFrame();
    void stepSingle();

    void insertDisk(int disk);

    void inputKeyPress(uint8_t key);
    void inputKeyRelease(uint8_t key);

    void setAudioFrequency(uint32_t freq);

    void syncAudio();

signals:
    void frame(const char* texture);
    void audio(const float* samples);
    void audioEvent(void);
    void update(NinState* state);
    void reset(const EmulatorInfo& info);

private:
    void workerMain();
    void workerUpdate();
    void workerStepFrame();
    void workerStepSingle();
    void closeRomRaw();

    QString getSaveLocation(const QString& prefix, const QString& name);

    static void audioCallback(void* emu, const float* samples);

    enum class WorkerState
    {
        Idle,
        Paused,
        Starting,
        Running,
        StepFrame,
        StepSingle,
        Stopping
    };

    using Clock     = std::chrono::high_resolution_clock;
    using Duration  = std::chrono::nanoseconds;
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    EmulatorInfo _info;

    std::thread              _thread;
    std::mutex               _mutex;
    std::condition_variable  _cv;
    std::atomic<WorkerState> _workerState;

    std::atomic_size_t        _frameCycles;
    std::atomic_size_t        _frameDelay;
    std::atomic_size_t        _cyc;
    std::atomic_uint_fast8_t  _input;
    std::atomic_uint_fast64_t _accumulator;

    std::atomic_uint_fast32_t _audioFrequency;

    std::mutex _audioMutex;
    float      _audioBuffer[NIN_AUDIO_SAMPLE_SIZE];

    char _frameBuffer[NIN_FRAME_SIZE];

    NinState* _state;
};

#endif
