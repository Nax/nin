#include <string.h>
#include <libnin/libnin.h>
#include <libnin/jit/il.h>
#include <libnin/jit/jit.h>

FLAT_LIST_IMPL(NinJitIlList, ninJitIlList, NinJitIlInstr)

static uint64_t ninJitIL(NinJitCodeIL* il, uint8_t op, uint64_t arg1, uint64_t arg2)
{
    int i;

    i = il->count;
    il->op[il->count] = op;
    il->arg1[il->count] = arg1;
    il->arg2[il->count] = arg2;
    il->count++;

    return IL_REG(i);
}

static const char* kMnemonics[] = {
    "load",
    "store",
    "or",
    "and",
    "xor"
};

static void dumpRef(uint64_t ref)
{
    switch (ref & 0x3)
    {
    case 0:
        printf("nil");
        break;
    case 1:
        printf("%%%d", (int)(ref >> 2));
        break;
    case 2:
        printf("0x%08x", (uint32_t)(ref >> 2));
        break;
    case 3:
        printf("PTR 0x%06llx", (ref >> 2));
        break;
    }
}

static void dumpIL(const NinJitCodeIL* il)
{
    for (int i = 0; i < il->count; ++i)
    {
        printf("%%%d = %s(", i, kMnemonics[il->op[i]]);
        dumpRef(il->arg1[i]);
        printf(", ");
        dumpRef(il->arg2[i]);
        printf(") [%d];\n", il->regs[i]);
    }
}

static int getReg(uint64_t ref)
{
    if ((ref & 0x03) != 0x01)
        return -1;
    return ((int)ref >> 2);
}

static int compareVRegs(void const* a, void const* b)
{
    const NinJitVirtualReg* ra;
    const NinJitVirtualReg* rb;

    ra = (const NinJitVirtualReg*)a;
    rb = (const NinJitVirtualReg*)b;

    return ((int)(rb->death - rb->reg) - (int)(ra->death - ra->reg));
}

static int allocSingleReg(int count, int* min, int* max, int start, int end)
{
    for (int i = 0; i < count; ++i)
    {
        if (start >= max[i])
        {
            max[i] = end;
            return i + 1;
        }
        if (end <= min[i])
        {
            min[i] = start;
            return i + 1;
        }
    }
    return 0;
}

static void ninJitRegAlloc(NinJitCodeIL* il, int regCount)
{
    NinJitVirtualReg vRegs[IL_MAX];
    int r;
    int pregMin[32];
    int pregMax[32];
    int sregMin[512];
    int sregMax[512];

    memset(il->regs, 0, IL_MAX * 2);
    il->stackSize = 0;

    for (int i = 0; i < 32; ++i)
    {
        pregMin[i] = -1;
        pregMax[i] = -1;
    }

    for (int i = 0; i < 512; ++i)
    {
        sregMin[i] = -1;
        sregMax[i] = -1;
    }

    /* We compute the death of every virtual reg */
    for (int i = 0; i < il->count; ++i)
    {
        vRegs[i].reg = i;
        vRegs[i].death = i;
        r = getReg(il->arg1[i]);
        if (r != -1)
            vRegs[r].death = i;
        r = getReg(il->arg2[i]);
        if (r != -1)
            vRegs[r].death = i;
    }

    /* We sort by decreasing lifetime */
    /* Note: this is dubious, a simple linear scan seems to produce better results */
    // qsort(vRegs, il->count, sizeof(*vRegs), &compareVRegs);

    /* We assign registers */
    for (int i = 0; i < il->count; ++i)
    {
        if ((r = allocSingleReg(regCount, pregMin, pregMax, vRegs[i].reg, vRegs[i].death)))
            il->regs[vRegs[i].reg] = r;
        else
        {
            r = allocSingleReg(512, sregMin, sregMax, vRegs[i].reg, vRegs[i].death);
            il->regs[vRegs[i].reg] = -r;
            if (r > il->stackSize)
                il->stackSize = r;
        }
    }
}

#define PANIC(n)   do { printf("--- PANIC --- %s:%d (0x%02x)\n", __FILE__, __LINE__, (n)); fflush(stdout); getchar(); } while (0)

static uint64_t ninJitAddrRead(NinState* state, NinJitCodeIL* il, uint8_t addrMode, uint16_t addr, uint64_t x, uint64_t y)
{
    switch (addrMode)
    {
    case SYM_ADDR_IMM:
        return (((uint8_t)addr << 2) | IL_TYPE_VALUE);
    default:
        PANIC(addrMode);
        return 0;
    }
}

static int isBasicBlockTerminator(int op)
{
    switch (op)
    {
    case SYM_OP_BRK:
    case SYM_OP_JMP:
    case SYM_OP_JSR:
    case SYM_OP_RTS:
    case SYM_OP_RTI:
    case SYM_OP_BCC:
    case SYM_OP_BCS:
    case SYM_OP_BEQ:
    case SYM_OP_BMI:
    case SYM_OP_BNE:
    case SYM_OP_BPL:
    case SYM_OP_BVC:
    case SYM_OP_BVS:
        return 1;
    default:
        return 0;
    }
}

#define BAD_OP()  do { printf("--- JIT BAD OP: 0x%02x ---\n", sym->operation); fflush(stdout); getchar(); } while (0)

NIN_API void ninJitMakeIL(NinState* state, NinJitCodeIL* il, uint32_t paddr)
{
    uint64_t refValue;
    uint64_t refRegA;
    uint64_t refRegX;
    uint64_t refRegY;
    uint32_t paddrMax;
    int end;
    const SymInstr* sym;
    uint16_t addr;

    il->count = 0;
    paddrMax = (paddr & ~0x1fff) + 0x2000;
    end = 0;

    refRegA = ninJitIL(il, IL_OP_LOAD, IL_ADDR(&state->cpu.regs[REG_A]), IL_NIL);
    refRegX = ninJitIL(il, IL_OP_LOAD, IL_ADDR(&state->cpu.regs[REG_X]), IL_NIL);
    refRegY = ninJitIL(il, IL_OP_LOAD, IL_ADDR(&state->cpu.regs[REG_Y]), IL_NIL);

    for (;;)
    {
        if (end || il->count + 16 > IL_MAX || paddr + 3 > paddrMax)
            break;

        paddr += ninJitDecodeSym(state, &sym, &addr, paddr);

        switch (sym->operation)
        {
        case SYM_OP_LDA:
            refRegA = ninJitAddrRead(state, il, sym->addressingMode, addr, refRegX, refRegY);
            break;
        case SYM_OP_SEI:
            break;
        case SYM_OP_CLD:
            break;
        default:
            BAD_OP();
            break;
        }

        /* Check for basic block terminators */
        end = isBasicBlockTerminator(sym->operation);
    }

    ninJitIL(il, IL_OP_STORE, IL_ADDR(&state->cpu.regs[REG_Y]), refRegY);
    ninJitIL(il, IL_OP_STORE, IL_ADDR(&state->cpu.regs[REG_X]), refRegX);
    ninJitIL(il, IL_OP_STORE, IL_ADDR(&state->cpu.regs[REG_A]), refRegA);

    ninJitRegAlloc(il, 5);
    dumpIL(il);

    getchar();
}
