#include <cstdio>
#include <chrono>
#include <thread>
#include "Emulator.h"

static void _audioCallback(void* arg, const int16_t* samples)
{
    ((Emulator*)arg)->handleAudio(samples);
}

static void _workerMain(Emulator* emu)
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
            QMetaObject::invokeMethod(emu, &Emulator::update);
            acc -= kDelay;
        }

        std::this_thread::sleep_for(std::chrono::nanoseconds(kDelay - acc));
    }
}

Emulator::Emulator()
: _state(nullptr)
, _running(false)
{
    _audio = new Audio;
    _window = new MainWindow(*this);
    _window->show();

    _input = 0;
}

void Emulator::loadRom(const char* path)
{
    FILE* f;

    if (_state)
    {
        ninDestroyState(_state);
        _state = nullptr;
    }

    f = fopen(path, "rb");
    if (f)
    {
        _state = ninCreateState(f);
        fclose(f);
        ninSetAudioCallback(_state, &_audioCallback, this);
    }
}

void Emulator::start()
{
    // Run the first frame
    _running = true;
    update();
    _worker = std::thread(_workerMain, this);
}

void Emulator::handleInput(uint8_t key, int pressed)
{
    if (pressed)
        _input |= key;
    else
        _input &= ~key;
}

void Emulator::handleAudio(const int16_t* samples)
{
    _audio->pushSamples(samples);
}

void Emulator::update()
{
    if (!_state || !_running)
        return;

    ninSetInput(_state, _input);
    ninRunFrame(_state);
    _window->updateTexture((const char*)ninGetScreenBuffer(_state));
}

void Emulator::pause()
{
    _running = false;
}

void Emulator::resume()
{
    _running = true;
}
