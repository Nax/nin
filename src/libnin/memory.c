#include <libnin/libnin.h>

static uint8_t badIO(NinState* state, uint16_t addr, int write)
{
    printf("Bad IO at 0x%04x, PC: 0x%04x (%c)\n", addr, state->cpu.pc, write ? 'w' : 'r');
    fflush(stdout);
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
    uint16_t addr;
    uint8_t tmp;

    switch (port)
    {
    case 0x14:
        addr = value << 8;
        //printf("DMA!! 0x%04x\n", addr);
        //getchar();
        for (uint16_t i = 0; i < 256; ++i)
        {
            tmp = ninMemoryRead8(state, addr | i);
            state->oam[i] = tmp;
        }
        break;
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
    {
        switch (addr & 0xff)
        {
        case 0x14:
        case 0x16:
            return ioRead(state, addr & 0xff);
        default:
            return ninApuRegRead(state, addr & 0xff);
        }
    }
    else if (addr < 0x6000)
        return badIO(state, addr, 0);
    else if (addr < 0x8000)
    {
        if ((addr & 0x1fff) < state->prgRamSize)
            return state->prgRam[addr & 0x1fff];
        else
            return badIO(state, addr, 0);
    }
    else if (addr < 0xc000)
        return state->prgRomBank[0][addr & 0x3fff];
    else
        return state->prgRomBank[1][addr & 0x3fff];
}

uint16_t ninMemoryRead16(NinState* state, uint16_t addr)
{
    uint8_t lo;
    uint8_t hi;

    lo = ninMemoryRead8(state, addr);
    hi = ninMemoryRead8(state, addr + 1);

    return lo | (hi << 8);
}

int ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value)
{
    if (addr < 0x2000)
        state->ram[addr & 0x7ff] = value;
    else if (addr < 0x4000)
        ninPpuRegWrite(state, addr, value);
    else if (addr < 0x4018)
    {
        switch (addr & 0xff)
        {
        case 0x14:
        case 0x16:
            ioWrite(state, addr & 0xff, value);
            break;
        default:
            ninApuRegWrite(state, addr & 0xff, value);
            break;
        }
    }
    else if (addr < 0x6000)
        badIO(state, addr, 1);
    else if (addr < 0x8000)
    {
        if ((addr & 0x1fff) < state->prgRamSize)
            state->prgRam[addr & 0x1fff] = value;
        else
            badIO(state, addr, 1);
    }
    else
        return state->prgWriteHandler(state, addr, value);
    return 0;
}

int ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value)
{
    int tmp;

    tmp = ninMemoryWrite8(state, addr, value & 0xff);
    tmp |= ninMemoryWrite8(state, addr + 1, value >> 8);

    return !!tmp;
}
