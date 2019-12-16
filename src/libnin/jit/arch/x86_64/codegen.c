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

static void emitLoadRel8(NinJitBuffer* buffer, uint8_t reg, void* base, void* addr)
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

static void emitAddReg64Imm(NinJitBuffer* buffer, uint8_t reg, uint32_t value)
{
    uint8_t tmp[7];

    tmp[0] = 0x48;
    tmp[1] = 0x81;
    tmp[2] = modrm(3, 0, reg);
    memcpy(tmp + 3, &value, 4);
    ninJitBufferPush(buffer, tmp, 7);
}

static void emitSubReg64Imm(NinJitBuffer* buffer, uint8_t reg, uint32_t value)
{
    uint8_t tmp[7];

    tmp[0] = 0x48;
    tmp[1] = 0x81;
    tmp[2] = modrm(3, 5, reg);
    memcpy(tmp + 3, &value, 4);
    ninJitBufferPush(buffer, tmp, 7);
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

    ninJitBufferCreate(&buffer);
    /* Prologue */
    emitSubReg64Imm(&buffer, R_RSP, 0x100);
    for (int i = 0; i < il->count; ++i)
    {
        switch (il->op[i])
        {
        case IL_OP_LOAD:
            emitLoadRel8(&buffer, il->regs[i], state, (void*)GET_VALUE(il->arg1[i]));
            break;
        }
    }
    /* Epilogue */
    emitAddReg64Imm(&buffer, R_RSP, 0x100);
    emitRet(&buffer);
    return jitFinalize(&buffer);
}
