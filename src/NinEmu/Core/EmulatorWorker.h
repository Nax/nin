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

#ifndef EMULATOR_WORKER_H
#define EMULATOR_WORKER_H

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <QObject>
#include <QString>
#include <nin/nin.h>
#include <NinEmu/Core/EmulatorInfo.h>

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

    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::nanoseconds;
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    EmulatorInfo                _info;

    std::thread                 _thread;
    std::mutex                  _mutex;
    std::condition_variable     _cv;
    std::atomic<WorkerState>    _workerState;

    std::atomic_size_t          _frameCycles;
    std::atomic_size_t          _frameDelay;
    std::atomic_size_t          _cyc;
    std::atomic_uint_fast8_t    _input;
    std::atomic_uint_fast64_t   _accumulator;

    std::atomic_uint_fast32_t   _audioFrequency;

    std::mutex  _audioMutex;
    float       _audioBuffer[NIN_AUDIO_SAMPLE_SIZE];

    char    _frameBuffer[NIN_FRAME_SIZE];

    NinState*   _state;
};

#endif
