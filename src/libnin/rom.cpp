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

    /* Get the length of every segment */
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

    /* Load data from the cart */
    state->cart.load(CART_PRG_ROM, prgRomBankCount, f);
    state->cart.load(CART_PRG_RAM, prgRamBankCount, nullptr);
    state->cart.load(CART_CHR_ROM, chrRomBankCount, f);
    state->cart.load(CART_CHR_RAM, chrRamBankCount, nullptr);

    /* We won't need the ROM from now on */
    fclose(f);

    /* Load the region */
    if (!nes2)
        state->info.setRegion(NIN_REGION_NTSC);
    else
        state->info.setRegion((NinRegion)header->nes2.region);

    /* Load the header misc. info */
    if (header->mirroring)
        state->mapper.mirror(NIN_MIRROR_H);
    else
        state->mapper.mirror(NIN_MIRROR_V);
    state->mapper.bankPrg16k(0, 0);
    state->mapper.bankPrg16k(1, -1);
    state->mapper.bankChr8k(0);
    state->mapper.configure((header->mapperHi << 4) | header->mapperLo, 0);

    state->battery = header->battery;


    /* Check that the file was actually long enough */
    /*
    if ((unsigned long)ftell(f) < sizeof(NinRomHeader) + state->prgRomSize + state->chrRomSize + state->trainerSize)
    {
        fclose(f);
        return NIN_ERROR_BAD_FILE;
    }
    */

    /* Apply a default configuration suitable for most mappers */
    state->readHandler = &ninMemoryReadNES;
    state->writeHandler = &ninMemoryWriteNES;

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

    state->mapper.bankChr8k(0);

    /* Load the disk */
    state->diskSystem.loadDisk(f);

    /* We won't need the ROM from now on */
    fclose(f);

    /* TODO: Add an FDS mapper */
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
