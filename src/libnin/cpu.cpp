/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libnin/APU.h>
#include <libnin/BusMain.h>
#include <libnin/CPU.h>
#include <libnin/IRQ.h>
#include <libnin/NMI.h>
#include <libnin/PPU.h>

/*
 * TODO: Redo this whole system.
 */
#define CYCLE()         do {                                                        \
    _cyc++;                                                                         \
    _apu.tick(1);                                                                   \
    _ppu.tick(3);                                                                   \
    /*                                                                              \
    state->regionData.cycleExtraCounter += state->regionData.cycleExtraIncrement;   \
    if (state->regionData.cycleExtraCounter == 5)                                   \
    {                                                                               \
        state->regionData.cycleExtraCounter = 0;                                    \
        state->frame |= ninPpuRunCycles(state, 1);                                  \
    }                                                                               \
    */                                                                              \
    /*state->diskSystem.tick();                                                   */\
} while (0)

#define ICASE1(n)   case n: instruction<(n)>(tmp); break
#define ICASE2(n)   ICASE1(n + 0x00); ICASE1(n + 0x01); ICASE1(n + 0x02); ICASE1(n + 0x03)
#define ICASE3(n)   ICASE2(n + 0x00); ICASE2(n + 0x04); ICASE2(n + 0x08); ICASE2(n + 0x0c)
#define ICASE4(n)   ICASE3(n + 0x00); ICASE3(n + 0x10); ICASE3(n + 0x20); ICASE3(n + 0x30)
#define EXECUTE(x)  do { switch (x) { ICASE4(0x00); ICASE4(0x40); ICASE4(0x80); ICASE4(0xc0); ICASE1(0x100); ICASE1(0x101); } } while(0)

#define X(str)          if (matchPattern(str, N))

#define READ(x)         do { tmp = _bus.read((x)); } while (0)
#define PUSH8(x)        do { stackPush8((x)); CYCLE(); } while (0)
#define PUSH16(x)       do { PUSH8((x) >> 8); PUSH8((x) & 0xff); } while (0)
#define POP8()          do { tmp = stackPop8(); CYCLE(); } while (0)
#define POP16()         do { POP8(); addr = tmp; POP8(); addr |= ((uint16_t)tmp << 8); } while (0)
#define ECYCLE(a, b)    do { if (((a + b) ^ (a)) & 0xff00) CYCLE(); } while (0)

static constexpr const uint8_t kHex[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15 };
static constexpr const uint8_t kBranchFlags[] = { PFLAG_N, PFLAG_V, PFLAG_C, PFLAG_Z };

static constexpr bool matchPattern(const char* pattern, uint16_t value)
{
    return !!((kHex[pattern[value / 4] - '0']) & (1 << (value % 4)));
}

using namespace libnin;

CPU::CPU(IRQ& irq, NMI& nmi, PPU& ppu, APU& apu, BusMain& bus)
: _irq{irq}
, _nmi{nmi}
, _ppu{ppu}
, _apu{apu}
, _bus{bus}
, _pc{}
, _regs{}
, _p{}
, _p2{}
, _nmi2{}
, _reset{true}
{

}

std::size_t CPU::tick(std::size_t cycles)
{
    uint8_t tmp;
    uint16_t op;
    bool isIRQ;

    _cyc = 0;
    if (_reset)
    {
        _p = PFLAG_I;
        _pc = _bus.read16(0xfffc);
        _regs[REG_S] = 0xfd;
        _reset = false;
    }

    for (;;)
    {
        isIRQ = (!(_p & PFLAG_I) && _irq.high());

        op = _bus.read(_pc);
        CYCLE();
        tmp = _bus.read(_pc + 1);
        CYCLE();

        if (_nmi2 || isIRQ)
        {
            if (isIRQ)
            {
                op = 0x100;
            }
            else
            {
                _nmi.ack();
                op = 0x101;
            }
        }
        else
        {
            _pc++;
        }

        _nmi2 = _nmi.high();
        //_p2 = _p;

        EXECUTE(op);

        if (_cyc >= cycles)
            break;
    }
    return _cyc - cycles;
}

template <std::uint16_t N>
void CPU::instruction(std::uint8_t tmp)
{
    std::uint16_t addr;
    std::uint8_t r0;
    std::uint8_t r1;
    std::uint8_t i;

    addr = 0;
    r0 = 0;
    r1 = 0;
    i = 0;

    switch (N)
    {
    case 0x18: _p &= ~PFLAG_C; break;
    case 0x58: _p &= ~PFLAG_I; break;
    case 0xb8: _p &= ~PFLAG_V; break;
    case 0xd8: _p &= ~PFLAG_D; break;

    case 0x38: _p |= PFLAG_C; break;
    case 0x78: _p |= PFLAG_I; break;
    case 0xf8: _p |= PFLAG_D; break;
    default: break;
    }

    X("00400000004000000050000000400000222222220050000022222222000000000") { r0 = REG_A; }
    X("00000000000000000000000000000000044404400000000000400000111100000") { r0 = REG_X; }
    X("00000000000000000000000000000000011101100000000011111000000000000") { r0 = REG_Y; }
    X("00000000000000000000000000000000000000000000004000000000000000000") { r0 = REG_S; }
    X("22622222226222222262222222722222004400102222222200000000222222220") { r1 = REG_A; }
    X("00000000000000000000000000000000000000004444044400400000001000000") { r1 = REG_X; }
    X("00000000000000000000000000000000001000001111010100100000000000000") { r1 = REG_Y; }
    X("00000000000000000000000000000000000000600000000000000000000000000") { r1 = REG_S; }
    X("20000606200006062000060620000606200003022000030320000606200006060") { i = _regs[REG_X]; }                                                 /* index-X */
    X("00000020000000200000002000000020000004200000042400000020000000200") { i = _regs[REG_Y]; }                                                 /* index-Y */
    X("26263626372736262627362626273626270737227727372737273626372736260") { _pc++; addr = tmp; }                                                /* op1 */
    X("00060026100700260007002600070026000700220007002700070026000700260") { CYCLE();  READ(_pc++); addr |= ((std::uint16_t)tmp << 8); }         /* op2 */
    X("20000600200006002000060020000600200007002000070020000600200006000") { addr = (addr + i) & 0xff; CYCLE(); }                                /* zero-index */
    X("00000022000000220000002200000022000000000000002700000022000000220") { ECYCLE(addr, i); }                                                  /* abs-extra */
    X("00000026000000260000002600000026000000220000002700000026000000260") { addr += i; }                                                        /* abs-index */
    X("20002000200020002000200020012000200020002000200020002000200020000") { addr = indirect(addr); CYCLE(); CYCLE(); }                          /* indirect */
    X("00002000000020000000200000002000000000000000200000002000000020000") { ECYCLE(addr, _regs[REG_Y]); }                                       /* post-extra */
    X("00002000000020000000200000002000000020000000200000002000000020000") { addr += _regs[REG_Y]; }                                             /* post-index */
    X("00000000000000000000000000000000000000000000000000000000000000000") { CYCLE(); }                                                          /* post-X */
    X("00000004000000040000000400000004000020220000000000000004000000040") { CYCLE(); }                                                          /* phantom read */
    X("26062626270726262606262626062626000000002707272727072626270726260") { READ(addr); CYCLE(); }                                              /* mem */
    X("04040404040404040404040404040404000000000000000004040404040404040") { _bus.write(addr, tmp); CYCLE(); }                                   /* RMW phantom writes */
    X("00400000004000000050000000400000277727720050004000500000001000000") { tmp = _regs[r0]; }                                                  /* reg -> tmp */
    X("00100000000000000000000000000000000000000000000000000000000000000") { tmp = (_p | ~PFLAG_MASK); }                                         /* flags -> tmp */
    X("22222222000000000000000000000000000000000000000000000000000000000") { tmp = _regs[REG_A] | tmp; }                                         /* ora */
    X("00000000222222220000000000000000000000000000000000000000000000000") { tmp = _regs[REG_A] & tmp; }                                         /* and */
    X("00000000000000002222222200000000000000000000000000000000000000000") { tmp = _regs[REG_A] ^ tmp; }                                         /* eor */
    X("00000000000000000000000022222222000000000000000000000000000000000") { tmp = adc(_regs[REG_A], tmp); }                                     /* adc */
    X("00000000000000000000000000000000000000000000000000000000222222220") { tmp = adc(_regs[REG_A], ~tmp); }                                    /* sbc */
    X("00000000000000000000000000000000000000000000000000100000041404040") { tmp++; }                                                            /* inc */
    X("00000000000000000000000000000000001000000000000004440404000000000") { tmp--; }                                                            /* dec */
    X("04440404000000000000000000000000000000000000000000000000000000000") { tmp = asl(tmp); }                                                   /* asl */
    X("00000000044404040000000000000000000000000000000000000000000000000") { tmp = rol(tmp); }                                                   /* rol */
    X("00000000000000000444040400000000000000000000000000000000000000000") { tmp = lsr(tmp); }                                                   /* lsr */
    X("00000000000000000000000004440404000000000000000000000000000000000") { tmp = ror(tmp); }                                                   /* ror */
    X("00000000010100000000000000000000000000000000000000000000000000000") { tmp = bit(tmp); }                                                   /* bit */
    X("00100000000000000010000000000000000000000000000000000000000000000") { PUSH8(tmp); }                                                              /* push */
    X("00000000001000000000000000100000000000000000000000000000000000000") { POP8(); CYCLE(); }                                                         /* pop */
    X("04040404040404040404040404040404270727220000000004040404040404040") { _bus.write(addr, tmp); CYCLE(); }                                          /* store */
    X("22622222226222222262222222722222005000507777276700500000223222220") { _regs[r1] = tmp; }                                                         /* load */
    X("00000000001000000000000000000000000000000000000000000000000000000") { _p = (tmp & PFLAG_MASK); }                                                 /* tmp -> flags */
    X("00000000000000000000000000000000000000000000000033232222110100000") { tmp = compare(_regs[r0], tmp); }                                           /* cmp */
    X("26662667276726662666266626760426007040107777276777772626373726260") { flagZ(tmp); }                                                              /* Z */
    X("26662667266626662666266626760426007040107777276777772626373726260") { flagN(tmp); }                                                              /* N */
    X("00000000000000001000000000000000000000000000000000000000000000000") { POP8(); _p = (tmp & PFLAG_MASK); POP16(); _p2 = _p; _pc = addr; CYCLE(); } /* rti */
    X("00000000000000000000000010000000000000000000000000000000000000000") { POP16(); _pc = addr + 1; CYCLE(); CYCLE(); } /* rts */
    X("00000000100000000000000000000000000000000000000000000000000000000") { PUSH16(_pc - 1); CYCLE(); } /* jsr */
    X("00000000100000000001000000010000000000000000000000000000000000000") { _pc = addr; } /* jmp */
    X("00001000000000000000100000000000000010000000000000001000000000000") { if (!(_p & kBranchFlags[N >> 6])) { CYCLE(); ECYCLE(_pc, ((std::int8_t)tmp)); _pc += (std::int8_t)tmp; } } /* branch-clear */
    X("00000000000010000000000000001000000000000000100000000000000010000") { if ((_p & kBranchFlags[N >> 6])) { CYCLE(); ECYCLE(_pc, ((std::int8_t)tmp)); _pc += (std::int8_t)tmp; } }  /* branch-set */
    X("10000000000000000000000000000000000000000000000000000000000000000") { _p |= PFLAG_B; _pc++; } /* BRK */
    X("10000000000000000000000000000000000000000000000000000000000000001") { addr = 0xfffe; } /* Vector: IRQ */
    X("00000000000000000000000000000000000000000000000000000000000000002") { addr = 0xfffa; } /* Vector: NMI */
    X("10000000000000000000000000000000000000000000000000000000000000003") { PUSH16(_pc); PUSH8(_p | PFLAG_1); _p &= ~(PFLAG_B); CYCLE(); CYCLE(); _p |= PFLAG_I; _pc = _bus.read16(addr); } /* brk */
}


void CPU::flagZ(std::uint8_t value)
{
    _p &= ~PFLAG_Z;
    _p |= ((!value) << 1);
}

void CPU::flagN(std::uint8_t value)
{
    _p &= ~PFLAG_N;
    if (value & 0x80)
        _p |= PFLAG_N;
}

std::uint8_t CPU::compare(std::uint8_t a, std::uint8_t b)
{
    _p &= ~(PFLAG_C);
    if (a >= b)
        _p |= PFLAG_C;
    return a - b;
}

void CPU::stackPush8(std::uint8_t value)
{
    _bus.write(0x100 | _regs[REG_S], value);
    _regs[REG_S]--;
}

std::uint8_t CPU::stackPop8()
{
    _regs[REG_S]++;
    return _bus.read(0x100 | _regs[REG_S]);
}

std::uint8_t CPU::asl(std::uint8_t v)
{
    _p &= ~PFLAG_C;
    if (v & 0x80) _p |= PFLAG_C;

    return (v << 1);
}

std::uint8_t CPU::rol(std::uint8_t v)
{
    std::uint8_t carry;

    carry = (_p & PFLAG_C) ? 0x01 : 0x00;
    _p &= ~PFLAG_C;
    if (v & 0x80) _p |= PFLAG_C;

    return ((v << 1) | carry);
}

std::uint8_t CPU::lsr(std::uint8_t v)
{
    _p &= ~PFLAG_C;
    if (v & 0x01) _p |= PFLAG_C;

    return (v >> 1);
}

std::uint8_t CPU::ror(std::uint8_t v)
{
    std::uint8_t carry;

    carry = (_p & PFLAG_C) ? 0x80 : 0x00;
    _p &= ~PFLAG_C;
    if (v & 0x01) _p |= PFLAG_C;

    return ((v >> 1) | carry);
}

std::uint8_t CPU::bit(std::uint8_t v)
{
    _p &= ~(0xc0);
    _p |= (v & 0xc0);

    return _regs[REG_A] & v;
}

std::uint8_t CPU::adc(std::uint8_t a, std::uint8_t b)
{
    std::uint16_t sum;
    std::uint8_t carryIn;

    carryIn = (_p & PFLAG_C) ? 1 : 0;
    sum = (std::uint16_t)a + (std::uint16_t)b + (std::uint16_t)carryIn;

    _p &= ~(PFLAG_C | PFLAG_V | PFLAG_N | PFLAG_Z);
    if (sum & 0x100) _p |= PFLAG_C;
    if ((sum & 0xff) == 0) _p |= PFLAG_Z;
    if ((a ^ sum) & (b ^ sum) & 0x80) _p |= PFLAG_V;
    if (sum & 0x80) _p |= PFLAG_N;

    return (sum & 0xff);
}

std::uint16_t CPU::indirect(std::uint16_t addr)
{
    std::uint8_t hi;
    std::uint8_t lo0;
    std::uint8_t lo1;
    std::uint8_t hh;
    std::uint8_t ll;

    hi = addr >> 8;
    lo0 = addr & 0xff;
    lo1 = lo0 + 1;

    ll = _bus.read(((std::uint16_t)hi << 8) | lo0);
    hh = _bus.read(((std::uint16_t)hi << 8) | lo1);

    return ((std::uint16_t)hh << 8) | ll;
}
