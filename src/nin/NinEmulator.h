#ifndef NIN_EMULATOR_H
#define NIN_EMULATOR_H

#include <nin/nin.h>
#include "NinMainWindow.h"

class NinEmulator
{
public:
    NinEmulator(const char* path);

private:
    NinState*       _state;
    NinMainWindow*  _window;
};

#endif
