#include <libnin/libnin.h>

#define zalloc(n)   (calloc(1, n))

static void initCPU(NinState* state)
{
    state->cpu.p = PFLAG_I;
    state->cpu.pc = ninMemoryRead16(state, 0xfffc);
    //state->cpu.pc = 0xc000;
    state->cpu.regs[REG_S] = 0xfd;
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
    printf("%d\r\n", state->prgRomSize);

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

    mapperNum = ((header[6] & 0xf0) >> 4) | (header[7] & 0xf0);
    ninApplyMapper(state, mapperNum);
}

NinState* ninCreateState(FILE* rom)
{
    NinState* state;

    state = zalloc(sizeof(*state));
    state->bitmap = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->audioSamples = zalloc(NIN_AUDIO_SAMPLE_SIZE * sizeof(int16_t));
    state->traceCache = zalloc(sizeof(NinTraceCache));
    for (size_t i = 0; i < 0x10000; ++i)
        state->traceCache->index[i] = TRACE_NONE;
    state->ram = zalloc(RAM_SIZE);
    state->vram = zalloc(VRAM_SIZE);
    state->palettes = zalloc(0x20);
    state->oam = zalloc(256);
    loadRom(state, rom);
    initCPU(state);

    return state;
}

void ninDestroyState(NinState* state)
{
    free(state->ram);
    free(state);
}

const uint32_t* ninGetScreenBuffer(NinState* state)
{
    return state->bitmap;
}

void ninSetInput(NinState* state, uint8_t input)
{
    state->controller = input;
}
