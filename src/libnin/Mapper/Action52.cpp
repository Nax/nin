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

template <>
void Mapper::handleReset<MapperID::Action52>()
{
    bankPrg32k(2, CART_PRG_ROM, 0);
    bankChr8k(0);
}

template <>
void Mapper::handleWrite<MapperID::Action52>(std::uint16_t addr, std::uint8_t value)
{
    std::uint16_t prgBank;
    std::uint16_t chrBank;

    if (addr >= 0x8000)
    {
        chrBank = ((addr & 0x000f) << 2) | (value & 0x03);
        prgBank = (addr >> 6) & 0x1f;
        switch ((addr >> 0xb) & 0x3)
        {
        case 0:
            prgBank += 0;
            break;
        case 1:
            prgBank += 32;
            break;
        case 2:
            break;
        case 3:
            prgBank += 64;
        }

        bankChr8k(chrBank);
        if (addr & 0x20)
        {
            bankPrg16k(2, CART_PRG_ROM, prgBank);
            bankPrg16k(4, CART_PRG_ROM, prgBank);
        }
        else
        {
            bankPrg16k(2, CART_PRG_ROM, (prgBank & 0xfffe) | 0);
            bankPrg16k(4, CART_PRG_ROM, (prgBank & 0xfffe) | 1);
        }
        mirror(addr & 0x2000 ? NIN_MIRROR_V : NIN_MIRROR_H);
    }
}

template <>
void Mapper::init<MapperID::Action52>()
{
    _handleReset = &Mapper::handleReset<MapperID::Action52>;
    _handleWrite = &Mapper::handleWrite<MapperID::Action52>;
}

}
