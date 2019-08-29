#include <libnin/libnin.h>
#include "uop.h"

#define INLINE __forceinline

static void flagZ(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_Z;
    state->cpu.p |= ((!value) << 1);
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

static uint8_t compare(NinState* state, uint8_t a, uint8_t b)
{
    state->cpu.p &= ~(PFLAG_C);
    if (a >= b)
        state->cpu.p |= PFLAG_C;
    return a - b;
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

static uint16_t indirect(NinState* state, uint16_t addr)
{
    uint8_t hi;
    uint8_t lo0;
    uint8_t lo1;
    uint8_t hh;
    uint8_t ll;

    hi = addr >> 8;
    lo0 = addr & 0xff;
    lo1 = lo0 + 1;

    ll = ninMemoryRead8(state, ((uint16_t)hi << 8) | lo0);
    hh = ninMemoryRead8(state, ((uint16_t)hi << 8) | lo1);

    return ((uint16_t)hh << 8) | ll;
}

static const uint8_t kHex[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15 };
static const uint8_t kBranchFlags[] = { PFLAG_N, PFLAG_V, PFLAG_C, PFLAG_Z };

#define X(n, str, code) do { if ((kHex[str[(n) / 4] - '0']) & (1 << ((n) % 4))) code } while (0)
#define CYCLE()         do { cyc++; ninPpuRunCycles(state, 3); } while (0)
#define READ(x)         do { tmp = ninMemoryRead8(state, (x)); } while (0)

#define INSTRUCTION(n)                                                                                                                                          \
do                                                                                                                                                              \
{                                                                                                                                                               \
    tmp = addr = r0 = r1 = i = 0;                                                                                                                               \
    switch (n)                                                                                                                                                  \
    {                                                                                                                                                           \
    case 0x18: state->cpu.p &= ~PFLAG_C; break;                                                                                                                 \
    case 0x58: state->cpu.p &= ~PFLAG_I; break;                                                                                                                 \
    case 0xb8: state->cpu.p &= ~PFLAG_V; break;                                                                                                                 \
    case 0xc8: state->cpu.p &= ~PFLAG_D; break;                                                                                                                 \
                                                                                                                                                                \
    case 0x38: state->cpu.p |= PFLAG_C; break;                                                                                                                  \
    case 0x78: state->cpu.p |= PFLAG_I; break;                                                                                                                  \
    case 0xf8: state->cpu.p |= PFLAG_D; break;                                                                                                      \
    default: break;                                                                                                                                 \
    }                                                                                                                                               \
                                                                                                                                                    \
    X(n, "0000000000000000000000000000000000000000000000002222222200000000", { r0 = REG_A; });                                                      \
    X(n, "0000000000000000000000000000000000000000000000000040000011110000", { r0 = REG_X; });                                                      \
    X(n, "0000000000000000000000000000000000000000000000001111100000000000", { r0 = REG_Y; });                                                      \
    X(n, "0000000000000000000000000000000000000000222222220000000000000000", { r1 = REG_A; });                                                      \
    X(n, "0000000000000000000000000000000000000000444404440040000000100000", { r1 = REG_X; });                                                      \
    X(n, "0000000000000000000000000000000000000000111101010010000000000000", { r1 = REG_Y; });                                                              \
    X(n, "2000060620000606200006062000060620000302200003032000060620000606", { i = state->cpu.regs[REG_X]; });                          /* index-X */       \
    X(n, "0000002000000020000000200000002000000420000004240000002000000020", { i = state->cpu.regs[REG_Y]; });                          /* index-Y */       \
    X(n, "2626362637273626262736262627362627073722772737273727362637273626", { tmp = ninMemoryRead8(state, pc++); addr = tmp; });       /* op1 */           \
    X(n, "2006202630072026200720262007202620072022200720272007202620072026", { CYCLE(); READ(pc++); addr |= ((uint16_t)tmp << 8); });   /* op2 */           \
    X(n, "2000060020000600200006002000060020000300200003002000060020000600", { CYCLE(); addr = (addr + i) & 0xff; });                   /* zero-index */    \
    X(n, "0000002600000026000000260000002600000022000000270000002600000026", { CYCLE(); addr += i; });                                  /* abs-index */     \
    X(n, "2000200020002000200020002001200020002000200020002000200020002000", { CYCLE(); addr = indirect(state, addr); });               /* indirect */      \
    X(n, "0000200000002000000020000000200000002000000020000000200000002000", { CYCLE(); addr += state->cpu.regs[REG_Y]; });             /* post-index */    \
    X(n, "2606262627072626260626262606262600000000270727272707262627072626", { CYCLE(); READ(addr); });                                 /* mem */           \
    X(n, "0000000000000000000000000000000000000000000000000000000000000000", { tmp = state->cpu.regs[r0]; });                           /* reg -> tmp */    \
    X(n, "2222222200000000000000000000000000000000000000000000000000000000", { tmp = state->cpu.regs[REG_A] | tmp; });                  /* ora */           \
    X(n, "0000000022222222000000000000000000000000000000000000000000000000", { tmp = state->cpu.regs[REG_A] & tmp; });                  /* and */           \
    X(n, "0000000000000000222222220000000000000000000000000000000000000000", { tmp = state->cpu.regs[REG_A] ^ tmp; });                  /* eor */           \
    X(n, "0000000000000000000000002222222200000000000000000000000000000000", { tmp = _adc(state, state->cpu.regs[REG_A], tmp); });      /* adc */           \
    X(n, "0000000000000000000000000000000000000000000000000010000004140404", { tmp++; });                                               /* inc */           \
    X(n, "0000000000000000000000000000000000100000000000000444040400000000", { tmp--; });                                               /* dec */           \
    X(n, "0000000000000000000000000000000027072722000000000404040404040404", { CYCLE(); ninMemoryWrite8(state, addr, tmp); });          /* store */         \
    X(n, "0000000000000000000000000000000000100000777727670000000000000000", { state->cpu.regs[r1] = tmp; });                           /* load */          \
    X(n, "0000000000000000000000000000000000000000000000000000000022222222", { tmp = _adc(state, state->cpu.regs[REG_A], ~tmp); });     /* sbc */           \
    X(n, "0000000000000000000000000000000000000000000000003323222211010000", { tmp = compare(state, state->cpu.regs[r0], tmp); });      /* cmp */           \
    X(n, "2666266627672666266626662666005000107777276737777777262637372626", { flagZ(state, tmp); });                                   /* Z */             \
    X(n, "2666266627672666266626662666005000107777276737777777262637372626", { flagN(state, tmp); });                                   /* N */             \
    X(n, "0000000000000000100000000000000000000000000000000000000000000000", { state->cpu.p = (stackPop8(state) & PFLAG_MASK); pc = stackPop16(state); }); /* rti */    \
    X(n, "0000000000000000000000001000000000000000000000000000000000000000", { pc = stackPop16(state) + 1; }); /* rts */        \
    X(n, "0000000010000000000000000000000000000000000000000000000000000000", { stackPush16(state, pc - 1); }); /* jsr */        \
    X(n, "0000000010000000000100000001000000000000000000000000000000000000", { pc = addr; }); /* jmp */                         \
    X(n, "0000100000000000000010000000000000001000000000000000100000000000", { if (!(state->cpu.p & kBranchFlags[(n) >> 6])) pc += (int8_t)tmp; }); /* branch-clear */ \
    X(n, "0000000000001000000000000000100000000000000010000000000000001000", { if ( (state->cpu.p & kBranchFlags[(n) >> 6])) pc += (int8_t)tmp; }); /* branch-set */   \
}                                                                                                                               \
while (0)

#define ICASE1(n)    case n: INSTRUCTION(n); break
#define ICASE2(n)   ICASE1(n + 0x00); ICASE1(n + 0x01); ICASE1(n + 0x02); ICASE1(n + 0x03)
#define ICASE3(n)   ICASE2(n + 0x00); ICASE2(n + 0x04); ICASE2(n + 0x08); ICASE2(n + 0x0c)
#define ICASE4(n)   ICASE3(n + 0x00); ICASE3(n + 0x10); ICASE3(n + 0x20); ICASE3(n + 0x30)
#define EXECUTE()   ICASE4()

void ninRunFrameCPU(NinState* state)
{
    uint16_t pc;
    uint16_t addr;
    uint8_t tmp;
    uint8_t op;
    uint8_t r0;
    uint8_t r1;
    uint8_t i;
    uint16_t cyc;

    pc = state->cpu.pc;
    cyc = 0;

    for (;;)
    {
        /* TODO: Refactor this */
        if (state->nmi)
        {
            state->nmi = 0;
            stackPush16(state, pc);
            stackPush8(state, state->cpu.p);
            printf("--- NMI ---\n");
            fflush(stdout);
            getchar();
            pc = ninMemoryRead16(state, 0xfffa);
            state->cpu.pc = pc;
        }

        cyc += 2;
        printf("CYC:%05d PC:0x%04x ", cyc, pc);
        cyc++;
        op = ninMemoryRead8(state, pc++);
        printf("OP:0x%02x A:0x%02x X:0x%02x Y:0x%02x S:0x%02x P:0x%02x\n", op, state->cpu.regs[REG_A], state->cpu.regs[REG_X], state->cpu.regs[REG_Y], state->cpu.regs[REG_S], state->cpu.p);
        fflush(stdout);
        //getchar();

        switch (op)
        {
            ICASE4(0x00);
            ICASE4(0x40);
            ICASE4(0x80);
            ICASE4(0xc0);
        }

        if (ninPpuRunCycles(state, 3))
            break;
    }

    state->cpu.pc = pc;
}
