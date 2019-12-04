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

static void initCPU(NinState* state)
{
    state->cpu.p = PFLAG_I;
    state->cpu.pc = ninMemoryRead16(state, 0xfffc);
    state->cpu.regs[REG_S] = 0xfd;
    state->cyc = 7;
}

NIN_API NinError ninCreateState(NinState** dst, const char* path)
{
    NinState* state;
    NinError err;

    state = zalloc(sizeof(*state));
    state->backBuffer = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->frontBuffer = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->ram = zalloc(RAM_SIZE);
    state->vram = zalloc(VRAM_SIZE);
    state->palettes = zalloc(0x20);
    state->oam = zalloc(256);

    state->apu.noise.feedback = 1;
    state->apu.dmc.address = 0x8000;
    state->audio.dividerNum = 48000;

    if ((err = ninLoadRom(state, path)) != NIN_OK)
    {
        ninDestroyState(state);
        *dst = NULL;
        return err;
    }

    initCPU(state);

    *dst = state;
    return NIN_OK;
}

void ninDestroyState(NinState* state)
{
    ninSyncSave(state);
    if (state->saveFile)
        fclose(state->saveFile);

    free(state->backBuffer);
    free(state->frontBuffer);
    free(state->ram);
    free(state->vram);
    free(state->palettes);
    free(state->oam);
    free(state->prgRam);
    free(state->prgRom);
    free(state->chrRam);
    free(state->chrRom);
    free(state);
}

const uint32_t* ninGetScreenBuffer(NinState* state)
{
    return state->frontBuffer;
}

void ninSetInput(NinState* state, uint8_t input)
{
    state->controller = input;
}
