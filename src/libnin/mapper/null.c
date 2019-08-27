#include <libnin/libnin.h>

int ninPrgWriteHandlerNull(NinState* state, uint16_t addr, uint8_t value)
{
    (void)state;
    (void)addr;
    (void)value;
    return 0;
}
