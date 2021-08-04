/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 *
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 *
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 *
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <cstring>
#include <libnin/State.h>
#include <nin/nin.h>

using namespace libnin;

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
    case NIN_INFO_DISK_SIDE_COUNT:
        *dst = state->disk.sideCount();
        break;
    case NIN_INFO_PC:
        *dst = state->cpu.pc;
        break;
    case NIN_INFO_REG_A:
        *dst = state->cpu.a;
        break;
    case NIN_INFO_REG_X:
        *dst = state->cpu.x;
        break;
    case NIN_INFO_REG_Y:
        *dst = state->cpu.y;
        break;
    case NIN_INFO_REG_S:
        *dst = state->cpu.s;
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
        state->interpreterCycle.tick(1);
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
    NinError  err;

    state = (NinState*)State::create(err, path);

    if (err == NIN_OK)
    {
        state->audio.setTargetFrequency(48000);
    }

    *dst = state;
    return err;
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

    if (state->info.system() != NIN_SYSTEM_FDS)
        return;

    f = std::fopen(path, "rb");
    if (!f)
        return;
    state->cart.load(CART_PRG_ROM, 1, f);
    state->mapper.bankPrg8k(5, CART_PRG_ROM, 0);
    std::fclose(f);
}

NIN_API int ninStepInstruction(NinState* state)
{
    for (int i = 0; i < 8; ++i)
    {
        state->interpreterCycle.tick(1);
        state->ppu.tick(3);
        state->apu.tick(1);
        state->mapper.tick();

        if (state->interpreterCycle.dispatching())
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

NIN_API void ninInsertDisk(NinState* state, int diskSide)
{
    state->disk.setSide(diskSide);
}
