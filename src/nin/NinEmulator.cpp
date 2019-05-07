#include <cstdio>
#include "NinEmulator.h"

static const unsigned kBufferCount = 64;

static void _audioCallback(void* arg, const int16_t* samples)
{
    ((NinEmulator*)arg)->handleAudio(samples);
}

NinEmulator::NinEmulator(const char* path)
{
    FILE* f;

    f = fopen(path, "rb");
    _state = ninCreateState(f);
    fclose(f);

    _audio = new Audio;
    ninSetAudioCallback(_state, &_audioCallback, this);

    _timer = new QTimer(this);

    _window = new NinMainWindow(*this);
    _window->show();

    _input = 0;
}

void NinEmulator::start()
{
    // Run the first frame
    update();

    connect(_timer, SIGNAL(timeout(void)), this, SLOT(update(void)));
    _timer->start(16);
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
