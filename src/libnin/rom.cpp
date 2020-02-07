/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <libnin/libnin.h>

static const char kHeaderMagicNES[] = { 'N', 'E', 'S', '\x1a' };
static const char kHeaderMagicFDS[] = { 'F', 'D', 'S', '\x1a' };

static NinError ninLoadRomNES(NinState* state, const NinRomHeader* header, FILE* f);
static NinError ninLoadRomFDS(NinState* state, const NinRomHeader* header, FILE* f);


NIN_API NinError ninLoadRom(NinState* state, const char* path)
{
    FILE* f;
    NinRomHeader header;

    /* Open the ROM */
    f = fopen(path, "rb");
    if (!f)
        return NIN_ERROR_IO;

    /* Read the header */
    memset(&header, 0, sizeof(header));
    fread(&header, sizeof(header), 1, f);

    /* Check for the NES signature */
    if (memcmp(header.magic, kHeaderMagicNES, 4) == 0)
        return ninLoadRomNES(state, &header, f);

    /* Check for the iNES signature */
    if (memcmp(header.magic, kHeaderMagicFDS, 4) == 0)
        return ninLoadRomFDS(state, &header, f);

    fclose(f);
    return NIN_ERROR_BAD_FILE;
}

static NinError ninLoadRomNES(NinState* state, const NinRomHeader* header, FILE* f)
{
    int nes2;
    std::uint16_t prgRomBankCount;
    std::uint16_t prgRamBankCount;
    std::uint16_t chrRomBankCount;
    std::uint16_t chrRamBankCount;

    nes2 = 0;
    /* Check for the nes2 signature */
    if (header->magicNes2 == 0x2)
        nes2 = 1;

    /* Load the region */
    if (!nes2)
        state->info.setRegion(NIN_REGION_NTSC);
    else
        state->info.setRegion((NinRegion)header->nes2.region);

    /* Load the header misc. info */
    state->mapper = (header->mapperHi << 4) | header->mapperLo;
    if (header->quadScreen)
    {
        state->mirroring = MIRRORING_QUAD;
    }
    else if (header->mirroring)
    {
        state->mirroring = MIRRORING_VERTICAL;
    }
    else
    {
        state->mirroring = MIRRORING_HORIZONTAL;
    }

    state->battery = header->battery;
    state->trainerSize = header->trainer ? 0x200 : 0;

    prgRomBankCount = header->prgRomSize * 2;
    prgRamBankCount = 1;
    chrRomBankCount = header->chrRomSize * 8;
    chrRamBankCount = chrRomBankCount ? 0 : 8;

    /* Zero is an error for PRG but allowed for CHR */
    if (!prgRomBankCount)
    {
        fclose(f);
        return NIN_ERROR_BAD_FILE;
    }

    /* Allocate the various components */
    state->cart.load(CART_PRG_ROM, prgRomBankCount, f);
    state->cart.load(CART_PRG_RAM, prgRamBankCount, nullptr);
    state->cart.load(CART_CHR_ROM, chrRomBankCount, f);
    state->cart.load(CART_CHR_RAM, chrRamBankCount, nullptr);


    /* Check that the file was actually long enough */
    /*
    if ((unsigned long)ftell(f) < sizeof(NinRomHeader) + state->prgRomSize + state->chrRomSize + state->trainerSize)
    {
        fclose(f);
        return NIN_ERROR_BAD_FILE;
    }
    */

    /* We won't need the ROM from now on */
    fclose(f);

    /* Apply mirroring */
    if (state->mirroring == MIRRORING_VERTICAL)
    {
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x400;
        state->nametables[2] = state->vram + 0x000;
        state->nametables[3] = state->vram + 0x400;
    }
    else
    {
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x000;
        state->nametables[2] = state->vram + 0x400;
        state->nametables[3] = state->vram + 0x400;
    }

    /* Apply a default configuration suitable for most mappers */
    state->prgWriteHandler = &ninPrgWriteHandlerNull;
    state->ppuMonitorHandler = &ninPpuMonitorHandlerNull;
    state->readHandler = &ninMemoryReadNES;
    state->writeHandler = &ninMemoryWriteNES;

    ninBankSwitchPrgRom16k(state, 0, 0);
    ninBankSwitchPrgRom16k(state, 1, -1);
    ninBankSwitchChrRom8k(state, 0);

    /* Mapper-specific logic */
    switch (state->mapper)
    {
    default:
        return NIN_ERROR_UNKNOWN_MAPPER;
    case 0:
        /* NROM */
        break;
    case 1:
        /* MMC1 */
        state->prgWriteHandler = &ninPrgWriteHandlerMMC1;
        break;
    case 2:
        /* UxROM */
        state->prgWriteHandler = &ninPrgWriteHandlerUXROM;
        break;
    case 3:
        /* CNROM */
        state->prgWriteHandler = &ninPrgWriteHandlerCNROM;
        break;
    case 4:
        /* MMC3 */
        state->prgWriteHandler = &ninPrgWriteHandlerMMC3;
        state->ppuMonitorHandler = &ninPpuMonitorHandlerMMC3;
        state->mapperRegs.mmc3.bank[6] = 0;
        state->mapperRegs.mmc3.bank[7] = 1;
        break;
    case 7:
        /* AXROM */
        ninBankSwitchPrgRom8k(state, 1, 2);
        for (int i = 0; i < 4; ++i)
            state->nametables[i] = state->vram;
        state->prgWriteHandler = &ninPrgWriteHandlerAXROM;
        break;
    case 9:
        /* MMC2 */
        state->prgWriteHandler = &ninPrgWriteHandlerMMC2;
        state->ppuMonitorHandler = &ninPpuMonitorHandlerMMC2;
        ninBankSwitchPrgRom8k(state, 1, -3);
        ninBankSwitchPrgRom8k(state, 2, -2);
        ninBankSwitchPrgRom8k(state, 3, -1);
        break;
    case 10:
        /* MMC4 */
        state->prgWriteHandler = &ninPrgWriteHandlerMMC4;
        state->ppuMonitorHandler = &ninPpuMonitorHandlerMMC2;
        break;
    case 11:
        /* ColorDreams */
        state->prgWriteHandler = &ninPrgWriteHandlerColorDreams;
        break;
    case 66:
        /* GXROM */
        state->prgWriteHandler = &ninPrgWriteHandlerGXROM;
        break;
    case 180:
        /* UxROM (180) */
        state->prgWriteHandler = &ninPrgWriteHandlerUXROM_180;
        break;
    }

    return NIN_OK;
}

static NinError ninLoadRomFDS(NinState* state, const NinRomHeader* header, FILE* f)
{
    state->info.setSystem(NIN_SYSTEM_FDS);

    /* PRG ROM is the FDS BIOS */
    state->cart.load(CART_PRG_ROM, 1, nullptr);
    state->cart.load(CART_PRG_RAM, 4, nullptr);
    state->cart.load(CART_CHR_ROM, 0, nullptr);
    state->cart.load(CART_CHR_RAM, 8, nullptr);

    ninBankSwitchChrRom8k(state, 0);

    /* Load the disk */
    state->diskSystem.loadDisk(f);

    /* We won't need the ROM from now on */
    fclose(f);

    /* Apply mirroring */
    if (state->mirroring == MIRRORING_VERTICAL)
    {
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x400;
        state->nametables[2] = state->vram + 0x000;
        state->nametables[3] = state->vram + 0x400;
    }
    else
    {
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x000;
        state->nametables[2] = state->vram + 0x400;
        state->nametables[3] = state->vram + 0x400;
    }

    /* TODO: Add an FDS mapper */
    state->prgWriteHandler = &ninPrgWriteHandlerNull;
    state->ppuMonitorHandler = &ninPpuMonitorHandlerNull;
    state->readHandler = &ninMemoryReadFDS;
    state->writeHandler = &ninMemoryWriteFDS;

    return NIN_OK;
}

NIN_API void ninLoadBiosFDS(NinState* state, const char* path)
{
    FILE* f;

    f = fopen(path, "rb");
    if (!f)
        return;
    state->cart.load(CART_PRG_ROM, 1, f);
    fclose(f);
    state->cpu.pc = ninMemoryRead16(state, 0xfffc);
}
