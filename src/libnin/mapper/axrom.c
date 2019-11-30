#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerAXROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    uint8_t bank;
    uint8_t nametable;
    uint16_t nametableOffset;

    bank = ((value & 0xf) * 2) % state->prgBankCount;
    nametable = (value >> 4) & 1;
    nametableOffset = (uint16_t)nametable * 0x400;

    ninBankSwitchPrgRom16k(state, 0, bank);
    ninBankSwitchPrgRom16k(state, 1, bank + 1);

    for (int i = 0; i < 4; ++i)
        state->nametables[i] = state->vram + nametableOffset;
}
