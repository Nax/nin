#include <nin/nin.h>
#include "uop.h"

void ninRunCycles(NinState* state, size_t cycles)
{
    uint16_t pc;
    NinTrace* trace;
    NinUop* uop;

    pc = state->cpu.pc;

    while (cycles--)
    {
        trace = ninGetTrace(state, pc);
        for (size_t i = 0; i < trace->length; ++i)
        {
            uop = trace->uops + i;
            pc += uop->len;
        }
        state->cpu.pc = pc;
    }
}
