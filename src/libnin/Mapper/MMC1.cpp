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

#include <libnin/Cart.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

namespace libnin
{

static void mapperBankPrg(Mapper& mapper, MapperMMC1& mmc1)
{
    switch (mmc1.prgBankMode)
    {
    case 0:
    case 1:
        mapper.bankPrg16k(2, CART_PRG_ROM, (mmc1.prgBank & 0xfe) | 0);
        mapper.bankPrg16k(4, CART_PRG_ROM, (mmc1.prgBank & 0xfe) | 1);
        break;
    case 2:
        mapper.bankPrg16k(2, CART_PRG_ROM, 0);
        mapper.bankPrg16k(4, CART_PRG_ROM, mmc1.prgBank & 0xff);
        break;
    case 3:
        mapper.bankPrg16k(2, CART_PRG_ROM, mmc1.prgBank & 0xff);
        mapper.bankPrg16k(4, CART_PRG_ROM, -1);
        break;
    }
}

static void mapperBankChr(Mapper& mapper, MapperMMC1& mmc1)
{
    if (mmc1.chrBankMode)
    {
        mapper.bankChr4k(0, mmc1.chrBank0);
        mapper.bankChr4k(1, mmc1.chrBank1);
    }
    else
    {
        mapper.bankChr4k(0, mmc1.chrBank0 & ~(0x01));
        mapper.bankChr4k(1, mmc1.chrBank0 | 0x01);
    }
}

static void mapperMirror(Mapper& mapper, MapperMMC1& mmc1)
{
    int mirrorMode;

    switch (mmc1.mirroring)
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
    default:
        UNREACHABLE();
    }
    mapper.mirror(mirrorMode);
}

static void mapperRegWrite(Mapper& mapper, MapperMMC1& mmc1, std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe000)
    {
    case 0x8000:
        /* 0x8000 - 0x9fff */
        mmc1.mirroring   = value & 0x03;
        mmc1.prgBankMode = (value >> 2) & 0x03;
        mmc1.chrBankMode = (value >> 4) & 0x01;

        mapperMirror(mapper, mmc1);
        mapperBankPrg(mapper, mmc1);
        mapperBankChr(mapper, mmc1);
        break;
    case 0xa000:
        /* 0xa000 - 0xbfff */
        mmc1.chrBank0 = value;
        mapperBankChr(mapper, mmc1);
        break;
    case 0xc000:
        /* 0xc000 - 0xdfff */
        mmc1.chrBank1 = value;
        mapperBankChr(mapper, mmc1);
        break;
    case 0xe000:
        /* 0xe000 - 0xffff */
        mmc1.prgBank = value & 0xf;
        mapperBankPrg(mapper, mmc1);
        break;
    }
}

template <>
void Mapper::handleReset<MapperID::MMC1>()
{
    _mmc1 = MapperMMC1{};
}

template <>
void Mapper::handleWrite<MapperID::MMC1>(std::uint16_t addr, std::uint8_t value)
{
    std::uint8_t shift;

    if (!(addr & 0x8000))
        return;

    if (value & 0x80)
    {
        /* Reset the internal registers */
        _mmc1.count       = 0;
        _mmc1.shift       = 0;
        _mmc1.prgBankMode = 3;
        mapperBankPrg(*this, _mmc1);
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
    mapperRegWrite(*this, _mmc1, addr, shift);
}

template <>
void Mapper::init<MapperID::MMC1>()
{
    _handleReset = &Mapper::handleReset<MapperID::MMC1>;
    _handleWrite = &Mapper::handleWrite<MapperID::MMC1>;
}

}
