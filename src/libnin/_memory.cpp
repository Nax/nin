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
            memcpy(dst + dOff, state->memory.ram + oOff, oLen);
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
            memcpy(dst + dOff, state->mapper.prg(i) + oOff, oLen);
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
            memcpy(dst + dOff, state->memory.ram + oOff, oLen);
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
        memcpy(dst + dOff, state->cart.segment(CART_PRG_RAM).base + oOff, oLen);
    }

    /* 0xe000-0xffff, PRG ROM */
    if (memoryExtractOverlap(start, len, 0xe000, 0x2000, &oOff, &oLen, &dOff))
    {
        memcpy(dst + dOff, state->cart.segment(CART_PRG_ROM).base + oOff, oLen);
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
