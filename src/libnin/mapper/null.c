#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerNull(NinState* state, uint16_t addr, uint8_t value)
{
    (void)state;
    (void)addr;
    (void)value;
}
