#include <libnin/libnin.h>

NinState::NinState()
: memory{}
, info{}
, cart{}
, irq{}
, mapper{memory, cart, irq}
, busVideo{memory, cart, mapper}
, audio{info}
, apu{info, irq, mapper, audio}
, diskSystem{info, irq}
, cpu{}
, ppu{}
, saveFile{}
, battery{}
, mirroring{}
, controller{}
, controllerLatch{}
, backBuffer{}
, frontBuffer{}
, oam{}
, readHandler{}
, nmi{}
, nmi2{}
, cyc{}
, frame{}
, frameOdd{}
{
}
