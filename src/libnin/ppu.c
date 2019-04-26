#include <math.h>
#include <libnin/libnin.h>

uint8_t ninPpuRegRead(NinState* state, uint16_t reg)
{
    static int sp0hack = 0;
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
        if (state->ppu.nmi & NMI_OCCURED)
            value |= 0x80;
        ninUnsetFlagNMI(state, NMI_OCCURED);
        if (sp0hack)
            value |= 0x40;
        sp0hack ^= 1;
        state->ppu.addrHalfFlag = 0;
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
        if ((state->ppu.addr & 0x3f00) == 0x3f00)
            value = ninVMemoryRead8(state, state->ppu.addr);
        else
        {
            value = state->ppu.readBuf;
            state->ppu.readBuf = ninVMemoryRead8(state, state->ppu.addr);
        }
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
        state->ppu.controller = value;
        // Set base nametable
        state->ppu.rt.t &= ~0x0c00;
        state->ppu.rt.t |= ((value & 0x03) << 10);
        break;
    case 0x01:
        break;
    case 0x02:
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    case 0x05: // PPUSCROLL
        if (!state->ppu.addrHalfFlag)
            state->ppu.newAddr = value;
        else
        {
            // Fine X
            state->ppu.rt.x = state->ppu.newAddr & 0x07;

            // Coarse X
            state->ppu.rt.t &= ~(0x001f);
            state->ppu.rt.t |= ((state->ppu.newAddr >> 3) & 0x1f);

            // Fine Y
            state->ppu.rt.t &= ~(0xe000);
            state->ppu.rt.t |= ((value & 0x07) << 12);

            // Coarse Y
            state->ppu.rt.t &= ~(0x03e0);
            state->ppu.rt.t |= ((value >> 3) & 0x1f) << 5;

            printf("-- SCROLL --: T: 0x%04x\n", state->ppu.rt.t);
        }
        state->ppu.addrHalfFlag ^= 1;
        break;
    case 0x06: // PPUADDR
        if (!state->ppu.addrHalfFlag)
            state->ppu.newAddr = value << 8;
        else
            state->ppu.addr = value | state->ppu.newAddr;
        state->ppu.addrHalfFlag ^= 1;
        break;
    case 0x07: //PPUDATA
        ninVMemoryWrite8(state, state->ppu.addr, value);
        state->ppu.addr += (state->ppu.controller & 0x04) ? 32 : 1;
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

static void _dumpFrame(NinState* state)
{
    static int counter;
    char buffer[512];
    FILE* file;
    uint16_t tmp16;
    uint32_t tmp32;

    snprintf(buffer, 512, "frame_%05d.bmp", counter++);
    file = fopen(buffer, "wb");
    fwrite("BM", 2, 1, file);
    tmp32 = 14 + 40 + BITMAP_X * BITMAP_Y * 3; fwrite(&tmp32, 4, 1, file);
    tmp32 = 0; fwrite(&tmp32, 4, 1, file);
    tmp32 = 54; fwrite(&tmp32, 4, 1, file);
    tmp32 = 40; fwrite(&tmp32, 4, 1, file);
    tmp32 = BITMAP_X; fwrite(&tmp32, 4, 1, file);
    tmp32 = -BITMAP_Y; fwrite(&tmp32, 4, 1, file);
    tmp16 = 1; fwrite(&tmp16, 2, 1, file);
    tmp16 = 24; fwrite(&tmp16, 2, 1, file);
    tmp32 = 0; fwrite(&tmp32, 4, 1, file);
    tmp32 = BITMAP_X * BITMAP_Y * 3; fwrite(&tmp32, 4, 1, file);
    tmp32 = 2048; fwrite(&tmp32, 4, 1, file);
    tmp32 = 2048; fwrite(&tmp32, 4, 1, file);
    tmp32 = 0; fwrite(&tmp32, 4, 1, file);
    tmp32 = 0; fwrite(&tmp32, 4, 1, file);

    for (size_t i = 0; i < BITMAP_X * BITMAP_Y; ++i)
        fwrite(state->bitmap + i, 3, 1, file);

    fclose(file);

    printf("\n\n\nFRAME RENDERED\n\n\n");
    //getchar();
}

void _renderSprite(NinState* state, int sprite)
{
    uint8_t y;
    uint8_t index;
    uint8_t attr;
    uint8_t x;
    size_t off;

    int w;
    int h;

    y =     state->oam[sprite * 4 + 0];
    index = state->oam[sprite * 4 + 1];
    attr =  state->oam[sprite * 4 + 2];
    x =     state->oam[sprite * 4 + 3];

    w = 256 - x;
    h = 239 - y;

    if (w > 8) w = 8;
    if (h > 8) h = 8;

    for (int j = 0; j < h; ++j)
    {
        for (int i = 0; i < w; ++i)
        {
            off = (j + y + 1) * BITMAP_X + (x + i);
            state->bitmap[off] |= 0x00ff0000;
        }
    }
}

void ninPpuRenderFrame(NinState* state)
{
    uint8_t scrollX;
    uint8_t scrollY;
    uint32_t value;
    uint8_t entry;
    uint8_t pattern[16];
    size_t screenX;
    size_t screenY;
    size_t off;

    scrollX = state->ppu.scrollX / 8;
    scrollY = state->ppu.scrollY / 8;
    uint16_t nameIndex;
    uint16_t nameX;
    uint16_t nameY;
    uint8_t hiNameX;
    uint8_t loNameX;

    printf("scroll: %d / %d\n", scrollX, scrollY);
    for (int y = 0; y < 30; ++y)
    {
        for (int x = 0; x < 32; ++x)
        {
            nameX = x + scrollX;
            hiNameX = nameX / 32;
            loNameX = nameX % 32;
            nameIndex = (0x400 * ((state->ppu.controller & 0x03) + hiNameX)) | (y * 32 + loNameX);
            entry = ninVMemoryRead8(state, 0x2000 | (nameIndex & 0xfff));
            for (int i = 0; i < 16; ++i)
                pattern[i] = ninVMemoryRead8(state, ((state->ppu.controller & 0x10) ? 0x1000 : 0) | (entry << 4) | i);
            for (int py = 0; py < 8; ++py)
            {
                for (int px = 0; px < 8; ++px)
                {
                    value = 0x00000000;
                    screenX = x * 8 + px;
                    screenY = y * 8 + py;
                    off = screenY * BITMAP_X + screenX;
                    if (pattern[py] & (1 << (7 - px)))
                        value |= 0x404040;
                    if (pattern[py + 8] & (1 << (7 - px)))
                        value |= 0x808080;
                    state->bitmap[off] = value;
                }
            }
        }
    }
    for (int i = 0; i < 64; ++i)
        _renderSprite(state, i);
    //_dumpFrame(state);
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

int ninPpuRunCycles(NinState* state, uint16_t cycles)
{
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

    static const uint32_t kTempPalette[] = {
        0xff000000, 0xff444444, 0xff888888, 0xffcccccc
    };

    static const uint16_t kHMask = 0x041f;

    uint8_t tmp;
    uint8_t shift;
    uint8_t palette;
    uint16_t tileNum;
    uint8_t colorIndex;
    int newFrame;
    NinRuntimePPU rt;

    newFrame = 0;
    rt = state->ppu.rt;

    while (cycles--)
    {
        /*
         * Scanlines are numbered from 0 to 261 (inclusive), 261 being
         * the dummy pre-render one.
         */

        if (rt.cycle == 256)
        {
            rt.v = _incY(rt.v);
            rt.v &= ~kHMask;
            rt.v |= (rt.t & kHMask);
        }

        /* Check for load cycles */
        if ((rt.scanline < 240 && ((rt.cycle >= 1 && rt.cycle <= 256) || (rt.cycle >= 321 && rt.cycle <= 336)))
            || (rt.scanline == 261 && rt.cycle >= 321 && rt.cycle <= 336))
        {
            switch ((rt.cycle - 1) & 7)
            {
            case 0:
                rt.shiftPatternLo <<= 8;
                rt.shiftPatternHi <<= 8;
                rt.shiftPaletteLo <<= 8;
                rt.shiftPaletteHi <<= 8;
                rt.shiftPatternLo |= rt.latchTileLo;
                rt.shiftPatternHi |= rt.latchTileHi;
                rt.shiftPaletteLo |= (rt.latchAttr & 0x01) ? 0xff : 0x00;
                rt.shiftPaletteHi |= (rt.latchAttr & 0x02) ? 0xff : 0x00;
                //printf("v (name): 0x%04x | %d | %d\n", rt.v, rt.cycle, rt.scanline);
                rt.latchName = ninVMemoryRead8(state, 0x2000 | (rt.v & 0xfff));
                break;
            case 2:
                rt.latchAttr = ninVMemoryRead8(state, 0x23c0 | (rt.v & 0x0C00) | ((rt.v >> 4) & 0x38) | ((rt.v >> 2) & 0x07));
                if (rt.v & 0x02)
                    rt.latchAttr >>= 2;
                if ((rt.v >> 5) & 0x02)
                    rt.latchAttr >>= 4;
                break;
            case 4:
                rt.latchTileLo = ninVMemoryRead8(state, 0x1000 | rt.latchName << 4 | ((rt.v >> 12) & 0x07));
                break;
            case 6:
                rt.latchTileHi = ninVMemoryRead8(state, 0x1000 | rt.latchName << 4 | 0x08 | ((rt.v >> 12) & 0x07));
                rt.v = _incX(rt.v);
                break;
            }
        }

        /* Check for shifts */
        /*
        if ((rt.scanline < 240 && rt.cycle >= 9 && rt.cycle <= 257 && (((rt.cycle - 1) & 0x07) == 0)) || ((rt.scanline < 240 || rt.scanline == 261) && (rt.cycle == 327 || rt.cycle == 335)))
        {
            rt.shiftPatternLo <<= 8;
            rt.shiftPatternHi <<= 8;
            rt.shiftPatternLo |= rt.latchTileLo;
            rt.shiftPatternHi |= rt.latchTileHi;
            //printf("v (shift): 0x%04x | %d | %d\n", rt.v, rt.cycle, rt.scanline);
            rt.v = _incX(rt.v);
        }
        */

        if (rt.scanline < 240)
        {
            /* Visible scanlines */
            if (rt.cycle >= 1 && rt.cycle <= 256)
            {
                shift = 15 - ((rt.cycle - 1) % 8);
                shift -= rt.x;
                tmp = (rt.shiftPatternLo >> shift) & 0x01;
                tmp |= (((rt.shiftPatternHi >> shift) & 0x01) << 1);

                palette = (rt.shiftPaletteLo >> shift) & 0x01;
                palette |= (((rt.shiftPaletteHi >> shift) & 0x01) << 1);
                if (tmp == 0)
                    colorIndex = ninVMemoryRead8(state, 0x3F00);
                else
                    colorIndex = ninVMemoryRead8(state, 0x3F00 | (palette << 2) | tmp) & 0x3f;

                state->bitmap[rt.scanline * BITMAP_X + (rt.cycle - 1)] = kPalette[colorIndex];
                //state->bitmap[rt.scanline * BITMAP_X + (rt.cycle - 1)] = kTempPalette[tmp];
            }
        }
        else if (rt.scanline == 240 && rt.cycle == 0)
        {
            ninSetFlagNMI(state, NMI_OCCURED);
        }
        else if (rt.scanline == 261 && rt.cycle == 0)
        {
            ninUnsetFlagNMI(state, NMI_OCCURED);
        }

        if (rt.scanline == 261 && rt.cycle == 304)
        {
            rt.v = rt.t;
        }

        /* Increment the cycle counter */
        rt.cycle++;
        if (rt.cycle == 341)
        {
            rt.cycle = 0;
            rt.scanline++;
            if (rt.scanline == 262)
            {
                rt.scanline = 0;
                newFrame = 1;
            }
        }
    }

    state->ppu.rt = rt;
    return newFrame;
}
