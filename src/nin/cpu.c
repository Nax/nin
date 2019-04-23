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

static void compare(NinState* state, uint8_t a, uint8_t b)
{
    uint8_t r;

    r = a - b;
    flagNZ(state, r);
    state->cpu.p &= ~(PFLAG_C);
    if (a >= b)
        state->cpu.p |= PFLAG_C;
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
            printf("PC: 0x%04x\n", pc);
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
            case UOP_ADDR_ABS_X:
                addr = uop->data + state->cpu.regs[REG_X];
                tmp = ninMemoryRead8(state, addr);
                break;
            case UOP_ADDR_ABS_X_NOREAD:
                addr = uop->data + state->cpu.regs[REG_X];
                break;
            case UOP_ADDR_ABS_Y:
                addr = uop->data + state->cpu.regs[REG_Y];
                tmp = ninMemoryRead8(state, addr);
                break;
            case UOP_ADDR_ABS_Y_NOREAD:
                addr = uop->data + state->cpu.regs[REG_Y];
                break;
            case UOP_P_SET:
                state->cpu.p |= uop->data;
                break;
            case UOP_P_UNSET:
                state->cpu.p &= ~(uop->data);
                break;
            case UOP_MOV:
                tmp = state->cpu.regs[uop->data & 0x03];
                state->cpu.regs[(uop->data >> 2) & 0x03] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_MOV_NOFLAG:
                tmp = state->cpu.regs[uop->data & 0x03];
                state->cpu.regs[(uop->data >> 2) & 0x03] = tmp;
                break;
            case UOP_LOAD:
                state->cpu.regs[uop->data] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_STORE:
                ninMemoryWrite8(state, addr, state->cpu.regs[uop->data]);
                break;
            case UOP_CMP:
                compare(state, state->cpu.regs[uop->data], tmp);
                break;
            case UOP_ADD_REG:
                tmp = state->cpu.regs[uop->data >> 8] + (uop->data & 0xff);
                state->cpu.regs[uop->data >> 8] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_ADD_MEM:
                tmp += (uop->data & 0xff);
                ninMemoryWrite8(state, addr, tmp);
                flagNZ(state, tmp);
                break;
            case UOP_BRANCH_SET:
                if (state->cpu.p & uop->data) pc += (int8_t)((uint8_t)(uop->data >> 8));
                break;
            case UOP_BRANCH_UNSET:
                if (!(state->cpu.p & uop->data)) pc += (int8_t)((uint8_t)(uop->data >> 8));
                break;
            case UOP_ORA:
                tmp |= state->cpu.regs[REG_A];
                state->cpu.regs[REG_A] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_AND:
                tmp &= state->cpu.regs[REG_A];
                state->cpu.regs[REG_A] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_EOR:
                tmp ^= state->cpu.regs[REG_A];
                state->cpu.regs[REG_A] = tmp;
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
