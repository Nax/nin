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

#ifndef LIBNIN_MAPPER_MMC1_H
#define LIBNIN_MAPPER_MMC1_H 1

#include <cstdint>

namespace libnin
{

struct MapperMMC1
{
    std::uint8_t shift;
    std::uint8_t count;
    std::uint8_t mirroring : 2;
    std::uint8_t prgBankMode : 2;
    std::uint8_t chrBankMode : 1;
    std::uint8_t chrBank0 : 5;
    std::uint8_t chrBank1 : 5;
    std::uint8_t prgBank : 4;
};

} // namespace libnin

#endif
