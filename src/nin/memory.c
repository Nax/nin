#include <nin/nin.h>

static uint16_t badIO(NinState* state, uint16_t addr, int write)
{
    printf("Bad IO at 0x%04x, PC: 0x%04x (%c)\n", addr, state->cpu.pc, write ? 'w' : 'r');
    getchar();
    return 0;
}

uint8_t ninMemoryRead8(NinState* state, uint16_t addr)
{
    if (addr < 0x2000)
        return state->ram[addr & 0x7ff];
    else if (addr >= 0x8000)
        return state->prgRom[addr & 0x7fff];
    return badIO(state, addr, 0);
}

uint16_t ninMemoryRead16(NinState* state, uint16_t addr)
{
    uint8_t lo;
    uint8_t hi;

    lo = ninMemoryRead8(state, addr);
    hi = ninMemoryRead8(state, addr + 1);

    return lo | (hi << 8);
}

void ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value)
{
    if (addr < 0x2000)
        state->ram[addr & 0x7ff] = value;
    else
        badIO(state, addr, 1);
}

void ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value)
{
    ninMemoryWrite8(state, addr, value & 0xff);
    ninMemoryWrite8(state, addr + 1, value >> 8);
}
