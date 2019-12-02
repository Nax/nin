#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerUXROM(NinState* state, uint16_t addr, uint8_t value)
{
    UNUSED(addr);
    ninBankSwitchPrgRom16k(state, 0, value);
}

NIN_API void ninPrgWriteHandlerUXROM_180(NinState* state, uint16_t addr, uint8_t value)
{
    UNUSED(addr);
    ninBankSwitchPrgRom16k(state, 1, value);
}
