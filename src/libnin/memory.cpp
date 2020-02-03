/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
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
        state->cyc += 1;
        ninPpuRunCycles(state, 3);
        state->apu.tick(1);
        for (uint16_t i = 0; i < 256; ++i)
        {
            tmp = ninMemoryRead8(state, addr | i);
            state->cyc++;
            ninPpuRunCycles(state, 3);
            state->apu.tick(1);
            state->oam[state->ppu.oamAddr++] = tmp;
            state->cyc++;
            ninPpuRunCycles(state, 3);
            state->apu.tick(1);
        }
        break;
    case 0x16:
        if (value & 0x01)
            state->controllerLatch = 0;
        break;
    }
}

uint8_t ninMemoryReadNES(NinState* state, uint16_t addr)
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
            return state->apu.regRead(addr & 0xff);
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
    else if (addr < 0xa000)
        return state->prgRomBank[0][addr & 0x1fff];
    else if (addr < 0xc000)
        return state->prgRomBank[1][addr & 0x1fff];
    else if (addr < 0xe000)
        return state->prgRomBank[2][addr & 0x1fff];
    else
        return state->prgRomBank[3][addr & 0x1fff];
}

uint8_t ninMemoryReadFDS(NinState* state, uint16_t addr)
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
            return state->apu.regRead(addr & 0xff);
        }
    }
    else if (addr < 0x4100)
        return state->diskSystem.regRead(addr);
    else if (addr < 0x6000)
        return badIO(state, addr, 0);
    else if (addr < 0xe000)
        return state->prgRam[addr - 0x6000];
    else
        return state->prgRom[addr & 0x1fff];
}

uint8_t ninMemoryRead8(NinState* state, uint16_t addr)
{
    return state->readHandler(state, addr);
}

uint16_t ninMemoryRead16(NinState* state, uint16_t addr)
{
    uint8_t lo;
    uint8_t hi;

    lo = ninMemoryRead8(state, addr);
    hi = ninMemoryRead8(state, addr + 1);

    return lo | ((uint16_t)hi << 8);
}

void ninMemoryWriteNES(NinState* state, uint16_t addr, uint8_t value)
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
            state->apu.regWrite(addr & 0xff, value);
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
        state->prgWriteHandler(state, addr, value);
}

void ninMemoryWriteFDS(NinState* state, uint16_t addr, uint8_t value)
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
            state->apu.regWrite(addr & 0xff, value);
            break;
        }
    }
    else if (addr < 0x4100)
        state->diskSystem.regWrite(addr, value);
    else if (addr < 0x6000)
        badIO(state, addr, 1);
    else if (addr < 0xe000)
    {
        state->prgRam[addr - 0x6000] = value;
    }
    else
        state->prgWriteHandler(state, addr, value);
}

void ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value)
{
    state->writeHandler(state, addr, value);
}

void ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value)
{
    ninMemoryWrite8(state, addr, value & 0xff);
    ninMemoryWrite8(state, addr + 1, value >> 8);
}

static int memoryExtractOverlap(uint16_t start, size_t len, uint16_t regionStart, size_t regionLen, uint16_t* overlapOffset, size_t* overlapLen, uint16_t* overlapOffsetInDest)
{
    size_t offset;
    size_t regionEnd;
    size_t transferEnd;
    size_t end;

    /* Check for non overlapping memory regions */
    if (start >= regionStart + regionLen)
        return 0;
    if (start + len < regionStart)
        return 0;

    /* Now we know that the regions DO overlap, check the first region byte that overlap */
    if (regionStart > start)
        offset = regionStart;
    else
        offset = start;

    *overlapOffset = offset - regionStart;
    regionEnd = regionStart + regionLen;
    transferEnd = start + len;
    end = regionEnd > transferEnd ? transferEnd : regionEnd;
    *overlapLen = end - offset;
    *overlapOffsetInDest = offset - start;

    return 1;
}

void ninDumpMemoryNES(NinState* state, uint8_t* dst, uint16_t start, size_t len)
{
    uint16_t oOff;
    size_t oLen;
    uint16_t dOff;

    /* RAM */
    for (int i = 0; i < 4; ++i)
    {
        if (memoryExtractOverlap(start, len, i * 0x800, 0x800, &oOff, &oLen, &dOff))
        {
            memcpy(dst + dOff, state->ram + oOff, oLen);
        }
    }

    /* 0x2000-0x7fff, not implemented */
    if (memoryExtractOverlap(start, len, 0x2000, 0x6000, &oOff, &oLen, &dOff))
    {
        memset(dst + dOff, 0xff, oLen);
    }

    /* 0x8000-0xffff, cart via mapper */
    for (int i = 0; i < 4; ++i)
    {
        if (memoryExtractOverlap(start, len, 0x8000 + i * 0x2000, 0x2000, &oOff, &oLen, &dOff))
        {
            memcpy(dst + dOff, state->prgRomBank[i] + oOff, oLen);
        }
    }
}

void ninDumpMemoryFDS(NinState* state, uint8_t* dst, uint16_t start, size_t len)
{
    uint16_t oOff;
    size_t oLen;
    uint16_t dOff;

    /* RAM */
    for (int i = 0; i < 4; ++i)
    {
        if (memoryExtractOverlap(start, len, i * 0x800, 0x800, &oOff, &oLen, &dOff))
        {
            memcpy(dst + dOff, state->ram + oOff, oLen);
        }
    }

    /* 0x2000-0x5fff, not implemented */
    if (memoryExtractOverlap(start, len, 0x2000, 0x4000, &oOff, &oLen, &dOff))
    {
        memset(dst + dOff, 0xff, oLen);
    }

    /* 0x6000-0xdfff, PRG RAM */
    if (memoryExtractOverlap(start, len, 0x6000, 0x8000, &oOff, &oLen, &dOff))
    {
        memcpy(dst + dOff, state->prgRam + oOff, oLen);
    }

    /* 0xe000-0xffff, PRG ROM */
    if (memoryExtractOverlap(start, len, 0xe000, 0x2000, &oOff, &oLen, &dOff))
    {
        memcpy(dst + dOff, state->prgRom + oOff, oLen);
    }
}

NIN_API void ninDumpMemory(NinState* state, uint8_t* dst, uint16_t start, size_t len)
{
    switch (state->info.system())
    {
    case NIN_SYSTEM_NES:
        ninDumpMemoryNES(state, dst, start, len);
        break;
    case NIN_SYSTEM_FDS:
        ninDumpMemoryFDS(state, dst, start, len);
        break;
    }
}
