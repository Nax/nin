#include <nin/nin.h>

#define TRAP do { printf("TRAP opcode 0x%02x at 0x%04x\n", op, pcOld); getchar(); } while (0)

#define ADDR_DX_INDIRECT    0x00
#define ADDR_D              0x01
#define ADDR_IMM            0x02
#define ADDR_A              0x03
#define ADDR_DY_INDIRECT    0x04
#define ADDR_DX             0x05
#define ADDR_AY             0x06
#define ADDR_AX             0x07
#define ADDR_NONE           0xff

#define EXPAND_CASE_READ(base, func, ...)       \
    case base:                                  \
        tmp = func(state);                      \
        if (1) __VA_ARGS__;                     \
        break

#define EXPAND_CASE_WRITE(base, func, ...)      \
    case base:                                  \
        if (1) __VA_ARGS__;                     \
        func(state, tmp);                       \
        break


#define CASE_ALU_READ(base, ...)                                    \
    EXPAND_CASE_READ(base + 0x00, readAddrIndirectDX, __VA_ARGS__); \
    EXPAND_CASE_READ(base + 0x04, readAddrD, __VA_ARGS__);          \
    EXPAND_CASE_READ(base + 0x08, readAddrImm, __VA_ARGS__);        \
    EXPAND_CASE_READ(base + 0x0c, readAddrA, __VA_ARGS__);          \
    EXPAND_CASE_READ(base + 0x10, readAddrIndirectDY, __VA_ARGS__); \
    EXPAND_CASE_READ(base + 0x14, readAddrDX, __VA_ARGS__);         \
    EXPAND_CASE_READ(base + 0x18, readAddrAY, __VA_ARGS__);         \
    EXPAND_CASE_READ(base + 0x1c, readAddrAX, __VA_ARGS__)

#define CASE_ALU_WRITE(base, ...)                                       \
    EXPAND_CASE_WRITE(base + 0x00, writeAddrIndirectDX, __VA_ARGS__);   \
    EXPAND_CASE_WRITE(base + 0x04, writeAddrD, __VA_ARGS__);            \
    EXPAND_CASE_WRITE(base + 0x08, writeAddrImm, __VA_ARGS__);          \
    EXPAND_CASE_WRITE(base + 0x0c, writeAddrA, __VA_ARGS__);            \
    EXPAND_CASE_WRITE(base + 0x10, writeAddrIndirectDY, __VA_ARGS__);   \
    EXPAND_CASE_WRITE(base + 0x14, writeAddrDX, __VA_ARGS__);           \
    EXPAND_CASE_WRITE(base + 0x18, writeAddrAY, __VA_ARGS__);           \
    EXPAND_CASE_WRITE(base + 0x1c, writeAddrAX, __VA_ARGS__)

#define CASE_LD(base, ...)                                      \
    EXPAND_CASE_READ(base + 0x00, readAddrImm, __VA_ARGS__);    \
    EXPAND_CASE_READ(base + 0x04, readAddrD, __VA_ARGS__);      \
    EXPAND_CASE_READ(base + 0x0c, readAddrA, __VA_ARGS__);      \
    EXPAND_CASE_READ(base + 0x14, readAddrDY, __VA_ARGS__);     \
    EXPAND_CASE_READ(base + 0x1c, readAddrAY, __VA_ARGS__)

static uint8_t readAddrIndirectDX(NinState* state)
{
    uint8_t arg;
    uint8_t hi;
    uint8_t lo;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    lo = ninMemoryRead8(state, (arg + state->cpu.x) & 0xff);
    hi = ninMemoryRead8(state, (arg + state->cpu.x + 1) & 0xff);
    return ninMemoryRead8(state, lo | (hi << 8));
}

static uint8_t readAddrD(NinState* state)
{
    uint8_t arg;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    return ninMemoryRead8(state, arg);
}

static uint8_t readAddrImm(NinState* state)
{
    return ninMemoryRead8(state, state->cpu.pc++);
}

static uint8_t readAddrA(NinState* state)
{
    uint16_t arg;

    arg = ninMemoryRead16(state, state->cpu.pc);
    state->cpu.pc += 2;
    return ninMemoryRead8(state, arg);
}

static uint8_t readAddrIndirectDY(NinState* state)
{
    uint8_t arg;
    uint8_t hi;
    uint8_t lo;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    lo = ninMemoryRead8(state, (arg) & 0xff);
    hi = ninMemoryRead8(state, (arg + 1) & 0xff);
    return ninMemoryRead8(state, (lo | (hi << 8)) + state->cpu.y);
}

static uint8_t readAddrDX(NinState* state)
{
    uint8_t arg;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    return ninMemoryRead8(state, (arg + state->cpu.x) & 0xff);
}

static uint8_t readAddrAY(NinState* state)
{
    uint16_t arg;

    arg = ninMemoryRead16(state, state->cpu.pc);
    state->cpu.pc += 2;
    return ninMemoryRead8(state, arg + state->cpu.y);
}

static uint8_t readAddrAX(NinState* state)
{
    uint16_t arg;

    arg = ninMemoryRead16(state, state->cpu.pc);
    state->cpu.pc += 2;
    return ninMemoryRead8(state, arg + state->cpu.x);
}

static uint8_t readAddrDY(NinState* state)
{
    uint8_t arg;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    return ninMemoryRead8(state, (arg + state->cpu.y) & 0xff);
}

static void writeAddrIndirectDX(NinState* state, uint8_t value)
{
    uint8_t arg;
    uint8_t hi;
    uint8_t lo;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    lo = ninMemoryRead8(state, (arg + state->cpu.x) & 0xff);
    hi = ninMemoryRead8(state, (arg + state->cpu.x + 1) & 0xff);
    ninMemoryWrite8(state, lo | (hi << 8), value);
}

static void writeAddrD(NinState* state, uint8_t value)
{
    uint8_t arg;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    ninMemoryWrite8(state, arg, value);
}

static void writeAddrImm(NinState* state, uint8_t value)
{
    (void)value;
    state->cpu.pc++;
}

static void writeAddrA(NinState* state, uint8_t value)
{
    uint16_t arg;

    arg = ninMemoryRead16(state, state->cpu.pc);
    state->cpu.pc += 2;
    ninMemoryWrite8(state, arg, value);
}

static void writeAddrIndirectDY(NinState* state, uint8_t value)
{
    uint8_t arg;
    uint8_t hi;
    uint8_t lo;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    lo = ninMemoryRead8(state, (arg) & 0xff);
    hi = ninMemoryRead8(state, (arg + 1) & 0xff);
    ninMemoryWrite8(state, (lo | (hi << 8)) + state->cpu.y, value);
}

static void writeAddrDX(NinState* state, uint8_t value)
{
    uint8_t arg;

    arg = ninMemoryRead8(state, state->cpu.pc++);
    ninMemoryWrite8(state, (arg + state->cpu.x) & 0xff, value);
}

static void writeAddrAY(NinState* state, uint8_t value)
{
    uint16_t arg;

    arg = ninMemoryRead16(state, state->cpu.pc);
    state->cpu.pc += 2;
    ninMemoryWrite8(state, arg + state->cpu.y, value);
}

static void writeAddrAX(NinState* state, uint8_t value)
{
    uint16_t arg;

    arg = ninMemoryRead16(state, state->cpu.pc);
    state->cpu.pc += 2;
    ninMemoryWrite8(state, arg + state->cpu.x, value);
}

static void flagZ(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_Z;
    state->cpu.p |= (!value) << 1;
}

static void flagN(NinState* state, uint8_t value)
{
    state->cpu.p &= ~PFLAG_N;
    state->cpu.p |= (value & 0x80);
}

static void flagNZ(NinState* state, uint8_t value)
{
    flagZ(state, value);
    flagN(state, value);
}

static void addCarry(NinState* state, uint8_t value)
{
    uint16_t sum;
    uint8_t carryIn;
    uint8_t carryOut;

    carryIn = state->cpu.p & 0x01;
    sum = ((uint16_t)state->cpu.a) + value + carryIn;
    carryOut = sum >> 8;
    state->cpu.a = sum & 0xff;
    flagNZ(state, state->cpu.a);
    state->cpu.p &= ~(PFLAG_C | PFLAG_V);
    state->cpu.p |= carryOut;
    state->cpu.p |= (carryOut ^ carryIn) << 6;
}

static void compare(NinState* state, uint8_t a, uint8_t b)
{
    uint8_t r;

    r = a - b;
    flagNZ(state, r);
    state->cpu.p &= ~(PFLAG_C);
    if (a >= b)
        state->cpu.p |= PFLAG_C;
}

void ninRunCycles(NinState* state, size_t cycles)
{
    uint16_t pcOld;
    uint8_t op;
    uint8_t tmp;

    while (cycles--)
    {
        pcOld = state->cpu.pc;
        op = ninMemoryRead8(state, state->cpu.pc++);

        switch (op)
        {
        case 0x10: // BPL
            tmp = ninMemoryRead8(state, state->cpu.pc++);
            if (!(state->cpu.p & PFLAG_N))
                state->cpu.pc += (int8_t)tmp;
            break;
        case 0x78: // SEI
            state->cpu.p |= PFLAG_I;
            break;
        case 0xd8: // CLD
            state->cpu.p &= ~PFLAG_D;
            break;
        CASE_ALU_READ(0x01,  { state->cpu.a |= tmp; flagNZ(state, state->cpu.a); }); // ORA
        CASE_ALU_READ(0x21,  { state->cpu.a &= tmp; flagNZ(state, state->cpu.a); }); // AND
        CASE_ALU_READ(0x41,  { state->cpu.a ^= tmp; flagNZ(state, state->cpu.a); }); // EOR
        CASE_ALU_READ(0x61,  { addCarry(state, tmp); });                             // ADC
        CASE_ALU_WRITE(0x81, { tmp = state->cpu.a; });                               // STA
        CASE_ALU_READ(0xa1,  { state->cpu.a = tmp;  flagNZ(state, state->cpu.a); }); // LDA
        CASE_ALU_READ(0xc1,  { compare(state, state->cpu.a, tmp); });                // CMP
        CASE_ALU_READ(0xe1,  { addCarry(state, ~tmp); });                            // SBC

        CASE_LD(0xa2, { state->cpu.x = tmp; flagNZ(state, state->cpu.x); });  // LDX

        case 0x9a: // TXS
            state->cpu.s = state->cpu.x;
            break;
        default:
            TRAP;
            break;
        }
    }
}
