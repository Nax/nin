/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
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

#ifndef LIBNIN_MAPPER_MMC2_H
#define LIBNIN_MAPPER_MMC2_H 1

#include <cstdint>
#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC2 : public Mapper
{
public:
    void handleInit();
    void handleWrite(std::uint16_t addr, std::uint8_t value);
    void handleVideoRead(std::uint16_t addr);

private:
    void applyChr();

    std::uint8_t _bankLatch0Lo;
    std::uint8_t _bankLatch0Hi;
    std::uint8_t _bankLatch1Lo;
    std::uint8_t _bankLatch1Hi;
    std::uint8_t _latch0 : 1;
    std::uint8_t _latch1 : 1;
};

} // namespace libnin

#endif
