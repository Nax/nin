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

template <bool conflicts, int bank, int shift>
static void applyUxROM(Mapper& mapper, std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        if (conflicts)
        {
            value &= mapper.bank(((addr - 0x8000) / 0x2000) + 2)[addr & 0x1fff];
        }
        mapper.bankPrg16k(bank, CART_PRG_ROM, value >> shift);
    }
}

#define X(mapper, conflicts, bank, shift)                                                                                                                      \
    template <> void Mapper::handleWrite<MapperID::mapper>(std::uint16_t addr, std::uint8_t value) { applyUxROM<conflicts, bank, shift>(*this, addr, value); } \
    template <> void Mapper::init<MapperID::mapper>() { _handleWrite = &Mapper::handleWrite<MapperID::mapper>; }

X(UxROM, true, 2, 0);
X(UxROM_NoConflicts, false, 2, 0);
X(UxROM_UN1ROM, true, 2, 2);
X(UxROM_UNROM180, true, 4, 0);

}
