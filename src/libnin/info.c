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
    case NIN_INFO_PC:
        *dst = state->cpu.pc;
        break;
    case NIN_INFO_REG_A:
        *dst = state->cpu.regs[REG_A];
        break;
    case NIN_INFO_REG_X:
        *dst = state->cpu.regs[REG_X];
        break;
    case NIN_INFO_REG_Y:
        *dst = state->cpu.regs[REG_Y];
        break;
    case NIN_INFO_REG_S:
        *dst = state->cpu.regs[REG_S];
        break;
    default:
        *dst = 0;
        break;
    }
}
