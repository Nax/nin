#include <libnin/libnin.h>

#define zalloc(n)   (calloc(1, n))

static void initCPU(NinState* state)
{
    state->cpu.p = PFLAG_I;
    state->cpu.pc = ninMemoryRead16(state, 0xfffc);
}

static void loadRom(NinState* state, FILE* rom)
{
    uint8_t header[16];

    fread(header, 16, 1, rom);
    state->prgRomSize = 16384 * header[4];
    state->chrRomSize = 8192 * header[5];
    state->prgRom = malloc(state->prgRomSize);
    state->chrRom = malloc(state->chrRomSize);
    fread(state->prgRom, state->prgRomSize, 1, rom);
    fread(state->chrRom, state->chrRomSize, 1, rom);
}

NinState* ninCreateState(FILE* rom)
{
    NinState* state;

    state = zalloc(sizeof(*state));
    state->bitmap = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
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
