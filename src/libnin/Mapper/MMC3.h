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

#ifndef LIBNIN_MAPPER_MMC3_H
#define LIBNIN_MAPPER_MMC3_H 1

#include <cstdint>
#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC3 : public Mapper
{
public:
    void handleInit();
    void handleInit_Multi37();
    void handleInit_Multi47();
    void handleWrite(std::uint16_t addr, std::uint8_t value);
    void handleWrite_Multi37(std::uint16_t addr, std::uint8_t value);
    void handleWrite_Multi47(std::uint16_t addr, std::uint8_t value);
    void handleVideoRead(std::uint16_t addr);

private:
    void apply();
    void bankPrg(int slot, int index);
    void bankChr(int slot, int index);

    std::uint8_t _bankSelect : 3;
    std::uint8_t _bank[8];
    std::uint8_t _bankModePrgRom : 1;
    std::uint8_t _bankModeChrRom : 1;

    std::uint8_t  _irqScanlineEnabled : 1;
    std::uint8_t  _irqScanlineReload : 1;
    std::uint8_t  _irqScanlineCounter;
    std::uint8_t  _irqScanlineReloadValue;
    std::uint16_t _irqScanlineFilterShifter;
    std::uint16_t _oldVmemAddr;

    std::uint32_t _prgBankAnd;
    std::uint32_t _prgBankOr;
    std::uint32_t _chrBankAnd;
    std::uint32_t _chrBankOr;
};

} // namespace libnin

#endif
