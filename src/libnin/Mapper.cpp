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

#include <libnin/Cart.h>
#include <libnin/Mapper.h>
#include <libnin/Mapper/AXROM.h>
#include <libnin/Mapper/Action52.h>
#include <libnin/Mapper/CNROM.h>
#include <libnin/Mapper/CPROM.h>
#include <libnin/Mapper/ColorDreams.h>
#include <libnin/Mapper/DXROM.h>
#include <libnin/Mapper/DiskSystem.h>
#include <libnin/Mapper/Mapper15.h>
#include <libnin/Mapper/GXROM.h>
#include <libnin/Mapper/MMC1.h>
#include <libnin/Mapper/MMC2.h>
#include <libnin/Mapper/MMC3.h>
#include <libnin/Mapper/MMC4.h>
#include <libnin/Mapper/MMC5.h>
#include <libnin/Mapper/UXROM.h>
#include <libnin/Memory.h>
#include <libnin/Util.h>

#define CONFIGURE_HANDLERS(T)                                                                                                                             \
    {                                                                                                                                                     \
        _handlerInit      = [](Mapper* m) { ((T*)m)->handleInit(); };                                                                                     \
        _handlerTick      = [](Mapper* m) { ((T*)m)->handleTick(); };                                                                                     \
        _handlerRead      = [](Mapper* m, std::uint16_t addr) { return ((T*)m)->handleRead(addr); };                                                      \
        _handlerWrite     = [](Mapper* m, std::uint16_t addr, std::uint8_t value) { ((T*)m)->handleWrite(addr, value); };                                 \
        _handlerVideoRead = [](Mapper* m, std::uint16_t addr) { ((T*)m)->handleVideoRead(addr); };                                                        \
        _handlerNtRead    = [](Mapper* m, int nametable, std::uint16_t offset) { return ((T*)m)->handleNtRead(nametable, offset); };                      \
        _handlerNtWrite   = [](Mapper* m, int nametable, std::uint16_t offset, std::uint8_t value) { ((T*)m)->handleNtWrite(nametable, offset, value); }; \
        _handlerChrRead   = [](Mapper* m, int bank, std::uint16_t offset) { return ((T*)m)->handleChrRead(bank, offset); };                               \
        _handlerChrWrite  = [](Mapper* m, int bank, std::uint16_t offset, std::uint8_t value) { ((T*)m)->handleChrWrite(bank, offset, value); };          \
    }

#define OVERRIDE_HANDLER_INIT(T, name)  _handlerInit = [](Mapper* m) { ((T*)m)->name(); }
#define OVERRIDE_HANDLER_WRITE(T, name) _handlerWrite = [](Mapper* m, std::uint16_t addr, std::uint8_t value) { ((T*)m)->name(addr, value); }

namespace libnin
{

Mapper::Mapper(Memory& memory, Cart& cart, Disk& disk, IRQ& irq)
: _memory{memory}
, _cart{cart}
, _disk{disk}
, _irq{irq}
, _mirrorMode{}
, _prg{}
, _prgWriteFlag{}
, _chr{}
, _nametables{}
{
}

NinError Mapper::configure(int mapper, int submapper)
{
    MapperID mapperID;

    mapperID = getMapperID(mapper, submapper);
    switch (mapperID)
    {
    case MapperID::Bad:
        return NIN_ERROR_BAD_MAPPER;
    case MapperID::Unknown:
        return NIN_ERROR_UNKNOWN_MAPPER;
    default:
        break;
    }

    mirror(NIN_MIRROR_H);
    bankChr8k(0);
    bankPrg8k(1, CART_PRG_RAM, 0);
    bankPrg8k(2, CART_PRG_ROM, 0);
    bankPrg8k(3, CART_PRG_ROM, 1);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);

    CONFIGURE_HANDLERS(Mapper);

    switch (mapperID)
    {
    case MapperID::NROM:
        break;
    case MapperID::AxROM:
    case MapperID::AxROM_Conflicts:
        CONFIGURE_HANDLERS(MapperAXROM);
        break;
    case MapperID::CNROM:
        CONFIGURE_HANDLERS(MapperCNROM);
        break;
    case MapperID::CPROM:
        CONFIGURE_HANDLERS(MapperCPROM);
        break;
    case MapperID::DxROM:
        CONFIGURE_HANDLERS(MapperDXROM);
        break;
    case MapperID::GxROM:
        CONFIGURE_HANDLERS(MapperGXROM);
        break;
    case MapperID::UxROM:
    case MapperID::UxROM_NoConflicts:
    case MapperID::UxROM_UN1ROM:
    case MapperID::UxROM_UNROM180:
        CONFIGURE_HANDLERS(MapperUXROM);
        break;
    case MapperID::MMC1:
        CONFIGURE_HANDLERS(MapperMMC1);
        break;
    case MapperID::MMC2:
        CONFIGURE_HANDLERS(MapperMMC2);
        break;
    case MapperID::MMC3:
    case MapperID::MMC3_Multi37:
    case MapperID::MMC3_Multi47:
        CONFIGURE_HANDLERS(MapperMMC3);
        break;
    case MapperID::MMC4:
        CONFIGURE_HANDLERS(MapperMMC4);
        break;
    case MapperID::MMC5:
        CONFIGURE_HANDLERS(MapperMMC5);
        break;
    case MapperID::Action52:
        CONFIGURE_HANDLERS(MapperAction52);
        break;
    case MapperID::ColorDreams:
        CONFIGURE_HANDLERS(MapperColorDreams);
        break;
    case MapperID::Mapper15:
        CONFIGURE_HANDLERS(Mapper15);
        break;
    case MapperID::FDS:
        CONFIGURE_HANDLERS(MapperDiskSystem);
        break;
    }

    /* Variants */
    switch (mapperID)
    {
    case MapperID::AxROM_Conflicts:
        OVERRIDE_HANDLER_WRITE(MapperAXROM, handleWrite_Conflicts);
        break;
    case MapperID::UxROM_NoConflicts:
        OVERRIDE_HANDLER_WRITE(MapperUXROM, handleWrite_NoConflicts);
        break;
    case MapperID::UxROM_UN1ROM:
        OVERRIDE_HANDLER_WRITE(MapperUXROM, handleWrite_UN1ROM);
        break;
    case MapperID::UxROM_UNROM180:
        OVERRIDE_HANDLER_WRITE(MapperUXROM, handleWrite_UNROM180);
        break;
    case MapperID::MMC3_Multi37:
        OVERRIDE_HANDLER_INIT(MapperMMC3, handleInit_Multi37);
        OVERRIDE_HANDLER_WRITE(MapperMMC3, handleWrite_Multi37);
        break;
    case MapperID::MMC3_Multi47:
        OVERRIDE_HANDLER_INIT(MapperMMC3, handleInit_Multi47);
        OVERRIDE_HANDLER_WRITE(MapperMMC3, handleWrite_Multi47);
        break;
    default:
        break;
    }

    return NIN_OK;
}

std::uint8_t Mapper::read(std::uint16_t addr)
{
    std::uint8_t value = _handlerRead(this, addr);
    int          slot  = ((addr - 0x4000) / 0x2000);

    return _prg[slot] ? _prg[slot][addr & 0x1fff] : value;
}

void Mapper::write(std::uint16_t addr, std::uint8_t value)
{
    int slot = ((addr - 0x4000) / 0x2000);

    if (_prg[slot] && _prgWriteFlag[slot])
    {
        _prg[slot][addr & 0x1fff] = value;
    }
    _handlerWrite(this, addr, value);
}

void Mapper::mirror(int mirrorMode)
{
    _mirrorMode = mirrorMode;

    switch (mirrorMode)
    {
    case NIN_MIRROR_A:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x000;
        _nametables[2] = _memory.vram + 0x000;
        _nametables[3] = _memory.vram + 0x000;
        break;
    case NIN_MIRROR_B:
        _nametables[0] = _memory.vram + 0x400;
        _nametables[1] = _memory.vram + 0x400;
        _nametables[2] = _memory.vram + 0x400;
        _nametables[3] = _memory.vram + 0x400;
        break;
    case NIN_MIRROR_H:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x400;
        _nametables[2] = _memory.vram + 0x000;
        _nametables[3] = _memory.vram + 0x400;
        break;
    case NIN_MIRROR_V:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x000;
        _nametables[2] = _memory.vram + 0x400;
        _nametables[3] = _memory.vram + 0x400;
        break;
    case NIN_MIRROR_4:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x400;
        _nametables[2] = _memory.vram + 0x800;
        _nametables[3] = _memory.vram + 0xc00;
        break;
    }
}

void Mapper::bankPrg8k(std::uint8_t slot, int domain, std::int16_t bank)
{
    const CartSegment& seg = _cart.segment(domain);
    bank += seg.bankCount;
    if (seg.base)
    {
        _prg[slot]          = seg.base + std::uintptr_t(std::uint16_t(bank) % seg.bankCount) * 0x2000;
        _prgWriteFlag[slot] = (domain == CART_PRG_RAM);
    }
    else
    {
        _prg[slot]          = nullptr;
        _prgWriteFlag[slot] = false;
    }
}

void Mapper::bankPrg16k(std::uint8_t slot, int domain, std::int16_t bank)
{
    bankPrg8k(slot + 0, domain, bank * 2 + 0);
    bankPrg8k(slot + 1, domain, bank * 2 + 1);
}

void Mapper::bankPrg32k(std::uint8_t slot, int domain, std::int16_t bank)
{
    bankPrg8k(slot + 0, domain, bank * 4 + 0);
    bankPrg8k(slot + 1, domain, bank * 4 + 1);
    bankPrg8k(slot + 2, domain, bank * 4 + 2);
    bankPrg8k(slot + 3, domain, bank * 4 + 3);
}

void Mapper::bankChr1k(std::uint8_t slot, std::int16_t bank)
{
    const CartSegment& segRam = _cart.segment(CART_CHR_RAM);
    const CartSegment& segRom = _cart.segment(CART_CHR_ROM);

    if (segRam.base)
    {
        bank += segRam.bankCount;
        _chr[slot] = segRam.base + std::uintptr_t(bank % segRam.bankCount) * 0x400;
    }
    else
    {
        bank += segRom.bankCount;
        _chr[slot] = segRom.base + std::uintptr_t(bank % segRom.bankCount) * 0x400;
    }
}

void Mapper::bankChr2k(std::uint8_t slot, std::int16_t bank)
{
    bankChr1k(slot * 2 + 0, bank * 2 + 0);
    bankChr1k(slot * 2 + 1, bank * 2 + 1);
}

void Mapper::bankChr4k(std::uint8_t slot, std::int16_t bank)
{
    bankChr1k(slot * 4 + 0, bank * 4 + 0);
    bankChr1k(slot * 4 + 1, bank * 4 + 1);
    bankChr1k(slot * 4 + 2, bank * 4 + 2);
    bankChr1k(slot * 4 + 3, bank * 4 + 3);
}

void Mapper::bankChr8k(std::int16_t bank)
{
    bankChr1k(0, bank * 8 + 0);
    bankChr1k(1, bank * 8 + 1);
    bankChr1k(2, bank * 8 + 2);
    bankChr1k(3, bank * 8 + 3);
    bankChr1k(4, bank * 8 + 4);
    bankChr1k(5, bank * 8 + 5);
    bankChr1k(6, bank * 8 + 6);
    bankChr1k(7, bank * 8 + 7);
}

void Mapper::handleInit()
{
}

void Mapper::handleTick()
{
}

std::uint8_t Mapper::handleRead(std::uint16_t addr)
{
    UNUSED(addr);
    return 0x00;
}

void Mapper::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    UNUSED(addr);
    UNUSED(value);
}

void Mapper::handleVideoRead(std::uint16_t addr)
{
    UNUSED(addr);
}

std::uint8_t Mapper::handleNtRead(int nametable, std::uint16_t offset)
{
    return _nametables[nametable][offset];
}

void Mapper::handleNtWrite(int nametable, std::uint16_t offset, std::uint8_t value)
{
    _nametables[nametable][offset] = value;
}

std::uint8_t Mapper::handleChrRead(int bank, std::uint16_t offset)
{
    return _chr[bank][offset];
}

void Mapper::handleChrWrite(int bank, std::uint16_t offset, std::uint8_t value)
{
    if (_cart.segment(CART_CHR_RAM).base)
    {
        _chr[bank][offset] = value;
    }
}

} // namespace libnin
