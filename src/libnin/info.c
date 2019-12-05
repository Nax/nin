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

NIN_API void ninInfoQueryInteger(NinState* state, NinInt32* dst, NinInfo info)
{
    switch (info)
    {
    case NIN_INFO_SYSTEM:
        *dst = state->system;
        break;
    case NIN_INFO_CLOCK_RATE:
        *dst = state->regionData.clockRate;
        break;
    case NIN_INFO_FRAME_CYCLES:
        *dst = state->regionData.frameCycles;
        break;
    case NIN_INFO_FRAME_DELAY:
        *dst = state->regionData.frameDelay;
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
