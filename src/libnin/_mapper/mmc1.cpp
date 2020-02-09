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

static void mirroringSwitch(NinState* state)
{
    switch (state->mapperRegs.mmc1.mirroring)
    {
    case 0x00:
        ninMirrorA(state);
        break;
    case 0x01:
        ninMirrorB(state);
        break;
    case 0x02:
        ninMirrorH(state);
        break;
    case 0x03:
        ninMirrorV(state);
        break;
    }
}

static void bankSwitch(NinState* state)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;

    switch (mmc1->prgBankMode)
    {
    case 0:
    case 1:
        ninBankSwitchPrgRom16k(state, 0, (mmc1->prgBank & 0xfe) | 0);
        ninBankSwitchPrgRom16k(state, 1, (mmc1->prgBank & 0xfe) | 1);
        break;
    case 2:
        ninBankSwitchPrgRom16k(state, 0, 0);
        ninBankSwitchPrgRom16k(state, 1, mmc1->prgBank & 0xff);
        break;
    case 3:
        ninBankSwitchPrgRom16k(state, 0, mmc1->prgBank & 0xff);
        ninBankSwitchPrgRom16k(state, 1, state->cart.segment(CART_PRG_ROM).bankCount - 1);
        break;
    }
}

static void bankSwitchChr(NinState* state)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;

    if (mmc1->chrBankMode)
    {
        ninBankSwitchChrRom4k(state, 0, mmc1->chrBank0);
        ninBankSwitchChrRom4k(state, 1, mmc1->chrBank1);
    }
    else
    {
        ninBankSwitchChrRom4k(state, 0, mmc1->chrBank0 & ~(0x01));
        ninBankSwitchChrRom4k(state, 1, mmc1->chrBank0 | 0x01);
    }
}

static int writeReg(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;

    switch ((addr & 0x7fff) >> 13)
    {
    case 0:
        /* 0x8000 - 0x9fff */
        mmc1->mirroring = value & 0x03;
        mmc1->prgBankMode = (value >> 2) & 0x03;
        mmc1->chrBankMode = (value >> 4) & 0x01;
        mirroringSwitch(state);
        bankSwitch(state);
        bankSwitchChr(state);
        return 1;
    case 1:
        /* 0xa000 - 0xbfff */
        mmc1->chrBank0 = value;
        bankSwitchChr(state);
        //getchar();
        return 0;
    case 2:
        /* 0xc000 - 0xdfff */
        mmc1->chrBank1 = value;
        bankSwitchChr(state);
        //getchar();
        return 0;
    case 3:
        /* 0xe000 - 0xffff */
        mmc1->prgBank = value & 0xf;
        bankSwitch(state);
        return 1;
    }
    return 1;
}

void ninPrgWriteHandlerMMC1(NinState* state, uint16_t addr, uint8_t value)
{
    NinMapperRegsMMC1* mmc1 = &state->mapperRegs.mmc1;
    uint8_t shift;

    if (value & 0x80)
    {
        /* Reset the internal registers */
        mmc1->count = 0;
        mmc1->shift = 0;
        mmc1->prgBankMode = 3;
        bankSwitch(state);
        return;
    }

    shift = (((value & 1) << 4) | (mmc1->shift >> 1));
    if (mmc1->count < 4)
    {
        mmc1->shift = shift;
        mmc1->count++;
        return;
    }
    else
    {
        mmc1->shift = 0;
        mmc1->count = 0;
        writeReg(state, addr, shift);
    }

}

#endif
