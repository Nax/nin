#include <nin/nin.h>
#include "uop.h"

static void ninEmitTraceUop(NinTrace* trace, uint8_t op, uint8_t len, uint16_t data)
{
    NinUop* uop;

    printf("uop: 0x%02x\n", op);

    uop = trace->uops + trace->length;
    trace->length++;
    uop->op = op;
    uop->len = len;
    uop->data = data;
}

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

static NinTrace* ninBuildTrace(NinState* state, uint16_t pc)
{
    NinTrace* trace;
    uint8_t op;
    uint8_t len;
    uint16_t prevLen;

    trace = ninAllocTrace(state, pc);
    prevLen = 0;

    while (trace->length < (TRACE_MAX_UOPS - 4))
    {
        len = 0;
        op = ninMemoryRead8(state, pc++);

        /* Decode the addressing mode */
        switch (op)
        {
        case 0x09: case 0x29: case 0x49: case 0x69: case 0x89: case 0xa9: case 0xc9: case 0xe9:
        case 0x0b: case 0x2b: case 0x4b: case 0x6b: case 0x8b: case 0xab: case 0xcb: case 0xeb:
        case 0x80: case 0xa0: case 0xc0: case 0xe0:
        case 0x82: case 0xa2: case 0xc2: case 0xe2:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_IMM, 0, ninMemoryRead8(state, pc++));
            break;
        case 0x0d: case 0x2d: case 0x4d: case 0x6d: case 0xad: case 0xcd: case 0xed:
        case 0x0e: case 0x2e: case 0x4e: case 0x6e: case 0xae: case 0xce: case 0xee:
        case 0x0f: case 0x2f: case 0x4f: case 0x6f: case 0xaf: case 0xcf: case 0xef:
        case 0x0c: case 0x2c: case 0xac: case 0xcc: case 0xec:
            len = 2;
            ninEmitTraceUop(trace, UOP_ADDR_ABS, 0, ninMemoryRead16(state, pc));
            pc += 2;
            break;
        case 0x8c: case 0x8d: case 0x8e: case 0x8f:
            len = 2;
            ninEmitTraceUop(trace, UOP_ADDR_ABS_NOREAD, 0, ninMemoryRead16(state, pc));
            pc += 2;
            break;
        case 0x1c: case 0x3c: case 0x5c: case 0x7c: case 0x9c: case 0xbc: case 0xdc: case 0xfc:
        case 0x1d: case 0x3d: case 0x5d: case 0x7d: case 0xbd: case 0xdd: case 0xfd:
        case 0x1e: case 0x3e: case 0x5e: case 0x7e: case 0xde: case 0xfe:
        case 0x1f: case 0x3f: case 0x5f: case 0x7f: case 0xdf: case 0xff:
            len = 2;
            ninEmitTraceUop(trace, UOP_ADDR_ABS_X, 0, ninMemoryRead16(state, pc));
            pc += 2;
            break;
        case 0x9d:
            len = 2;
            ninEmitTraceUop(trace, UOP_ADDR_ABS_X_NOREAD, 0, ninMemoryRead16(state, pc));
            pc += 2;
            break;
        case 0x19: case 0x39: case 0x59: case 0x79: case 0xb9: case 0xd9: case 0xf9:
        case 0x1b: case 0x3b: case 0x5b: case 0x7b: case 0x9b: case 0xbb: case 0xdb: case 0xfb:
        case 0x9e: case 0xbe:
        case 0x9f: case 0xbf:
            len = 2;
            ninEmitTraceUop(trace, UOP_ADDR_ABS_Y, 0, ninMemoryRead16(state, pc));
            pc += 2;
            break;
        case 0x99:
            len = 2;
            ninEmitTraceUop(trace, UOP_ADDR_ABS_Y_NOREAD, 0, ninMemoryRead16(state, pc));
            pc += 2;
            break;
        case 0x04: case 0x24: case 0x44: case 0x64: case 0xa4: case 0xc4: case 0xe4:
        case 0x05: case 0x25: case 0x45: case 0x65: case 0xa5: case 0xc5: case 0xe5:
        case 0x06: case 0x26: case 0x46: case 0x66: case 0xa6: case 0xc6: case 0xe6:
        case 0x07: case 0x27: case 0x47: case 0x67: case 0xa7: case 0xc7: case 0xe7:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x84: case 0x85: case 0x86: case 0x87:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_NOREAD, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x14: case 0x34: case 0x54: case 0x74: case 0xb4: case 0xd4: case 0xf4:
        case 0x15: case 0x35: case 0x55: case 0x75: case 0xb5: case 0xd5: case 0xf5:
        case 0x16: case 0x36: case 0x56: case 0x76: case 0xd6: case 0xf6:
        case 0x17: case 0x37: case 0x57: case 0x77: case 0xd7: case 0xf7:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_X, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x94: case 0x95:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_X_NOREAD, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0xb6:
        case 0xb7:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_Y, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x96:
        case 0x97:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_Y_NOREAD, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x01: case 0x21: case 0x41: case 0x61: case 0xa1: case 0xc1: case 0xe1:
        case 0x03: case 0x23: case 0x43: case 0x63: case 0xa3: case 0xc3: case 0xe3:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_X_INDIRECT, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x81:
        case 0x83:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_X_INDIRECT_NOREAD, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x11: case 0x31: case 0x51: case 0x71: case 0xb1: case 0xd1: case 0xf1:
        case 0x13: case 0x33: case 0x53: case 0x73: case 0xb3: case 0xd3: case 0xf3:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_Y_INDIRECT, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        case 0x91:
        case 0x93:
            len = 1;
            ninEmitTraceUop(trace, UOP_ADDR_ZERO_Y_INDIRECT_NOREAD, 0, ninMemoryRead8(state, pc));
            pc += 1;
            break;
        }

        /* Decode the body */
        switch (op)
        {
            /* Flags */
        case 0x18:
            ninEmitTraceUop(trace, UOP_P_UNSET, 1, PFLAG_C);
            break;
        case 0x58:
            ninEmitTraceUop(trace, UOP_P_UNSET, 1, PFLAG_I);
            break;
        case 0xb8:
            ninEmitTraceUop(trace, UOP_P_UNSET, 1, PFLAG_V);
            break;
        case 0xd8:
            ninEmitTraceUop(trace, UOP_P_UNSET, 1, PFLAG_D);
            break;
        case 0x38:
            ninEmitTraceUop(trace, UOP_P_SET, 1, PFLAG_C);
            break;
        case 0x78:
            ninEmitTraceUop(trace, UOP_P_SET, 1, PFLAG_I);
            break;
        case 0xf8:
            ninEmitTraceUop(trace, UOP_P_SET, 1, PFLAG_D);
            break;

            /* Move */
        case 0xaa:
            ninEmitTraceUop(trace, UOP_MOV, 1, REG_A | (REG_X << 2));
            break;
        case 0xa8:
            ninEmitTraceUop(trace, UOP_MOV, 1, REG_A | (REG_Y << 2));
            break;
        case 0xba:
            ninEmitTraceUop(trace, UOP_MOV_NOFLAG, 1, REG_S | (REG_X << 2));
            break;
        case 0x8a:
            ninEmitTraceUop(trace, UOP_MOV, 1, REG_X | (REG_A << 2));
            break;
        case 0x9a:
            ninEmitTraceUop(trace, UOP_MOV, 1, REG_X | (REG_S << 2));
            break;
        case 0x98:
            ninEmitTraceUop(trace, UOP_MOV, 1, REG_Y | (REG_A << 2));
            break;

            /* Load */
        case 0xa1: case 0xa5: case 0xa9: case 0xad: case 0xb1: case 0xb5: case 0xb9: case 0xbd:
            ninEmitTraceUop(trace, UOP_LOAD, len + 1, REG_A); // LDA
            break;
        case 0xa2: case 0xa6: case 0xae: case 0xb6: case 0xbe:
            ninEmitTraceUop(trace, UOP_LOAD, len + 1, REG_X); // LDX
            break;
        case 0xa0: case 0xa4: case 0xac: case 0xb4: case 0xbc:
            ninEmitTraceUop(trace, UOP_LOAD, len + 1, REG_Y); // LDY
            break;

            /* Store */
        case 0x81: case 0x85: case 0x89: case 0x8d: case 0x91: case 0x95: case 0x99: case 0x9d:
            ninEmitTraceUop(trace, UOP_STORE, len + 1, REG_A);
            break;
        case 0x86: case 0x8e: case 0x96:
            ninEmitTraceUop(trace, UOP_STORE, len + 1, REG_X);
            break;
        case 0x84: case 0x8c: case 0x94:
            ninEmitTraceUop(trace, UOP_STORE, len + 1, REG_Y);
            break;

            /* Compare */
        case 0xc1: case 0xc5: case 0xc9: case 0xcd: case 0xd1: case 0xd5: case 0xd9: case 0xdd:
            ninEmitTraceUop(trace, UOP_CMP, len + 1, REG_A);
            break;
        case 0xe0: case 0xe4: case 0xec:
            ninEmitTraceUop(trace, UOP_CMP, len + 1, REG_X);
            break;
        case 0xc0: case 0xc4: case 0xcc:
            ninEmitTraceUop(trace, UOP_CMP, len + 1, REG_Y);
            break;

            /* Add */
        case 0xe6: case 0xee: case 0xf6: case 0xfe:
            ninEmitTraceUop(trace, UOP_ADD_MEM, len + 1, 0x01);
            break;
        case 0xc6: case 0xce: case 0xd6: case 0xde:
            ninEmitTraceUop(trace, UOP_ADD_MEM, len + 1, 0xff);
            break;
        case 0xe8:
            ninEmitTraceUop(trace, UOP_ADD_REG, 1, 0x01 | (REG_X << 8));
            break;
        case 0xc8:
            ninEmitTraceUop(trace, UOP_ADD_REG, 1, 0x01 | (REG_Y << 8));
            break;
        case 0xca:
            ninEmitTraceUop(trace, UOP_ADD_REG, 1, 0xff | (REG_X << 8));
            break;
        case 0x88:
            ninEmitTraceUop(trace, UOP_ADD_REG, 1, 0xff | (REG_Y << 8));
            break;

            /* Branch */
        case 0x10:
            ninEmitTraceUop(trace, UOP_BRANCH_UNSET, 2, PFLAG_N | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0x50:
            ninEmitTraceUop(trace, UOP_BRANCH_UNSET, 2, PFLAG_V | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0x90:
            ninEmitTraceUop(trace, UOP_BRANCH_UNSET, 2, PFLAG_C | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0xd0:
            ninEmitTraceUop(trace, UOP_BRANCH_UNSET, 2, PFLAG_Z | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0x30:
            ninEmitTraceUop(trace, UOP_BRANCH_SET, 2, PFLAG_N | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0x70:
            ninEmitTraceUop(trace, UOP_BRANCH_SET, 2, PFLAG_V | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0xb0:
            ninEmitTraceUop(trace, UOP_BRANCH_SET, 2, PFLAG_C | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;
        case 0xf0:
            ninEmitTraceUop(trace, UOP_BRANCH_SET, 2, PFLAG_Z | ((uint16_t)ninMemoryRead8(state, pc++) << 8));
            goto end;

            /* Jump */
        case 0x4c:
            ninEmitTraceUop(trace, UOP_JMP, 3, ninMemoryRead16(state, pc));
            pc += 2;
            goto end;
        case 0x6c:
            ninEmitTraceUop(trace, UOP_JMP_INDIRECT, 3, ninMemoryRead16(state, pc));
            pc += 2;
            goto end;
        case 0x20:
            ninEmitTraceUop(trace, UOP_JSR, 3, ninMemoryRead16(state, pc));
            pc += 2;
            goto end;
        case 0x60:
            ninEmitTraceUop(trace, UOP_RTS, 1, 0);
            goto end;

            /* ALU */
        case 0x01: case 0x05: case 0x09: case 0x0d: case 0x11: case 0x15: case 0x19: case 0x1d:
            ninEmitTraceUop(trace, UOP_ORA, len + 1, 0);
            break;
        case 0x21: case 0x25: case 0x29: case 0x2d: case 0x31: case 0x35: case 0x39: case 0x3d:
            ninEmitTraceUop(trace, UOP_AND, len + 1, 0);
            break;
        case 0x41: case 0x45: case 0x49: case 0x4d: case 0x51: case 0x55: case 0x59: case 0x5d:
            ninEmitTraceUop(trace, UOP_EOR, len + 1, 0);
            break;
        case 0x61: case 0x65: case 0x69: case 0x6d: case 0x71: case 0x75: case 0x79: case 0x7d:
            ninEmitTraceUop(trace, UOP_ADC, len + 1, 0);
            break;
        case 0xe1: case 0xe5: case 0xe9: case 0xed: case 0xf1: case 0xf5: case 0xf9: case 0xfd:
            ninEmitTraceUop(trace, UOP_ADC, len + 1, 0xff);
            break;

            /* Shift */
        case 0x06: case 0x0e: case 0x16: case 0x1e:
            ninEmitTraceUop(trace, UOP_ASL, len + 1, 0);
            break;
        case 0x0a:
            ninEmitTraceUop(trace, UOP_ASL_REG, 1, 0);
            break;
        case 0x26: case 0x2e: case 0x36: case 0x3e:
            ninEmitTraceUop(trace, UOP_ROL, len + 1, 0);
            break;
        case 0x2a:
            ninEmitTraceUop(trace, UOP_ROL_REG, 1, 0);
            break;
        case 0x46: case 0x4e: case 0x56: case 0x5e:
            ninEmitTraceUop(trace, UOP_LSR, len + 1, 0);
            break;
        case 0x4a:
            ninEmitTraceUop(trace, UOP_LSR_REG, 1, 0);
            break;
        case 0x66: case 0x6e: case 0x76: case 0x7e:
            ninEmitTraceUop(trace, UOP_ROR, len + 1, 0);
            break;
        case 0x6a:
            ninEmitTraceUop(trace, UOP_ROR_REG, 1, 0);
            break;
        }

        /* Debug */
        if (trace->length == prevLen)
        {
            printf("Opcode 0x%02x generated no uop, trace error?\n", op);
            getchar();
        }
        prevLen = trace->length;
    }

end:
    printf("Generated new trace at 0x%04x (length: %d)\n", trace->pc, trace->length);
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
