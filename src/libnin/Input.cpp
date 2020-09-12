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

#include <libnin/Input.h>

using namespace libnin;

Input::Input()
: _polling{}
, _state{}
, _controller{}
, _controllerLatch{}
{
}

void Input::set(std::uint8_t value)
{
    _state = value;
}

void Input::poll(bool polling)
{
    _polling = polling;
    if (_polling)
        reset();
}

std::uint8_t Input::read()
{
    if (_polling)
        reset();
    if (_controllerLatch < 8)
        return ((_controller >> _controllerLatch++) & 0x01);
    return 0x01;
}

void Input::reset()
{
    _controller      = _state;
    _controllerLatch = 0;
}
