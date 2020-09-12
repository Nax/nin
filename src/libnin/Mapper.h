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

#ifndef LIBNIN_MAPPER_H
#define LIBNIN_MAPPER_H 1

#include <cstdint>
#include <libnin/Mapper/DiskSystem.h>
#include <libnin/Mapper/MMC1.h>
#include <libnin/Mapper/MMC2.h>
#include <libnin/Mapper/MMC3.h>
#include <libnin/Mapper/MMC5.h>
#include <libnin/MapperID.h>
#include <libnin/NonCopyable.h>
#include <nin/nin.h>

#define NIN_MIRROR_A 0
#define NIN_MIRROR_B 1
#define NIN_MIRROR_H 2
#define NIN_MIRROR_V 3

namespace libnin
{

class Memory;
class Cart;
class Disk;
class IRQ;
class Mapper : private NonCopyable
{
public:
    Mapper(Memory& memory, Cart& cart, Disk& disk, IRQ& irq);

    NinError configure(int mapper, int submapper);

    std::uint8_t*       bank(int slot) { return _prg[slot]; }
    const std::uint8_t* bank(int slot) const { return _prg[slot]; }
    std::uint8_t*       chr(int slot) { return _chr[slot]; }
    const std::uint8_t* chr(int slot) const { return _chr[slot]; }

    void reset() { (this->*_handleReset)(); }
    void tick() { (this->*_handleTick)(); }

    std::uint8_t read(std::uint16_t addr);
    void         write(std::uint16_t addr, std::uint8_t value);
    std::uint8_t ntRead(int bank, std::uint16_t addr) { return (this->*_handleNtRead)(bank, addr); }
    void         ntWrite(int bank, std::uint16_t addr, std::uint8_t value) { (this->*_handleNtWrite)(bank, addr, value); }
    std::uint8_t chrRead(int bank, std::uint16_t addr) { return (this->*_handleChrRead)(bank, addr); }
    void         chrWrite(int bank, std::uint16_t addr, std::uint8_t value) { (this->*_handleChrWrite)(bank, addr, value); }

    void videoRead(std::uint16_t addr) { (this->*_handleVideoRead)(addr); }
    void writePassive(std::uint16_t addr, std::uint8_t value) { (this->*_handleWrite)(addr, value); }

    void mirror(int mirrorMode);

    void bankPrg8k(std::uint8_t slot, int domain, std::int16_t bank);
    void bankPrg16k(std::uint8_t slot, int domain, std::int16_t bank);
    void bankPrg32k(std::uint8_t slot, int domain, std::int16_t bank);

    void bankChr1k(std::uint8_t slot, std::int16_t bank);
    void bankChr2k(std::uint8_t slot, std::int16_t bank);
    void bankChr4k(std::uint8_t slot, std::int16_t bank);
    void bankChr8k(std::int16_t bank);

private:
    using HandlerReset     = void (Mapper::*)(void);
    using HandlerTick      = void (Mapper::*)(void);
    using HandlerRead      = std::uint8_t (Mapper::*)(std::uint16_t);
    using HandlerWrite     = void (Mapper::*)(std::uint16_t, std::uint8_t);
    using HandlerVideoRead = void (Mapper::*)(std::uint16_t);
    using HandlerNtRead    = std::uint8_t (Mapper::*)(int, std::uint16_t);
    using HandlerNtWrite   = void (Mapper::*)(int, std::uint16_t, std::uint8_t);
    using HandlerChrRead   = std::uint8_t (Mapper::*)(int, std::uint16_t);
    using HandlerChrWrite  = void (Mapper::*)(int, std::uint16_t, std::uint8_t);

    template <MapperID> void         initMatching(MapperID mapper);
    template <MapperID> void         init();
    template <MapperID> void         handleReset();
    template <MapperID> void         handleTick();
    template <MapperID> std::uint8_t handleRead(std::uint16_t addr);
    template <MapperID> void         handleWrite(std::uint16_t addr, std::uint8_t value);
    template <MapperID> void         handleVideoRead(std::uint16_t addr);
    template <MapperID> std::uint8_t handleNtRead(int nametable, std::uint16_t offset);
    template <MapperID> void         handleNtWrite(int nametable, std::uint16_t offset, std::uint8_t value);
    template <MapperID> std::uint8_t handleChrRead(int bank, std::uint16_t offset);
    template <MapperID> void         handleChrWrite(int bank, std::uint16_t offset, std::uint8_t value);

    Memory& _memory;
    Cart&   _cart;
    Disk&   _disk;
    IRQ&    _irq;

    HandlerReset     _handleReset;
    HandlerTick      _handleTick;
    HandlerRead      _handleRead;
    HandlerWrite     _handleWrite;
    HandlerVideoRead _handleVideoRead;
    HandlerNtRead    _handleNtRead;
    HandlerNtWrite   _handleNtWrite;
    HandlerChrRead   _handleChrRead;
    HandlerChrWrite  _handleChrWrite;

    std::uint8_t* _prg[6];
    bool          _prgWriteFlag[6];
    std::uint8_t* _chr[8];
    std::uint8_t* _nametables[4];

    union
    {
        MapperMMC1       _mmc1;
        MapperMMC2       _mmc2;
        MapperMMC3       _mmc3;
        MapperMMC5       _mmc5;
        MapperDiskSystem _diskSystem;
    };
};

} // namespace libnin

#endif
