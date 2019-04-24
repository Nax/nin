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

void ninPpuRenderFrame(NinState* state)
{
    uint32_t value;
    uint8_t entry;
    uint8_t pattern[16];
    size_t screenX;
    size_t screenY;
    size_t off;

    for (int y = 0; y < 30; ++y)
    {
        for (int x = 0; x < 32; ++x)
        {
            entry = ninVMemoryRead8(state, 0x2000 | (0x400 * (state->ppu.controller & 0x03)) | (y * 32 + x));
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
    //_dumpFrame(state);
}
