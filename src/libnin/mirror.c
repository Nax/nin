#include <libnin/libnin.h>

NIN_API void ninMirrorA(NinState* state)
{
    state->nametables[0] = state->vram + 0x000;
    state->nametables[1] = state->vram + 0x000;
    state->nametables[2] = state->vram + 0x000;
    state->nametables[3] = state->vram + 0x000;
}

NIN_API void ninMirrorB(NinState* state)
{
    state->nametables[0] = state->vram + 0x400;
    state->nametables[1] = state->vram + 0x400;
    state->nametables[2] = state->vram + 0x400;
    state->nametables[3] = state->vram + 0x400;
}

NIN_API void ninMirrorH(NinState* state)
{
    state->nametables[0] = state->vram + 0x000;
    state->nametables[1] = state->vram + 0x400;
    state->nametables[2] = state->vram + 0x000;
    state->nametables[3] = state->vram + 0x400;
}

NIN_API void ninMirrorV(NinState* state)
{
    state->nametables[0] = state->vram + 0x000;
    state->nametables[1] = state->vram + 0x000;
    state->nametables[2] = state->vram + 0x400;
    state->nametables[3] = state->vram + 0x400;
}
