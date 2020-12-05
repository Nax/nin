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

#include <libnin/MapperID.h>

using namespace libnin;

MapperID libnin::getMapperID(int mapper, int submapper)
{
    switch (mapper)
    {
    case 0:
        return MapperID::NROM;
    case 1:
        return MapperID::MMC1;
    case 2:
        if (submapper == 1)
            return MapperID::UxROM_NoConflicts;
        return MapperID::UxROM;
    case 3:
        return MapperID::CNROM;
    case 4:
        return MapperID::MMC3;
    case 5:
        return MapperID::MMC5;
    case 7:
        if (submapper == 2)
            return MapperID::AxROM_Conflicts;
        return MapperID::AxROM;
    case 9:
        return MapperID::MMC2;
    case 10:
        return MapperID::MMC4;
    case 11:
        return MapperID::ColorDreams;
    case 13:
        return MapperID::CPROM;
    case 15:
        return MapperID::Mapper15;
    case 20:
        return MapperID::FDS;
    case 37:
        return MapperID::MMC3_Multi37;
    case 47:
        return MapperID::MMC3_Multi47;
    case 66:
        return MapperID::GxROM;
    case 94:
        return MapperID::UxROM_UN1ROM;
    case 180:
        return MapperID::UxROM_UNROM180;
    case 206:
        return MapperID::DxROM;
    case 228:
        return MapperID::Action52;
    default:
        return MapperID::Unknown;
    }
}
