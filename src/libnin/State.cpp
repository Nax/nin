#include <cstdio>
#include <cstring>
#include <libnin/Util.h>
#include <libnin/State.h>
#include <libnin/RomHeader.h>

using namespace libnin;

static const char kHeaderMagicNES[] = { 'N', 'E', 'S', '\x1a' };
static const char kHeaderMagicFDS[] = { 'F', 'D', 'S', '\x1a' };

static NinError loadRomNES(State& state, const RomHeader& header, std::FILE* f)
{
    NinError err;
    bool nes2{};
    std::uint16_t prgRomBankCount;
    std::uint16_t prgRamBankCount;
    std::uint16_t chrRomBankCount;
    std::uint16_t chrRamBankCount;
    int mapper{};
    int submapper{};

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
    if (header.mirroring)
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

static NinError loadRomFDS(State& state, const RomHeader& header, std::FILE* f)
{
    UNUSED(header);

    state.info.setSystem(NIN_SYSTEM_FDS);

    /* PRG ROM is the FDS BIOS */
    state.cart.load(CART_PRG_ROM, 1, nullptr);
    state.cart.load(CART_PRG_RAM, 4, nullptr);
    state.cart.load(CART_CHR_ROM, 0, nullptr);
    state.cart.load(CART_CHR_RAM, 8, nullptr);

    state.mapper.bankChr8k(0);

    /* Load the disk */
    state.diskSystem.loadDisk(f);

    /* We won't need the ROM from now on */
    std::fclose(f);

    return NIN_OK;
}

static NinError loadRom(State& state, const char* path)
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
        return loadRomNES(state, header, f);

    /* Check for the iNES signature */
    if (std::memcmp(header.magic, kHeaderMagicFDS, 4) == 0)
        return loadRomFDS(state, header, f);

    std::fclose(f);
    return NIN_ERROR_BAD_FILE;
}

State::State()
: memory{}
, info{}
, cart{}
, save{cart}
, input{}
, irq{}
, nmi{}
, video{}
, mapper{memory, cart, irq}
, busVideo{memory, cart, mapper}
, audio{info}
, apu{info, irq, mapper, audio}
, diskSystem{info, irq}
, ppu{info, memory, nmi, busVideo, mapper, video}
, busMain{memory, cart, mapper, ppu, apu, input}
, cpu{memory, irq, nmi, ppu, apu, busMain}
{

}

State* State::create(NinError& err, const char* path)
{
    State* s = new State;
    NinError e;

    err = loadRom(*s, path);
    if (err)
    {
        delete s;
        s = nullptr;
    }
    return s;
}

