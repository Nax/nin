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

    void inputKeyPress(uint8_t key);
    void inputKeyRelease(uint8_t key);

    void setAudioFrequency(uint32_t freq);

    void syncAudio();

signals:
    void frame(const char* texture);
    void audio(const float* samples);
    void audioEvent(void);
    void update(NinState* state);
    void reset();

private:
    void workerMain();
    void workerUpdate();
    void closeRomRaw();

    static void audioCallback(void* emu, const float* samples);

    enum class WorkerState
    {
        Idle,
        Paused,
        Starting,
        Running,
        Stopping
    };

    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::nanoseconds;
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    std::thread                 _thread;
    std::mutex                  _mutex;
    std::condition_variable     _cv;
    std::atomic<WorkerState>    _workerState;

    std::atomic_size_t          _frameCycles;
    std::atomic_size_t          _frameDelay;
    std::atomic_size_t          _cyc;
    std::atomic_uint8_t         _input;
    std::atomic_uint64_t        _accumulator;

    std::atomic_uint32_t        _audioFrequency;

    std::mutex  _audioMutex;
    float       _audioBuffer[NIN_AUDIO_SAMPLE_SIZE];

    char    _frameBuffer[NIN_FRAME_SIZE];

    NinState*   _state;
};

#endif
