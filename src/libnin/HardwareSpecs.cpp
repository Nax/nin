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

#include <libnin/HardwareSpecs.h>

using namespace libnin;

const HardwareSpecs HardwareSpecs::NTSC = {
    1789773,
    29781,
    16639267,
    0,
    0,
    20,
    29828,
    {7457, 14913, 22371, 29829, 37281},
    {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068},
    {214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27},
};

const HardwareSpecs HardwareSpecs::PAL = {
    1662607,
    33248,
    19997200,
    1,
    1,
    70,
    33251,
    {8312, 16626, 24938, 33252, 41560},
    {4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708, 944, 1890, 3778},
    {199, 177, 158, 149, 138, 118, 105, 99, 88, 74, 66, 59, 49, 39, 33, 25},
};
