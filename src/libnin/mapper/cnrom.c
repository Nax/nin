#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerCNROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    ninBankSwitchChrRom8k(state, value & 0x3);
}
