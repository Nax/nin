#include <libnin/libnin.h>

static uint8_t badIO(NinState* state, uint16_t addr, int write)
{
    printf("PPU bad IO at 0x%04x, PC: 0x%04x (%c)\n", addr, state->cpu.pc, write ? 'w' : 'r');
    //getchar();
    return 0;
}

uint8_t ninVMemoryRead8(NinState* state, uint16_t addr)
{
    addr = addr & 0x3fff;
    if (addr < 0x2000)
        return state->chrRom[addr];
    if (addr >= 0x2000 && addr < 0x3f00)
        return state->vram[addr & 0x7ff];
    else
        return state->palettes[addr & 0x1f];
}

void ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value)
{
    addr = addr & 0x3fff;
    if (addr < 0x2000)
        badIO(state, addr, 1);
    else if (addr >= 0x2000 && addr < 0x3f00)
        state->vram[addr & 0x7ff] = value;
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
