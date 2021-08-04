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

#include <cstdio>
#include <cstring>
#include <libnin/RomHeader.h>
#include <libnin/State.h>
#include <libnin/Util.h>

using namespace libnin;

static const char kHeaderMagicNES[] = {'N', 'E', 'S', '\x1a'};
static const char kHeaderMagicFDS[] = {'F', 'D', 'S', '\x1a'};

static NinError loadRomNES(State& state, const RomHeader& header, std::FILE* f)
{
    NinError      err;
    bool          nes2{};
    std::uint16_t prgRomBankCount;
    std::uint16_t prgRamBankCount;
    std::uint16_t chrRomBankCount;
    std::uint16_t chrRamBankCount;
    int           mapper{};
    int           submapper{};

    /* Check for the nes2 signature */
    if (header.magicNes2 == 0x2)
        nes2 = true;

    /* Get the length of every segment */
    if (!nes2)
    {
        prgRomBankCount = header.prgRomSize * 2;
        prgRamBankCount = 1;
        chrRomBankCount = header.chrRomSize * 8;
        chrRamBankCount = chrRomBankCount ? 0 : 8;
    }
    else
    {
        prgRomBankCount = (header.prgRomSize + 256 * header.nes2.prgRomSizeHi) * 2;
        chrRomBankCount = (header.chrRomSize + 256 * header.nes2.chrRomSizeHi) * 8;
        prgRamBankCount = header.nes2.prgRamSizeShift ? (1 << (header.nes2.prgRamSizeShift - 7)) : 0;
        chrRamBankCount = header.nes2.chrRamSizeShift ? (1 << (header.nes2.chrRamSizeShift - 4)) : 0;
    }

    /* Zero is an error for PRG but allowed for CHR */
    if (!prgRomBankCount)
    {
        std::fclose(f);
        return NIN_ERROR_BAD_FILE;
    }

    /* Load data from the cart */
    state.cart.load(CART_PRG_ROM, prgRomBankCount, f);
    state.cart.load(CART_PRG_RAM, prgRamBankCount, nullptr);
    state.cart.load(CART_CHR_ROM, chrRomBankCount, f);
    state.cart.load(CART_CHR_RAM, chrRamBankCount, nullptr);

    /* We won't need the ROM from now on */
    std::fclose(f);

    /* Load the region */
    if (!nes2)
        state.info.setRegion(NIN_REGION_NTSC);
    else
        state.info.setRegion((NinRegion)header.nes2.region);

    /* Load the mapper */
    mapper = (header.mapperHi << 4) | header.mapperLo;
    if (nes2)
    {
        mapper |= ((std::uint16_t)header.nes2.mapperEx << 8);
        submapper = header.nes2.submapper;
    }

    if ((err = state.mapper.configure(mapper, submapper)) != NIN_OK)
        return err;

    /* Load the header misc. info */
    if (header.quadScreen)
    {
        state.mapper.mirror(NIN_MIRROR_4);
    }
    else if (header.mirroring)
        state.mapper.mirror(NIN_MIRROR_H);
    else
        state.mapper.mirror(NIN_MIRROR_V);
    state.mapper.reset();
    state.save.setBattery(!!header.battery);

    /* Check that the file was actually long enough */
    /*
    if ((unsigned long)ftell(f) < sizeof(NinRomHeader) + state->prgRomSize + state->chrRomSize + state->trainerSize)
    {
        fclose(f);
        return NIN_ERROR_BAD_FILE;
    }
    */

    return NIN_OK;
}

static NinError loadRomFDS(State& state, const std::uint8_t* header, std::FILE* f)
{
    int sideCount;

    state.info.setSystem(NIN_SYSTEM_FDS);
    state.info.setRegion(NIN_REGION_NTSC);

    /* Load the disk */
    if (header)
    {
        sideCount = header[4];
    }
    else
    {
        std::fseek(f, 0, SEEK_END);
        sideCount = std::ftell(f) / Disk::DiskSizeArchive;
    }
    state.disk.load(f, header ? 16 : 0, sideCount);

    /* We won't need the ROM from now on */
    std::fclose(f);

    /* PRG ROM is the FDS BIOS */
    state.cart.load(CART_PRG_ROM, 1, nullptr);
    state.cart.load(CART_PRG_RAM, 4, nullptr);
    state.cart.load(CART_CHR_ROM, 0, nullptr);
    state.cart.load(CART_CHR_RAM, 8, nullptr);

    state.mapper.configure(20, 0);
    state.mapper.mirror(NIN_MIRROR_H);
    state.mapper.reset();

    return NIN_OK;
}

static NinError loadRom(State& state, const char* path)
{
    std::FILE*   f;
    std::uint8_t headerRaw[16];
    RomHeader    header{};

    /* Open the ROM */
    f = fopen(path, "rb");
    if (!f)
        return NIN_ERROR_IO;

    /* Read the header */
    std::fread(headerRaw, 16, 1, f);
    std::memcpy(&header, headerRaw, sizeof(header));

    /* Check for the NES signature */
    if (std::memcmp(header.magic, kHeaderMagicNES, 4) == 0)
        return loadRomNES(state, header, f);

    /* Check for the iNES signature */
    if (std::memcmp(header.magic, kHeaderMagicFDS, 4) == 0)
        return loadRomFDS(state, headerRaw, f);
    if (std::memcmp(headerRaw, "\x01*NINTENDO-HVC*\x01", 16) == 0)
        return loadRomFDS(state, nullptr, f);

    std::fclose(f);
    return NIN_ERROR_BAD_FILE;
}

State::State()
: memory{}
, info{}
, cart{}
, disk{}
, save{cart}
, input{}
, irq{}
, nmi{}
, video{}
, mapper{memory, cart, disk, irq}
, busVideo{memory, cart, mapper}
, audio{info}
, apu{info, irq, mapper, audio}
, ppu{info, memory, nmi, busVideo, mapper, video}
, busMain{memory, cart, mapper, ppu, apu, input}
, interpreterCycle{memory, irq, nmi, ppu, apu, busMain}
{
}

State::~State() {}

State* State::create(NinError& err, const char* path)
{
    State* s = new State;

    err = loadRom(*s, path);
    if (err)
    {
        delete s;
        s = nullptr;
    }
    return s;
}
