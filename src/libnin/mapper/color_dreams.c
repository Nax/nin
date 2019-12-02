#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerColorDreams(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    ninBankSwitchPrgRom32k(state, value & 0x3);
    ninBankSwitchChrRom8k(state, (value >> 4));
}
