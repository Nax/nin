#include <cstdio>
#include <cstring>
#include <libnin/State.h>

static const char kHeaderMagicNES[] = { 'N', 'E', 'S', '\x1a' };
static const char kHeaderMagicFDS[] = { 'F', 'D', 'S', '\x1a' };

NinState::NinState()
: memory{}
, info{}
, cart{}
, input{}
, irq{}
, nmi{}
, mapper{memory, cart, irq}
, busVideo{memory, cart, mapper}
, audio{info}
, apu{info, irq, mapper, audio}
, diskSystem{info, irq}
, video{}
, ppu{info, memory, nmi, busVideo, mapper, video}
, busMain{memory, cart, mapper, ppu, apu, input}
, cpu{irq, nmi, ppu, apu, busMain}
, save{cart}
{
}

NinError NinState::loadRom(const char* path)
{
    std::FILE* f;
    RomHeader header{};

    /* Open the ROM */
    f = fopen(path, "rb");
    if (!f)
        return NIN_ERROR_IO;

    /* Read the header */
    std::fread(&header, sizeof(header), 1, f);

    /* Check for the NES signature */
    if (std::memcmp(header.magic, kHeaderMagicNES, 4) == 0)
        return loadRomNES(header, f);

    /* Check for the iNES signature */
    if (std::memcmp(header.magic, kHeaderMagicFDS, 4) == 0)
        return loadRomFDS(header, f);

    std::fclose(f);
    return NIN_ERROR_BAD_FILE;
}

NinError NinState::loadRomNES(const RomHeader& header, std::FILE* f)
{
    bool nes2{};
    std::uint16_t prgRomBankCount;
    std::uint16_t prgRamBankCount;
    std::uint16_t chrRomBankCount;
    std::uint16_t chrRamBankCount;

    /* Check for the nes2 signature */
    if (header.magicNes2 == 0x2)
        nes2 = true;

    /* Get the length of every segment */
    prgRomBankCount = header.prgRomSize * 2;
    prgRamBankCount = 1;
    chrRomBankCount = header.chrRomSize * 8;
    chrRamBankCount = chrRomBankCount ? 0 : 8;

    /* Zero is an error for PRG but allowed for CHR */
    if (!prgRomBankCount)
    {
        std::fclose(f);
        return NIN_ERROR_BAD_FILE;
    }

    /* Load data from the cart */
    cart.load(CART_PRG_ROM, prgRomBankCount, f);
    cart.load(CART_PRG_RAM, prgRamBankCount, nullptr);
    cart.load(CART_CHR_ROM, chrRomBankCount, f);
    cart.load(CART_CHR_RAM, chrRamBankCount, nullptr);

    /* We won't need the ROM from now on */
    std::fclose(f);

    /* Load the region */
    if (!nes2)
        info.setRegion(NIN_REGION_NTSC);
    else
        info.setRegion((NinRegion)header.nes2.region);

    /* Load the header misc. info */
    if (header.mirroring)
        mapper.mirror(NIN_MIRROR_H);
    else
        mapper.mirror(NIN_MIRROR_V);
    mapper.bankPrg16k(0, 0);
    mapper.bankPrg16k(1, -1);
    mapper.bankChr8k(0);
    if (!mapper.configure((header.mapperHi << 4) | header.mapperLo, 0))
    {
        return NIN_ERROR_BAD_MAPPER;
    }
    save.setBattery(!!header.battery);

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

NinError NinState::loadRomFDS(const RomHeader& header, std::FILE* f)
{
    info.setSystem(NIN_SYSTEM_FDS);

    /* PRG ROM is the FDS BIOS */
    cart.load(CART_PRG_ROM, 1, nullptr);
    cart.load(CART_PRG_RAM, 4, nullptr);
    cart.load(CART_CHR_ROM, 0, nullptr);
    cart.load(CART_CHR_RAM, 8, nullptr);

    mapper.bankChr8k(0);

    /* Load the disk */
    diskSystem.loadDisk(f);

    /* We won't need the ROM from now on */
    std::fclose(f);

    return NIN_OK;
}
