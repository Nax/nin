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

#ifndef LIBNIN_MAPPER_VARIANT_H
#define LIBNIN_MAPPER_VARIANT_H 1

#include <cstdint>
#include <libnin/Mapper/AXROM.h>
#include <libnin/Mapper/Action52.h>
#include <libnin/Mapper/CNROM.h>
#include <libnin/Mapper/ColorDreams.h>
#include <libnin/Mapper/DXROM.h>
#include <libnin/Mapper/DiskSystem.h>
#include <libnin/Mapper/GXROM.h>
#include <libnin/Mapper/MMC1.h>
#include <libnin/Mapper/MMC2.h>
#include <libnin/Mapper/MMC3.h>
#include <libnin/Mapper/MMC4.h>
#include <libnin/Mapper/MMC5.h>
#include <libnin/Mapper/UXROM.h>
#include <libnin/NonCopyable.h>
#include <nin/nin.h>

namespace libnin
{

union MapperVariant
{
public:
    MapperVariant(Memory& memory, Cart& cart, Disk& disk, IRQ& irq)
    : mapper{memory, cart, disk, irq}
    {
    }

    ~MapperVariant() {}

    Mapper            mapper;
    MapperAXROM       axrom;
    MapperCNROM       cnrom;
    MapperDXROM       dxrom;
    MapperGXROM       gxrom;
    MapperUXROM       uxrom;
    MapperMMC1        mmc1;
    MapperMMC2        mmc2;
    MapperMMC3        mmc3;
    MapperMMC4        mmc4;
    MapperMMC5        mmc5;
    MapperAction52    action52;
    MapperColorDreams colorDreams;
    MapperDiskSystem  diskSystem;
};

} // namespace libnin

#endif
