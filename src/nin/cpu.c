#include <nin/nin.h>
#include "uop.h"

static void flagZ(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_Z;
    state->cpu.p |= (!value) << 1;
}

static void flagN(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_N;
    state->cpu.p |= (value & 0x80);
}

static void flagNZ(NinState* state, uint8_t value)
{
    flagZ(state, value);
    flagN(state, value);
}

void ninRunCycles(NinState* state, size_t cycles)
{
    uint16_t pc;
    NinTrace* trace;
    NinUop* uop;
    uint8_t op;
    uint8_t tmp;
    uint16_t addr;

    pc = state->cpu.pc;

    while (cycles--)
    {
        trace = ninGetTrace(state, pc);
        for (size_t i = 0; i < trace->length; ++i)
        {
            uop = trace->uops + i;
            pc += uop->len;
            op = uop->op;

            switch (op)
            {
            case UOP_ADDR_IMM:
                tmp = (uint8_t)uop->data;
                break;
            case UOP_ADDR_ABS:
                addr = uop->data;
                tmp = ninMemoryRead8(state, addr);
                break;
            case UOP_ADDR_ABS_NOREAD:
                addr = uop->data;
                break;
            case UOP_P_SET:
                state->cpu.p |= uop->data;
                break;
            case UOP_P_UNSET:
                state->cpu.p &= ~(uop->data);
                break;
            case UOP_TAX:
                tmp = state->cpu.a;
                state->cpu.x = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_TAY:
                tmp = state->cpu.a;
                state->cpu.y = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_TSX:
                tmp = state->cpu.s;
                state->cpu.x = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_TXA:
                tmp = state->cpu.x;
                state->cpu.a = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_TXS:
                tmp = state->cpu.x;
                state->cpu.s = tmp;
                break;
            case UOP_TYA:
                tmp = state->cpu.y;
                state->cpu.a = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_BRANCH_SET:
                if (state->cpu.p & uop->data) pc += (int8_t)uop->data;
                break;
            case UOP_BRANCH_UNSET:
                if (!(state->cpu.p & uop->data)) pc += (int8_t)uop->data;
                break;
            case UOP_STA:
                ninMemoryWrite8(state, addr, state->cpu.a);
                break;
            case UOP_LDA:
                state->cpu.a = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_LDX:
                state->cpu.x = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_LDY:
                state->cpu.y = tmp;
                flagNZ(state, tmp);
                break;
            default:
                printf("Unknown uop: 0x%02x\n", op);
                getchar();
                break;
            }
        }
        state->cpu.pc = pc;
    }
}
