#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerCNROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    value &= 0x3;
    ninBankSwitchChrRom4k(state, 0, value * 2);
    ninBankSwitchChrRom4k(state, 1, value * 2 + 1);
}
