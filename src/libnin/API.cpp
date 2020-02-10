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

#include <nin/nin.h>
#include <libnin/libnin.h>

NIN_API void ninAudioSetFrequency(NinState* state, uint32_t frequency)
{
    state->audio.setTargetFrequency(frequency);
}

NIN_API void ninAudioSetCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg)
{
    state->audio.setCallback(callback, arg);
}

NIN_API void ninInfoQueryInteger(NinState* state, NinInt32* dst, NinInfo info)
{
    switch (info)
    {
    case NIN_INFO_SYSTEM:
        *dst = state->info.system();
        break;
    case NIN_INFO_CLOCK_RATE:
        *dst = state->info.specs().clockRate;
        break;
    case NIN_INFO_FRAME_CYCLES:
        *dst = state->info.specs().frameCycles;
        break;
    case NIN_INFO_FRAME_DELAY:
        *dst = state->info.specs().frameDelay;
        break;
    case NIN_INFO_PC:
        *dst = state->cpu.pc;
        break;
    case NIN_INFO_REG_A:
        *dst = state->cpu.regs[REG_A];
        break;
    case NIN_INFO_REG_X:
        *dst = state->cpu.regs[REG_X];
        break;
    case NIN_INFO_REG_Y:
        *dst = state->cpu.regs[REG_Y];
        break;
    case NIN_INFO_REG_S:
        *dst = state->cpu.regs[REG_S];
        break;
    default:
        *dst = 0;
        break;
    }
}

NIN_API void ninSetSaveFile(NinState* state, const char* path)
{
    FILE* f;

    if (!state->battery)
        return;

    f = fopen(path, "ab");
    if (f)
        fclose(f);

    f = fopen(path, "r+b");
    if (f)
    {
        state->cart.load(CART_PRG_RAM, state->cart.segment(CART_PRG_RAM).bankCount, f);
        state->saveFile = f;
    }
}

NIN_API void ninSyncSave(NinState* state)
{
    FILE* f;
    const CartSegment& prgRam = state->cart.segment(CART_PRG_RAM);

    f = state->saveFile;
    if (!f)
        return;

    fseek(f, 0, SEEK_SET);
    fwrite(prgRam.base, prgRam.bankCount * 0x2000, 1, f);
    fflush(f);
}

NIN_API const uint32_t* ninGetScreenBuffer(NinState* state)
{
    return state->video.front();
}

NIN_API void ninSetInput(NinState* state, uint8_t input)
{
    state->input.set(input);
}
