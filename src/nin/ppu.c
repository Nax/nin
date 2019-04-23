#include <nin/nin.h>

uint8_t ninPpuRegRead(NinState* state, uint16_t reg)
{
    uint8_t value;
    uint8_t mask;

    value = 0;
    mask = 0;

    switch (reg & 0x07)
    {
    case 0x00:
        break;
    case 0x01:
        break;
    case 0x02: // PPUSTATUS
        mask = 0xff;
        if (state->ppu.nmi & NMI_OCCURED)
            value |= 0x80;
        ninUnsetFlagNMI(state, NMI_OCCURED);
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    case 0x05:
        break;
    case 0x06:
        break;
    case 0x07: // PPUDATA
        mask = 0xff;
        value = ninVMemoryRead8(state, state->ppu.addr);
        state->ppu.addr++;
        break;
    }

    value = (value & mask) | (state->ppu.latch & ~mask);
    state->ppu.latch = value;
    return value;
}

void ninPpuRegWrite(NinState* state, uint16_t reg, uint8_t value)
{
    state->ppu.latch = value;

    switch (reg & 0x07)
    {
    case 0x00: // PPUCTRL
        if (value & 0x80)
            ninSetFlagNMI(state, NMI_OUTPUT);
        else
            ninUnsetFlagNMI(state, NMI_OUTPUT);
        break;
    case 0x01:
        break;
    case 0x02:
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    case 0x05:
        break;
    case 0x06: // PPUADDR
        if (!state->ppu.addrHalfFlag)
            state->ppu.addr = (value << 8) | (state->ppu.addr & 0xff);
        else
            state->ppu.addr = value | (state->ppu.addr & 0xff00);
        state->ppu.addrHalfFlag ^= 1;
        break;
    case 0x07: //PPUDATA
        ninVMemoryWrite8(state, state->ppu.addr, value);
        state->ppu.addr++;
        break;
    }
}

void ninSetFlagNMI(NinState* state, uint8_t flag)
{
    uint8_t prev;

    prev = state->ppu.nmi;
    state->ppu.nmi |= flag;

    if ((prev != state->ppu.nmi) && (state->ppu.nmi == (NMI_OCCURED | NMI_OUTPUT)))
        state->nmi = 1;
}

void ninUnsetFlagNMI(NinState* state, uint8_t flag)
{
    state->ppu.nmi &= ~flag;
}
