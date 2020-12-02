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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nin/nin.h>

#define SEC_NTSC(x) ((std::size_t)((x)*1789773))

static void dummyAudio(void*, const float*)
{
}

static std::uint32_t qhash(const std::uint8_t* data, std::size_t len)
{
    uint32_t hash = 0;

    for (std::size_t i = 0; i < len; ++i)
    {
        hash += data[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

int main(int argc, char** argv)
{
    std::uint32_t hash;
    NinState*     state;
    std::size_t   dummy;
    std::uint8_t  tmp[0x800];

    if (argc != 3)
        return 1;

    ninCreateState(&state, argv[1]);
    if (!state)
        return 1;

    ninAudioSetCallback(state, &dummyAudio, nullptr);
    ninRunCycles(state, SEC_NTSC(std::atoi(argv[2])), &dummy);
    ninDumpNametable(state, tmp, 0);
    ninDumpNametable(state, tmp + 0x400, 1);

    hash = qhash(tmp, 0x800);

    std::printf("0x%08x\n", hash);

    return 0;
}
