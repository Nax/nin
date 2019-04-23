#include <nin/nin.h>

static const uint8_t kHexDecode[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15
};

#define t(op, str) if (kHexDecode[(uint8_t)str[op / 4]] & (1ull << (op & 3)))

#define EXEC_OP(op)     \
    t(op, "00a0000000a0000000a0000000a0000050a0000050a0000050a0000050a00000") { tmp = ninMemoryRead8(state, state->cpu.pc++); }                     /* Imm */           \
    t(op, "000f00af100f00af000f00af000f00af000f00af000f00af000f00af000f00af") { addr = ninMemoryRead16(state, state->cpu.pc); state->cpu.pc += 2; } /* Abs */           \
    t(op, "000f00af100f00af000f00af000f00af000f00af000f00af000f00af000f00af") { tmp = ninMemoryRead8(state, addr); }                                /* tmp <- addr */   \
    t(op, "2222222200000000000000000000000000000000000000000000000000000000") { tmp |= state->cpu.a; }                                              /* ORA */           \
    t(op, "0000000022222222000000000000000000000000000000000000000000000000") { tmp &= state->cpu.a; }                                              /* AND */           \
    t(op, "0000000000000000222222220000000000000000000000000000000000000000") { tmp ^= state->cpu.a; }                                              /* EOR */           \
    t(op, "0000000000000000000000002222222200000000000000000000000000000000") { tmp = _adc(state, state->cpu.a, tmp); }                             /* ADC */           \
    t(op, "0000000000000000000000000000000000000000000000000000000022222222") { tmp = _adc(state, state->cpu.a, ~tmp); }                            /* SBC */           \
    t(op, "2222222222222222222222222222222200000000000000000000000022222222") { state->cpu.a = tmp; }                                               /* A <- tmp */      \
    ;

#define CASE_OP1(x)     \
    case x:             \
        EXEC_OP(x);     \
        break

#define CASE_OP4(x)     CASE_OP1(x + 0); CASE_OP1(x + 1); CASE_OP1(x + 2); CASE_OP1(x + 3)
#define CASE_OP16(x)    CASE_OP4(x + 0); CASE_OP4(x + 4); CASE_OP4(x + 8); CASE_OP4(x + 12)
#define CASE_OP64(x)    CASE_OP16(x + 0); CASE_OP16(x + 16); CASE_OP16(x + 32); CASE_OP16(x + 48)
#define CASE_OP256(x)   CASE_OP64(x + 0); CASE_OP64(x + 64); CASE_OP64(x + 128); CASE_OP64(x + 192)

static uint8_t _adc(NinState* state, uint8_t a, uint8_t b)
{
    uint16_t sum;
    uint8_t carryIn;
    uint8_t carryOut;
    uint8_t tmp;

    carryIn = state->cpu.p & 0x01;
    sum = ((uint16_t)a) + b + carryIn;
    carryOut = sum >> 8;
    tmp = sum & 0xff;
    state->cpu.p &= ~(PFLAG_C | PFLAG_V);
    state->cpu.p |= carryOut;
    state->cpu.p |= (carryOut ^ carryIn) << 6;
    return tmp;
}

void ninRunCycles(NinState* state, size_t cycles)
{
    uint8_t     op;
    uint8_t     tmp;
    uint16_t    addr;

    while (cycles--)
    {
        op = ninMemoryRead8(state, state->cpu.pc++);

        switch(op) { CASE_OP256(0); }
    }
}
