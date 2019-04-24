#include <cstdio>
#include "NinEmulator.h"

NinEmulator::NinEmulator(const char* path)
{
    FILE* f;

    f = fopen(path, "rb");
    _state = ninCreateState(f);
    fclose(f);

    _window = new NinMainWindow(*this);
    _window->show();
}
