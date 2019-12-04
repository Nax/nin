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

NIN_API void ninBankSwitchPrgRom8k(NinState* state, uint8_t slot, uint8_t bank)
{
    state->prgRomBank[slot] = state->prgRom + (bank & (state->prgBankCount - 1)) * 0x2000;
}

NIN_API void ninBankSwitchPrgRom16k(NinState* state, uint8_t slot, uint8_t bank)
{
    ninBankSwitchPrgRom8k(state, slot * 2 + 0, bank * 2 + 0);
    ninBankSwitchPrgRom8k(state, slot * 2 + 1, bank * 2 + 1);
}

NIN_API void ninBankSwitchPrgRom32k(NinState* state, uint8_t bank)
{
    ninBankSwitchPrgRom8k(state, 0, bank * 4 + 0);
    ninBankSwitchPrgRom8k(state, 1, bank * 4 + 1);
    ninBankSwitchPrgRom8k(state, 2, bank * 4 + 2);
    ninBankSwitchPrgRom8k(state, 3, bank * 4 + 3);
}

NIN_API void ninBankSwitchChrRom1k(NinState* state, uint8_t slot, uint8_t bank)
{
    state->chrBank[slot] = (state->chrRam ? state->chrRam : state->chrRom) + (bank & (state->chrBankCount - 1)) * 0x400;
}

NIN_API void ninBankSwitchChrRom4k(NinState* state, uint8_t slot, uint8_t bank)
{
    ninBankSwitchChrRom1k(state, slot * 4 + 0, bank * 4 + 0);
    ninBankSwitchChrRom1k(state, slot * 4 + 1, bank * 4 + 1);
    ninBankSwitchChrRom1k(state, slot * 4 + 2, bank * 4 + 2);
    ninBankSwitchChrRom1k(state, slot * 4 + 3, bank * 4 + 3);
}

NIN_API void ninBankSwitchChrRom8k(NinState* state, uint8_t bank)
{
    ninBankSwitchChrRom1k(state, 0, bank * 8 + 0);
    ninBankSwitchChrRom1k(state, 1, bank * 8 + 1);
    ninBankSwitchChrRom1k(state, 2, bank * 8 + 2);
    ninBankSwitchChrRom1k(state, 3, bank * 8 + 3);
    ninBankSwitchChrRom1k(state, 4, bank * 8 + 4);
    ninBankSwitchChrRom1k(state, 5, bank * 8 + 5);
    ninBankSwitchChrRom1k(state, 6, bank * 8 + 6);
    ninBankSwitchChrRom1k(state, 7, bank * 8 + 7);
}
