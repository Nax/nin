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

namespace libnin
{

struct MapperMMC3
{
    std::uint8_t bankSelect : 3;
    std::uint8_t bank[8];
    std::uint8_t bankModePrgRom : 1;
    std::uint8_t bankModeChrRom : 1;

    std::uint8_t  irqScanlineEnabled : 1;
    std::uint8_t  irqScanlineReload : 1;
    std::uint8_t  irqScanlineCounter;
    std::uint8_t  irqScanlineReloadValue;
    std::uint16_t irqScanlineFilterShifter;
    std::uint16_t oldVmemAddr;
};

} // namespace libnin

#endif
