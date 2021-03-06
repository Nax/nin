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

#ifndef LIBNIN_MAPPER_ID_H
#define LIBNIN_MAPPER_ID_H

namespace libnin
{

enum class MapperID
{
    Bad     = -2,
    Unknown = -1,
    NROM    = 0,
    MMC1,
    MMC2,
    MMC3,
    MMC3_Multi37,
    MMC3_Multi47,
    MMC4,
    MMC5,
    AxROM,
    AxROM_Conflicts,
    CNROM,
    CPROM,
    DxROM,
    GxROM,
    UxROM,
    UxROM_NoConflicts,
    UxROM_UN1ROM,
    UxROM_UNROM180,
    ColorDreams,
    Action52,
    Mapper15,
    FDS
};

MapperID getMapperID(int mapper, int submapper);

} // namespace libnin

#endif
