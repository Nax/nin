#include <libnin/libnin.h>

static const char kHeaderMagic[] = { 'N', 'E', 'S', '\x1a' };

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

    /* Check for the iNES signature */
    if (memcmp(header.magic, kHeaderMagic, 4) != 0)
        return NIN_ERROR_BAD_FILE;

    /* Load the header misc. info */
    state->mapper = (header.mapperHi << 4) | header.mapperLo;
    if (header.quadScreen)
    {
        state->mirroring = MIRRORING_QUAD;
    }
    else if (header.mirroring)
    {
        state->mirroring = MIRRORING_VERTICAL;
    }
    else
    {
        state->mirroring = MIRRORING_HORIZONTAL;
    }
    state->battery = header.battery;
    state->trainerSize = header.trainer ? 0x200 : 0;
    state->prgRomSize = 0x4000 * header.prgRomSize;
    state->chrRomSize = 0x2000 * header.chrRomSize;
    state->prgRamSize = 0x2000;

    /* Zero is an error for PRG but allowed for CHR */
    if (!state->prgRomSize)
    {
        fclose(f);
        return NIN_ERROR_BAD_FILE;
    }
    if (!state->chrRomSize)
        state->chrRamSize = 0x2000;

    /* Allocate the various components */
    state->prgRom = zalloc(state->prgRomSize);
    state->prgRam = zalloc(state->prgRamSize);
    state->chrRom = zalloc(state->chrRomSize);
    state->chrRam = zalloc(state->chrRamSize);

    /* Compute bank counts */
    state->prgBankCount = state->prgRomSize / 0x4000;
    state->chrBankCount = (state->chrRomSize ? state->chrRomSize : state->chrRamSize) / 0x1000;

    /* Actually read the ROM */
    fread(state->prgRam + 0x1000, state->trainerSize, 1, f);
    fread(state->prgRom, state->prgRomSize, 1, f);
    if (state->chrRom)
        fread(state->chrRom, state->chrRomSize, 1, f);

    /* Check that the file was actually long enough */
    if (ftell(f) < sizeof(header) + state->prgRomSize + state->chrRomSize + state->trainerSize)
    {
        fclose(f);
        return NIN_ERROR_BAD_FILE;
    }

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
    state->prgRomBank[0] = state->prgRom;
    state->prgRomBank[1] = state->prgRom + (state->prgBankCount - 1) * 0x4000;

    if (state->chrRom)
    {
        state->chrBank[0] = state->chrRom;
        state->chrBank[1] = state->chrRom + 0x1000;
    }
    else
    {
        state->chrBank[0] = state->chrRam;
        state->chrBank[1] = state->chrRam + 0x1000;
    }

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
    case 7:
        /* AXROM */
        state->prgRomBank[0] = state->prgRom;
        state->prgRomBank[1] = state->prgRom + 0x4000;
        for (int i = 0; i < 4; ++i)
            state->nametables[i] = state->vram;
        state->prgWriteHandler = &ninPrgWriteHandlerAXROM;
        break;
    }

    return NIN_OK;
}