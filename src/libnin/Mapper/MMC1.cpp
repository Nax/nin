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

#include <libnin/Mapper.h>
#include <libnin/Util.h>

using namespace libnin;

void Mapper::mmc1RegWrite(std::uint16_t addr, std::uint8_t value)
{
    switch ((addr & 0x7fff) >> 13)
    {
    case 0:
        /* 0x8000 - 0x9fff */
        _mmc1.mirroring = value & 0x03;
        _mmc1.prgBankMode = (value >> 2) & 0x03;
        _mmc1.chrBankMode = (value >> 4) & 0x01;
        mmc1BankPrg();
        mmc1BankPrg();
        mmc1Mirror();
        break;
    case 1:
        /* 0xa000 - 0xbfff */
        _mmc1.chrBank0 = value;
        mmc1BankChr();
        break;
    case 2:
        /* 0xc000 - 0xdfff */
        _mmc1.chrBank1 = value;
        mmc1BankChr();
        break;
    case 3:
        /* 0xe000 - 0xffff */
        _mmc1.prgBank = value & 0xf;
        mmc1BankPrg();
        break;
    }
}

void Mapper::mmc1BankPrg()
{
    switch (_mmc1.prgBankMode)
    {
    case 0:
    case 1:
        bankPrg16k(0, (_mmc1.prgBank & 0xfe) | 0);
        bankPrg16k(1, (_mmc1.prgBank & 0xfe) | 1);
        break;
    case 2:
        bankPrg16k(0, 0);
        bankPrg16k(1, _mmc1.prgBank & 0xff);
        break;
    case 3:
        bankPrg16k(0, _mmc1.prgBank & 0xff);
        bankPrg16k(1, -1);
        break;
    }
}

void Mapper::mmc1BankChr()
{
    if (_mmc1.chrBankMode)
    {
        bankChr4k(0, _mmc1.chrBank0);
        bankChr4k(1, _mmc1.chrBank1);
    }
    else
    {
        bankChr4k(0, _mmc1.chrBank0 & ~(0x01));
        bankChr4k(1, _mmc1.chrBank0 | 0x01);
    }
}

void Mapper::mmc1Mirror()
{
    int mirrorMode;

    switch (_mmc1.mirroring)
    {
    case 0x00:
        mirrorMode = NIN_MIRROR_A;
        break;
    case 0x01:
        mirrorMode = NIN_MIRROR_B;
        break;
    case 0x02:
        mirrorMode = NIN_MIRROR_H;
        break;
    case 0x03:
        mirrorMode = NIN_MIRROR_V;
        break;
    }
    mirror(mirrorMode);
}

void Mapper::write_MMC1(std::uint16_t addr, std::uint8_t value)
{
    std::uint8_t shift;

    if (value & 0x80)
    {
        /* Reset the internal registers */
        _mmc1.count = 0;
        _mmc1.shift = 0;
        _mmc1.prgBankMode = 3;
        mmc1BankPrg();
        return;
    }

    shift = (((value & 1) << 4) | (_mmc1.shift >> 1));
    if (_mmc1.count < 4)
    {
        _mmc1.shift = shift;
        _mmc1.count++;
        return;
    }

    /* Serial write completed, apply the effect */
    _mmc1.shift = 0;
    _mmc1.count = 0;
    mmc1RegWrite(addr, shift);
}
