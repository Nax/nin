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

static void updateBank(NinState* state)
{
    NinMapperRegsMMC3* mmc3;

    mmc3 = &state->mapperRegs.mmc3;
    if (mmc3->bankModePrgRom == 0)
    {
        ninBankSwitchPrgRom8k(state, 0, mmc3->bank[6]);
        ninBankSwitchPrgRom8k(state, 1, mmc3->bank[7]);
        ninBankSwitchPrgRom8k(state, 2, state->prgBankCount * 2 - 2);
        ninBankSwitchPrgRom8k(state, 3, state->prgBankCount * 2 - 1);
    }
    else
    {
        ninBankSwitchPrgRom8k(state, 0, state->prgBankCount * 2 - 2);
        ninBankSwitchPrgRom8k(state, 1, mmc3->bank[7]);
        ninBankSwitchPrgRom8k(state, 2, mmc3->bank[6]);
        ninBankSwitchPrgRom8k(state, 3, state->prgBankCount * 2 - 1);
    }

    if (mmc3->bankModeChrRom == 0)
    {
        ninBankSwitchChrRom1k(state, 0, mmc3->bank[0] & ~1);
        ninBankSwitchChrRom1k(state, 1, mmc3->bank[0] |  1);
        ninBankSwitchChrRom1k(state, 2, mmc3->bank[1] & ~1);
        ninBankSwitchChrRom1k(state, 3, mmc3->bank[1] |  1);
        ninBankSwitchChrRom1k(state, 4, mmc3->bank[2]);
        ninBankSwitchChrRom1k(state, 5, mmc3->bank[3]);
        ninBankSwitchChrRom1k(state, 6, mmc3->bank[4]);
        ninBankSwitchChrRom1k(state, 7, mmc3->bank[5]);
    }
    else
    {
        ninBankSwitchChrRom1k(state, 0, mmc3->bank[2]);
        ninBankSwitchChrRom1k(state, 1, mmc3->bank[3]);
        ninBankSwitchChrRom1k(state, 2, mmc3->bank[4]);
        ninBankSwitchChrRom1k(state, 3, mmc3->bank[5]);
        ninBankSwitchChrRom1k(state, 4, mmc3->bank[0] & ~1);
        ninBankSwitchChrRom1k(state, 5, mmc3->bank[0] | 1);
        ninBankSwitchChrRom1k(state, 6, mmc3->bank[1] & ~1);
        ninBankSwitchChrRom1k(state, 7, mmc3->bank[1] | 1);
    }
}

void ninPrgWriteHandlerMMC3(NinState* state, uint16_t addr, uint8_t value)
{
    uint16_t selector;
    NinMapperRegsMMC3* mmc3;

    selector = (addr & 0xe001);
    mmc3 = &state->mapperRegs.mmc3;

    switch (selector)
    {
    case 0x8000: // Bank select
        mmc3->bankSelect = value & 0x7;
        mmc3->bankModePrgRom = (value >> 6) & 0x01;
        mmc3->bankModeChrRom = (value >> 7) & 0x01;
        updateBank(state);
        break;
    case 0x8001: // Bank data
        mmc3->bank[mmc3->bankSelect] = value;
        updateBank(state);
        break;
    case 0xa000:
        if ((value & 0x01) == 0)
            ninMirrorH(state);
        else
            ninMirrorV(state);
        break;
    case 0xc000:
        state->irqScanlineReloadValue = value;
        break;
    case 0xc001:
        state->irqScanlineReload = 1;
        break;
    case 0xe000:
        state->irqScanlineEnabled = 0;
        ninClearIRQ(state, IRQ_SCANLINE);
        break;
    case 0xe001:
        state->irqScanlineEnabled = 1;
        break;
    }
}

void ninPpuMonitorHandlerMMC3(NinState* state, uint16_t addr)
{
    if (addr >= 0x3f00)
        return;

    if (((state->oldVmemAddr & 0x1000) == 0x0000) && ((addr & 0x1000) == 0x1000))
    {
        if (!state->irqScanlineFilterShifter)
        {
            if (state->irqScanlineCounter == 0 && state->irqScanlineEnabled)
            {
                ninSetIRQ(state, IRQ_SCANLINE);
            }
            if (state->irqScanlineCounter == 0 || state->irqScanlineReload)
            {
                state->irqScanlineCounter = state->irqScanlineReloadValue;
                state->irqScanlineReload = 0;
            }
            else
                state->irqScanlineCounter--;
        }
        state->irqScanlineFilterShifter = 0x8000;
    }
    else
        state->irqScanlineFilterShifter >>= 1;
    state->oldVmemAddr = addr;
}
