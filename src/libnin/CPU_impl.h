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

#ifndef LIBNIN_CPU_IMPL_H
#define LIBNIN_CPU_IMPL_H 1

#include <libnin/CPU.h>
#include <libnin/IRQ.h>
#include <libnin/NMI.h>

#define PollInterrupts         \
    _nmiPending = _nmi.high(); \
    _irqPending = (_irq.high() && !(_p & PFLAG_I));
#define PollInterrupts2                         \
    _nmiPending = (_nmiPending || _nmi.high()); \
    _irqPending = (_irqPending || (_irq.high() && !(_p & PFLAG_I)));

#define SelectSourceA _selSrc = REG_A;
#define SelectSourceX _selSrc = REG_X;
#define SelectSourceY _selSrc = REG_Y;

#define SelectDestA _selDst = REG_A;
#define SelectDestX _selDst = REG_X;
#define SelectDestY _selDst = REG_Y;

#define AddrSet_NMI   _addr = 0xfffa;
#define AddrSet_RESET _addr = 0xfffc;
#define AddrSet_BRK   _addr = 0xfffe;

#define DecS  _s--;
#define IncS  _s++;
#define IncPC _pc++;

#define AddrImpl      read(_pc);
#define AddrImplIncPC read(_pc++);
#define AddrZero      _addr = read(_pc++);
#define AddrZeroX     _addr = (_addr + _x) & 0xff;
#define AddrZeroY     _addr = (_addr + _y) & 0xff;
#define AddrAbsLo     _addr = ((_addr & 0xff00) | read(_pc++));
#define AddrAbsHi     _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8);
#define AddrAbsHiX                                       \
    _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; \
    _addr      = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8);
#define AddrAbsHiY                                       \
    _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; \
    _addr      = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8);
#define AddrCarry \
    read(_addr);  \
    _addr += (((std::uint16_t)_addrCarry) << 8);
#define AddrCarry_SHX \
    read(_addr);      \
    if (_addrCarry) _addr = (((_addr & 0x00ff) | ((_addr >> 8) + 1)) & _x);
#define AddrCarry_SHY \
    read(_addr);      \
    if (_addrCarry) _addr = (((_addr & 0x00ff) | ((_addr >> 8) + 1)) & _y);
#define AddrCarry_AHX \
    read(_addr);      \
    if (_addrCarry) _addr = (((_addr & 0x00ff) | ((_addr >> 8) + 1)) & _a & _x);
#define AddrCarryIf  \
    if (!_addrCarry) \
    {
#define AddrCarryEnd       \
    return &CPU::dispatch; \
    }
#define AddrCarryFix   _addr += 0x100;
#define AddrIndirectLo _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8);
#define AddrIndirectHi _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8);
#define AddrIndirectY                                    \
    _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; \
    _addr      = (_addr & 0xff00) | ((_addr + _y) & 0xff);

#define DummyLoad read(_addr);

#define TmpLoad     std::uint8_t tmp = read(_addr);
#define TmpLoadImm  std::uint8_t tmp = read(_pc++);
#define TmpLoadZero std::uint8_t tmp = _memory.ram[_addr];
#define TmpLoadAcc  std::uint8_t tmp = _a;
#define TmpLoadRmw  std::uint8_t tmp = _rmw;
#define TmpStoreAcc _a = tmp;
#define TmpStoreRmw _rmw = tmp;

#define RmwLoad      _rmw = read(_addr);
#define RmwLoadZero  _rmw = _memory.ram[_addr];
#define RmwStore     next = write(_addr, _rmw, next);
#define RmwStoreZero _memory.ram[_addr] = _rmw;

#define BranchClearC           \
    if (_p & PFLAG_C)          \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchClearN           \
    if (_p & PFLAG_N)          \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchClearV           \
    if (_p & PFLAG_V)          \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchClearZ           \
    if (_p & PFLAG_Z)          \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchSetC             \
    if (!(_p & PFLAG_C))       \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchSetN             \
    if (!(_p & PFLAG_N))       \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchSetV             \
    if (!(_p & PFLAG_V))       \
    {                          \
        return &CPU::dispatch; \
    }
#define BranchSetZ             \
    if (!(_p & PFLAG_Z))       \
    {                          \
        return &CPU::dispatch; \
    }

#define BranchTake                                                      \
    _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; \
    _addr      = _pc + (std::int8_t)_addr;                              \
    _pc        = ((_pc & 0xff00) | (_addr & 0xff));
#define BranchTake2                         \
    if (!_addrCarry) return &CPU::dispatch; \
    _pc = _addr;

#define PushPCL   _memory.ram[0x100 | _s] = (_pc & 0xff);
#define PushPCH   _memory.ram[0x100 | _s] = (_pc >> 8);
#define PushP     _memory.ram[0x100 | _s] = _p | PFLAG_1 | PFLAG_B;
#define PushP_NoB _memory.ram[0x100 | _s] = _p | PFLAG_1;
#define PushA     _memory.ram[0x100 | _s] = _a;
#define PullPCL   _pc = (_pc & 0xff00) | _memory.ram[0x100 | _s];
#define PullPCH   _pc = (_pc & 0x00ff) | ((std::uint16_t)_memory.ram[0x100 | _s] << 8);
#define PullP     _p = _memory.ram[0x100 | _s] & ~(PFLAG_1 | PFLAG_B);
#define PullA                     \
    _a = _memory.ram[0x100 | _s]; \
    flagNZ(_a);

#define VectorPCL         _pc = ((_pc & 0xff00) | read(_addr));
#define VectorPCH         _pc = ((_pc & 0x00ff) | (((std::uint16_t)read(_addr + 1)) << 8));
#define VectorPCH_NoCarry _pc = ((_pc & 0x00ff) | ((std::uint16_t)read((_addr & 0xff00) | ((_addr + 1) & 0xff))) << 8);

#define ImmLoadA      \
    _a = read(_pc++); \
    flagNZ(_a);
#define ImmLoadX      \
    _x = read(_pc++); \
    flagNZ(_x);
#define ImmLoadY      \
    _y = read(_pc++); \
    flagNZ(_y);

#define OpORA              \
    _regs[_selDst] |= tmp; \
    flagNZ(_regs[_selDst]);
#define OpAND              \
    _regs[_selDst] &= tmp; \
    flagNZ(_regs[_selDst]);
#define OpEOR              \
    _regs[_selDst] ^= tmp; \
    flagNZ(_regs[_selDst]);
#define OpCMP                                    \
    _p &= ~PFLAG_C;                              \
    _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; \
    flagNZ(_regs[_selDst] - tmp);
#define OpBIT                             \
    _p &= ~(PFLAG_Z | PFLAG_N | PFLAG_V); \
    _p |= (tmp & 0xc0);                   \
    _p |= ((_a & tmp) ? 0 : PFLAG_Z);
#define OpADC _a = adc(_a, tmp);
#define OpSBC _a = adc(_a, ~tmp);

#define OpINC flagNZ(++tmp);
#define OpDEC flagNZ(--tmp);
#define OpASL                           \
    _p &= ~PFLAG_C;                     \
    _p |= ((tmp & 0x80) ? PFLAG_C : 0); \
    tmp <<= 1;                          \
    flagNZ(tmp);
#define OpLSR                             \
    _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); \
    _p |= ((tmp & 0x01) ? PFLAG_C : 0);   \
    tmp >>= 1;                            \
    _p |= (tmp ? 0 : PFLAG_Z);
#define OpROL                                      \
    std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); \
    _p &= ~PFLAG_C;                                \
    _p |= ((tmp & 0x80) ? PFLAG_C : 0);            \
    tmp = ((tmp << 1) | c);                        \
    flagNZ(tmp);
#define OpROR                                      \
    std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); \
    _p &= ~PFLAG_C;                                \
    _p |= ((tmp & 0x01) ? PFLAG_C : 0);            \
    tmp = ((tmp >> 1) | c);                        \
    flagNZ(tmp);

#define OpSLO                           \
    _p &= ~PFLAG_C;                     \
    _p |= ((tmp & 0x80) ? PFLAG_C : 0); \
    tmp <<= 1;                          \
    _a |= tmp;                          \
    flagNZ(_a);
#define OpRLA                                      \
    std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); \
    _p &= ~PFLAG_C;                                \
    _p |= ((tmp & 0x80) ? PFLAG_C : 0);            \
    tmp = ((tmp << 1) | c);                        \
    _a &= tmp;                                     \
    flagNZ(_a);
#define OpSRE                           \
    _p &= ~PFLAG_C;                     \
    _p |= ((tmp & 0x01) ? PFLAG_C : 0); \
    tmp >>= 1;                          \
    _a ^= tmp;                          \
    flagNZ(_a);
#define OpRRA                                      \
    std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); \
    _p &= ~PFLAG_C;                                \
    _p |= ((tmp & 0x01) ? PFLAG_C : 0);            \
    tmp = ((tmp >> 1) | c);                        \
    _a  = adc(_a, tmp);
#define OpLAX      \
    _a = _x = tmp; \
    flagNZ(tmp);

#define OpANC       \
    _p &= ~PFLAG_C; \
    _a &= tmp;      \
    flagNZ(_a);     \
    _p |= (_p & PFLAG_N) ? PFLAG_C : 0;
#define OpALR                             \
    _a &= tmp;                            \
    _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); \
    _p |= ((_a & 0x01) ? PFLAG_C : 0);    \
    _a >>= 1;                             \
    _p |= (_a ? 0 : PFLAG_Z);
#define OpARR                                      \
    _a &= tmp;                                     \
    std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); \
    _p &= ~(PFLAG_C | PFLAG_V);                    \
    _a = ((_a >> 1) | c);                          \
    flagNZ(_a);                                    \
    _p |= ((_a & 0x40) ? PFLAG_C : 0);             \
    _p |= (((((_a >> 1) & 0x20) ^ (_a & 0x20))) ? PFLAG_V : 0);
#define OpXAA      \
    _a = _x & tmp; \
    flagNZ(_a);
#define OpAXS                          \
    _x &= _a;                          \
    _p &= ~PFLAG_C;                    \
    _p |= ((_x >= tmp) ? PFLAG_C : 0); \
    _x -= tmp;                         \
    flagNZ(_x);

#define OpDCP                        \
    tmp--;                           \
    _p &= ~PFLAG_C;                  \
    _p |= (_a >= tmp) ? PFLAG_C : 0; \
    flagNZ(_a - tmp);
#define OpISC \
    tmp++;    \
    _a = adc(_a, ~tmp);

#define INX flagNZ(++_x);
#define INY flagNZ(++_y);
#define DEX flagNZ(--_x);
#define DEY flagNZ(--_y);

#define FlagClearC _p &= ~PFLAG_C;
#define FlagClearI _p &= ~PFLAG_I;
#define FlagClearV _p &= ~PFLAG_V;
#define FlagClearD _p &= ~PFLAG_D;

#define FlagSetC _p |= PFLAG_C;
#define FlagSetI _p |= PFLAG_I;
#define FlagSetD _p |= PFLAG_D;

#define WriteReg     next = write(_addr, _regs[_selSrc], next);
#define WriteRegZero _memory.ram[_addr] = _regs[_selSrc];

#define WriteReg_SAX next = write(_addr, _a & _x, next);
#define WriteReg_SHX next = write(_addr, _x & ((_addr >> 8) + 1), next);
#define WriteReg_SHY next = write(_addr, _y & ((_addr >> 8) + 1), next);
#define WriteReg_AHX next = write(_addr, _a & _x & ((_addr >> 8) + 1), next);
#define WriteReg_TAS \
    _s   = _a & _x;  \
    next = write(_addr, _s & ((_addr >> 8) + 1), next);
#define WriteRegZero_SAX _memory.ram[_addr] = _a & _x;

#define ReadReg                   \
    _regs[_selDst] = read(_addr); \
    flagNZ(_regs[_selDst]);
#define ReadRegCarry                           \
    _regs[_selDst] = read(_addr);              \
    flagNZ(_regs[_selDst]);                    \
    _addr += ((std::uint16_t)_addrCarry << 8); \
    if (!_addrCarry) return (&CPU::dispatch);
#define ReadRegZero                      \
    _regs[_selDst] = _memory.ram[_addr]; \
    flagNZ(_regs[_selDst]);

#define ReadReg_AX         \
    _a = _x = read(_addr); \
    flagNZ(_a);
#define ReadRegCarry_AX                        \
    _a = _x = read(_addr);                     \
    flagNZ(_a);                                \
    _addr += ((std::uint16_t)_addrCarry << 8); \
    if (!_addrCarry) return (&CPU::dispatch);
#define ReadRegZero_AX            \
    _a = _x = _memory.ram[_addr]; \
    flagNZ(_a);

#define ReadReg_LAS                  \
    _a = _x = _s = read(_addr) & _s; \
    flagNZ(_s);
#define ReadRegCarry_LAS                       \
    _a = _x = _s = read(_addr) & _s;           \
    flagNZ(_s);                                \
    _addr += ((std::uint16_t)_addrCarry << 8); \
    if (!_addrCarry) return (&CPU::dispatch);

#define CarryFix _addr += ((std::uint16_t)_addrCarry << 8);

#define TransferAX \
    _x = _a;       \
    flagNZ(_a);
#define TransferAY \
    _y = _a;       \
    flagNZ(_a);
#define TransferSX \
    _x = _s;       \
    flagNZ(_s);
#define TransferXA \
    _a = _x;       \
    flagNZ(_x);
#define TransferXS _s = _x;
#define TransferYA \
    _a = _y;       \
    flagNZ(_y);

#define Nop      /* nop */ ;
#define ReadAddr read(_addr);
#define ReadAddrCarry \
    read(_addr);      \
    if (!_addrCarry) return (&CPU::dispatch);

#define SwitchPC _pc = (_addr | ((std::uint16_t)read(_pc) << 8));

#endif
