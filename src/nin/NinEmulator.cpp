#include <cstdio>
#include <chrono>
#include <thread>
#include "NinEmulator.h"

static const unsigned kBufferCount = 64;

static void _audioCallback(void* arg, const int16_t* samples)
{
    ((NinEmulator*)arg)->handleAudio(samples);
}

static void _workerMain(NinEmulator* emu)
{
    static const uint64_t kDelay = 16666666;

    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::nanoseconds;
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    TimePoint prev;
    TimePoint now;
    uint64_t acc;

    prev = Clock::now();
    acc = 0;
    for (;;)
    {
        now = Clock::now();
        acc += (now - prev).count();
        prev = now;

        while (acc >= kDelay)
        {
            QMetaObject::invokeMethod(emu, &NinEmulator::update);
            acc -= kDelay;
        }

        std::this_thread::sleep_for(std::chrono::nanoseconds(kDelay - acc));
    }
}

NinEmulator::NinEmulator(const char* path)
{
    FILE* f;

    f = fopen(path, "rb");
    _state = ninCreateState(f);
    fclose(f);

    _audio = new Audio;
    ninSetAudioCallback(_state, &_audioCallback, this);

    _window = new NinMainWindow(*this);
    _window->show();

    _input = 0;
}

void NinEmulator::start()
{
    // Run the first frame
    update();

    _worker = std::thread(_workerMain, this);
}

void NinEmulator::handleInput(uint8_t key, int pressed)
{
    if (pressed)
        _input |= key;
    else
        _input &= ~key;

    printf("Input: 0x%02x\n", _input);
}

void NinEmulator::handleAudio(const int16_t* samples)
{
    _audio->pushSamples(samples);
}

void NinEmulator::update()
{
    ninSetInput(_state, _input);
    ninRunFrame(_state);
    _window->update((const char*)ninGetScreenBuffer(_state));
}
