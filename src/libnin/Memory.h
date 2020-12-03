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

#ifndef LIBNIN_MEMORY_H
#define LIBNIN_MEMORY_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class Memory : private NonCopyable
{
public:
    Memory();

    std::uint8_t ram[0x800];
    std::uint8_t vram[0x1000];
    std::uint8_t exvram[0x800];
    std::uint8_t palettes[0x20];
    std::uint8_t oam[0x100];
};

} // namespace libnin

#endif
