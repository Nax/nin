#include <libnin/libnin.h>
#include "uop.h"

static void flagZ(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_Z;
    if (!value)
    {
        state->cpu.p |= PFLAG_Z;
    }
    //state->cpu.p |= ((!value) << 1);
}

static void flagN(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_N;
    if (value & 0x80)
        state->cpu.p |= PFLAG_N;
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

static void stackPush8(NinState* state, uint8_t value)
{
    ninMemoryWrite8(state, 0x100 | state->cpu.regs[REG_S], value);
    state->cpu.regs[REG_S]--;
}

static uint8_t stackPop8(NinState* state)
{
    state->cpu.regs[REG_S]++;
    return ninMemoryRead8(state, 0x100 | state->cpu.regs[REG_S]);
}

static void stackPush16(NinState* state, uint16_t value)
{
    stackPush8(state, value >> 8);
    stackPush8(state, value & 0xff);
}

static uint16_t stackPop16(NinState* state)
{
    uint8_t hi;
    uint8_t lo;

    lo = stackPop8(state);
    hi = stackPop8(state);

    return lo | ((uint16_t)hi << 8);
}

static uint8_t incr(uint8_t value)
{
    return (value & 0x80) ? 0xff : 0x01;
}

static void _carry(NinState* state, uint8_t value)
{
    if (value)
        state->cpu.p |= PFLAG_C;
    else
        state->cpu.p &= ~PFLAG_C;
}

static uint8_t _adc(NinState* state, uint8_t a, uint8_t b)
{
    uint16_t sum;
    uint8_t carryIn;

    carryIn = (state->cpu.p & PFLAG_C) ? 1 : 0;
    sum = (uint16_t)a + (uint16_t)b + (uint16_t)carryIn;

    state->cpu.p &= ~(PFLAG_C | PFLAG_V | PFLAG_N | PFLAG_Z);
    if (sum & 0x100) state->cpu.p |= PFLAG_C;
    if ((sum & 0xff) == 0) state->cpu.p |= PFLAG_Z;
    if ((a ^ sum) & (b ^ sum) & 0x80) state->cpu.p |= PFLAG_V;
    if (sum & 0x80) state->cpu.p |= PFLAG_N;

    return (sum & 0xff);
}

void ninRunFrameCPU(NinState* state)
{
    int leaveTrace;
    uint16_t pc;
    uint8_t cycles;
    NinTrace* trace;
    NinUop* uop;
    uint8_t tmp;
    uint8_t c;
    uint16_t addr;

    pc = state->cpu.pc;

    for (;;)
    {
        trace = ninGetTrace(state, pc);
        leaveTrace = 0;
        for (size_t i = 0; i < 1 /*trace->length */; ++i)
        {
            uop = trace->uops + i;
            pc += uop->len;
            
            switch (uop->mode)
            {
            default:
                printf("Invalid addressing mode: 0x%02x\n", uop->mode);
                getchar();
                break;
            case ADDR_NONE:
                cycles = 2;
                break;
            case ADDR_IMM:
                cycles = 2;
                tmp = (uint8_t)uop->addr;
                break;
            case ADDR_ABS:
                cycles = 4;
                addr = uop->addr;
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ABS_NOREAD:
                cycles = 3;
                addr = uop->addr;
                break;
            case ADDR_ABS_INDIRECT_NOREAD:
                cycles = 5; // FIXME
                addr = ninMemoryRead8(state, uop->addr);
                addr |= ((ninMemoryRead8(state, ((uop->addr + 1) & 0xff) | (uop->addr & 0xff00))) << 8);
                break;
            case ADDR_ABS_X:
                cycles = 4;
                addr = uop->addr + state->cpu.regs[REG_X];
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ABS_X_NOREAD:
                cycles = 3;
                addr = uop->addr + state->cpu.regs[REG_X];
                break;
            case ADDR_ABS_Y:
                cycles = 4;
                addr = uop->addr + state->cpu.regs[REG_Y];
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ABS_Y_NOREAD:
                cycles = 3;
                addr = uop->addr + state->cpu.regs[REG_Y];
                break;
            case ADDR_ZERO:
                cycles = 3;
                addr = uop->addr;
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ZERO_NOREAD:
                cycles = 2;
                addr = uop->addr;
                break;
            case ADDR_ZERO_X:
                cycles = 4;
                addr = (uop->addr + state->cpu.regs[REG_X]) & 0xff;
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ZERO_X_NOREAD:
                cycles = 3;
                addr = (uop->addr + state->cpu.regs[REG_X]) & 0xff;
                break;
            case ADDR_ZERO_Y:
                cycles = 4;
                addr = (uop->addr + state->cpu.regs[REG_Y]) & 0xff;
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ZERO_Y_NOREAD:
                cycles = 3;
                addr = (uop->addr + state->cpu.regs[REG_Y]) & 0xff;
                break;
            case ADDR_ZERO_X_INDIRECT:
                cycles = 6;
                tmp = (uop->addr + state->cpu.regs[REG_X]) & 0xff;
                addr = ninMemoryRead8(state, tmp);
                addr |= (ninMemoryRead8(state, (tmp + 1) & 0xff)) << 8;
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ZERO_X_INDIRECT_NOREAD:
                cycles = 5;
                tmp = (uop->addr + state->cpu.regs[REG_X]) & 0xff;
                addr = ninMemoryRead8(state, tmp);
                addr |= (ninMemoryRead8(state, (tmp + 1) & 0xff)) << 8;
                break;
            case ADDR_ZERO_Y_INDIRECT:
                cycles = 6;
                addr = ninMemoryRead8(state, uop->addr);
                addr |= (ninMemoryRead8(state, (uop->addr + 1) & 0xff)) << 8;
                addr += state->cpu.regs[REG_Y];
                tmp = ninMemoryRead8(state, addr);
                break;
            case ADDR_ZERO_Y_INDIRECT_NOREAD:
                cycles = 5;
                addr = ninMemoryRead8(state, uop->addr);
                addr |= (ninMemoryRead8(state, (uop->addr + 1) & 0xff)) << 8;
                addr += state->cpu.regs[REG_Y];
                break;
            }

            switch (uop->op)
            {
            default:
                printf("Unknown uop: 0x%02x\n", uop->op);
                getchar();
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
                leaveTrace = ninMemoryWrite8(state, addr, state->cpu.regs[uop->data]);
                break;
            case UOP_CMP:
                compare(state, state->cpu.regs[uop->data], tmp);
                break;
            case UOP_ADD_REG:
                tmp = state->cpu.regs[uop->data & 0x03] + incr(uop->data);
                state->cpu.regs[uop->data & 0x03] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_ADD_MEM:
                tmp += incr(uop->data);
                leaveTrace = ninMemoryWrite8(state, addr, tmp);
                flagNZ(state, tmp);
                break;
            case UOP_TEST:
                state->cpu.p &= ~(0xc2);
                state->cpu.p |= (tmp & 0xc0);
                if (!(tmp & state->cpu.regs[REG_A]))
                    state->cpu.p |= PFLAG_Z;
                break;
            case UOP_BRANCH_SET:
                if (state->cpu.p & uop->data) pc += (int8_t)tmp;
                break;
            case UOP_BRANCH_UNSET:
                if (!(state->cpu.p & uop->data)) pc += (int8_t)tmp;
                break;
            case UOP_JMP:
                pc = addr;
                break;
            case UOP_JSR:
                stackPush16(state, pc - 1);
                pc = addr;
                break;
            case UOP_RTS:
                pc = stackPop16(state) + 1;
                break;
            case UOP_RTI:
                state->cpu.p = stackPop8(state) & PFLAG_MASK;
                pc = stackPop16(state);
                break;
            case UOP_BRK:
                pc++;
                stackPush16(state, pc);
                stackPush8(state, state->cpu.p | PFLAG_1 | PFLAG_B);
                pc = ninMemoryRead16(state, 0xfffe);
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
            case UOP_ADC:
                state->cpu.regs[REG_A] = _adc(state, state->cpu.regs[REG_A], tmp ^ uop->data);
                break;
            case UOP_ASL:
                _carry(state, tmp & 0x80);
                tmp <<= 1;
                flagNZ(state, tmp);
                leaveTrace = ninMemoryWrite8(state, addr, tmp);
                break;
            case UOP_ASL_REG:
                tmp = state->cpu.regs[REG_A];
                _carry(state, tmp & 0x80);
                tmp <<= 1;
                flagNZ(state, tmp);
                state->cpu.regs[REG_A] = tmp;
                break;
            case UOP_ROL:
                c = state->cpu.p & PFLAG_C;
                _carry(state, tmp & 0x80);
                tmp <<= 1;
                tmp |= c;
                flagNZ(state, tmp);
                leaveTrace = ninMemoryWrite8(state, addr, tmp);
                break;
            case UOP_ROL_REG:
                tmp = state->cpu.regs[REG_A];
                c = state->cpu.p & PFLAG_C;
                _carry(state, tmp & 0x80);
                tmp <<= 1;
                tmp |= c;
                flagNZ(state, tmp);
                state->cpu.regs[REG_A] = tmp;
                break;
            case UOP_LSR:
                _carry(state, tmp & 0x01);
                tmp >>= 1;
                flagNZ(state, tmp);
                leaveTrace = ninMemoryWrite8(state, addr, tmp);
                break;
            case UOP_LSR_REG:
                tmp = state->cpu.regs[REG_A];
                _carry(state, tmp & 0x01);
                tmp >>= 1;
                flagNZ(state, tmp);
                state->cpu.regs[REG_A] = tmp;
                break;
            case UOP_ROR:
                c = state->cpu.p & PFLAG_C;
                _carry(state, tmp & 0x01);
                tmp >>= 1;
                tmp |= (c << 7);
                flagNZ(state, tmp);
                leaveTrace = ninMemoryWrite8(state, addr, tmp);
                break;
            case UOP_ROR_REG:
                tmp = state->cpu.regs[REG_A];
                c = state->cpu.p & PFLAG_C;
                _carry(state, tmp & 0x01);
                tmp >>= 1;
                tmp |= (c << 7);
                flagNZ(state, tmp);
                state->cpu.regs[REG_A] = tmp;
                break;
            case UOP_PUSHA:
                stackPush8(state, state->cpu.regs[REG_A]);
                break;
            case UOP_PUSHP:
                stackPush8(state, state->cpu.p | PFLAG_1 | PFLAG_B);
                break;
            case UOP_POPA:
                tmp = stackPop8(state);
                state->cpu.regs[REG_A] = tmp;
                flagNZ(state, tmp);
                break;
            case UOP_POPP:
                state->cpu.p = stackPop8(state) & PFLAG_MASK;
                break;
            case UOP_NOP:
                break;
            }

            state->cpu.pc = pc;

            if (leaveTrace)
            {
                printf("FLUSH TRACES 0x%04x\n", pc);
                fflush(stdout);
                ninFlushTraces(state);
            }

            ninRunCyclesAPU(state, cycles);
            if (ninPpuRunCycles(state, cycles * 3)) goto end;

            if (leaveTrace)
                break;

            if (state->nmi)
            {
                state->nmi = 0;
                stackPush16(state, pc);
                stackPush8(state, state->cpu.p);
                printf("--- NMI ---\n");
                pc = ninMemoryRead16(state, 0xfffa);
                state->cpu.pc = pc;
            }
        }
    }

end:
    return;
}
