/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 *
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 *
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 *
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef LIBNIN_INTERPRETER_CYCLE_IMPL_H
#define LIBNIN_INTERPRETER_CYCLE_IMPL_H 1

#include <libnin/InterpreterCycle.h>
#include <libnin/IRQ.h>
#include <libnin/NMI.h>

#define PollInterrupts         \
    _nmiPending = _nmi.high(); \
    _irqPending = (_irq.high() && !(_cpu.p & PFLAG_I));
#define PollInterrupts2                         \
    _nmiPending = (_nmiPending || _nmi.high()); \
    _irqPending = (_irqPending || (_irq.high() && !(_cpu.p & PFLAG_I)));

#define SelectSourceA _selSrc = REG_A;
#define SelectSourceX _selSrc = REG_X;
#define SelectSourceY _selSrc = REG_Y;

#define SelectDestA _selDst = REG_A;
#define SelectDestX _selDst = REG_X;
#define SelectDestY _selDst = REG_Y;

#define AddrSet_NMI   _addr = 0xfffa;
#define AddrSet_RESET _addr = 0xfffc;
#define AddrSet_BRK   _addr = 0xfffe;

#define DecS  _cpu.s--;
#define IncS  _cpu.s++;
#define IncPC _cpu.pc++;

#define AddrImpl      read(_cpu.pc);
#define AddrImplIncPC read(_cpu.pc++);
#define AddrZero      _addr = read(_cpu.pc++);
#define AddrZeroX     _addr = (_addr + _cpu.x) & 0xff;
#define AddrZeroY     _addr = (_addr + _cpu.y) & 0xff;
#define AddrAbsLo     _addr = ((_addr & 0xff00) | read(_cpu.pc++));
#define AddrAbsHi     _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_cpu.pc++)) << 8);
#define AddrAbsHiX                                       \
    _addrCarry = (((_addr & 0xff) + _cpu.x) > 0xff) ? 1 : 0; \
    _addr      = (((_addr + _cpu.x) & 0x00ff) | ((std::uint16_t)read(_cpu.pc++)) << 8);
#define AddrAbsHiY                                       \
    _addrCarry = (((_addr & 0xff) + _cpu.y) > 0xff) ? 1 : 0; \
    _addr      = (((_addr + _cpu.y) & 0x00ff) | ((std::uint16_t)read(_cpu.pc++)) << 8);
#define AddrCarry \
    read(_addr);  \
    _addr += (((std::uint16_t)_addrCarry) << 8);
#define AddrCarry_SHX \
    read(_addr);      \
    if (_addrCarry) _addr = (((_addr & 0x00ff) | ((_addr >> 8) + 1)) & _cpu.x);
#define AddrCarry_SHY \
    read(_addr);      \
    if (_addrCarry) _addr = (((_addr & 0x00ff) | ((_addr >> 8) + 1)) & _cpu.y);
#define AddrCarry_AHX \
    read(_addr);      \
    if (_addrCarry) _addr = (((_addr & 0x00ff) | ((_addr >> 8) + 1)) & _cpu.a & _cpu.x);
#define AddrCarryIf  \
    if (!_addrCarry) \
    {
#define AddrCarryEnd       \
    return &InterpreterCycle::dispatch; \
    }
#define AddrCarryFix   _addr += 0x100;
#define AddrIndirectLo _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8);
#define AddrIndirectHi _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8);
#define AddrIndirectY                                    \
    _addrCarry = (((_addr & 0xff) + _cpu.y) > 0xff) ? 1 : 0; \
    _addr      = (_addr & 0xff00) | ((_addr + _cpu.y) & 0xff);

#define DummyLoad read(_addr);

#define TmpLoad     std::uint8_t tmp = read(_addr);
#define TmpLoadImm  std::uint8_t tmp = read(_cpu.pc++);
#define TmpLoadZero std::uint8_t tmp = _memory.ram[_addr];
#define TmpLoadAcc  std::uint8_t tmp = _cpu.a;
#define TmpLoadRmw  std::uint8_t tmp = _rmw;
#define TmpStoreAcc _cpu.a = tmp;
#define TmpStoreRmw _rmw = tmp;

#define RmwLoad      _rmw = read(_addr);
#define RmwLoadZero  _rmw = _memory.ram[_addr];
#define RmwStore     next = write(_addr, _rmw, next);
#define RmwStoreZero _memory.ram[_addr] = _rmw;

#define BranchClearC           \
    if (_cpu.p & PFLAG_C)      \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchClearN           \
    if (_cpu.p & PFLAG_N)      \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchClearV           \
    if (_cpu.p & PFLAG_V)      \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchClearZ           \
    if (_cpu.p & PFLAG_Z)      \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchSetC             \
    if (!(_cpu.p & PFLAG_C))   \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchSetN             \
    if (!(_cpu.p & PFLAG_N))   \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchSetV             \
    if (!(_cpu.p & PFLAG_V))   \
    {                          \
        return &InterpreterCycle::dispatch; \
    }
#define BranchSetZ             \
    if (!(_cpu.p & PFLAG_Z))   \
    {                          \
        return &InterpreterCycle::dispatch; \
    }

#define BranchTake                                                      \
    _addrCarry = (((_cpu.pc + (std::int8_t)_addr) ^ _cpu.pc) & 0xff00) ? 1 : 0; \
    _addr      = _cpu.pc + (std::int8_t)_addr;                              \
    _cpu.pc        = ((_cpu.pc & 0xff00) | (_addr & 0xff));
#define BranchTake2                         \
    if (!_addrCarry) return &InterpreterCycle::dispatch; \
    _cpu.pc = _addr;

#define PushPCL   _memory.ram[0x100 | _cpu.s] = (_cpu.pc & 0xff);
#define PushPCH   _memory.ram[0x100 | _cpu.s] = (_cpu.pc >> 8);
#define PushP     _memory.ram[0x100 | _cpu.s] = _cpu.p | PFLAG_1 | PFLAG_B;
#define PushP_NoB _memory.ram[0x100 | _cpu.s] = _cpu.p | PFLAG_1;
#define PushA     _memory.ram[0x100 | _cpu.s] = _cpu.a;
#define PullPCL   _cpu.pc = (_cpu.pc & 0xff00) | _memory.ram[0x100 | _cpu.s];
#define PullPCH   _cpu.pc = (_cpu.pc & 0x00ff) | ((std::uint16_t)_memory.ram[0x100 | _cpu.s] << 8);
#define PullP     _cpu.p = _memory.ram[0x100 | _cpu.s] & ~(PFLAG_1 | PFLAG_B);
#define PullA                     \
    _cpu.a = _memory.ram[0x100 | _cpu.s]; \
    flagNZ(_cpu.a);

#define VectorPCL         _cpu.pc = ((_cpu.pc & 0xff00) | read(_addr));
#define VectorPCH         _cpu.pc = ((_cpu.pc & 0x00ff) | (((std::uint16_t)read(_addr + 1)) << 8));
#define VectorPCH_NoCarry _cpu.pc = ((_cpu.pc & 0x00ff) | ((std::uint16_t)read((_addr & 0xff00) | ((_addr + 1) & 0xff))) << 8);

#define ImmLoadA      \
    _cpu.a = read(_cpu.pc++); \
    flagNZ(_cpu.a);
#define ImmLoadX      \
    _cpu.x = read(_cpu.pc++); \
    flagNZ(_cpu.x);
#define ImmLoadY      \
    _cpu.y = read(_cpu.pc++); \
    flagNZ(_cpu.y);

#define OpORA              \
    _cpu.regs[_selDst] |= tmp; \
    flagNZ(_cpu.regs[_selDst]);
#define OpAND              \
    _cpu.regs[_selDst] &= tmp; \
    flagNZ(_cpu.regs[_selDst]);
#define OpEOR              \
    _cpu.regs[_selDst] ^= tmp; \
    flagNZ(_cpu.regs[_selDst]);
#define OpCMP                                    \
    _cpu.p &= ~PFLAG_C;                              \
    _cpu.p |= (_cpu.regs[_selDst] >= tmp) ? PFLAG_C : 0; \
    flagNZ(_cpu.regs[_selDst] - tmp);
#define OpBIT                             \
    _cpu.p &= ~(PFLAG_Z | PFLAG_N | PFLAG_V); \
    _cpu.p |= (tmp & 0xc0);                   \
    _cpu.p |= ((_cpu.a & tmp) ? 0 : PFLAG_Z);
#define OpADC _cpu.a = adc(_cpu.a, tmp);
#define OpSBC _cpu.a = adc(_cpu.a, ~tmp);

#define OpINC flagNZ(++tmp);
#define OpDEC flagNZ(--tmp);
#define OpASL                           \
    _cpu.p &= ~PFLAG_C;                     \
    _cpu.p |= ((tmp & 0x80) ? PFLAG_C : 0); \
    tmp <<= 1;                          \
    flagNZ(tmp);
#define OpLSR                             \
    _cpu.p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); \
    _cpu.p |= ((tmp & 0x01) ? PFLAG_C : 0);   \
    tmp >>= 1;                            \
    _cpu.p |= (tmp ? 0 : PFLAG_Z);
#define OpROL                                      \
    std::uint8_t c = (_cpu.p & PFLAG_C ? 0x01 : 0x00); \
    _cpu.p &= ~PFLAG_C;                                \
    _cpu.p |= ((tmp & 0x80) ? PFLAG_C : 0);            \
    tmp = ((tmp << 1) | c);                        \
    flagNZ(tmp);
#define OpROR                                      \
    std::uint8_t c = (_cpu.p & PFLAG_C ? 0x80 : 0x00); \
    _cpu.p &= ~PFLAG_C;                                \
    _cpu.p |= ((tmp & 0x01) ? PFLAG_C : 0);            \
    tmp = ((tmp >> 1) | c);                        \
    flagNZ(tmp);

#define OpSLO                           \
    _cpu.p &= ~PFLAG_C;                     \
    _cpu.p |= ((tmp & 0x80) ? PFLAG_C : 0); \
    tmp <<= 1;                          \
    _cpu.a |= tmp;                          \
    flagNZ(_cpu.a);
#define OpRLA                                      \
    std::uint8_t c = (_cpu.p & PFLAG_C ? 0x01 : 0x00); \
    _cpu.p &= ~PFLAG_C;                                \
    _cpu.p |= ((tmp & 0x80) ? PFLAG_C : 0);            \
    tmp = ((tmp << 1) | c);                        \
    _cpu.a &= tmp;                                     \
    flagNZ(_cpu.a);
#define OpSRE                           \
    _cpu.p &= ~PFLAG_C;                     \
    _cpu.p |= ((tmp & 0x01) ? PFLAG_C : 0); \
    tmp >>= 1;                          \
    _cpu.a ^= tmp;                          \
    flagNZ(_cpu.a);
#define OpRRA                                      \
    std::uint8_t c = (_cpu.p & PFLAG_C ? 0x80 : 0x00); \
    _cpu.p &= ~PFLAG_C;                                \
    _cpu.p |= ((tmp & 0x01) ? PFLAG_C : 0);            \
    tmp = ((tmp >> 1) | c);                        \
    _cpu.a  = adc(_cpu.a, tmp);
#define OpLAX      \
    _cpu.a = _cpu.x = tmp; \
    flagNZ(tmp);

#define OpANC       \
    _cpu.p &= ~PFLAG_C; \
    _cpu.a &= tmp;      \
    flagNZ(_cpu.a);     \
    _cpu.p |= (_cpu.p & PFLAG_N) ? PFLAG_C : 0;
#define OpALR                             \
    _cpu.a &= tmp;                            \
    _cpu.p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); \
    _cpu.p |= ((_cpu.a & 0x01) ? PFLAG_C : 0);    \
    _cpu.a >>= 1;                             \
    _cpu.p |= (_cpu.a ? 0 : PFLAG_Z);
#define OpARR                                      \
    _cpu.a &= tmp;                                     \
    std::uint8_t c = (_cpu.p & PFLAG_C ? 0x80 : 0x00); \
    _cpu.p &= ~(PFLAG_C | PFLAG_V);                    \
    _cpu.a = ((_cpu.a >> 1) | c);                          \
    flagNZ(_cpu.a);                                    \
    _cpu.p |= ((_cpu.a & 0x40) ? PFLAG_C : 0);             \
    _cpu.p |= (((((_cpu.a >> 1) & 0x20) ^ (_cpu.a & 0x20))) ? PFLAG_V : 0);
#define OpXAA      \
    _cpu.a = _cpu.x & tmp; \
    flagNZ(_cpu.a);
#define OpAXS                          \
    _cpu.x &= _cpu.a;                          \
    _cpu.p &= ~PFLAG_C;                    \
    _cpu.p |= ((_cpu.x >= tmp) ? PFLAG_C : 0); \
    _cpu.x -= tmp;                         \
    flagNZ(_cpu.x);

#define OpDCP                        \
    tmp--;                           \
    _cpu.p &= ~PFLAG_C;                  \
    _cpu.p |= (_cpu.a >= tmp) ? PFLAG_C : 0; \
    flagNZ(_cpu.a - tmp);
#define OpISC \
    tmp++;    \
    _cpu.a = adc(_cpu.a, ~tmp);

#define INX flagNZ(++_cpu.x);
#define INY flagNZ(++_cpu.y);
#define DEX flagNZ(--_cpu.x);
#define DEY flagNZ(--_cpu.y);

#define FlagClearC _cpu.p &= ~PFLAG_C;
#define FlagClearI _cpu.p &= ~PFLAG_I;
#define FlagClearV _cpu.p &= ~PFLAG_V;
#define FlagClearD _cpu.p &= ~PFLAG_D;

#define FlagSetC _cpu.p |= PFLAG_C;
#define FlagSetI _cpu.p |= PFLAG_I;
#define FlagSetD _cpu.p |= PFLAG_D;

#define WriteReg     next = write(_addr, _cpu.regs[_selSrc], next);
#define WriteRegZero _memory.ram[_addr] = _cpu.regs[_selSrc];

#define WriteReg_SAX next = write(_addr, _cpu.a & _cpu.x, next);
#define WriteReg_SHX next = write(_addr, _cpu.x & ((_addr >> 8) + 1), next);
#define WriteReg_SHY next = write(_addr, _cpu.y & ((_addr >> 8) + 1), next);
#define WriteReg_AHX next = write(_addr, _cpu.a & _cpu.x & ((_addr >> 8) + 1), next);
#define WriteReg_TAS \
    _cpu.s   = _cpu.a & _cpu.x;  \
    next = write(_addr, _cpu.s & ((_addr >> 8) + 1), next);
#define WriteRegZero_SAX _memory.ram[_addr] = _cpu.a & _cpu.x;

#define ReadReg                   \
    _cpu.regs[_selDst] = read(_addr); \
    flagNZ(_cpu.regs[_selDst]);
#define ReadRegCarry                           \
    _cpu.regs[_selDst] = read(_addr);              \
    flagNZ(_cpu.regs[_selDst]);                    \
    _addr += ((std::uint16_t)_addrCarry << 8); \
    if (!_addrCarry) return (&InterpreterCycle::dispatch);
#define ReadRegZero                      \
    _cpu.regs[_selDst] = _memory.ram[_addr]; \
    flagNZ(_cpu.regs[_selDst]);

#define ReadReg_AX         \
    _cpu.a = _cpu.x = read(_addr); \
    flagNZ(_cpu.a);
#define ReadRegCarry_AX                        \
    _cpu.a = _cpu.x = read(_addr);                     \
    flagNZ(_cpu.a);                                \
    _addr += ((std::uint16_t)_addrCarry << 8); \
    if (!_addrCarry) return (&InterpreterCycle::dispatch);
#define ReadRegZero_AX            \
    _cpu.a = _cpu.x = _memory.ram[_addr]; \
    flagNZ(_cpu.a);

#define ReadReg_LAS                  \
    _cpu.a = _cpu.x = _cpu.s = read(_addr) & _cpu.s; \
    flagNZ(_cpu.s);
#define ReadRegCarry_LAS                       \
    _cpu.a = _cpu.x = _cpu.s = read(_addr) & _cpu.s;           \
    flagNZ(_cpu.s);                                \
    _addr += ((std::uint16_t)_addrCarry << 8); \
    if (!_addrCarry) return (&InterpreterCycle::dispatch);

#define CarryFix _addr += ((std::uint16_t)_addrCarry << 8);

#define TransferAX \
    _cpu.x = _cpu.a;       \
    flagNZ(_cpu.a);
#define TransferAY \
    _cpu.y = _cpu.a;       \
    flagNZ(_cpu.a);
#define TransferSX \
    _cpu.x = _cpu.s;       \
    flagNZ(_cpu.s);
#define TransferXA \
    _cpu.a = _cpu.x;       \
    flagNZ(_cpu.x);
#define TransferXS _cpu.s = _cpu.x;
#define TransferYA \
    _cpu.a = _cpu.y;       \
    flagNZ(_cpu.y);

#define Nop      /* nop */ ;
#define ReadAddr read(_addr);
#define ReadAddrCarry \
    read(_addr);      \
    if (!_addrCarry) return (&InterpreterCycle::dispatch);

#define SwitchPC _cpu.pc = (_addr | ((std::uint16_t)read(_cpu.pc) << 8));

#endif
