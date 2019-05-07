#include <libnin/libnin.h>

void ninRunFrame(NinState* state)
{
    memset(state->bitmap, 0, BITMAP_X * BITMAP_Y * 4);
    ninRunFrameCPU(state);
}
