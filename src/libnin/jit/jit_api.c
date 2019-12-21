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
    NinJitCodeIL il;
    ninJitMakeIL(state, &il, paddr);

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
