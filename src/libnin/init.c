#include <libnin/libnin.h>

#define zalloc(n)   (calloc(1, n))

static void initCPU(NinState* state)
{
    state->cpu.p = PFLAG_I;
    state->cpu.pc = ninMemoryRead16(state, 0xfffc);
    state->cpu.regs[REG_S] = 0xfd;
    state->cyc = 7;
    state->apu.noise.feedback = 1;
    state->apu.dmc.address = 0x8000;
}

static void loadRom(NinState* state, FILE* rom)
{
    uint8_t header[16];
    uint8_t mapperNum;

    fread(header, 16, 1, rom);
    state->prgRomSize = 16384 * header[4];
    state->chrRomSize = 8192 * header[5];

    state->prgRom = malloc(state->prgRomSize);
    fread(state->prgRom, state->prgRomSize, 1, rom);

    state->prgRamSize = 0x2000;
    state->prgRam = zalloc(state->prgRamSize);

    if (state->chrRomSize)
    {
        state->chrRom = malloc(state->chrRomSize);
        fread(state->chrRom, state->chrRomSize, 1, rom);
    }
    else
    {
        state->chrRamSize = 0x2000;
        state->chrRam = zalloc(state->chrRamSize);
    }

    /* Mirroring */
    if (!(header[6] & 0x01))
    {
        /* Horizontal */
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x000;
        state->nametables[2] = state->vram + 0x400;
        state->nametables[3] = state->vram + 0x400;
    }
    else
    {
        /* Vertical */
        state->nametables[0] = state->vram + 0x000;
        state->nametables[1] = state->vram + 0x400;
        state->nametables[2] = state->vram + 0x000;
        state->nametables[3] = state->vram + 0x400;
    }

    mapperNum = ((header[6] & 0xf0) >> 4) | (header[7] & 0xf0);
    ninApplyMapper(state, mapperNum);
}

NinState* ninCreateState(const char* path)
{
    FILE* file;
    NinState* state;

    file = fopen(path, "rb");
    if (!file)
        return NULL;

    state = zalloc(sizeof(*state));

    state->backBuffer = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->frontBuffer = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->audioSamples = zalloc(NIN_AUDIO_SAMPLE_SIZE * sizeof(float) * 8);
    state->audioSamplesFiltered = zalloc(NIN_AUDIO_SAMPLE_SIZE * sizeof(int16_t));
    state->ram = zalloc(RAM_SIZE);
    state->vram = zalloc(VRAM_SIZE);
    state->palettes = zalloc(0x20);
    state->oam = zalloc(256);
    loadRom(state, file);
    initCPU(state);

    fclose(file);

    return state;
}

void ninDestroyState(NinState* state)
{
    ninSyncSave(state);
    if (state->saveFile)
        fclose(state->saveFile);

    free(state->backBuffer);
    free(state->frontBuffer);
    free(state->audioSamples);
    free(state->audioSamplesFiltered);
    free(state->ram);
    free(state->vram);
    free(state->palettes);
    free(state->oam);
    free(state->prgRam);
    free(state->prgRom);
    free(state->chrRam);
    free(state->chrRom);
    free(state);
}

const uint32_t* ninGetScreenBuffer(NinState* state)
{
    return state->frontBuffer;
}

void ninSetInput(NinState* state, uint8_t input)
{
    state->controller = input;
}
