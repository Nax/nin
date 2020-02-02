#include <libnin/libnin.h>

NinState::NinState()
: hwSpecs(HardwareSpecs::NTSC)
, audio(hwSpecs)
, apu(hwSpecs, irq, audio)
{
}
