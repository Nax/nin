#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerNull(NinState* state, uint16_t addr, uint8_t value)
{
    UNUSED(state);
    UNUSED(addr);
    UNUSED(value);
}

NIN_API void ninPpuMonitorHandlerNull(NinState* state, uint16_t addr)
{
    UNUSED(state);
    UNUSED(addr);
}
