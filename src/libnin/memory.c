#include <libnin/libnin.h>

static uint16_t badIO(NinState* state, uint16_t addr, int write)
{
    printf("Bad IO at 0x%04x, PC: 0x%04x (%c)\n", addr, state->cpu.pc, write ? 'w' : 'r');
    //getchar();
    return 0;
}

static uint8_t ioRead(NinState* state, uint8_t port)
{
    uint8_t tmp;

    switch (port)
    {
    case 0x16:
        if (state->controllerLatch < 8)
        {
            tmp = (state->controller >> state->controllerLatch) & 0x01;
            state->controllerLatch++;
            return tmp;
        }
        else
            return 0x01;
    case 0x17:
        return 0x00;
    }
    return 0;
}

static void ioWrite(NinState* state, uint8_t port, uint8_t value)
{
    switch (port)
    {
    case 0x16:
        if (value & 0x01)
            state->controllerLatch = 0;
        break;
    }
}

uint8_t ninMemoryRead8(NinState* state, uint16_t addr)
{
    if (addr < 0x2000)
        return state->ram[addr & 0x7ff];
    else if (addr < 0x4000)
        return ninPpuRegRead(state, addr);
    else if (addr < 0x4018)
        return ioRead(state, addr & 0xff);
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
    else if (addr < 0x4000)
        ninPpuRegWrite(state, addr, value);
    else if (addr < 0x4018)
        ioWrite(state, addr & 0xff, value);
    else
        badIO(state, addr, 1);
}

void ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value)
{
    ninMemoryWrite8(state, addr, value & 0xff);
    ninMemoryWrite8(state, addr + 1, value >> 8);
}
