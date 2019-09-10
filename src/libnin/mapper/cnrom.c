#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerCNROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    value %= state->chrBankCount;
    state->chrBank[0] = state->chrRom + 0x2000 * value;
    state->chrBank[1] = state->chrRom + 0x2000 * value + 0x1000;
}
