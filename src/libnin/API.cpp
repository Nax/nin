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
#include <nin/nin.h>
#include <libnin/State.h>

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
        *dst = state->cpu.pc();
        break;
    case NIN_INFO_REG_A:
        *dst = state->cpu.reg(REG_A);
        break;
    case NIN_INFO_REG_X:
        *dst = state->cpu.reg(REG_X);
        break;
    case NIN_INFO_REG_Y:
        *dst = state->cpu.reg(REG_Y);
        break;
    case NIN_INFO_REG_S:
        *dst = state->cpu.reg(REG_S);
        break;
    default:
        *dst = 0;
        break;
    }
}

NIN_API void ninSetSaveFile(NinState* state, const char* path)
{
    state->save.setSaveFile(path);
}

NIN_API void ninSyncSave(NinState* state)
{
    state->save.sync();
}

NIN_API const uint32_t* ninGetScreenBuffer(NinState* state)
{
    return state->video.front();
}

NIN_API void ninSetInput(NinState* state, uint8_t input)
{
    state->input.set(input);
}

NIN_API int ninRunCycles(NinState* state, size_t cycles, size_t* cyc)
{
    for (std::size_t i = 0; i < cycles; ++i)
    {
        state->cpu.tick(1);
        state->ppu.tick(3);
        state->apu.tick(1);
        state->mapper.tick();
    }

    if (cyc)
    {
        *cyc = 0;
    }

    return state->video.changed();
}

NIN_API void ninDumpMemory(NinState* state, uint8_t* dst, uint16_t start, size_t len)
{
    state->busMain.dump(dst, start, len);
}

NIN_API NinError ninCreateState(NinState** dst, const char* path)
{
    NinState* state;
    NinError err;

    state = new NinState{};
    state->audio.setTargetFrequency(48000);

    if ((err = state->loadRom(path)) != NIN_OK)
    {
        ninDestroyState(state);
        *dst = NULL;
        return err;
    }

    *dst = state;
    return NIN_OK;
}

NIN_API void ninDestroyState(NinState* state)
{
    if (state)
    {
        delete state;
    }
}

NIN_API void ninLoadBiosFDS(NinState* state, const char* path)
{
    std::FILE* f;

    f = std::fopen(path, "rb");
    if (!f)
        return;
    state->cart.load(CART_PRG_ROM, 1, f);
    std::fclose(f);
}

NIN_API int ninStepInstruction(NinState* state)
{
    for (int i = 0; i < 8; ++i)
    {
        state->cpu.tick(1);
        state->ppu.tick(3);
        state->apu.tick(1);
        state->mapper.tick();

        if (state->cpu.dispatching())
            break;
    }

    return state->video.changed();
}

NIN_API void ninDumpNametable(NinState* state, uint8_t* dst, int nametable)
{
    std::uint16_t base;

    base = (nametable & 0x01) ? 0x400 : 0x000;
    std::memcpy(dst, state->memory.vram + base, 0x400);
}
