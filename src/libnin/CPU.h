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

#ifndef LIBNIN_CPU_H
#define LIBNIN_CPU_H 1

#include <cstdint>
#include <cstddef>

#define REG_A 0x00
#define REG_X 0x01
#define REG_Y 0x02
#define REG_S 0x03

#define PFLAG_C 0x01
#define PFLAG_Z 0x02
#define PFLAG_I 0x04
#define PFLAG_D 0x08
#define PFLAG_B 0x10
#define PFLAG_1 0x20
#define PFLAG_V 0x40
#define PFLAG_N 0x80

#define PFLAG_MASK (~(PFLAG_B | PFLAG_1))

namespace libnin
{

class CPU
{
public:
    CPU();

    std::size_t   cycle;
    std::uint16_t pc;
    union
    {
        std::uint8_t regs[4];
        struct
        {
            std::uint8_t a;
            std::uint8_t x;
            std::uint8_t y;
            std::uint8_t s;
        };
    };
    std::uint8_t p;
    std::uint8_t p2;
};

}

#endif
