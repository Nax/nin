#include <libnin/libnin.h>

NinState::NinState()
: info{}
, audio(info)
, apu(info, irq, audio)
, diskSystem(info, irq)
{
}
