#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerAXROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    uint8_t nametable;
    uint16_t nametableOffset;

    nametable = (value >> 4) & 1;
    nametableOffset = (uint16_t)nametable * 0x400;

    ninBankSwitchPrgRom32k(state, value & 0xf);

    for (int i = 0; i < 4; ++i)
        state->nametables[i] = state->vram + nametableOffset;
}
