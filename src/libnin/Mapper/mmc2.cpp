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

#if 0

#include <libnin/libnin.h>

static void applycMMC2(NinState* state)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;

    ninBankSwitchChrRom4k(state, 0, mmc2->latch0 ? mmc2->bankLatch0Hi : mmc2->bankLatch0Lo);
    ninBankSwitchChrRom4k(state, 1, mmc2->latch1 ? mmc2->bankLatch1Hi : mmc2->bankLatch1Lo);
}

void ninPrgWriteHandlerCommonMMC2_MMC4(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;
    switch (addr & 0xf000)
    {
    case 0xb000:
        mmc2->bankLatch0Lo = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xc000:
        mmc2->bankLatch0Hi = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xd000:
        mmc2->bankLatch1Lo = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xe000:
        mmc2->bankLatch1Hi = value & 0x1f;
        applycMMC2(state);
        break;
    case 0xf000:
        if (value & 0x1)
            ninMirrorV(state);
        else
            ninMirrorH(state);
    }
}

void ninPrgWriteHandlerMMC2(NinState* state, uint16_t addr, uint8_t value)
{
    ninPrgWriteHandlerCommonMMC2_MMC4(state, addr, value);
    switch (addr & 0xf000)
    {
    case 0xa000: // PRG ROM bank
        ninBankSwitchPrgRom8k(state, 0, value & 0xf);
        break;
    }
}

void ninPpuMonitorHandlerMMC2(NinState* state, uint16_t addr)
{
    NinMapperRegsMMC2* mmc2 = &state->mapperRegs.mmc2;

    if (addr == 0x0fd8)
    {
        mmc2->latch0 = 0;
        applycMMC2(state);
    }
    else if (addr == 0x0fe8)
    {
        mmc2->latch0 = 1;
        applycMMC2(state);
    }
    else if (addr >= 0x1fd8 && addr <= 0x1fdf)
    {
        mmc2->latch1 = 0;
        applycMMC2(state);
    }
    else if (addr >= 0x1fe8 && addr <= 0x1fef)
    {
        mmc2->latch1 = 1;
        applycMMC2(state);
    }
}

#endif
