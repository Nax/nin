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

#ifndef LIBNIN_STATE_H
#define LIBNIN_STATE_H 1

#include <cstdlib>
#include <libnin/APU.h>
#include <libnin/Audio.h>
#include <libnin/BusMain.h>
#include <libnin/BusVideo.h>
#include <libnin/CPU.h>
#include <libnin/Cart.h>
#include <libnin/Disk.h>
#include <libnin/HardwareInfo.h>
#include <libnin/IRQ.h>
#include <libnin/Input.h>
#include <libnin/Mapper.h>
#include <libnin/MapperVariant.h>
#include <libnin/Memory.h>
#include <libnin/NMI.h>
#include <libnin/NonCopyable.h>
#include <libnin/PPU.h>
#include <libnin/Save.h>
#include <libnin/Util.h>
#include <libnin/Video.h>
#include <nin/nin.h>

#define RAM_SIZE  0x800
#define VRAM_SIZE 0x800

#define BITMAP_X 256
#define BITMAP_Y 240

#define DEBUG_LEVEL 0

#define MIRRORING_HORIZONTAL 0
#define MIRRORING_VERTICAL   1
#define MIRRORING_QUAD       2

#define CLOCK_RATE_NTSC 1789773

namespace libnin
{

class State : private NonCopyable
{
public:
    State();
    ~State();

    static State* create(NinError& err, const char* path);

    Memory       memory;
    HardwareInfo info;
    Cart         cart;
    Disk         disk;
    Save         save;
    Input        input;
    IRQ          irq;
    NMI          nmi;
    Video        video;
    union
    {
        Mapper        mapper;
        MapperVariant mapperVariant;
    };

    BusVideo busVideo;
    Audio    audio;
    APU      apu;
    PPU      ppu;
    BusMain  busMain;
    CPU      cpu;
};

} // namespace libnin

/* Expose the state to the world */
struct NinState : public libnin::State
{
};

#endif
