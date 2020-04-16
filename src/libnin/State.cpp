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
    state.cart->load(CART_PRG_ROM, prgRomBankCount, f);
    state.cart->load(CART_PRG_RAM, prgRamBankCount, nullptr);
    state.cart->load(CART_CHR_ROM, chrRomBankCount, f);
    state.cart->load(CART_CHR_RAM, chrRamBankCount, nullptr);

    /* We won't need the ROM from now on */
    std::fclose(f);

    /* Load the region */
    if (!nes2)
        state.info->setRegion(NIN_REGION_NTSC);
    else
        state.info->setRegion((NinRegion)header.nes2.region);

    /* Load the mapper */
    state.mapper = std::unique_ptr<Mapper>(Mapper::create(*state.memory, *state.cart, *state.irq, (header.mapperHi << 4) | header.mapperLo, 0));
    if (!state.mapper)
    {
        return NIN_ERROR_BAD_MAPPER;
    }

    /* Load the header misc. info */
    if (header.mirroring)
        state.mapper->mirror(NIN_MIRROR_H);
    else
        state.mapper->mirror(NIN_MIRROR_V);
    state.mapper->handleReset();
    state.save->setBattery(!!header.battery);

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

    state.info->setSystem(NIN_SYSTEM_FDS);

    /* PRG ROM is the FDS BIOS */
    state.cart->load(CART_PRG_ROM, 1, nullptr);
    state.cart->load(CART_PRG_RAM, 4, nullptr);
    state.cart->load(CART_CHR_ROM, 0, nullptr);
    state.cart->load(CART_CHR_RAM, 8, nullptr);

    state.mapper->bankChr8k(0);

    /* Load the disk */
    state.diskSystem->loadDisk(f);

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

State* State::create(NinError& err, const char* path)
{
    State* s = new State;
    NinError e;

    /* Init the common systems */
    s->memory = std::make_unique<Memory>();
    s->info = std::make_unique<HardwareInfo>();
    s->cart = std::make_unique<Cart>();
    s->save = std::make_unique<Save>(*s->cart);
    s->input = std::make_unique<Input>();
    s->irq = std::make_unique<IRQ>();
    s->nmi = std::make_unique<NMI>();
    s->video = std::make_unique<Video>();

    /* Load the rom - this will also select a mapper */
    e = loadRom(*s, path);
    if (e) goto error;

    /* Init the other common systems - these depends on the mapper as well */
    s->busVideo = std::make_unique<BusVideo>(*s->memory, *s->cart, *s->mapper);
    s->audio = std::make_unique<Audio>(*s->info);
    s->apu = std::make_unique<APU>(*s->info, *s->irq, *s->mapper, *s->audio);
    s->diskSystem = std::make_unique<DiskSystem>(*s->info, *s->irq);
    s->ppu = std::make_unique<PPU>(*s->info, *s->memory, *s->nmi, *s->busVideo, *s->mapper, *s->video);
    s->busMain = std::make_unique<BusMain>(*s->memory, *s->cart, *s->mapper, *s->ppu, *s->apu, *s->input);
    s->cpu = std::make_unique<CPU>(*s->memory, *s->irq, *s->nmi, *s->ppu, *s->apu, *s->busMain);

    err = NIN_OK;
    return s;

error:
    delete s;
    err = e;
    return nullptr;
}

