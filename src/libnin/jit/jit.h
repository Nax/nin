#ifndef LIBNIN_JIT_H
#define LIBNIN_JIT_H 1

#include <nin/nin.h>
#include <libnin/jit/jit_sym.h>
#include <libnin/jit/jit_instr.h>

#define JIT_ENTRY_CACHE_SIZE    1024

#define SYMOP_ADDR_IMM                      0x00
#define SYMOP_ADDR_ZEROPAGE                 0x01
#define SYMOP_ADDR_ZEROPAGE_INDIRECT_X      0x02
#define SYMOP_ADDR_ZEROPAGE_INDIRECT_Y      0x03

#define SYMOP_OP_ORA 0x20
#define SYMOP_OP_AND 0x21
#define SYMOP_OP_EOR 0x22
#define SYMOP_OP_ADC 0x23
#define SYMOP_OP_STA 0x24
#define SYMOP_OP_LDA 0x25
#define SYMOP_OP_CMP 0x26
#define SYMOP_OP_SBC 0x27

#define SYMOP_BRANCH 0x80

#define IL_MAX  1024

#define IL_TYPE_NIL     0x00
#define IL_TYPE_REG     0x01
#define IL_TYPE_VALUE   0x02
#define IL_TYPE_ADDR    0x03

#define IL_NIL          0x00
#define IL_REG(x)       (((x) << 2) | IL_TYPE_REG)
#define IL_VALUE(x)     (((x) << 2) | IL_TYPE_VALUE)
#define IL_ADDR(x)      ((((uintptr_t)(x)) << 2) | IL_TYPE_ADDR)

#define IL_OP_LOAD  0x00
#define IL_OP_STORE 0x01
#define IL_OP_OR    0x02
#define IL_OP_AND   0x03
#define IL_OP_XOR   0x04

typedef int (*NINJITFUNC)(NinState* state);

typedef struct
{
    uint8_t     op;
    uint16_t    addr;
} NinJitSymOp;

typedef struct
{
    uint32_t    paddr[JIT_ENTRY_CACHE_SIZE];
    NINJITFUNC  func[JIT_ENTRY_CACHE_SIZE];
} NinJitEntryCache;

typedef struct
{
    int reg;
    int death;
} NinJitVirtualReg;

typedef struct
{
    int         count;
    uint8_t     op[IL_MAX];
    uint64_t    arg1[IL_MAX];
    uint64_t    arg2[IL_MAX];
    int16_t     regs[IL_MAX];
    uint16_t    stackSize;
} NinJitCodeIL;

typedef struct
{
    NinJitEntryCache entryCache;
} NinJit;

typedef struct {
    char*   data;
    size_t  size;
    size_t  capacity;
} NinJitBuffer;

NIN_API NINJITFUNC  ninCompileIL(NinState* state, NinJitCodeIL* il);
NIN_API void        ninJitMakeIL(NinState* state, NinJitCodeIL* il, uint32_t paddr);
NIN_API void        ninJitInit(NinState* state);
NIN_API NINJITFUNC  ninJitGetFunction(NinState* state, uint16_t pc);

NIN_API void ninJitBufferCreate(NinJitBuffer* buffer);
NIN_API void ninJitBufferDestroy(NinJitBuffer* buffer);
NIN_API void ninJitBufferPush(NinJitBuffer* buffer, const char* data, size_t len);

#endif
