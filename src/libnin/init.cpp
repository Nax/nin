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

    state = new NinState{};
    state->backBuffer = new uint32_t[BITMAP_X * BITMAP_Y]();
    state->frontBuffer = new uint32_t[BITMAP_X * BITMAP_Y]();
    state->ram = new uint8_t[RAM_SIZE]();
    state->vram = new uint8_t[VRAM_SIZE]();
    state->palettes = new uint8_t[0x20]();
    state->oam = new uint8_t[0x100]();

    state->audio.setTargetFrequency(48000);

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
    if (state)
    {
        ninSyncSave(state);
        if (state->saveFile)
            fclose(state->saveFile);

        delete [] state->backBuffer;
        delete [] state->frontBuffer;
        delete [] state->ram;
        delete [] state->vram;
        delete [] state->palettes;
        delete [] state->oam;
        delete state;
    }
}

const uint32_t* ninGetScreenBuffer(NinState* state)
{
    return state->frontBuffer;
}

void ninSetInput(NinState* state, uint8_t input)
{
    state->controller = input;
}
