#include <nin/nin.h>

#define TRAP do { printf("TRAP opcode 0x%02x at 0x%04x\n", op, pcOld); getchar(); } while (0)

void ninRunCycles(NinState* state, size_t cycles)
{
    uint16_t pcOld;
    uint8_t op;

    while (cycles--)
    {
        pcOld = state->cpu.pc;
        op = ninMemoryRead8(state, state->cpu.pc++);

        switch (op)
        {
        default:
            TRAP;
            break;
        }
    }
}
