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
}

void NinEmulator::start()
{
    connect(_timer, SIGNAL(timeout(void)), this, SLOT(update(void)));
    _timer->start(16);
}

void NinEmulator::update()
{
    ninRunFrame(_state);
    _window->update((const char*)ninGetScreenBuffer(_state));
}
