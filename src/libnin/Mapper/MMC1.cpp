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
