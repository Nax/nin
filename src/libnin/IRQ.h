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

#ifndef LIBNIN_IRQ_H
#define LIBNIN_IRQ_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

#define IRQ_APU_FRAME 0x01
#define IRQ_APU_DMC   0x02
#define IRQ_MAPPER1   0x04
#define IRQ_MAPPER2   0x08

namespace libnin
{

class IRQ : private NonCopyable
{
public:
    IRQ()
    : _irq(0)
    {
    }

    bool high() const { return !!_irq; }
    bool check(std::uint8_t flag) const { return !!(_irq & flag); }

    void set(std::uint8_t flag) { _irq |= flag; }
    void unset(std::uint8_t flag) { _irq &= ~flag; }

private:
    std::uint8_t _irq;
};

}; // namespace libnin

#endif
