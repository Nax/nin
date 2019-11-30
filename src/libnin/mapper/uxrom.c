#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerUXROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    value %= state->prgBankCount;
    ninBankSwitchPrgRom16k(state, 0, value);
}
