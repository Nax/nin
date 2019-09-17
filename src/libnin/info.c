#include <libnin/libnin.h>

NIN_API void ninInfoQueryInteger(NinState* state, NinInt32* dst, NinInfo info)
{
    switch (info)
    {
    case NIN_INFO_CLOCK_RATE:
        *dst = state->regionData.clockRate;
        break;
    case NIN_INFO_FRAME_CYCLES:
        *dst = state->regionData.frameCycles;
        break;
    case NIN_INFO_FRAME_DELAY:
        *dst = state->regionData.frameDelay;
        break;
    default:
        *dst = 0;
        break;
    }
}
