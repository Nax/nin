#include <libnin/libnin.h>

static void initCPU(NinState* state)
{
    state->cpu.p = PFLAG_I;
    state->cpu.pc = ninMemoryRead16(state, 0xfffc);
    state->cpu.regs[REG_S] = 0xfd;
    state->cyc = 7;
    state->apu.noise.feedback = 1;
    state->apu.dmc.address = 0x8000;
}

NIN_API NinError ninCreateState(NinState** dst, const char* path)
{
    NinState* state;
    NinError err;

    state = zalloc(sizeof(*state));
    state->backBuffer = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->frontBuffer = zalloc(BITMAP_X * BITMAP_Y * sizeof(uint32_t));
    state->audioSamples = zalloc(NIN_AUDIO_SAMPLE_SIZE * sizeof(float) * 8);
    state->audioSamplesFiltered = zalloc(NIN_AUDIO_SAMPLE_SIZE * sizeof(int16_t));
    state->ram = zalloc(RAM_SIZE);
    state->vram = zalloc(VRAM_SIZE);
    state->palettes = zalloc(0x20);
    state->oam = zalloc(256);

    if ((err = ninLoadRom(state, path)) != NIN_OK)
    {
        ninDestroyState(state);
        *dst = NULL;
        return err;
    }

    initCPU(state);

    *dst = state;
    return NIN_OK;
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
