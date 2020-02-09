#include <libnin/libnin.h>

NinState::NinState()
: memory{}
, info{}
, mapper{memory, cart}
, audio(info)
, apu(info, irq, audio)
, diskSystem(info, irq)
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
, chrBank{}
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
