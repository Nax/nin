#include <libnin/libnin.h>

void ninPrgWriteHandlerMMC4(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;
    ninPrgWriteHandlerCommonMMC2_MMC4(state, addr, value);
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        ninBankSwitchPrgRom16k(state, 0, value & 0xf);
        break;
    }
}
