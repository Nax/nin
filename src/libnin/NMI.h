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

#ifndef LIBNIN_NMI_H
#define LIBNIN_NMI_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

#define NMI_OCCURED 0x01
#define NMI_OUTPUT  0x02

namespace libnin
{

class NMI : private NonCopyable
{
public:
    NMI()
    : _status{}
    , _latch{}
    {
    }

    bool high() const { return _latch; }
    bool check(std::uint8_t flag) const { return !!(_status & flag); }

    void set(std::uint8_t flag)
    {
        std::uint8_t prev;

        prev = _status;
        _status |= flag;

        if ((prev != _status) && (_status == (NMI_OCCURED | NMI_OUTPUT)))
            _latch = true;
    }

    void unset(std::uint8_t flag)
    {
        _status &= ~flag;
    }

    void ack()
    {
        _latch = false;
    }

private:
    std::uint8_t _status : 2;
    bool         _latch : 1;
};

}; // namespace libnin

#endif
