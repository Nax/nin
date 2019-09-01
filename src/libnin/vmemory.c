#include <libnin/libnin.h>

static uint8_t badIO(NinState* state, uint16_t addr, int write)
{
    printf("PPU bad IO at 0x%04x, PC: 0x%04x (%c)\n", addr, state->cpu.pc, write ? 'w' : 'r');
    fflush(stdout);
    //getchar();
    return 0;
}

uint8_t ninVMemoryRead8(NinState* state, uint16_t addr)
{
    addr = addr & 0x3fff;
    if (addr < 0x1000)
        return state->chrBank[0][addr];
    else if (addr < 0x2000)
        return state->chrBank[1][addr & 0xfff];
    else if (addr < 0x2400)
        return state->nametables[0][addr & 0x3ff];
    else if (addr < 0x2800)
        return state->nametables[1][addr & 0x3ff];
    else if (addr < 0x2c00)
        return state->nametables[2][addr & 0x3ff];
    else if (addr < 0x3000)
        return state->nametables[3][addr & 0x3ff];
    else if (addr < 0x3400)
        return state->nametables[0][addr & 0x3ff];
    else if (addr < 0x3800)
        return state->nametables[1][addr & 0x3ff];
    else if (addr < 0x3c00)
        return state->nametables[2][addr & 0x3ff];
    else if (addr < 0x3f00)
        return state->nametables[3][addr & 0x3ff];
    else
        return state->palettes[addr & 0x1f];
}

void ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value)
{
    addr = addr & 0x3fff;
    if (addr < 0x1000)
    {
        if (state->chrRam)
            state->chrBank[0][addr] = value;
        else
            badIO(state, addr, 1);
    }
    else if (addr < 0x2000)
    {
        if (state->chrRam)
            state->chrBank[1][addr & 0xfff] = value;
        else
            badIO(state, addr, 1);
    }
    else if (addr < 0x2400)
        state->nametables[0][addr & 0x3ff] = value;
    else if (addr < 0x2800)
        state->nametables[1][addr & 0x3ff] = value;
    else if (addr < 0x2c00)
        state->nametables[2][addr & 0x3ff] = value;
    else if (addr < 0x3000)
        state->nametables[3][addr & 0x3ff] = value;
    else if (addr < 0x3400)
        state->nametables[0][addr & 0x3ff] = value;
    else if (addr < 0x3800)
        state->nametables[1][addr & 0x3ff] = value;
    else if (addr < 0x3c00)
        state->nametables[2][addr & 0x3ff] = value;
    else if (addr < 0x3f00)
        state->nametables[3][addr & 0x3ff] = value;
    else
    {
        if ((addr & 0x03) == 0)
        {
            state->palettes[addr & 0x0f] = value;
            state->palettes[0x10 | (addr & 0x0f)] = value;
        }
        else
            state->palettes[addr & 0x1f] = value;
    }
}
