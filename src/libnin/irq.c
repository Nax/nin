#include <libnin/libnin.h>

NIN_API void ninSetIRQ(NinState* state, uint8_t flag)
{
    state->irqFlags |= flag;
    state->irq = 1;
}

NIN_API void ninClearIRQ(NinState* state, uint8_t flag)
{
    state->irqFlags &= ~flag;
    if (!state->irqFlags)
        state->irq = 0;
}
