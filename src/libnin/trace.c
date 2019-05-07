#include <libnin/libnin.h>
#include "uop.h"

static NinTrace* ninAllocTrace(NinState* state, uint16_t addr)
{
    NinTrace* trace;
    uint16_t index;

    index = state->traceCache->cursor;
    trace = state->traceCache->traces + index;
    state->traceCache->cursor = (index + 1) % TRACE_CACHE_SIZE;

    if (trace->length > 0)
        state->traceCache->index[trace->pc] = TRACE_NONE;
    trace->length = 0;
    trace->pc = addr;
    state->traceCache->index[addr] = index;

    return trace;
}

#define MOV(a, b)   (a | (b << 2))
#define INC(x)      (x)
#define DEC(x)      (0x80 | x)

static NinTrace* ninBuildTrace(NinState* state, uint16_t pc)
{
    NinTrace* trace;
    NinUop* uop;
    uint8_t op;

    trace = ninAllocTrace(state, pc);

    while (trace->length < TRACE_MAX_UOPS)
    {
        uop = trace->uops + trace->length;
        trace->length++;
        op = ninMemoryRead8(state, pc++);

        /* Decode the addressing mode */
        switch (op)
        {
        default:
            uop->len = 1;
            uop->mode = ADDR_NONE;
            break;
        case 0x09: case 0x29: case 0x49: case 0x69: case 0x89: case 0xa9: case 0xc9: case 0xe9:
        case 0x0b: case 0x2b: case 0x4b: case 0x6b: case 0x8b: case 0xab: case 0xcb: case 0xeb:
        case 0x80: case 0xa0: case 0xc0: case 0xe0:
        case 0x82: case 0xa2: case 0xc2: case 0xe2:
        case 0x10: case 0x30: case 0x50: case 0x70: case 0x90: case 0xb0: case 0xd0: case 0xf0:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_IMM;
            pc += 1;
            break;
        case 0x0d: case 0x2d: case 0x4d: case 0x6d: case 0xad: case 0xcd: case 0xed:
        case 0x0e: case 0x2e: case 0x4e: case 0x6e: case 0xae: case 0xce: case 0xee:
        case 0x0f: case 0x2f: case 0x4f: case 0x6f: case 0xaf: case 0xcf: case 0xef:
        case 0x0c: case 0x2c: case 0xac: case 0xcc: case 0xec:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS;
            pc += 2;
            break;
        case 0x8c: case 0x8d: case 0x8e: case 0x8f:
        case 0x4c:
        case 0x20:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS_NOREAD;
            pc += 2;
            break;
        case 0x6c:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS_INDIRECT_NOREAD;
            pc += 2;
            break;
        case 0x1c: case 0x3c: case 0x5c: case 0x7c: case 0x9c: case 0xbc: case 0xdc: case 0xfc:
        case 0x1d: case 0x3d: case 0x5d: case 0x7d: case 0xbd: case 0xdd: case 0xfd:
        case 0x1e: case 0x3e: case 0x5e: case 0x7e: case 0xde: case 0xfe:
        case 0x1f: case 0x3f: case 0x5f: case 0x7f: case 0xdf: case 0xff:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS_X;
            pc += 2;
            break;
        case 0x9d:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS_X_NOREAD;
            pc += 2;
            break;
        case 0x19: case 0x39: case 0x59: case 0x79: case 0xb9: case 0xd9: case 0xf9:
        case 0x1b: case 0x3b: case 0x5b: case 0x7b: case 0x9b: case 0xbb: case 0xdb: case 0xfb:
        case 0x9e: case 0xbe:
        case 0x9f: case 0xbf:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS_Y;
            pc += 2;
            break;
        case 0x99:
            uop->len = 3;
            uop->addr = ninMemoryRead16(state, pc);
            uop->mode = ADDR_ABS_Y_NOREAD;
            pc += 2;
            break;
        case 0x04: case 0x24: case 0x44: case 0x64: case 0xa4: case 0xc4: case 0xe4:
        case 0x05: case 0x25: case 0x45: case 0x65: case 0xa5: case 0xc5: case 0xe5:
        case 0x06: case 0x26: case 0x46: case 0x66: case 0xa6: case 0xc6: case 0xe6:
        case 0x07: case 0x27: case 0x47: case 0x67: case 0xa7: case 0xc7: case 0xe7:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO;
            pc += 1;
            break;
        case 0x84: case 0x85: case 0x86: case 0x87:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_NOREAD;
            pc += 1;
            break;
        case 0x14: case 0x34: case 0x54: case 0x74: case 0xb4: case 0xd4: case 0xf4:
        case 0x15: case 0x35: case 0x55: case 0x75: case 0xb5: case 0xd5: case 0xf5:
        case 0x16: case 0x36: case 0x56: case 0x76: case 0xd6: case 0xf6:
        case 0x17: case 0x37: case 0x57: case 0x77: case 0xd7: case 0xf7:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_X;
            pc += 1;
            break;
        case 0x94: case 0x95:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_X_NOREAD;
            pc += 1;
            break;
        case 0xb6:
        case 0xb7:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_Y;
            pc += 1;
            break;
        case 0x96:
        case 0x97:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_Y_NOREAD;
            pc += 1;
            break;
        case 0x01: case 0x21: case 0x41: case 0x61: case 0xa1: case 0xc1: case 0xe1:
        case 0x03: case 0x23: case 0x43: case 0x63: case 0xa3: case 0xc3: case 0xe3:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_X_INDIRECT;
            pc += 1;
            break;
        case 0x81:
        case 0x83:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_X_INDIRECT_NOREAD;
            pc += 1;
            break;
        case 0x11: case 0x31: case 0x51: case 0x71: case 0xb1: case 0xd1: case 0xf1:
        case 0x13: case 0x33: case 0x53: case 0x73: case 0xb3: case 0xd3: case 0xf3:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_Y_INDIRECT;
            pc += 1;
            break;
        case 0x91:
        case 0x93:
            uop->len = 2;
            uop->addr = ninMemoryRead8(state, pc);
            uop->mode = ADDR_ZERO_Y_INDIRECT_NOREAD;
            pc += 1;
            break;
        }

        /* Decode the body */
        switch (op)
        {
            /* Flags */
        case 0x18:
            uop->op = UOP_P_UNSET;
            uop->data = PFLAG_C;
            break;
        case 0x58:
            uop->op = UOP_P_UNSET;
            uop->data = PFLAG_I;
            break;
        case 0xb8:
            uop->op = UOP_P_UNSET;
            uop->data = PFLAG_V;
            break;
        case 0xd8:
            uop->op = UOP_P_UNSET;
            uop->data = PFLAG_D;
            break;
        case 0x38:
            uop->op = UOP_P_SET;
            uop->data = PFLAG_C;
            break;
        case 0x78:
            uop->op = UOP_P_SET;
            uop->data = PFLAG_I;
            break;
        case 0xf8:
            uop->op = UOP_P_SET;
            uop->data = PFLAG_D;
            break;

            /* Move */
        case 0xaa:
            uop->op = UOP_MOV;
            uop->data = MOV(REG_A, REG_X);
            break;
        case 0xa8:
            uop->op = UOP_MOV;
            uop->data = MOV(REG_A, REG_Y);
            break;
        case 0xba:
            uop->op = UOP_MOV_NOFLAG;
            uop->data = MOV(REG_S, REG_X);
            break;
        case 0x8a:
            uop->op = UOP_MOV;
            uop->data = MOV(REG_X, REG_A);
            break;
        case 0x9a:
            uop->op = UOP_MOV;
            uop->data = MOV(REG_X, REG_S);
            break;
        case 0x98:
            uop->op = UOP_MOV;
            uop->data = MOV(REG_Y, REG_A);
            break;

            /* Load */
        case 0xa1: case 0xa5: case 0xa9: case 0xad: case 0xb1: case 0xb5: case 0xb9: case 0xbd:
            uop->op = UOP_LOAD;
            uop->data = REG_A;
            break;
        case 0xa2: case 0xa6: case 0xae: case 0xb6: case 0xbe:
            uop->op = UOP_LOAD;
            uop->data = REG_X;
            break;
        case 0xa0: case 0xa4: case 0xac: case 0xb4: case 0xbc:
            uop->op = UOP_LOAD;
            uop->data = REG_Y;
            break;

            /* Store */
        case 0x81: case 0x85: case 0x89: case 0x8d: case 0x91: case 0x95: case 0x99: case 0x9d:
            uop->op = UOP_STORE;
            uop->data = REG_A;
            break;
        case 0x86: case 0x8e: case 0x96:
            uop->op = UOP_STORE;
            uop->data = REG_X;
            break;
        case 0x84: case 0x8c: case 0x94:
            uop->op = UOP_STORE;
            uop->data = REG_Y;
            break;

            /* Compare */
        case 0xc1: case 0xc5: case 0xc9: case 0xcd: case 0xd1: case 0xd5: case 0xd9: case 0xdd:
            uop->op = UOP_CMP;
            uop->data = REG_A;
            break;
        case 0xe0: case 0xe4: case 0xec:
            uop->op = UOP_CMP;
            uop->data = REG_X;
            break;
        case 0xc0: case 0xc4: case 0xcc:
            uop->op = UOP_CMP;
            uop->data = REG_Y;
            break;

            /* Add */
        case 0xe6: case 0xee: case 0xf6: case 0xfe:
            uop->op = UOP_ADD_MEM;
            uop->data = INC(0);
            break;
        case 0xc6: case 0xce: case 0xd6: case 0xde:
            uop->op = UOP_ADD_MEM;
            uop->data = DEC(0);
            break;
        case 0xe8:
            uop->op = UOP_ADD_REG;
            uop->data = INC(REG_X);
            break;
        case 0xc8:
            uop->op = UOP_ADD_REG;
            uop->data = INC(REG_Y);
            break;
        case 0xca:
            uop->op = UOP_ADD_REG;
            uop->data = DEC(REG_X);
            break;
        case 0x88:
            uop->op = UOP_ADD_REG;
            uop->data = DEC(REG_Y);
            break;

            /* Test (BIT) */
        case 0x24: case 0x2c:
            uop->op = UOP_TEST;
            break;

            /* Branch */
        case 0x10:
            uop->op = UOP_BRANCH_UNSET;
            uop->data = PFLAG_N;
            goto end;
        case 0x50:
            uop->op = UOP_BRANCH_UNSET;
            uop->data = PFLAG_V;
            goto end;
        case 0x90:
            uop->op = UOP_BRANCH_UNSET;
            uop->data = PFLAG_C;
            goto end;
        case 0xd0:
            uop->op = UOP_BRANCH_UNSET;
            uop->data = PFLAG_Z;
            goto end;
        case 0x30:
            uop->op = UOP_BRANCH_SET;
            uop->data = PFLAG_N;
            goto end;
        case 0x70:
            uop->op = UOP_BRANCH_SET;
            uop->data = PFLAG_V;
            goto end;
        case 0xb0:
            uop->op = UOP_BRANCH_SET;
            uop->data = PFLAG_C;
            goto end;
        case 0xf0:
            uop->op = UOP_BRANCH_SET;
            uop->data = PFLAG_Z;
            goto end;

            /* Jump */
        case 0x4c: case 0x6c:
            uop->op = UOP_JMP;
            goto end;
        case 0x20:
            uop->op = UOP_JSR;
            goto end;
        case 0x60:
            uop->op = UOP_RTS;
            goto end;
        case 0x40:
            uop->op = UOP_RTI;
            goto end;
        case 0x00:
            uop->op = UOP_BRK;
            goto end;

            /* ALU */
        case 0x01: case 0x05: case 0x09: case 0x0d: case 0x11: case 0x15: case 0x19: case 0x1d:
            uop->op = UOP_ORA;
            break;
        case 0x21: case 0x25: case 0x29: case 0x2d: case 0x31: case 0x35: case 0x39: case 0x3d:
            uop->op = UOP_AND;
            break;
        case 0x41: case 0x45: case 0x49: case 0x4d: case 0x51: case 0x55: case 0x59: case 0x5d:
            uop->op = UOP_EOR;
            break;
        case 0x61: case 0x65: case 0x69: case 0x6d: case 0x71: case 0x75: case 0x79: case 0x7d:
            uop->op = UOP_ADC;
            uop->data = 0x00;
            break;
        case 0xe1: case 0xe5: case 0xe9: case 0xed: case 0xf1: case 0xf5: case 0xf9: case 0xfd:
            uop->op = UOP_ADC;
            uop->data = 0xff;
            break;

            /* Shift */
        case 0x06: case 0x0e: case 0x16: case 0x1e:
            uop->op = UOP_ASL;
            break;
        case 0x0a:
            uop->op = UOP_ASL_REG;
            break;
        case 0x26: case 0x2e: case 0x36: case 0x3e:
            uop->op = UOP_ROL;
            break;
        case 0x2a:
            uop->op = UOP_ROL_REG;
            break;
        case 0x46: case 0x4e: case 0x56: case 0x5e:
            uop->op = UOP_LSR;
            break;
        case 0x4a:
            uop->op = UOP_LSR_REG;
            break;
        case 0x66: case 0x6e: case 0x76: case 0x7e:
            uop->op = UOP_ROR;
            break;
        case 0x6a:
            uop->op = UOP_ROR_REG;
            break;

            /* Stack */
        case 0x48:
            uop->op = UOP_PUSHA;
            break;
        case 0x08:
            uop->op = UOP_PUSHP;
            break;
        case 0x68:
            uop->op = UOP_POPA;
            break;
        case 0x28:
            uop->op = UOP_POPP;
            break;

            /* NOP */
        case 0xea:
            uop->op = UOP_NOP;
            break;

        default:
            printf("Opcode 0x%02x generated no uop, trace error?\n", op);
            getchar();
            break;
        }
    }

end:
    //printf("Generated new trace at 0x%04x (length: %d)\n", trace->pc, trace->length);
    return trace;
}

NinTrace* ninGetTrace(NinState* state, uint16_t addr)
{
    uint16_t index;
    NinTrace* trace;

    index = state->traceCache->index[addr];
    if (index != TRACE_NONE)
    {
        trace = state->traceCache->traces + index;
    }
    else
    {
        trace = ninBuildTrace(state, addr);
    }

    return trace;
}
