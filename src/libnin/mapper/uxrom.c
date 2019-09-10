#include <libnin/libnin.h>

NIN_API void ninPrgWriteHandlerUXROM(NinState* state, uint16_t addr, uint8_t value)
{
    (void)addr;
    value %= state->prgBankCount;
    state->prgRomBank[0] = state->prgRom + 0x4000 * value;
}
