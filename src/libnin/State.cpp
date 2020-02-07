#include <libnin/libnin.h>

NinState::NinState()
: info{}
, audio(info)
, apu(info, irq, audio)
, diskSystem(info, irq)
, saveFile{}
, mapper{}
, battery{}
, mirroring{}
, controller{}
, controllerLatch{}
, backBuffer{}
, frontBuffer{}
, audioSamplesCount{}
, audioCycles{}
, ram{}
, vram{}
, palettes{}
, oam{}
, mapperRegs{}
, prgWriteHandler{}
, ppuMonitorHandler{}
, readHandler{}
, writeHandler{}
, prgRomBank{}
, chrBank{}
, trainerSize{}
, nametables{}
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
