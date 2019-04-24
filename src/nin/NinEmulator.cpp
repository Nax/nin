#include <cstdio>
#include "NinEmulator.h"

NinEmulator::NinEmulator(const char* path)
{
    FILE* f;

    f = fopen(path, "rb");
    _state = ninCreateState(f);
    fclose(f);

    _timer = new QTimer(this);

    _window = new NinMainWindow(*this);
    _window->show();

    _input = 0;
}

void NinEmulator::start()
{
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

void NinEmulator::update()
{
    ninSetInput(_state, _input);
    ninRunFrame(_state);
    _window->update((const char*)ninGetScreenBuffer(_state));
}
