#include <libnin/libnin.h>

NinState::NinState()
: memory{}
, info{}
, cart{}
, irq{}
, mapper{memory, cart}
, audio(info)
, apu(info, irq, audio)
, diskSystem(info, irq)
, cpu{}
, ppu{}
, saveFile{}
, battery{}
, mirroring{}
, controller{}
, controllerLatch{}
, backBuffer{}
, frontBuffer{}
, audioSamplesCount{}
, audioCycles{}
, palettes{}
, oam{}
, mapperRegs{}
, ppuMonitorHandler{}
, readHandler{}
, nmi{}
, nmi2{}
, cyc{}
, frame{}
, frameOdd{}
, irqScanlineEnabled{}
, irqScanlineReload{}
, irqScanlineCounter{}
, irqScanlineReloadValue{}
, irqScanlineFilterShifter{}
, oldVmemAddr{}
{
}
