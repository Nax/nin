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
#include <libnin/Mapper/MMC1.h>
#include <libnin/Util.h>

using namespace libnin;

MapperMMC1::MapperMMC1(Memory& memory, Cart& cart, IRQ& irq)
: Mapper{memory, cart, irq}
, _shift{}
, _count{}
, _mirroring{}
, _prgBankMode{}
, _chrBankMode{}
, _chrBank0{}
, _chrBank1{}
, _prgBank{}
{

}

void MapperMMC1::handleWrite(std::uint16_t addr, std::uint8_t value)
{
	std::uint8_t shift;

	if (!(addr & 0x8000))
		return;

	if (value & 0x80)
	{
		/* Reset the internal registers */
		_count = 0;
		_shift = 0;
		_prgBankMode = 3;
		mapperBankPrg();
		return;
	}

	shift = (((value & 1) << 4) | (_shift >> 1));
	if (_count < 4)
	{
		_shift = shift;
		_count++;
		return;
	}

	/* Serial write completed, apply the effect */
	_shift = 0;
	_count = 0;
	mapperRegWrite(addr, shift);
}

void MapperMMC1::mapperRegWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr & 0xe000)
    {
    case 0x8000:
        /* 0x8000 - 0x9fff */
        _mirroring = value & 0x03;
        _prgBankMode = (value >> 2) & 0x03;
        _chrBankMode = (value >> 4) & 0x01;

        mapperMirror();
        mapperBankPrg();
        mapperBankChr();
        break;
    case 0xa000:
        /* 0xa000 - 0xbfff */
        _chrBank0 = value;
		mapperBankChr();
        break;
    case 0xc000:
        /* 0xc000 - 0xdfff */
        _chrBank1 = value;
		mapperBankChr();
        break;
    case 0xe000:
        /* 0xe000 - 0xffff */
        _prgBank = value & 0xf;
		mapperBankPrg();
        break;
    }
}

void MapperMMC1::mapperBankPrg()
{
    switch (_prgBankMode)
    {
    case 0:
    case 1:
        bankPrg16k(2, CART_PRG_ROM, (_prgBank & 0xfe) | 0);
        bankPrg16k(4, CART_PRG_ROM, (_prgBank & 0xfe) | 1);
        break;
    case 2:
        bankPrg16k(2, CART_PRG_ROM, 0);
        bankPrg16k(4, CART_PRG_ROM, _prgBank & 0xff);
        break;
    case 3:
        bankPrg16k(2, CART_PRG_ROM, _prgBank & 0xff);
        bankPrg16k(4, CART_PRG_ROM, -1);
        break;
    }
}

void MapperMMC1::mapperBankChr()
{
    if (_chrBankMode)
    {
        bankChr4k(0, _chrBank0);
        bankChr4k(1, _chrBank1);
    }
    else
    {
        bankChr4k(0, _chrBank0 & ~(0x01));
        bankChr4k(1, _chrBank0 | 0x01);
    }
}

void MapperMMC1::mapperMirror()
{
    int mirrorMode;

    switch (_mirroring)
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
