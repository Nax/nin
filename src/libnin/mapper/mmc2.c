#include <libnin/libnin.h>

static void applycMMC2(NinState* state)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;

    ninBankSwitchChrRom4k(state, 0, mmc2->latch0 ? mmc2->bankLatch0Hi : mmc2->bankLatch0Lo);
    ninBankSwitchChrRom4k(state, 1, mmc2->latch1 ? mmc2->bankLatch1Hi : mmc2->bankLatch1Lo);
}

void ninPrgWriteHandlerCommonMMC2_MMC4(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;
    switch (addr & 0xf000)
    {
    case 0xb000:
        mmc2->bankLatch0Lo = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xc000:
        mmc2->bankLatch0Hi = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xd000:
        mmc2->bankLatch1Lo = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xe000:
        mmc2->bankLatch1Hi = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xf000:
        if (value & 0x1)
            ninMirrorV(state);
        else
            ninMirrorH(state);
    }
}

void ninPrgWriteHandlerMMC2(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;
    ninPrgWriteHandlerCommonMMC2_MMC4(state, addr, value);
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        ninBankSwitchPrgRom8k(state, 0, value & 0xf);
        break;
    }
}

NIN_API void ninPpuMonitorHandlerMMC2(NinState* state, uint16_t addr)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;

    if (addr == 0x0fd8)
    {
        mmc2->latch0 = 0;
        applycMMC2(state);
    }
    else if (addr == 0x0fe8)
    {
        mmc2->latch0 = 1;
        applycMMC2(state);
    }
    else if (addr >= 0x1fd8 && addr <= 0x1fdf)
    {
        mmc2->latch1 = 0;
        applycMMC2(state);
    }
    else if (addr >= 0x1fe8 && addr <= 0x1fef)
    {
        mmc2->latch1 = 1;
        applycMMC2(state);
    }
}
