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
    state->ppuMonitorHandler(state, addr);
    if (addr < 0x2000)
        return state->chrBank[addr / 0x400][addr & 0x3ff];
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
    if (addr < 0x2000)
    {
        if (state->cart.segment(CART_CHR_RAM).base)
            state->chrBank[addr / 0x400][addr & 0x3ff] = value;
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
