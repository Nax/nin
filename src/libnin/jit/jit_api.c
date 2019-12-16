#include <libnin/libnin.h>
#include <libnin/jit/jit.h>

#define JIT_BADADDR 0xffffffff

void ninJitInit(NinState* state)
{
    NinJit* jit;

    jit = &state->jit;
    for (int i = 0; i < JIT_ENTRY_CACHE_SIZE; ++i)
    {
        jit->entryCache.paddr[i] = JIT_BADADDR;
    }
}

static NINJITFUNC ninJitCompile(NinState* state, uint32_t paddr)
{
    NinJitSymOp ops[256];
    uint32_t max;
    int oldCount;
    int count;

    max = (paddr & ~0x1fff) + 0x2000;
    count = 0;

    printf("=== JIT AT 0x%08x ===\n", paddr);
    for (;;)
    {
        if (paddr + 2 >= max || (count + 5) >= 256)
            break;
        oldCount = count;

        switch (state->prgRom[paddr])
        {
#define X(sym, size)    do { ops[count++].op = sym; } while (0)
#include "jit_sym.inc"
#undef X
        }

        paddr++;
        for (int i = oldCount; i < count; ++i)
        {
            switch (ops[i].op)
            {
            case SYMOP_ADDR_IMM:
            case SYMOP_ADDR_ZEROPAGE:
            case SYMOP_ADDR_ZEROPAGE_INDIRECT_X:
            case SYMOP_ADDR_ZEROPAGE_INDIRECT_Y:
                ops[i].addr = state->prgRom[paddr];
                paddr++;
                break;
            default:
                ops[i].addr = 0;
                break;
            }
        }
    }

    for (int i = 0; i < count; ++i)
    {
        printf("SYMOP: 0x%02x 0x%04x\n", ops[i].op, ops[i].addr);
    }
    fflush(stdout);

    NinJitCodeIL il;
    ninJitMakeIL(state, &il, ops, count);

    return ninCompileIL(state, &il);
}

NINJITFUNC ninJitGetFunction(NinState* state, uint16_t pc)
{
    NinJit* jit;
    uint8_t bankSlot;
    uint32_t paddr;

    if (pc < 0x8000)
        return NULL;

    jit = &state->jit;
    bankSlot = (pc & 0x7fff) / 0x2000;
    paddr = (state->prgRomBankIndex[bankSlot] * 0x2000) | (pc & 0x1fff);

    for (int i = 0; i < JIT_ENTRY_CACHE_SIZE; ++i)
    {
        if (jit->entryCache.paddr[i] != JIT_BADADDR)
            return jit->entryCache.func[i];
    }

    printf("PC: 0x%04x\n", pc);
    return ninJitCompile(state, paddr);
}
