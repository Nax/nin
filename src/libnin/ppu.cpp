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

#include <cstring>
#include <math.h>
#include <libnin/libnin.h>

#define RT state->ppu.rt

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
        mask = 0xe0;
        if (RT.scanline == 241 && RT.cycle == 0) // Pre-set race
        {
            RT.inhibitNmi = 1;
        }
        else if (RT.scanline == 241 && RT.cycle == 1) // Same-set
        {
            RT.inhibitNmi = 1;
            value |= 0x80;
        }
        else if (RT.scanline == 241 && RT.cycle == 2) // Post-set
        {
            ninUnsetFlagNMI(state, NMI_OCCURED);
            state->nmi = 0;
            value |= 0x80;
        }
        else if ((state->ppu.nmi & NMI_OCCURED)) // Normal operation
        {
            value |= 0x80;
        }
        if ((state->ppu.nmi & NMI_OCCURED))
            value |= 0x80;
        ninUnsetFlagNMI(state, NMI_OCCURED);
        if (state->ppu.zeroHitFlag)
            value |= 0x40;
        state->ppu.w = 0;
        break;
    case 0x03:
        break;
    case 0x04: // OAMDATA
        mask = 0xff;
        value = state->oam[state->ppu.oamAddr];
        break;
    case 0x05:
        break;
    case 0x06:
        break;
    case 0x07: // PPUDATA
        mask = 0xff;
        if ((state->ppu.rt.v & 0x3f00) == 0x3f00)
        {
            value = ninVMemoryRead8(state, state->ppu.rt.v);
            state->ppu.readBuf = ninVMemoryRead8(state, state->ppu.rt.v & 0x2fff);
        }
        else
        {
            value = state->ppu.readBuf;
            state->ppu.readBuf = ninVMemoryRead8(state, state->ppu.rt.v);
        }
        state->ppu.rt.v += (state->ppu.controller & 0x04) ? 32 : 1;
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
        state->ppu.controller = value;
        // Set base nametable
        state->ppu.rt.t &= ~0x0c00;
        state->ppu.rt.t |= ((value & 0x03) << 10);
        state->ppu.rt.largeSprites = ((value >> 5) & 1);
        if (value & 0x80)
            ninSetFlagNMI(state, NMI_OUTPUT);
        else
            ninUnsetFlagNMI(state, NMI_OUTPUT);
        break;
    case 0x01:
        state->ppu.rt.maskEnableBackground = ((value & 0x08) >> 3);
        state->ppu.rt.maskEnableSprites = ((value & 0x10) >> 4);
        state->ppu.rt.maskEnableBackgroundLeft = !!(value & 0x02);
        state->ppu.rt.maskEnableSpritesLeft = !!(value & 0x04);
        break;
    case 0x02:
        break;
    case 0x03: // OAMADDR
        state->ppu.oamAddr = value;
        break;
    case 0x04: // OAMDATA
        state->oam[state->ppu.oamAddr++] = value;
        break;
    case 0x05: // PPUSCROLL
        if (!state->ppu.w)
        {
            // Fine X
            state->ppu.rt.x = value & 0x07;

            // Coarse X
            state->ppu.rt.t &= ~(0x001f);
            state->ppu.rt.t |= ((value >> 3) & 0x1f);
        }
        else
        {
            // Fine Y
            state->ppu.rt.t &= ~(0x7000);
            state->ppu.rt.t |= (((uint16_t)value & 0x07) << 12);

            // Coarse Y
            state->ppu.rt.t &= ~(0x03e0);
            state->ppu.rt.t |= (((uint16_t)value >> 3) & 0x1f) << 5;
        }
        state->ppu.w ^= 1;
        break;
    case 0x06: // PPUADDR
        if (!state->ppu.w)
        {
            state->ppu.rt.t &= 0x00ff;
            state->ppu.rt.t |= (((uint16_t)value << 8) & 0x3fff);
        }
        else
        {
            state->ppu.rt.t &= 0xff00;
            state->ppu.rt.t |= (value & 0x00ff);
            state->ppu.rt.v = state->ppu.rt.t;
            state->ppuMonitorHandler(state, state->ppu.rt.v);
        }
        state->ppu.w ^= 1;
        break;
    case 0x07: //PPUDATA
        ninVMemoryWrite8(state, state->ppu.rt.v, value);
        state->ppu.rt.v += ((state->ppu.controller & 0x04) ? 32 : 1);
        break;
    }
}

void ninSetFlagNMI(NinState* state, uint8_t flag)
{
    uint8_t prev;

    prev = state->ppu.nmi;
    state->ppu.nmi |= flag;

    if (prev != state->ppu.nmi)
    {
        if (state->ppu.nmi == (NMI_OCCURED | NMI_OUTPUT))
            state->nmi = 1;
    }
}

void ninUnsetFlagNMI(NinState* state, uint8_t flag)
{
    state->ppu.nmi &= ~flag;
}

static uint16_t _incX(uint16_t v)
{
    if ((v & 0x001f) == 31)
    {
        v &= ~0x001f;
        v ^= 0x0400;
    }
    else
        v++;
    return v;
}

static uint16_t _incY(uint16_t v)
{
    uint16_t y;

    if ((v & 0x7000) != 0x7000)
        v += 0x1000;
    else
    {
        v &= ~0x7000;
        y = (v & 0x03E0) >> 5;
        if (y == 29)
        {
            y = 0;
            v ^= 0x0800;
        }
        else if (y == 31)
            y = 0;
        else
            y++;
        v = (v & ~0x03E0) | (y << 5);
    }

    return v;
}

static uint8_t bitswap8(uint8_t b)
{
    uint8_t v;

    v = 0;
    v |= (b & 0x80) >> 7;
    v |= (b & 0x40) >> 5;
    v |= (b & 0x20) >> 3;
    v |= (b & 0x10) >> 1;
    v |= (b & 0x08) << 1;
    v |= (b & 0x04) << 3;
    v |= (b & 0x02) << 5;
    v |= (b & 0x01) << 7;

    return v;
}

/* A R G B */
static const uint32_t kPalette[] = {
    0xff7c7c7c, 0xfffc0000, 0xffbc0000, 0xffbc2844,
    0xff840094, 0xff2000a8, 0xff0010a8, 0xff001488,
    0xff003050, 0xff007800, 0xff006800, 0xff005800,
    0xff584000, 0xff000000, 0xff000000, 0xff000000,
    0xffbcbcbc, 0xfff87800, 0xfff85800, 0xfffc4468,
    0xffcc00d8, 0xff5800e4, 0xff0038f8, 0xff105ce4,
    0xff007cac, 0xff00b800, 0xff00a800, 0xff44a800,
    0xff888800, 0xff000000, 0xff000000, 0xff000000,
    0xfff8f8f8, 0xfffcbc3c, 0xfffc8868, 0xfff87898,
    0xfff878f8, 0xff9858f8, 0xff5878f8, 0xff44a0fc,
    0xff00b8f8, 0xff18f8b8, 0xff54d858, 0xff98f858,
    0xffd8e800, 0xff787878, 0xff000000, 0xff000000,
    0xfffcfcfc, 0xfffce4a4, 0xfff8b8b8, 0xfff8b8d8,
    0xfff8b8f8, 0xffc0a4f8, 0xffb0d0f0, 0xffa8e0fc,
    0xff78d8f8, 0xff78f8d8, 0xffb8f8b8, 0xffd8f8b8,
    0xfffcfc00, 0xfff8d8f8, 0xff000000, 0xff000000,
};

static const uint16_t kHMask = 0x041f;

static void swapBuffers(NinState* state)
{
    uint32_t* tmp;

    tmp = state->backBuffer;
    state->backBuffer = state->frontBuffer;
    state->frontBuffer = tmp;

    memset(state->backBuffer, 0, BITMAP_X * BITMAP_Y * 4);
}

static void bgReload(NinState* state)
{
    RT.shiftPatternLo <<= 8;
    RT.shiftPatternHi <<= 8;
    RT.shiftPaletteLo <<= 8;
    RT.shiftPaletteHi <<= 8;
    RT.shiftPatternLo |= RT.latchTileLo;
    RT.shiftPatternHi |= RT.latchTileHi;
    RT.shiftPaletteLo |= (RT.latchAttr & 0x01) ? 0xff : 0x00;
    RT.shiftPaletteHi |= (RT.latchAttr & 0x02) ? 0xff : 0x00;
}

static void bgFetch(NinState* state, uint16_t x)
{
    switch (x & 0x07)
    {
    case 0x01:
        RT.latchName = ninVMemoryRead8(state, 0x2000 | (RT.v & 0xfff));
        break;
    case 0x03:
        RT.latchAttr = ninVMemoryRead8(state, 0x23c0 | (RT.v & 0x0c00) | ((RT.v >> 4) & 0x38) | ((RT.v >> 2) & 0x07));
        if (RT.v & 0x02)
            RT.latchAttr >>= 2;
        if ((RT.v >> 5) & 0x02)
            RT.latchAttr >>= 4;
        break;
    case 0x05:
        RT.latchTileLo = ninVMemoryRead8(state, ((state->ppu.controller & 0x10) ? 0x1000 : 0x0000) | RT.latchName << 4 | ((RT.v >> 12) & 0x07));
        break;
    case 0x07:
        RT.latchTileHi = ninVMemoryRead8(state, ((state->ppu.controller & 0x10) ? 0x1000 : 0x0000) | RT.latchName << 4 | 0x08 | ((RT.v >> 12) & 0x07));
        RT.v = _incX(RT.v);
        break;
    default:
        break;
    }
}

static void emitPixel(NinState* state, uint16_t x)
{
    uint16_t addr;
    uint8_t shift;
    uint8_t spMask;
    uint8_t bgIndex;
    uint8_t spIndex;
    uint8_t palette;
    uint8_t color;

    bool bgEnabled = RT.maskEnableBackground && (x >= 8 || RT.maskEnableBackgroundLeft);
    bool spEnabled = RT.maskEnableSprites && (x >= 8 || RT.maskEnableSpritesLeft) && RT.scanline;

    shift = 15 - (x % 8);
    shift -= RT.x;

    if (bgEnabled)
    {
        bgIndex = (RT.shiftPatternLo >> shift) & 0x01;
        bgIndex |= (((RT.shiftPatternHi >> shift) & 0x01) << 1);

        palette = (RT.shiftPaletteLo >> shift) & 0x01;
        palette |= (((RT.shiftPaletteHi >> shift) & 0x01) << 1);

        addr = 0x3F00;
        if (bgIndex)
            addr = 0x3F00 | (palette << 2) | bgIndex;
        color = ninVMemoryRead8(state, addr) & 0x3f;
    }
    else
    {
        bgIndex = 0;
        color = 0x3f;
    }

    if (spEnabled)
    {
        for (unsigned i = 0; i < 8; ++i)
        {
            if (x >= RT.latchSpriteBitmapX[i] && x < RT.latchSpriteBitmapX[i] + 8)
            {
                spMask = (0x80 >> (x - RT.latchSpriteBitmapX[i]));
                spIndex = !!(RT.latchSpriteBitmapLo[i] & spMask);
                spIndex |= ((!!(RT.latchSpriteBitmapHi[i] & spMask)) << 1);
                palette = RT.latchSpriteBitmapAttr[i] & 0x03;

                if (i == 0 && spIndex && bgIndex && RT.zeroHit && RT.maskEnableBackground && RT.maskEnableSprites && x != 255)
                    state->ppu.zeroHitFlag = 1;

                if (spIndex && (!(RT.latchSpriteBitmapAttr[i] & 0x20) || !bgIndex))
                {
                    addr = 0x3F10 | (palette << 2) | spIndex;
                    color = ninVMemoryRead8(state, addr) & 0x3f;
                    break;
                }
            }
        }
    }

    state->backBuffer[RT.scanline * BITMAP_X + x] = kPalette[color];
}

static void spriteEvaluation(NinState* state, uint16_t cycle)
{
    unsigned n;
    uint16_t addr;
    uint8_t tile;
    uint8_t y;
    uint16_t alt;
    uint8_t height;

    height = RT.largeSprites ? 16 : 8;

    if (cycle == 0)
    {
        RT.oam2Index = 0;
        RT.zeroHit = RT.zeroHitNext;
        RT.zeroHitNext = 0;
    }
    if (cycle >= 1 && cycle <= 64 && !(cycle & 0x01))
    {
        /* Clear OAM2 */
        n = (cycle - 1) / 2;
        RT.oam2[n / 4].raw[n % 4] = 0xff;
    }
    if (cycle >= 65 && cycle <= 192 && !(cycle & 0x01) && RT.oam2Index < 8)
    {
        /* Actual evaluation */
        n = (cycle - 65) / 2;
        if (RT.scanline >= state->oamSprites[n].y && RT.scanline < (uint16_t)state->oamSprites[n].y + height)
        {
            if (n == 0)
                RT.zeroHitNext = 1;
            RT.oam2[RT.oam2Index++] = state->oamSprites[n];
        }
    }
    if (cycle >= 257 && cycle <= 320 && ((cycle % 8) == 0))
    {
        n = (cycle - 257) / 8;

        if (n >= RT.oam2Index)
        {
            RT.latchSpriteBitmapX[n] = 0x00;
            RT.latchSpriteBitmapAttr[n] = 0x00;
            RT.latchSpriteBitmapLo[n] = 0x00;
            RT.latchSpriteBitmapHi[n] = 0x00;
            state->ppuMonitorHandler(state, 0x1000);
            state->ppuMonitorHandler(state, 0x1000);
        }
        else
        {
            y = RT.scanline - RT.oam2[n].y;
            tile = RT.oam2[n].tile;
            alt = 0;
            if (!RT.largeSprites && (state->ppu.controller & 0x08))
            {
                alt = 0x1000;
            }
            if (RT.largeSprites && (tile & 1))
            {
                tile &= ~0x01;
                alt = 0x1000;
            }
            if (RT.oam2[n].yFlip)
            {
                y = height - y - 1;
            }
            if (RT.largeSprites && y > 7)
            {
                y &= 7;
                tile++;
            }
            addr = alt | (tile << 4) | y;

            RT.latchSpriteBitmapX[n] = RT.oam2[n].x;
            RT.latchSpriteBitmapAttr[n] = RT.oam2[n].raw[2];
            RT.latchSpriteBitmapLo[n] = ninVMemoryRead8(state, addr);
            RT.latchSpriteBitmapHi[n] = ninVMemoryRead8(state, addr | 0x08);

            if (RT.oam2[n].xFlip)
            {
                RT.latchSpriteBitmapLo[n] = bitswap8(RT.latchSpriteBitmapLo[n]);
                RT.latchSpriteBitmapHi[n] = bitswap8(RT.latchSpriteBitmapHi[n]);
            }
        }
    }
}

template <bool prerender>
static void scanline(NinState* state)
{
    uint16_t cycle;
    int isRendering;

    cycle = RT.cycle;
    isRendering = (RT.maskEnableBackground || RT.maskEnableSprites);

    if (RT.scanline == 0 && RT.maskEnableBackground && cycle == 0 && state->frameOdd)
    {
        RT.cycle++;
    }
    if (prerender && cycle == 1)
    {
        ninUnsetFlagNMI(state, NMI_OCCURED);
        state->ppu.zeroHitFlag = 0;
        RT.zeroHit = 0;
        RT.zeroHitNext = 0;
    }
    if (isRendering)
    {
        if (!prerender) spriteEvaluation(state, cycle);
        if ((cycle > 0 && cycle <= 256) || (cycle >= 321 && cycle <= 336))
        {
            bgFetch(state, cycle - 1);
        }
        if (!prerender && cycle >= 2 && cycle <= 257)
            emitPixel(state, cycle - 2);
        if ((cycle >= 9 && cycle <= 257 && ((cycle % 8) == 1)) || cycle == 329 || cycle == 337)
        {
            bgReload(state);
        }
        if (cycle == 256)
        {
            RT.v = _incY(RT.v);
        }
        if (cycle == 257)
        {
            RT.v &= ~kHMask;
            RT.v |= (RT.t & kHMask);
        }
        if (prerender && (cycle >= 280 && cycle <= 304))
        {
            RT.v &= kHMask;
            RT.v |= (RT.t & ~kHMask);
        }
    }
}

int ninPpuRunCycles(NinState* state, uint16_t cycles)
{
    int newFrame;

    newFrame = 0;

    while (cycles--)
    {
        if (RT.scanline < 240) scanline<false>(state);
        if (RT.scanline == 241 + state->info.specs().vblank) scanline<true>(state);
        if (RT.scanline == 241 && RT.cycle == 1)
        {
            if (!RT.inhibitNmi)
                ninSetFlagNMI(state, NMI_OCCURED);
            RT.inhibitNmi = 0;
        }

        RT.cycle++;
        if (RT.cycle == 341)
        {
            RT.cycle = 0;
            RT.scanline++;
            if (RT.scanline == 242 + state->info.specs().vblank)
            {
                swapBuffers(state);
                RT.scanline = 0;
                newFrame = 1;
                state->frameOdd ^= 1;
            }
        }
    }

    return newFrame;
}
