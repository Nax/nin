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

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <nin/nin.h>

#define COUNT 10000000
#define SLICE 4096

using Clock     = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration  = std::chrono::duration<double>;

static void dummyAudio(void*, const float*)
{
}

int main(int argc, char** argv)
{
    NinState*     state;
    TimePoint     before;
    TimePoint     after;
    double        duration;
    std::uint64_t cps;
    size_t        cyc;
    size_t        tmp;

    if (argc != 2)
        return 1;

    ninCreateState(&state, argv[1]);
    if (!state)
        return 1;

    srand((unsigned)time(NULL));
    ninAudioSetCallback(state, &dummyAudio, nullptr);

    cyc    = 0;
    before = Clock::now();
    for (;;)
    {
        ninSetInput(state, (rand() & 0xff));
        ninRunCycles(state, SLICE, &tmp);
        cyc += SLICE;
        cyc += tmp;
        if (cyc >= COUNT)
            break;
    }
    after    = Clock::now();
    duration = std::chrono::duration_cast<Duration>(after - before).count();
    cps      = (std::uint64_t)((double)cyc / duration);
    printf("%llu cycles/s\n", (long long unsigned int)cps);
    return 0;
}
