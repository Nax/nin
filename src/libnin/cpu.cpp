#include <libnin/libnin.h>

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

static uint8_t asl(NinState* state, uint8_t v)
{
    state->cpu.p &= ~PFLAG_C;
    if (v & 0x80) state->cpu.p |= PFLAG_C;

    return (v << 1);
}

static uint8_t rol(NinState* state, uint8_t v)
{
    uint8_t carry;

    carry = (state->cpu.p & PFLAG_C) ? 0x01 : 0x00;
    state->cpu.p &= ~PFLAG_C;
    if (v & 0x80) state->cpu.p |= PFLAG_C;

    return ((v << 1) | carry);
}

static uint8_t lsr(NinState* state, uint8_t v)
{
    state->cpu.p &= ~PFLAG_C;
    if (v & 0x01) state->cpu.p |= PFLAG_C;

    return (v >> 1);
}

static uint8_t ror(NinState* state, uint8_t v)
{
    uint8_t carry;

    carry = (state->cpu.p & PFLAG_C) ? 0x80 : 0x00;
    state->cpu.p &= ~PFLAG_C;
    if (v & 0x01) state->cpu.p |= PFLAG_C;

    return ((v >> 1) | carry);
}

static uint8_t bit(NinState* state, uint8_t v)
{
    state->cpu.p &= ~(0xc0);
    state->cpu.p |= (v & 0xc0);

    return state->cpu.regs[REG_A] & v;
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

static constexpr const uint8_t kHex[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15 };
static constexpr const uint8_t kBranchFlags[] = { PFLAG_N, PFLAG_V, PFLAG_C, PFLAG_Z };

static constexpr const bool matchPattern(const char* pattern, uint8_t value)
{
    return !!((kHex[pattern[value / 4] - '0']) & (1 << (value % 4)));
}

#define X(str)          if (matchPattern(str, N))
#define CYCLE()         do { state->cyc++; state->frame |= ninPpuRunCycles(state, 3); ninRunCyclesAPU(state, 1); } while (0)
#define READ(x)         do { tmp = ninMemoryRead8(state, (x)); } while (0)

template <uint8_t N>
static void instruction(NinState* state)
{
    uint8_t tmp;
    uint16_t addr;
    uint8_t r0;
    uint8_t r1;
    uint8_t i;

    tmp = 0;
    addr = 0;
    r0 = 0;
    r1 = 0;
    i = 0;

    switch (N)
    {
    case 0x18: state->cpu.p &= ~PFLAG_C; break;
    case 0x58: state->cpu.p &= ~PFLAG_I; break;
    case 0xb8: state->cpu.p &= ~PFLAG_V; break;
    case 0xd8: state->cpu.p &= ~PFLAG_D; break;

    case 0x38: state->cpu.p |= PFLAG_C; break;
    case 0x78: state->cpu.p |= PFLAG_I; break;
    case 0xf8: state->cpu.p |= PFLAG_D; break;
    default: break;
    }

    X("0040000000400000005000000040000022222222005000002222222200000000") { r0 = REG_A; }
    X("0000000000000000000000000000000004440440000000000040000011110000") { r0 = REG_X; }
    X("0000000000000000000000000000000001110110000000001111100000000000") { r0 = REG_Y; }
    X("0000000000000000000000000000000000000000000000400000000000000000") { r0 = REG_S; }
    X("2262222222622222226222222272222200440010222222220000000022222222") { r1 = REG_A; }
    X("0000000000000000000000000000000000000000444404440040000000100000") { r1 = REG_X; }
    X("0000000000000000000000000000000000100000111101010010000000000000") { r1 = REG_Y; }
    X("0000000000000000000000000000000000000060000000000000000000000000") { r1 = REG_S; }
    X("2000060620000606200006062000060620000302200003032000060620000606") { i = state->cpu.regs[REG_X]; }                          /* index-X */
    X("0000002000000020000000200000002000000420000004240000002000000020") { i = state->cpu.regs[REG_Y]; }                          /* index-Y */
    X("2626362637273626262736262627362627073722772737273727362637273626") { tmp = ninMemoryRead8(state, state->cpu.pc++); addr = tmp; }       /* op1 */
    X("0006002610070026000700260007002600070022000700270007002600070026") { CYCLE(); READ(state->cpu.pc++); addr |= ((uint16_t)tmp << 8); }   /* op2 */
    X("2000060020000600200006002000060020000700200027002000060020000600") { CYCLE(); addr = (addr + i) & 0xff; }                   /* zero-index */
    X("0000002600000026000000260000002600000022000000270000002600000026") { CYCLE(); addr += i; }                                  /* abs-index */
    X("2000200020002000200020002001200020002000200020002000200020002000") { CYCLE(); addr = indirect(state, addr); }               /* indirect */
    X("0000200000002000000020000000200000002000000020000000200000002000") { CYCLE(); addr += state->cpu.regs[REG_Y]; }             /* post-index */
    X("2606262627072626260626262606262600000000270727272707262627072626") { CYCLE(); READ(addr); }                                 /* mem */
    X("0040000000400000005000000040000027772772005000400050000000100000") { tmp = state->cpu.regs[r0]; }                           /* reg -> tmp */
    X("0010000000000000000000000000000000000000000000000000000000000000") { tmp = (state->cpu.p | ~PFLAG_MASK); }                  /* flags -> tmp */
    X("2222222200000000000000000000000000000000000000000000000000000000") { tmp = state->cpu.regs[REG_A] | tmp; }                  /* ora */
    X("0000000022222222000000000000000000000000000000000000000000000000") { tmp = state->cpu.regs[REG_A] & tmp; }                  /* and */
    X("0000000000000000222222220000000000000000000000000000000000000000") { tmp = state->cpu.regs[REG_A] ^ tmp; }                  /* eor */
    X("0000000000000000000000002222222200000000000000000000000000000000") { tmp = _adc(state, state->cpu.regs[REG_A], tmp); }      /* adc */
    X("0000000000000000000000000000000000000000000000000000000022222222") { tmp = _adc(state, state->cpu.regs[REG_A], ~tmp); }     /* sbc */
    X("0000000000000000000000000000000000000000000000000010000004140404") { tmp++; }                                               /* inc */
    X("0000000000000000000000000000000000100000000000000444040400000000") { tmp--; }                                               /* dec */
    X("0444040400000000000000000000000000000000000000000000000000000000") { tmp = asl(state, tmp); }                               /* asl */
    X("0000000004440404000000000000000000000000000000000000000000000000") { tmp = rol(state, tmp); }                               /* rol */
    X("0000000000000000044404040000000000000000000000000000000000000000") { tmp = lsr(state, tmp); }                               /* lsr */
    X("0000000000000000000000000444040400000000000000000000000000000000") { tmp = ror(state, tmp); }                               /* ror */
    X("0000000001010000000000000000000000000000000000000000000000000000") { tmp = bit(state, tmp); }                               /* bit */
    X("0010000000000000001000000000000000000000000000000000000000000000") { stackPush8(state, tmp); }                              /* push */
    X("0000000000100000000000000010000000000000000000000000000000000000") { tmp = stackPop8(state); }                              /* pop */
    X("0404040404040404040404040404040427072722000000000404040404040404") { CYCLE(); ninMemoryWrite8(state, addr, tmp); }          /* store */
    X("2262222222622222226222222272222200500050777727670050000022322222") { state->cpu.regs[r1] = tmp; }                           /* load */
    X("0000000000100000000000000000000000000000000000000000000000000000") { state->cpu.p = (tmp & PFLAG_MASK); }                   /* tmp -> flags */
    X("0000000000000000000000000000000000000000000000003323222211010000") { tmp = compare(state, state->cpu.regs[r0], tmp); }      /* cmp */
    X("2666266727672666266626662676042600706737777727777777262637372626") { flagZ(state, tmp); }                                   /* Z */
    X("2666266726662666266626662676042600706737777727777777262637372626") { flagN(state, tmp); }                                   /* N */
    X("0000000000000000100000000000000000000000000000000000000000000000") { state->cpu.p = (stackPop8(state) & PFLAG_MASK); state->cpu.pc = stackPop16(state); } /* rti */
    X("0000000000000000000000001000000000000000000000000000000000000000") { state->cpu.pc = stackPop16(state) + 1; } /* rts */
    X("0000000010000000000000000000000000000000000000000000000000000000") { stackPush16(state, state->cpu.pc - 1); } /* jsr */
    X("0000000010000000000100000001000000000000000000000000000000000000") { state->cpu.pc = addr; } /* jmp */
    X("0000100000000000000010000000000000001000000000000000100000000000") { if (!(state->cpu.p & kBranchFlags[N >> 6])) state->cpu.pc += (int8_t)tmp; } /* branch-clear */
    X("0000000000001000000000000000100000000000000010000000000000001000") { if ( (state->cpu.p & kBranchFlags[N >> 6])) state->cpu.pc += (int8_t)tmp; } /* branch-set */
}

#define ICASE1(n)   case n: instruction<(n)>(state); break
#define ICASE2(n)   ICASE1(n + 0x00); ICASE1(n + 0x01); ICASE1(n + 0x02); ICASE1(n + 0x03)
#define ICASE3(n)   ICASE2(n + 0x00); ICASE2(n + 0x04); ICASE2(n + 0x08); ICASE2(n + 0x0c)
#define ICASE4(n)   ICASE3(n + 0x00); ICASE3(n + 0x10); ICASE3(n + 0x20); ICASE3(n + 0x30)
#define EXECUTE()   ICASE4()

NIN_API void ninRunFrameCPU(NinState* state)
{
    uint8_t op;

    state->frame = 0;
    for (;;)
    {
        if (state->nmi)
        {
            state->nmi = 0;
            stackPush16(state, state->cpu.pc);
            stackPush8(state, state->cpu.p);
            state->cpu.pc = ninMemoryRead16(state, 0xfffa);
        }

        op = ninMemoryRead8(state, state->cpu.pc++);
        state->cyc += 2;
        ninRunCyclesAPU(state, 2);
        state->frame = ninPpuRunCycles(state, 6);

        switch (op)
        {
        ICASE4(0x00);
        ICASE4(0x40);
        ICASE4(0x80);
        ICASE4(0xc0);
        }

        if (state->frame)
            break;
    }
}
