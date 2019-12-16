#include <string.h>
#include <libnin/libnin.h>
#include <libnin/jit/jit.h>
#include <windows.h>

#define GET_TYPE(x)     ((x) & 0x03)
#define GET_VALUE(x)    ((x) >> 2)

#define ARG_REG 1

#define R_RAX 0
#define R_RCX 1
#define R_RDX 2
#define R_RBX 3
#define R_RSP 4
#define R_RBP 5
#define R_RSI 6
#define R_RDI 7

#define REX_B   0x01
#define REX_X   0x02
#define REX_R   0x04
#define REX_W   0x08

#define STACK_INTERNAL  0x20

/* On Windows, RCX holds the arg */
static uint8_t kRegTable[] = {
    0, // RAX
    3, // RBX
    2, // RDX
    5, // RBP
    6, // RSI
    7  // RDI (scratch)
};

static uint8_t modrm(uint8_t mod, uint8_t reg, uint8_t rm)
{
    return ((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7);
}

static int emitIndexModRM(uint8_t* dst, uint8_t regBase, int32_t offset, uint8_t regValue)
{
    int i;
    int mod;

    i = 0;
    if (offset == 0)
    {
        mod = 0;
    }
    else if (offset >= -128 && offset <= 127)
    {
        mod = 1;
    }
    else
        mod = 2;
    dst[i++] = modrm(mod, regValue, regBase);
    if (regBase == R_RSP || regBase == R_RBP)
    {
        dst[i++] = 0x24;
    }
    switch (mod)
    {
    case 0:
        break;
    case 1:
        dst[i++] = (int8_t)offset;
        break;
    case 2:
        memcpy(dst + i, &offset, 4);
        i += 4;
        break;
    }

    return i;
}

static uint8_t rex(uint8_t mod)
{
    return (0x40 | (mod & 0x0f));
}

static void emitLoadRegRel8(NinJitBuffer* buffer, uint8_t reg, void* base, void* addr)
{
    int32_t diff;
    uint8_t tmp[7];

    if (reg >= 5)
        return;

    diff = (int32_t)((intptr_t)addr - (intptr_t)base);
    tmp[0] = 0x0f;
    tmp[1] = 0xb6;
    tmp[2] = modrm(2, kRegTable[reg - 1], ARG_REG);
    memcpy(tmp + 3, &diff, 4);
    ninJitBufferPush(buffer, tmp, 7);
}

static void emitStoreRegRel8(NinJitBuffer* buffer, uint8_t reg, void* base, void* addr)
{
    int32_t diff;
    uint8_t tmp[7];

    if (reg >= 5)
        return;

    diff = (int32_t)((intptr_t)addr - (intptr_t)base);
    tmp[0] = rex(0);
    tmp[1] = 0x88;
    tmp[2] = modrm(2, kRegTable[reg - 1], ARG_REG);
    memcpy(tmp + 3, &diff, 4);
    ninJitBufferPush(buffer, tmp, 7);
}

static void emitLoadReg64Off(NinJitBuffer* buffer, uint8_t regBase, int32_t offset, uint8_t regValue)
{
    uint8_t tmp[8];
    int i;

    i = 0;
    tmp[i++] = rex(REX_W);
    tmp[i++] = 0x8b;
    i += emitIndexModRM(tmp + i, regBase, offset, regValue);
    ninJitBufferPush(buffer, tmp, i);
}

static void emitStoreReg64Off(NinJitBuffer* buffer, uint8_t regBase, int32_t offset, uint8_t regValue)
{
    uint8_t tmp[8];
    int i;

    i = 0;
    tmp[i++] = rex(REX_W);
    tmp[i++] = 0x89;
    i += emitIndexModRM(tmp + i, regBase, offset, regValue);
    ninJitBufferPush(buffer, tmp, i);
}

static void emitAddReg64Imm(NinJitBuffer* buffer, uint8_t reg, uint32_t value)
{
    uint8_t tmp[7];

    tmp[0] = rex(REX_W);
    tmp[1] = 0x81;
    tmp[2] = modrm(3, 0, reg);
    memcpy(tmp + 3, &value, 4);
    ninJitBufferPush(buffer, tmp, 7);
}

static void emitSubReg64Imm(NinJitBuffer* buffer, uint8_t reg, uint32_t value)
{
    uint8_t tmp[7];

    tmp[0] = rex(REX_W);
    tmp[1] = 0x81;
    tmp[2] = modrm(3, 5, reg);
    memcpy(tmp + 3, &value, 4);
    ninJitBufferPush(buffer, tmp, 7);
}

static void emitRegMov32(NinJitBuffer* buffer, uint8_t srcReg, uint8_t dstReg)
{
    uint8_t tmp[2];

    tmp[0] = 0x89;
    tmp[1] = modrm(3, srcReg, dstReg);

    ninJitBufferPush(buffer, tmp, 2);
}

static void emitBasicImm(NinJitBuffer* buffer, uint8_t reg, int32_t imm, int op)
{
    uint8_t tmp[6];

    tmp[0] = 0x81;
    tmp[1] = modrm(3, op, reg);
    memcpy(tmp + 2, &imm, 4);
    ninJitBufferPush(buffer, tmp, 6);
}

static void emitBasicReg(NinJitBuffer* buffer, uint8_t reg1, uint8_t reg2, uint8_t op)
{
    uint8_t tmp[6];

    tmp[0] = op;
    tmp[1] = modrm(3, reg2, reg1);
    ninJitBufferPush(buffer, tmp, 2);
}

static void emitBasic(NinJitBuffer* buffer, const NinJitCodeIL* il, uint64_t a1, uint64_t a2, int16_t dst, uint8_t opMod, uint8_t opReg)
{
    int t1;
    int t2;
    intptr_t v1;
    intptr_t v2;
    uint64_t tmp;

    uint8_t reg1;
    uint8_t reg2;
    uint8_t regDest;


    t1 = GET_TYPE(a1);
    t2 = GET_TYPE(a2);
    regDest = kRegTable[dst - 1];

    if (t1 == IL_TYPE_VALUE)
    {
        // Immediate must come second
        tmp = a1;
        a1 = a2;
        a2 = tmp;

        t1 = GET_TYPE(a1);
        t2 = GET_TYPE(a2);
    }

    v1 = GET_VALUE(a1);
    v2 = GET_VALUE(a2);

    /* We have two possible cases to handle */
    /* Either we have an immediate, or we have two regs */
    if (t2 == IL_TYPE_VALUE)
    {
        /* Handle the immediate case */
        reg1 = kRegTable[il->regs[v1] - 1];
        if (reg1 != regDest)
        {
            /* The operation is in-place, we need to mov the value first */
            emitRegMov32(buffer, reg1, regDest);
        }
        emitBasicImm(buffer, regDest, v2, opMod);
    }
    else
    {
        /* Handle the r/r case */
        reg1 = kRegTable[il->regs[v1] - 1];
        reg2 = kRegTable[il->regs[v2] - 1];

        /* if reg2 is the dst, we swap */
        if (reg2 == regDest)
        {
            tmp = reg1;
            reg1 = reg2;
            reg2 = tmp;
        }

        /* If reg1 is still not the dst, we need to insert a mov */
        if (reg1 != regDest)
            emitRegMov32(buffer, reg1, regDest);
        emitBasicReg(buffer, regDest, reg2, opReg);
    }
}

static void emitRet(NinJitBuffer* buffer)
{
    uint8_t tmp;

    tmp = 0xc3;
    ninJitBufferPush(buffer, &tmp, 1);
}

static NINJITFUNC jitFinalize(NinJitBuffer* buffer)
{
    char* data;
    DWORD old;

    data = VirtualAlloc(NULL, buffer->size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(data, buffer->data, buffer->size);
    VirtualProtect(data, buffer->size, PAGE_EXECUTE_READ, &old);
    FlushInstructionCache(GetCurrentProcess(), data, buffer->size);
    ninJitBufferDestroy(buffer);

    return (NINJITFUNC)data;
}

NIN_API NINJITFUNC ninCompileIL(NinState* state, NinJitCodeIL* il)
{
    NinJitBuffer buffer;
    uint32_t stackSize;

    ninJitBufferCreate(&buffer);

    /* Prologue */
    stackSize = STACK_INTERNAL + il->stackSize;
    emitSubReg64Imm(&buffer, R_RSP, stackSize);
    emitStoreReg64Off(&buffer, R_RSP, 0x00, R_RBX);
    emitStoreReg64Off(&buffer, R_RSP, 0x08, R_RBP);
    emitStoreReg64Off(&buffer, R_RSP, 0x10, R_RDI);
    emitStoreReg64Off(&buffer, R_RSP, 0x18, R_RSI);

    for (int i = 0; i < il->count; ++i)
    {
        switch (il->op[i])
        {
        case IL_OP_LOAD:
            emitLoadRegRel8(&buffer, il->regs[i], state, (void*)GET_VALUE(il->arg1[i]));
            break;
        case IL_OP_STORE:
            emitStoreRegRel8(&buffer, il->regs[i], state, (void*)GET_VALUE(il->arg1[i]));
            break;
        case IL_OP_OR:
            emitBasic(&buffer, il, il->arg1[i], il->arg2[i], il->regs[i], 1, 0x09);
            break;
        case IL_OP_AND:
            emitBasic(&buffer, il, il->arg1[i], il->arg2[i], il->regs[i], 4, 0x21);
            break;
        case IL_OP_XOR:
            emitBasic(&buffer, il, il->arg1[i], il->arg2[i], il->regs[i], 6, 0x31);
            break;
        }
    }

    /* Epilogue */
    emitLoadReg64Off(&buffer, R_RSP, 0x00, R_RBX);
    emitLoadReg64Off(&buffer, R_RSP, 0x08, R_RBP);
    emitLoadReg64Off(&buffer, R_RSP, 0x10, R_RDI);
    emitLoadReg64Off(&buffer, R_RSP, 0x18, R_RSI);
    emitAddReg64Imm(&buffer, R_RSP, stackSize);
    emitRet(&buffer);

    return jitFinalize(&buffer);
}
