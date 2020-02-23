#include <libnin/CPU.h>
#include <libnin/Memory.h>

using namespace libnin;

template<> CPU::Handler CPU::instruction<0x103>(void) { _pc = ((_pc & 0x00ff) | (((std::uint16_t)read(_addr + 1)) << 8)); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x104>(void) { _pc = ((_pc & 0xff00) | read(_addr)); return ((Handler)&CPU::instruction<0x103>); }
template<> CPU::Handler CPU::instruction<0x105>(void) { _memory.ram[0x100 | _s] = _p | PFLAG_1 | PFLAG_B; _s--; _p |= PFLAG_I; return ((Handler)&CPU::instruction<0x104>); }
template<> CPU::Handler CPU::instruction<0x106>(void) { _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return ((Handler)&CPU::instruction<0x105>); }
template<> CPU::Handler CPU::instruction<0x107>(void) { _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return ((Handler)&CPU::instruction<0x106>); }
template<> CPU::Handler CPU::instruction<0x108>(void) { _s--; return ((Handler)&CPU::instruction<0x104>); }
template<> CPU::Handler CPU::instruction<0x109>(void) { _s--; return ((Handler)&CPU::instruction<0x108>); }
template<> CPU::Handler CPU::instruction<0x10a>(void) { _s--; return ((Handler)&CPU::instruction<0x109>); }
template<> CPU::Handler CPU::instruction<0x10b>(void) { _memory.ram[0x100 | _s] = _p | PFLAG_1; _s--; _p |= PFLAG_I; return ((Handler)&CPU::instruction<0x104>); }
template<> CPU::Handler CPU::instruction<0x10c>(void) { _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return ((Handler)&CPU::instruction<0x10b>); }
template<> CPU::Handler CPU::instruction<0x10d>(void) { _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return ((Handler)&CPU::instruction<0x10c>); }
template<> CPU::Handler CPU::instruction<0x10e>(void) { _memory.ram[0x100 | _s] = _p | PFLAG_1; _s--; return ((Handler)&CPU::instruction<0x104>); }
template<> CPU::Handler CPU::instruction<0x10f>(void) { _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return ((Handler)&CPU::instruction<0x10e>); }
template<> CPU::Handler CPU::instruction<0x110>(void) { _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return ((Handler)&CPU::instruction<0x10f>); }
template<> CPU::Handler CPU::instruction<0x111>(void) { _memory.ram[0x100 | _s] = _p | PFLAG_1 | PFLAG_B; _s--; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x112>(void) { _p = _memory.ram[0x100 | _s] & ~(PFLAG_1 | PFLAG_B); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x113>(void) { _s++; return ((Handler)&CPU::instruction<0x112>); }
template<> CPU::Handler CPU::instruction<0x114>(void) { _memory.ram[0x100 | _s] = _a; _s--; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x115>(void) { _a = _memory.ram[0x100 | _s]; flagNZ(_a); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x116>(void) { _s++; return ((Handler)&CPU::instruction<0x115>); }
template<> CPU::Handler CPU::instruction<0x117>(void) { _pc = (_addr | ((std::uint16_t)read(_pc) << 8)); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x118>(void) { _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return ((Handler)&CPU::instruction<0x117>); }
template<> CPU::Handler CPU::instruction<0x119>(void) { _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return ((Handler)&CPU::instruction<0x118>); }
template<> CPU::Handler CPU::instruction<0x11a>(void) { /* nop */ return ((Handler)&CPU::instruction<0x119>); }
template<> CPU::Handler CPU::instruction<0x11b>(void) { _pc = (_pc & 0x00ff) | ((std::uint16_t)_memory.ram[0x100 | _s] << 8); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x11c>(void) { _pc = (_pc & 0xff00) | _memory.ram[0x100 | _s]; _s++; return ((Handler)&CPU::instruction<0x11b>); }
template<> CPU::Handler CPU::instruction<0x11d>(void) { _p = _memory.ram[0x100 | _s] & ~(PFLAG_1 | PFLAG_B); _s++; return ((Handler)&CPU::instruction<0x11c>); }
template<> CPU::Handler CPU::instruction<0x11e>(void) { _s++; return ((Handler)&CPU::instruction<0x11d>); }
template<> CPU::Handler CPU::instruction<0x11f>(void) { read(_pc); _pc++; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x120>(void) { _pc = (_pc & 0x00ff) | ((std::uint16_t)_memory.ram[0x100 | _s] << 8); return ((Handler)&CPU::instruction<0x11f>); }
template<> CPU::Handler CPU::instruction<0x121>(void) { _pc = (_pc & 0xff00) | _memory.ram[0x100 | _s]; _s++; return ((Handler)&CPU::instruction<0x120>); }
template<> CPU::Handler CPU::instruction<0x122>(void) { _s++; return ((Handler)&CPU::instruction<0x121>); }
template<> CPU::Handler CPU::instruction<0x123>(void) { _pc = ((_pc & 0x00ff) | ((std::uint16_t)read((_addr & 0xff00) | ((_addr + 1) & 0xff))) << 8); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x124>(void) { _pc = ((_pc & 0xff00) | read(_addr)); return ((Handler)&CPU::instruction<0x123>); }
template<> CPU::Handler CPU::instruction<0x125>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x124>); }
template<> CPU::Handler CPU::instruction<0x126>(void) { /* nop */ return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x127>(void) { if (!_addrCarry) return (Handler)&CPU::dispatch; _pc = _addr; return ((Handler)&CPU::instruction<0x126>); }
template<> CPU::Handler CPU::instruction<0x128>(void) { _memory.ram[_addr] = _regs[_selSrc]; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x129>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x128>); }
template<> CPU::Handler CPU::instruction<0x12a>(void) { write(_addr, _regs[_selSrc]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x12b>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x12a>); }
template<> CPU::Handler CPU::instruction<0x12c>(void) { read(_addr); _addr += (((std::uint16_t)_addrCarry) << 8); return ((Handler)&CPU::instruction<0x12a>); }
template<> CPU::Handler CPU::instruction<0x12d>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x12c>); }
template<> CPU::Handler CPU::instruction<0x12e>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x12c>); }
template<> CPU::Handler CPU::instruction<0x12f>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x12a>); }
template<> CPU::Handler CPU::instruction<0x130>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x12f>); }
template<> CPU::Handler CPU::instruction<0x131>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x130>); }
template<> CPU::Handler CPU::instruction<0x132>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x12c>); }
template<> CPU::Handler CPU::instruction<0x133>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x132>); }
template<> CPU::Handler CPU::instruction<0x134>(void) { _addr = (_addr + _y) & 0xff; return ((Handler)&CPU::instruction<0x128>); }
template<> CPU::Handler CPU::instruction<0x135>(void) { _regs[_selDst] = _memory.ram[_addr]; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x136>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x135>); }
template<> CPU::Handler CPU::instruction<0x137>(void) { _regs[_selDst] = read(_addr); flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x138>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x137>); }
template<> CPU::Handler CPU::instruction<0x139>(void) { _regs[_selDst] = read(_addr); flagNZ(_regs[_selDst]); _addr += ((std::uint16_t)_addrCarry << 8); if (!_addrCarry) return ((Handler)&CPU::dispatch); return ((Handler)&CPU::instruction<0x137>); }
template<> CPU::Handler CPU::instruction<0x13a>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x139>); }
template<> CPU::Handler CPU::instruction<0x13b>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x139>); }
template<> CPU::Handler CPU::instruction<0x13c>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x137>); }
template<> CPU::Handler CPU::instruction<0x13d>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x13c>); }
template<> CPU::Handler CPU::instruction<0x13e>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x13d>); }
template<> CPU::Handler CPU::instruction<0x13f>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x139>); }
template<> CPU::Handler CPU::instruction<0x140>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x13f>); }
template<> CPU::Handler CPU::instruction<0x141>(void) { _addr = (_addr + _y) & 0xff; return ((Handler)&CPU::instruction<0x135>); }
template<> CPU::Handler CPU::instruction<0x142>(void) { std::uint8_t tmp = _memory.ram[_addr]; _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x143>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x142>); }
template<> CPU::Handler CPU::instruction<0x144>(void) { std::uint8_t tmp = read(_addr); _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x145>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x144>); }
template<> CPU::Handler CPU::instruction<0x146>(void) { std::uint8_t tmp = read(_addr); if (!_addrCarry) { _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return (Handler)&CPU::dispatch; } _addr += 0x100; return ((Handler)&CPU::instruction<0x144>); }
template<> CPU::Handler CPU::instruction<0x147>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x146>); }
template<> CPU::Handler CPU::instruction<0x148>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x146>); }
template<> CPU::Handler CPU::instruction<0x149>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x144>); }
template<> CPU::Handler CPU::instruction<0x14a>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x149>); }
template<> CPU::Handler CPU::instruction<0x14b>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x14a>); }
template<> CPU::Handler CPU::instruction<0x14c>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x146>); }
template<> CPU::Handler CPU::instruction<0x14d>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x14c>); }
template<> CPU::Handler CPU::instruction<0x14e>(void) { std::uint8_t tmp = _memory.ram[_addr]; _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x14f>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x14e>); }
template<> CPU::Handler CPU::instruction<0x150>(void) { std::uint8_t tmp = read(_addr); _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x151>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x150>); }
template<> CPU::Handler CPU::instruction<0x152>(void) { std::uint8_t tmp = read(_addr); if (!_addrCarry) { _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return (Handler)&CPU::dispatch; } _addr += 0x100; return ((Handler)&CPU::instruction<0x150>); }
template<> CPU::Handler CPU::instruction<0x153>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x152>); }
template<> CPU::Handler CPU::instruction<0x154>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x152>); }
template<> CPU::Handler CPU::instruction<0x155>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x150>); }
template<> CPU::Handler CPU::instruction<0x156>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x155>); }
template<> CPU::Handler CPU::instruction<0x157>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x156>); }
template<> CPU::Handler CPU::instruction<0x158>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x152>); }
template<> CPU::Handler CPU::instruction<0x159>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x158>); }
template<> CPU::Handler CPU::instruction<0x15a>(void) { std::uint8_t tmp = _memory.ram[_addr]; _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x15b>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x15a>); }
template<> CPU::Handler CPU::instruction<0x15c>(void) { std::uint8_t tmp = read(_addr); _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x15d>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x15c>); }
template<> CPU::Handler CPU::instruction<0x15e>(void) { std::uint8_t tmp = read(_addr); if (!_addrCarry) { _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return (Handler)&CPU::dispatch; } _addr += 0x100; return ((Handler)&CPU::instruction<0x15c>); }
template<> CPU::Handler CPU::instruction<0x15f>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x15e>); }
template<> CPU::Handler CPU::instruction<0x160>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x15e>); }
template<> CPU::Handler CPU::instruction<0x161>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x15c>); }
template<> CPU::Handler CPU::instruction<0x162>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x161>); }
template<> CPU::Handler CPU::instruction<0x163>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x162>); }
template<> CPU::Handler CPU::instruction<0x164>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x15e>); }
template<> CPU::Handler CPU::instruction<0x165>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x164>); }
template<> CPU::Handler CPU::instruction<0x166>(void) { std::uint8_t tmp = _memory.ram[_addr]; _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x167>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x166>); }
template<> CPU::Handler CPU::instruction<0x168>(void) { std::uint8_t tmp = read(_addr); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x169>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x168>); }
template<> CPU::Handler CPU::instruction<0x16a>(void) { std::uint8_t tmp = read(_addr); if (!_addrCarry) { _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return (Handler)&CPU::dispatch; } _addr += 0x100; return ((Handler)&CPU::instruction<0x168>); }
template<> CPU::Handler CPU::instruction<0x16b>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x16a>); }
template<> CPU::Handler CPU::instruction<0x16c>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x16a>); }
template<> CPU::Handler CPU::instruction<0x16d>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x168>); }
template<> CPU::Handler CPU::instruction<0x16e>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x16d>); }
template<> CPU::Handler CPU::instruction<0x16f>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x16e>); }
template<> CPU::Handler CPU::instruction<0x170>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x16a>); }
template<> CPU::Handler CPU::instruction<0x171>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x170>); }
template<> CPU::Handler CPU::instruction<0x172>(void) { std::uint8_t tmp = _memory.ram[_addr]; _a = adc(_a, tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x173>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x172>); }
template<> CPU::Handler CPU::instruction<0x174>(void) { std::uint8_t tmp = read(_addr); _a = adc(_a, tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x175>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x174>); }
template<> CPU::Handler CPU::instruction<0x176>(void) { std::uint8_t tmp = read(_addr); if (!_addrCarry) { _a = adc(_a, tmp); return (Handler)&CPU::dispatch; } _addr += 0x100; return ((Handler)&CPU::instruction<0x174>); }
template<> CPU::Handler CPU::instruction<0x177>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x176>); }
template<> CPU::Handler CPU::instruction<0x178>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x176>); }
template<> CPU::Handler CPU::instruction<0x179>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x174>); }
template<> CPU::Handler CPU::instruction<0x17a>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x179>); }
template<> CPU::Handler CPU::instruction<0x17b>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x17a>); }
template<> CPU::Handler CPU::instruction<0x17c>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x176>); }
template<> CPU::Handler CPU::instruction<0x17d>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x17c>); }
template<> CPU::Handler CPU::instruction<0x17e>(void) { std::uint8_t tmp = _memory.ram[_addr]; _a = adc(_a, ~tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x17f>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x17e>); }
template<> CPU::Handler CPU::instruction<0x180>(void) { std::uint8_t tmp = read(_addr); _a = adc(_a, ~tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x181>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x180>); }
template<> CPU::Handler CPU::instruction<0x182>(void) { std::uint8_t tmp = read(_addr); if (!_addrCarry) { _a = adc(_a, ~tmp); return (Handler)&CPU::dispatch; } _addr += 0x100; return ((Handler)&CPU::instruction<0x180>); }
template<> CPU::Handler CPU::instruction<0x183>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x182>); }
template<> CPU::Handler CPU::instruction<0x184>(void) { _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x182>); }
template<> CPU::Handler CPU::instruction<0x185>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return ((Handler)&CPU::instruction<0x180>); }
template<> CPU::Handler CPU::instruction<0x186>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x185>); }
template<> CPU::Handler CPU::instruction<0x187>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x186>); }
template<> CPU::Handler CPU::instruction<0x188>(void) { _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return ((Handler)&CPU::instruction<0x182>); }
template<> CPU::Handler CPU::instruction<0x189>(void) { _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return ((Handler)&CPU::instruction<0x188>); }
template<> CPU::Handler CPU::instruction<0x18a>(void) { _memory.ram[_addr] = _rmw; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x18b>(void) { std::uint8_t tmp = _rmw; _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp <<= 1; flagNZ(tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18a>); }
template<> CPU::Handler CPU::instruction<0x18c>(void) { _rmw = _memory.ram[_addr]; return ((Handler)&CPU::instruction<0x18b>); }
template<> CPU::Handler CPU::instruction<0x18d>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x18c>); }
template<> CPU::Handler CPU::instruction<0x18e>(void) { write(_addr, _rmw); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x18f>(void) { write(_addr, _rmw); std::uint8_t tmp = _rmw; _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp <<= 1; flagNZ(tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18e>); }
template<> CPU::Handler CPU::instruction<0x190>(void) { _rmw = read(_addr); return ((Handler)&CPU::instruction<0x18f>); }
template<> CPU::Handler CPU::instruction<0x191>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x190>); }
template<> CPU::Handler CPU::instruction<0x192>(void) { read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return ((Handler)&CPU::instruction<0x190>); }
template<> CPU::Handler CPU::instruction<0x193>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x192>); }
template<> CPU::Handler CPU::instruction<0x194>(void) { std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp = ((tmp << 1) | c); flagNZ(tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18a>); }
template<> CPU::Handler CPU::instruction<0x195>(void) { _rmw = _memory.ram[_addr]; return ((Handler)&CPU::instruction<0x194>); }
template<> CPU::Handler CPU::instruction<0x196>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x195>); }
template<> CPU::Handler CPU::instruction<0x197>(void) { write(_addr, _rmw); std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp = ((tmp << 1) | c); flagNZ(tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18e>); }
template<> CPU::Handler CPU::instruction<0x198>(void) { _rmw = read(_addr); return ((Handler)&CPU::instruction<0x197>); }
template<> CPU::Handler CPU::instruction<0x199>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x198>); }
template<> CPU::Handler CPU::instruction<0x19a>(void) { read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return ((Handler)&CPU::instruction<0x198>); }
template<> CPU::Handler CPU::instruction<0x19b>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x19a>); }
template<> CPU::Handler CPU::instruction<0x19c>(void) { std::uint8_t tmp = _rmw; _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp >>= 1; _p |= (tmp ? 0 : PFLAG_Z); _rmw = tmp; return ((Handler)&CPU::instruction<0x18a>); }
template<> CPU::Handler CPU::instruction<0x19d>(void) { _rmw = _memory.ram[_addr]; return ((Handler)&CPU::instruction<0x19c>); }
template<> CPU::Handler CPU::instruction<0x19e>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x19d>); }
template<> CPU::Handler CPU::instruction<0x19f>(void) { write(_addr, _rmw); std::uint8_t tmp = _rmw; _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp >>= 1; _p |= (tmp ? 0 : PFLAG_Z); _rmw = tmp; return ((Handler)&CPU::instruction<0x18e>); }
template<> CPU::Handler CPU::instruction<0x1a0>(void) { _rmw = read(_addr); return ((Handler)&CPU::instruction<0x19f>); }
template<> CPU::Handler CPU::instruction<0x1a1>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1a0>); }
template<> CPU::Handler CPU::instruction<0x1a2>(void) { read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return ((Handler)&CPU::instruction<0x1a0>); }
template<> CPU::Handler CPU::instruction<0x1a3>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1a2>); }
template<> CPU::Handler CPU::instruction<0x1a4>(void) { std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp = ((tmp >> 1) | c); flagNZ(tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18a>); }
template<> CPU::Handler CPU::instruction<0x1a5>(void) { _rmw = _memory.ram[_addr]; return ((Handler)&CPU::instruction<0x1a4>); }
template<> CPU::Handler CPU::instruction<0x1a6>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x1a5>); }
template<> CPU::Handler CPU::instruction<0x1a7>(void) { write(_addr, _rmw); std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp = ((tmp >> 1) | c); flagNZ(tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18e>); }
template<> CPU::Handler CPU::instruction<0x1a8>(void) { _rmw = read(_addr); return ((Handler)&CPU::instruction<0x1a7>); }
template<> CPU::Handler CPU::instruction<0x1a9>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1a8>); }
template<> CPU::Handler CPU::instruction<0x1aa>(void) { read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return ((Handler)&CPU::instruction<0x1a8>); }
template<> CPU::Handler CPU::instruction<0x1ab>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1aa>); }
template<> CPU::Handler CPU::instruction<0x1ac>(void) { std::uint8_t tmp = _rmw; flagNZ(++tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18a>); }
template<> CPU::Handler CPU::instruction<0x1ad>(void) { _rmw = _memory.ram[_addr]; return ((Handler)&CPU::instruction<0x1ac>); }
template<> CPU::Handler CPU::instruction<0x1ae>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x1ad>); }
template<> CPU::Handler CPU::instruction<0x1af>(void) { write(_addr, _rmw); std::uint8_t tmp = _rmw; flagNZ(++tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18e>); }
template<> CPU::Handler CPU::instruction<0x1b0>(void) { _rmw = read(_addr); return ((Handler)&CPU::instruction<0x1af>); }
template<> CPU::Handler CPU::instruction<0x1b1>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1b0>); }
template<> CPU::Handler CPU::instruction<0x1b2>(void) { read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return ((Handler)&CPU::instruction<0x1b0>); }
template<> CPU::Handler CPU::instruction<0x1b3>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1b2>); }
template<> CPU::Handler CPU::instruction<0x1b4>(void) { std::uint8_t tmp = _rmw; flagNZ(--tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18a>); }
template<> CPU::Handler CPU::instruction<0x1b5>(void) { _rmw = _memory.ram[_addr]; return ((Handler)&CPU::instruction<0x1b4>); }
template<> CPU::Handler CPU::instruction<0x1b6>(void) { _addr = (_addr + _x) & 0xff; return ((Handler)&CPU::instruction<0x1b5>); }
template<> CPU::Handler CPU::instruction<0x1b7>(void) { write(_addr, _rmw); std::uint8_t tmp = _rmw; flagNZ(--tmp); _rmw = tmp; return ((Handler)&CPU::instruction<0x18e>); }
template<> CPU::Handler CPU::instruction<0x1b8>(void) { _rmw = read(_addr); return ((Handler)&CPU::instruction<0x1b7>); }
template<> CPU::Handler CPU::instruction<0x1b9>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1b8>); }
template<> CPU::Handler CPU::instruction<0x1ba>(void) { read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return ((Handler)&CPU::instruction<0x1b8>); }
template<> CPU::Handler CPU::instruction<0x1bb>(void) { _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1ba>); }
template<> CPU::Handler CPU::instruction<0x1bc>(void) { std::uint8_t tmp = _memory.ram[_addr]; _p &= ~(PFLAG_Z | PFLAG_N | PFLAG_V); _p |= (tmp & 0xc0); _p |= ((_a & tmp) ? 0 : PFLAG_Z); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x1bd>(void) { std::uint8_t tmp = read(_addr); _p &= ~(PFLAG_Z | PFLAG_N | PFLAG_V); _p |= (tmp & 0xc0); _p |= ((_a & tmp) ? 0 : PFLAG_Z); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x1be>(void) { _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return ((Handler)&CPU::instruction<0x1bd>); }
template<> CPU::Handler CPU::instruction<0x000>(void) { _addr = 0xfffe; read(_pc++); return ((Handler)&CPU::instruction<0x107>); }
template<> CPU::Handler CPU::instruction<0x001>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x14b>); }
template<> CPU::Handler CPU::instruction<0x002>(void) { return debug_not_impl(0x002); }
template<> CPU::Handler CPU::instruction<0x003>(void) { return debug_not_impl(0x003); }
template<> CPU::Handler CPU::instruction<0x004>(void) { return debug_not_impl(0x004); }
template<> CPU::Handler CPU::instruction<0x005>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x142>); }
template<> CPU::Handler CPU::instruction<0x006>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x18c>); }
template<> CPU::Handler CPU::instruction<0x007>(void) { return debug_not_impl(0x007); }
template<> CPU::Handler CPU::instruction<0x008>(void) { read(_pc); return ((Handler)&CPU::instruction<0x111>); }
template<> CPU::Handler CPU::instruction<0x009>(void) { _selDst = REG_A; std::uint8_t tmp = read(_pc++); _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x00a>(void) { read(_pc); std::uint8_t tmp = _a; _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp <<= 1; flagNZ(tmp); _a = tmp; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x00b>(void) { return debug_not_impl(0x00b); }
template<> CPU::Handler CPU::instruction<0x00c>(void) { return debug_not_impl(0x00c); }
template<> CPU::Handler CPU::instruction<0x00d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x145>); }
template<> CPU::Handler CPU::instruction<0x00e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x191>); }
template<> CPU::Handler CPU::instruction<0x00f>(void) { return debug_not_impl(0x00f); }
template<> CPU::Handler CPU::instruction<0x010>(void) { _addr = read(_pc++); if (_p & PFLAG_N) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x011>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x14d>); }
template<> CPU::Handler CPU::instruction<0x012>(void) { return debug_not_impl(0x012); }
template<> CPU::Handler CPU::instruction<0x013>(void) { return debug_not_impl(0x013); }
template<> CPU::Handler CPU::instruction<0x014>(void) { return debug_not_impl(0x014); }
template<> CPU::Handler CPU::instruction<0x015>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x143>); }
template<> CPU::Handler CPU::instruction<0x016>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x18d>); }
template<> CPU::Handler CPU::instruction<0x017>(void) { return debug_not_impl(0x017); }
template<> CPU::Handler CPU::instruction<0x018>(void) { _p &= ~PFLAG_C; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x019>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x148>); }
template<> CPU::Handler CPU::instruction<0x01a>(void) { return debug_not_impl(0x01a); }
template<> CPU::Handler CPU::instruction<0x01b>(void) { return debug_not_impl(0x01b); }
template<> CPU::Handler CPU::instruction<0x01c>(void) { return debug_not_impl(0x01c); }
template<> CPU::Handler CPU::instruction<0x01d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x147>); }
template<> CPU::Handler CPU::instruction<0x01e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x193>); }
template<> CPU::Handler CPU::instruction<0x01f>(void) { return debug_not_impl(0x01f); }
template<> CPU::Handler CPU::instruction<0x020>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x11a>); }
template<> CPU::Handler CPU::instruction<0x021>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x157>); }
template<> CPU::Handler CPU::instruction<0x022>(void) { return debug_not_impl(0x022); }
template<> CPU::Handler CPU::instruction<0x023>(void) { return debug_not_impl(0x023); }
template<> CPU::Handler CPU::instruction<0x024>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1bc>); }
template<> CPU::Handler CPU::instruction<0x025>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x14e>); }
template<> CPU::Handler CPU::instruction<0x026>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x195>); }
template<> CPU::Handler CPU::instruction<0x027>(void) { return debug_not_impl(0x027); }
template<> CPU::Handler CPU::instruction<0x028>(void) { read(_pc); return ((Handler)&CPU::instruction<0x113>); }
template<> CPU::Handler CPU::instruction<0x029>(void) { _selDst = REG_A; std::uint8_t tmp = read(_pc++); _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x02a>(void) { read(_pc); std::uint8_t tmp = _a; std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp = ((tmp << 1) | c); flagNZ(tmp); _a = tmp; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x02b>(void) { return debug_not_impl(0x02b); }
template<> CPU::Handler CPU::instruction<0x02c>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1be>); }
template<> CPU::Handler CPU::instruction<0x02d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x151>); }
template<> CPU::Handler CPU::instruction<0x02e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x199>); }
template<> CPU::Handler CPU::instruction<0x02f>(void) { return debug_not_impl(0x02f); }
template<> CPU::Handler CPU::instruction<0x030>(void) { _addr = read(_pc++); if (!(_p & PFLAG_N)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x031>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x159>); }
template<> CPU::Handler CPU::instruction<0x032>(void) { return debug_not_impl(0x032); }
template<> CPU::Handler CPU::instruction<0x033>(void) { return debug_not_impl(0x033); }
template<> CPU::Handler CPU::instruction<0x034>(void) { return debug_not_impl(0x034); }
template<> CPU::Handler CPU::instruction<0x035>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x14f>); }
template<> CPU::Handler CPU::instruction<0x036>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x196>); }
template<> CPU::Handler CPU::instruction<0x037>(void) { return debug_not_impl(0x037); }
template<> CPU::Handler CPU::instruction<0x038>(void) { _p |= PFLAG_C; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x039>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x154>); }
template<> CPU::Handler CPU::instruction<0x03a>(void) { return debug_not_impl(0x03a); }
template<> CPU::Handler CPU::instruction<0x03b>(void) { return debug_not_impl(0x03b); }
template<> CPU::Handler CPU::instruction<0x03c>(void) { return debug_not_impl(0x03c); }
template<> CPU::Handler CPU::instruction<0x03d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x153>); }
template<> CPU::Handler CPU::instruction<0x03e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x19b>); }
template<> CPU::Handler CPU::instruction<0x03f>(void) { return debug_not_impl(0x03f); }
template<> CPU::Handler CPU::instruction<0x040>(void) { read(_pc); return ((Handler)&CPU::instruction<0x11e>); }
template<> CPU::Handler CPU::instruction<0x041>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x163>); }
template<> CPU::Handler CPU::instruction<0x042>(void) { return debug_not_impl(0x042); }
template<> CPU::Handler CPU::instruction<0x043>(void) { return debug_not_impl(0x043); }
template<> CPU::Handler CPU::instruction<0x044>(void) { return debug_not_impl(0x044); }
template<> CPU::Handler CPU::instruction<0x045>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x15a>); }
template<> CPU::Handler CPU::instruction<0x046>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x19d>); }
template<> CPU::Handler CPU::instruction<0x047>(void) { return debug_not_impl(0x047); }
template<> CPU::Handler CPU::instruction<0x048>(void) { read(_pc); return ((Handler)&CPU::instruction<0x114>); }
template<> CPU::Handler CPU::instruction<0x049>(void) { _selDst = REG_A; std::uint8_t tmp = read(_pc++); _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x04a>(void) { read(_pc); std::uint8_t tmp = _a; _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp >>= 1; _p |= (tmp ? 0 : PFLAG_Z); _a = tmp; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x04b>(void) { return debug_not_impl(0x04b); }
template<> CPU::Handler CPU::instruction<0x04c>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x117>); }
template<> CPU::Handler CPU::instruction<0x04d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x15d>); }
template<> CPU::Handler CPU::instruction<0x04e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1a1>); }
template<> CPU::Handler CPU::instruction<0x04f>(void) { return debug_not_impl(0x04f); }
template<> CPU::Handler CPU::instruction<0x050>(void) { _addr = read(_pc++); if (_p & PFLAG_V) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x051>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x165>); }
template<> CPU::Handler CPU::instruction<0x052>(void) { return debug_not_impl(0x052); }
template<> CPU::Handler CPU::instruction<0x053>(void) { return debug_not_impl(0x053); }
template<> CPU::Handler CPU::instruction<0x054>(void) { return debug_not_impl(0x054); }
template<> CPU::Handler CPU::instruction<0x055>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x15b>); }
template<> CPU::Handler CPU::instruction<0x056>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x19e>); }
template<> CPU::Handler CPU::instruction<0x057>(void) { return debug_not_impl(0x057); }
template<> CPU::Handler CPU::instruction<0x058>(void) { _p &= ~PFLAG_I; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x059>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x160>); }
template<> CPU::Handler CPU::instruction<0x05a>(void) { return debug_not_impl(0x05a); }
template<> CPU::Handler CPU::instruction<0x05b>(void) { return debug_not_impl(0x05b); }
template<> CPU::Handler CPU::instruction<0x05c>(void) { return debug_not_impl(0x05c); }
template<> CPU::Handler CPU::instruction<0x05d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x15f>); }
template<> CPU::Handler CPU::instruction<0x05e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1a3>); }
template<> CPU::Handler CPU::instruction<0x05f>(void) { return debug_not_impl(0x05f); }
template<> CPU::Handler CPU::instruction<0x060>(void) { read(_pc); return ((Handler)&CPU::instruction<0x122>); }
template<> CPU::Handler CPU::instruction<0x061>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x17b>); }
template<> CPU::Handler CPU::instruction<0x062>(void) { return debug_not_impl(0x062); }
template<> CPU::Handler CPU::instruction<0x063>(void) { return debug_not_impl(0x063); }
template<> CPU::Handler CPU::instruction<0x064>(void) { return debug_not_impl(0x064); }
template<> CPU::Handler CPU::instruction<0x065>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x172>); }
template<> CPU::Handler CPU::instruction<0x066>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1a5>); }
template<> CPU::Handler CPU::instruction<0x067>(void) { return debug_not_impl(0x067); }
template<> CPU::Handler CPU::instruction<0x068>(void) { read(_pc); return ((Handler)&CPU::instruction<0x116>); }
template<> CPU::Handler CPU::instruction<0x069>(void) { _selDst = REG_A; std::uint8_t tmp = read(_pc++); _a = adc(_a, tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x06a>(void) { read(_pc); std::uint8_t tmp = _a; std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp = ((tmp >> 1) | c); flagNZ(tmp); _a = tmp; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x06b>(void) { return debug_not_impl(0x06b); }
template<> CPU::Handler CPU::instruction<0x06c>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x125>); }
template<> CPU::Handler CPU::instruction<0x06d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x175>); }
template<> CPU::Handler CPU::instruction<0x06e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1a9>); }
template<> CPU::Handler CPU::instruction<0x06f>(void) { return debug_not_impl(0x06f); }
template<> CPU::Handler CPU::instruction<0x070>(void) { _addr = read(_pc++); if (!(_p & PFLAG_V)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x071>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x17d>); }
template<> CPU::Handler CPU::instruction<0x072>(void) { return debug_not_impl(0x072); }
template<> CPU::Handler CPU::instruction<0x073>(void) { return debug_not_impl(0x073); }
template<> CPU::Handler CPU::instruction<0x074>(void) { return debug_not_impl(0x074); }
template<> CPU::Handler CPU::instruction<0x075>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x173>); }
template<> CPU::Handler CPU::instruction<0x076>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1a6>); }
template<> CPU::Handler CPU::instruction<0x077>(void) { return debug_not_impl(0x077); }
template<> CPU::Handler CPU::instruction<0x078>(void) { _p |= PFLAG_I; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x079>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x178>); }
template<> CPU::Handler CPU::instruction<0x07a>(void) { return debug_not_impl(0x07a); }
template<> CPU::Handler CPU::instruction<0x07b>(void) { return debug_not_impl(0x07b); }
template<> CPU::Handler CPU::instruction<0x07c>(void) { return debug_not_impl(0x07c); }
template<> CPU::Handler CPU::instruction<0x07d>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x177>); }
template<> CPU::Handler CPU::instruction<0x07e>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1ab>); }
template<> CPU::Handler CPU::instruction<0x07f>(void) { return debug_not_impl(0x07f); }
template<> CPU::Handler CPU::instruction<0x080>(void) { return debug_not_impl(0x080); }
template<> CPU::Handler CPU::instruction<0x081>(void) { _selSrc = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x131>); }
template<> CPU::Handler CPU::instruction<0x082>(void) { return debug_not_impl(0x082); }
template<> CPU::Handler CPU::instruction<0x083>(void) { return debug_not_impl(0x083); }
template<> CPU::Handler CPU::instruction<0x084>(void) { _selSrc = REG_Y; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x128>); }
template<> CPU::Handler CPU::instruction<0x085>(void) { _selSrc = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x128>); }
template<> CPU::Handler CPU::instruction<0x086>(void) { _selSrc = REG_X; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x128>); }
template<> CPU::Handler CPU::instruction<0x087>(void) { return debug_not_impl(0x087); }
template<> CPU::Handler CPU::instruction<0x088>(void) { flagNZ(--_y); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x089>(void) { return debug_not_impl(0x089); }
template<> CPU::Handler CPU::instruction<0x08a>(void) { read(_pc); _a = _x; flagNZ(_x); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x08b>(void) { return debug_not_impl(0x08b); }
template<> CPU::Handler CPU::instruction<0x08c>(void) { _selSrc = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x12b>); }
template<> CPU::Handler CPU::instruction<0x08d>(void) { _selSrc = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x12b>); }
template<> CPU::Handler CPU::instruction<0x08e>(void) { _selSrc = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x12b>); }
template<> CPU::Handler CPU::instruction<0x08f>(void) { return debug_not_impl(0x08f); }
template<> CPU::Handler CPU::instruction<0x090>(void) { _addr = read(_pc++); if (_p & PFLAG_C) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x091>(void) { _selSrc = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x133>); }
template<> CPU::Handler CPU::instruction<0x092>(void) { return debug_not_impl(0x092); }
template<> CPU::Handler CPU::instruction<0x093>(void) { return debug_not_impl(0x093); }
template<> CPU::Handler CPU::instruction<0x094>(void) { _selSrc = REG_Y; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x129>); }
template<> CPU::Handler CPU::instruction<0x095>(void) { _selSrc = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x129>); }
template<> CPU::Handler CPU::instruction<0x096>(void) { _selSrc = REG_X; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x134>); }
template<> CPU::Handler CPU::instruction<0x097>(void) { return debug_not_impl(0x097); }
template<> CPU::Handler CPU::instruction<0x098>(void) { read(_pc); _a = _y; flagNZ(_y); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x099>(void) { _selSrc = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x12e>); }
template<> CPU::Handler CPU::instruction<0x09a>(void) { read(_pc); _s = _x; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x09b>(void) { return debug_not_impl(0x09b); }
template<> CPU::Handler CPU::instruction<0x09c>(void) { return debug_not_impl(0x09c); }
template<> CPU::Handler CPU::instruction<0x09d>(void) { _selSrc = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x12d>); }
template<> CPU::Handler CPU::instruction<0x09e>(void) { return debug_not_impl(0x09e); }
template<> CPU::Handler CPU::instruction<0x09f>(void) { return debug_not_impl(0x09f); }
template<> CPU::Handler CPU::instruction<0x0a0>(void) { _y = read(_pc++); flagNZ(_y); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0a1>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x13e>); }
template<> CPU::Handler CPU::instruction<0x0a2>(void) { _x = read(_pc++); flagNZ(_x); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0a3>(void) { return debug_not_impl(0x0a3); }
template<> CPU::Handler CPU::instruction<0x0a4>(void) { _selDst = REG_Y; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x135>); }
template<> CPU::Handler CPU::instruction<0x0a5>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x135>); }
template<> CPU::Handler CPU::instruction<0x0a6>(void) { _selDst = REG_X; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x135>); }
template<> CPU::Handler CPU::instruction<0x0a7>(void) { return debug_not_impl(0x0a7); }
template<> CPU::Handler CPU::instruction<0x0a8>(void) { read(_pc); _y = _a; flagNZ(_a); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0a9>(void) { _a = read(_pc++); flagNZ(_a); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0aa>(void) { read(_pc); _x = _a; flagNZ(_a); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0ab>(void) { return debug_not_impl(0x0ab); }
template<> CPU::Handler CPU::instruction<0x0ac>(void) { _selDst = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x138>); }
template<> CPU::Handler CPU::instruction<0x0ad>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x138>); }
template<> CPU::Handler CPU::instruction<0x0ae>(void) { _selDst = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x138>); }
template<> CPU::Handler CPU::instruction<0x0af>(void) { return debug_not_impl(0x0af); }
template<> CPU::Handler CPU::instruction<0x0b0>(void) { _addr = read(_pc++); if (!(_p & PFLAG_C)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x0b1>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x140>); }
template<> CPU::Handler CPU::instruction<0x0b2>(void) { return debug_not_impl(0x0b2); }
template<> CPU::Handler CPU::instruction<0x0b3>(void) { return debug_not_impl(0x0b3); }
template<> CPU::Handler CPU::instruction<0x0b4>(void) { _selDst = REG_Y; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x136>); }
template<> CPU::Handler CPU::instruction<0x0b5>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x136>); }
template<> CPU::Handler CPU::instruction<0x0b6>(void) { _selDst = REG_X; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x141>); }
template<> CPU::Handler CPU::instruction<0x0b7>(void) { return debug_not_impl(0x0b7); }
template<> CPU::Handler CPU::instruction<0x0b8>(void) { _p &= ~PFLAG_V; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0b9>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x13b>); }
template<> CPU::Handler CPU::instruction<0x0ba>(void) { read(_pc); _x = _s; flagNZ(_s); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0bb>(void) { return debug_not_impl(0x0bb); }
template<> CPU::Handler CPU::instruction<0x0bc>(void) { _selDst = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x13a>); }
template<> CPU::Handler CPU::instruction<0x0bd>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x13a>); }
template<> CPU::Handler CPU::instruction<0x0be>(void) { _selDst = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x13b>); }
template<> CPU::Handler CPU::instruction<0x0bf>(void) { return debug_not_impl(0x0bf); }
template<> CPU::Handler CPU::instruction<0x0c0>(void) { _selDst = REG_Y; std::uint8_t tmp = read(_pc++); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0c1>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x16f>); }
template<> CPU::Handler CPU::instruction<0x0c2>(void) { return debug_not_impl(0x0c2); }
template<> CPU::Handler CPU::instruction<0x0c3>(void) { return debug_not_impl(0x0c3); }
template<> CPU::Handler CPU::instruction<0x0c4>(void) { _selDst = REG_Y; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x166>); }
template<> CPU::Handler CPU::instruction<0x0c5>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x166>); }
template<> CPU::Handler CPU::instruction<0x0c6>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1b5>); }
template<> CPU::Handler CPU::instruction<0x0c7>(void) { return debug_not_impl(0x0c7); }
template<> CPU::Handler CPU::instruction<0x0c8>(void) { flagNZ(++_y); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0c9>(void) { _selDst = REG_A; std::uint8_t tmp = read(_pc++); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0ca>(void) { flagNZ(--_x); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0cb>(void) { return debug_not_impl(0x0cb); }
template<> CPU::Handler CPU::instruction<0x0cc>(void) { _selDst = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x169>); }
template<> CPU::Handler CPU::instruction<0x0cd>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x169>); }
template<> CPU::Handler CPU::instruction<0x0ce>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1b9>); }
template<> CPU::Handler CPU::instruction<0x0cf>(void) { return debug_not_impl(0x0cf); }
template<> CPU::Handler CPU::instruction<0x0d0>(void) { _addr = read(_pc++); if (_p & PFLAG_Z) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x0d1>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x171>); }
template<> CPU::Handler CPU::instruction<0x0d2>(void) { return debug_not_impl(0x0d2); }
template<> CPU::Handler CPU::instruction<0x0d3>(void) { return debug_not_impl(0x0d3); }
template<> CPU::Handler CPU::instruction<0x0d4>(void) { return debug_not_impl(0x0d4); }
template<> CPU::Handler CPU::instruction<0x0d5>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x167>); }
template<> CPU::Handler CPU::instruction<0x0d6>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1b6>); }
template<> CPU::Handler CPU::instruction<0x0d7>(void) { return debug_not_impl(0x0d7); }
template<> CPU::Handler CPU::instruction<0x0d8>(void) { _p &= ~PFLAG_D; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0d9>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x16c>); }
template<> CPU::Handler CPU::instruction<0x0da>(void) { return debug_not_impl(0x0da); }
template<> CPU::Handler CPU::instruction<0x0db>(void) { return debug_not_impl(0x0db); }
template<> CPU::Handler CPU::instruction<0x0dc>(void) { return debug_not_impl(0x0dc); }
template<> CPU::Handler CPU::instruction<0x0dd>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x16b>); }
template<> CPU::Handler CPU::instruction<0x0de>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1bb>); }
template<> CPU::Handler CPU::instruction<0x0df>(void) { return debug_not_impl(0x0df); }
template<> CPU::Handler CPU::instruction<0x0e0>(void) { _selDst = REG_X; std::uint8_t tmp = read(_pc++); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0e1>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x187>); }
template<> CPU::Handler CPU::instruction<0x0e2>(void) { return debug_not_impl(0x0e2); }
template<> CPU::Handler CPU::instruction<0x0e3>(void) { return debug_not_impl(0x0e3); }
template<> CPU::Handler CPU::instruction<0x0e4>(void) { _selDst = REG_X; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x166>); }
template<> CPU::Handler CPU::instruction<0x0e5>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x17e>); }
template<> CPU::Handler CPU::instruction<0x0e6>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1ad>); }
template<> CPU::Handler CPU::instruction<0x0e7>(void) { return debug_not_impl(0x0e7); }
template<> CPU::Handler CPU::instruction<0x0e8>(void) { flagNZ(++_x); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0e9>(void) { _selDst = REG_A; std::uint8_t tmp = read(_pc++); _a = adc(_a, ~tmp); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0ea>(void) { /* nop */ /* nop */ return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0eb>(void) { return debug_not_impl(0x0eb); }
template<> CPU::Handler CPU::instruction<0x0ec>(void) { _selDst = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x169>); }
template<> CPU::Handler CPU::instruction<0x0ed>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x181>); }
template<> CPU::Handler CPU::instruction<0x0ee>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1b1>); }
template<> CPU::Handler CPU::instruction<0x0ef>(void) { return debug_not_impl(0x0ef); }
template<> CPU::Handler CPU::instruction<0x0f0>(void) { _addr = read(_pc++); if (!(_p & PFLAG_Z)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return ((Handler)&CPU::instruction<0x127>); }
template<> CPU::Handler CPU::instruction<0x0f1>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x189>); }
template<> CPU::Handler CPU::instruction<0x0f2>(void) { return debug_not_impl(0x0f2); }
template<> CPU::Handler CPU::instruction<0x0f3>(void) { return debug_not_impl(0x0f3); }
template<> CPU::Handler CPU::instruction<0x0f4>(void) { return debug_not_impl(0x0f4); }
template<> CPU::Handler CPU::instruction<0x0f5>(void) { _selDst = REG_A; _addr = read(_pc++); return ((Handler)&CPU::instruction<0x17f>); }
template<> CPU::Handler CPU::instruction<0x0f6>(void) { _addr = read(_pc++); return ((Handler)&CPU::instruction<0x1ae>); }
template<> CPU::Handler CPU::instruction<0x0f7>(void) { return debug_not_impl(0x0f7); }
template<> CPU::Handler CPU::instruction<0x0f8>(void) { _p |= PFLAG_D; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0f9>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x184>); }
template<> CPU::Handler CPU::instruction<0x0fa>(void) { return debug_not_impl(0x0fa); }
template<> CPU::Handler CPU::instruction<0x0fb>(void) { return debug_not_impl(0x0fb); }
template<> CPU::Handler CPU::instruction<0x0fc>(void) { return debug_not_impl(0x0fc); }
template<> CPU::Handler CPU::instruction<0x0fd>(void) { _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x183>); }
template<> CPU::Handler CPU::instruction<0x0fe>(void) { _addr = ((_addr & 0xff00) | read(_pc++)); return ((Handler)&CPU::instruction<0x1b3>); }
template<> CPU::Handler CPU::instruction<0x0ff>(void) { return debug_not_impl(0x0ff); }
template<> CPU::Handler CPU::instruction<0x100>(void) { _addr = 0xfffc; read(_pc); _p |= PFLAG_I; return ((Handler)&CPU::instruction<0x10a>); }
template<> CPU::Handler CPU::instruction<0x101>(void) { _addr = 0xfffe; read(_pc); return ((Handler)&CPU::instruction<0x10d>); }
template<> CPU::Handler CPU::instruction<0x102>(void) { _addr = 0xfffa; read(_pc); return ((Handler)&CPU::instruction<0x110>); }

const CPU::Handler CPU::kStates[] = {
    ((CPU::Handler)&CPU::instruction<0x000>), ((CPU::Handler)&CPU::instruction<0x001>), ((CPU::Handler)&CPU::instruction<0x002>), ((CPU::Handler)&CPU::instruction<0x003>),
    ((CPU::Handler)&CPU::instruction<0x004>), ((CPU::Handler)&CPU::instruction<0x005>), ((CPU::Handler)&CPU::instruction<0x006>), ((CPU::Handler)&CPU::instruction<0x007>),
    ((CPU::Handler)&CPU::instruction<0x008>), ((CPU::Handler)&CPU::instruction<0x009>), ((CPU::Handler)&CPU::instruction<0x00a>), ((CPU::Handler)&CPU::instruction<0x00b>),
    ((CPU::Handler)&CPU::instruction<0x00c>), ((CPU::Handler)&CPU::instruction<0x00d>), ((CPU::Handler)&CPU::instruction<0x00e>), ((CPU::Handler)&CPU::instruction<0x00f>),
    ((CPU::Handler)&CPU::instruction<0x010>), ((CPU::Handler)&CPU::instruction<0x011>), ((CPU::Handler)&CPU::instruction<0x012>), ((CPU::Handler)&CPU::instruction<0x013>),
    ((CPU::Handler)&CPU::instruction<0x014>), ((CPU::Handler)&CPU::instruction<0x015>), ((CPU::Handler)&CPU::instruction<0x016>), ((CPU::Handler)&CPU::instruction<0x017>),
    ((CPU::Handler)&CPU::instruction<0x018>), ((CPU::Handler)&CPU::instruction<0x019>), ((CPU::Handler)&CPU::instruction<0x01a>), ((CPU::Handler)&CPU::instruction<0x01b>),
    ((CPU::Handler)&CPU::instruction<0x01c>), ((CPU::Handler)&CPU::instruction<0x01d>), ((CPU::Handler)&CPU::instruction<0x01e>), ((CPU::Handler)&CPU::instruction<0x01f>),
    ((CPU::Handler)&CPU::instruction<0x020>), ((CPU::Handler)&CPU::instruction<0x021>), ((CPU::Handler)&CPU::instruction<0x022>), ((CPU::Handler)&CPU::instruction<0x023>),
    ((CPU::Handler)&CPU::instruction<0x024>), ((CPU::Handler)&CPU::instruction<0x025>), ((CPU::Handler)&CPU::instruction<0x026>), ((CPU::Handler)&CPU::instruction<0x027>),
    ((CPU::Handler)&CPU::instruction<0x028>), ((CPU::Handler)&CPU::instruction<0x029>), ((CPU::Handler)&CPU::instruction<0x02a>), ((CPU::Handler)&CPU::instruction<0x02b>),
    ((CPU::Handler)&CPU::instruction<0x02c>), ((CPU::Handler)&CPU::instruction<0x02d>), ((CPU::Handler)&CPU::instruction<0x02e>), ((CPU::Handler)&CPU::instruction<0x02f>),
    ((CPU::Handler)&CPU::instruction<0x030>), ((CPU::Handler)&CPU::instruction<0x031>), ((CPU::Handler)&CPU::instruction<0x032>), ((CPU::Handler)&CPU::instruction<0x033>),
    ((CPU::Handler)&CPU::instruction<0x034>), ((CPU::Handler)&CPU::instruction<0x035>), ((CPU::Handler)&CPU::instruction<0x036>), ((CPU::Handler)&CPU::instruction<0x037>),
    ((CPU::Handler)&CPU::instruction<0x038>), ((CPU::Handler)&CPU::instruction<0x039>), ((CPU::Handler)&CPU::instruction<0x03a>), ((CPU::Handler)&CPU::instruction<0x03b>),
    ((CPU::Handler)&CPU::instruction<0x03c>), ((CPU::Handler)&CPU::instruction<0x03d>), ((CPU::Handler)&CPU::instruction<0x03e>), ((CPU::Handler)&CPU::instruction<0x03f>),
    ((CPU::Handler)&CPU::instruction<0x040>), ((CPU::Handler)&CPU::instruction<0x041>), ((CPU::Handler)&CPU::instruction<0x042>), ((CPU::Handler)&CPU::instruction<0x043>),
    ((CPU::Handler)&CPU::instruction<0x044>), ((CPU::Handler)&CPU::instruction<0x045>), ((CPU::Handler)&CPU::instruction<0x046>), ((CPU::Handler)&CPU::instruction<0x047>),
    ((CPU::Handler)&CPU::instruction<0x048>), ((CPU::Handler)&CPU::instruction<0x049>), ((CPU::Handler)&CPU::instruction<0x04a>), ((CPU::Handler)&CPU::instruction<0x04b>),
    ((CPU::Handler)&CPU::instruction<0x04c>), ((CPU::Handler)&CPU::instruction<0x04d>), ((CPU::Handler)&CPU::instruction<0x04e>), ((CPU::Handler)&CPU::instruction<0x04f>),
    ((CPU::Handler)&CPU::instruction<0x050>), ((CPU::Handler)&CPU::instruction<0x051>), ((CPU::Handler)&CPU::instruction<0x052>), ((CPU::Handler)&CPU::instruction<0x053>),
    ((CPU::Handler)&CPU::instruction<0x054>), ((CPU::Handler)&CPU::instruction<0x055>), ((CPU::Handler)&CPU::instruction<0x056>), ((CPU::Handler)&CPU::instruction<0x057>),
    ((CPU::Handler)&CPU::instruction<0x058>), ((CPU::Handler)&CPU::instruction<0x059>), ((CPU::Handler)&CPU::instruction<0x05a>), ((CPU::Handler)&CPU::instruction<0x05b>),
    ((CPU::Handler)&CPU::instruction<0x05c>), ((CPU::Handler)&CPU::instruction<0x05d>), ((CPU::Handler)&CPU::instruction<0x05e>), ((CPU::Handler)&CPU::instruction<0x05f>),
    ((CPU::Handler)&CPU::instruction<0x060>), ((CPU::Handler)&CPU::instruction<0x061>), ((CPU::Handler)&CPU::instruction<0x062>), ((CPU::Handler)&CPU::instruction<0x063>),
    ((CPU::Handler)&CPU::instruction<0x064>), ((CPU::Handler)&CPU::instruction<0x065>), ((CPU::Handler)&CPU::instruction<0x066>), ((CPU::Handler)&CPU::instruction<0x067>),
    ((CPU::Handler)&CPU::instruction<0x068>), ((CPU::Handler)&CPU::instruction<0x069>), ((CPU::Handler)&CPU::instruction<0x06a>), ((CPU::Handler)&CPU::instruction<0x06b>),
    ((CPU::Handler)&CPU::instruction<0x06c>), ((CPU::Handler)&CPU::instruction<0x06d>), ((CPU::Handler)&CPU::instruction<0x06e>), ((CPU::Handler)&CPU::instruction<0x06f>),
    ((CPU::Handler)&CPU::instruction<0x070>), ((CPU::Handler)&CPU::instruction<0x071>), ((CPU::Handler)&CPU::instruction<0x072>), ((CPU::Handler)&CPU::instruction<0x073>),
    ((CPU::Handler)&CPU::instruction<0x074>), ((CPU::Handler)&CPU::instruction<0x075>), ((CPU::Handler)&CPU::instruction<0x076>), ((CPU::Handler)&CPU::instruction<0x077>),
    ((CPU::Handler)&CPU::instruction<0x078>), ((CPU::Handler)&CPU::instruction<0x079>), ((CPU::Handler)&CPU::instruction<0x07a>), ((CPU::Handler)&CPU::instruction<0x07b>),
    ((CPU::Handler)&CPU::instruction<0x07c>), ((CPU::Handler)&CPU::instruction<0x07d>), ((CPU::Handler)&CPU::instruction<0x07e>), ((CPU::Handler)&CPU::instruction<0x07f>),
    ((CPU::Handler)&CPU::instruction<0x080>), ((CPU::Handler)&CPU::instruction<0x081>), ((CPU::Handler)&CPU::instruction<0x082>), ((CPU::Handler)&CPU::instruction<0x083>),
    ((CPU::Handler)&CPU::instruction<0x084>), ((CPU::Handler)&CPU::instruction<0x085>), ((CPU::Handler)&CPU::instruction<0x086>), ((CPU::Handler)&CPU::instruction<0x087>),
    ((CPU::Handler)&CPU::instruction<0x088>), ((CPU::Handler)&CPU::instruction<0x089>), ((CPU::Handler)&CPU::instruction<0x08a>), ((CPU::Handler)&CPU::instruction<0x08b>),
    ((CPU::Handler)&CPU::instruction<0x08c>), ((CPU::Handler)&CPU::instruction<0x08d>), ((CPU::Handler)&CPU::instruction<0x08e>), ((CPU::Handler)&CPU::instruction<0x08f>),
    ((CPU::Handler)&CPU::instruction<0x090>), ((CPU::Handler)&CPU::instruction<0x091>), ((CPU::Handler)&CPU::instruction<0x092>), ((CPU::Handler)&CPU::instruction<0x093>),
    ((CPU::Handler)&CPU::instruction<0x094>), ((CPU::Handler)&CPU::instruction<0x095>), ((CPU::Handler)&CPU::instruction<0x096>), ((CPU::Handler)&CPU::instruction<0x097>),
    ((CPU::Handler)&CPU::instruction<0x098>), ((CPU::Handler)&CPU::instruction<0x099>), ((CPU::Handler)&CPU::instruction<0x09a>), ((CPU::Handler)&CPU::instruction<0x09b>),
    ((CPU::Handler)&CPU::instruction<0x09c>), ((CPU::Handler)&CPU::instruction<0x09d>), ((CPU::Handler)&CPU::instruction<0x09e>), ((CPU::Handler)&CPU::instruction<0x09f>),
    ((CPU::Handler)&CPU::instruction<0x0a0>), ((CPU::Handler)&CPU::instruction<0x0a1>), ((CPU::Handler)&CPU::instruction<0x0a2>), ((CPU::Handler)&CPU::instruction<0x0a3>),
    ((CPU::Handler)&CPU::instruction<0x0a4>), ((CPU::Handler)&CPU::instruction<0x0a5>), ((CPU::Handler)&CPU::instruction<0x0a6>), ((CPU::Handler)&CPU::instruction<0x0a7>),
    ((CPU::Handler)&CPU::instruction<0x0a8>), ((CPU::Handler)&CPU::instruction<0x0a9>), ((CPU::Handler)&CPU::instruction<0x0aa>), ((CPU::Handler)&CPU::instruction<0x0ab>),
    ((CPU::Handler)&CPU::instruction<0x0ac>), ((CPU::Handler)&CPU::instruction<0x0ad>), ((CPU::Handler)&CPU::instruction<0x0ae>), ((CPU::Handler)&CPU::instruction<0x0af>),
    ((CPU::Handler)&CPU::instruction<0x0b0>), ((CPU::Handler)&CPU::instruction<0x0b1>), ((CPU::Handler)&CPU::instruction<0x0b2>), ((CPU::Handler)&CPU::instruction<0x0b3>),
    ((CPU::Handler)&CPU::instruction<0x0b4>), ((CPU::Handler)&CPU::instruction<0x0b5>), ((CPU::Handler)&CPU::instruction<0x0b6>), ((CPU::Handler)&CPU::instruction<0x0b7>),
    ((CPU::Handler)&CPU::instruction<0x0b8>), ((CPU::Handler)&CPU::instruction<0x0b9>), ((CPU::Handler)&CPU::instruction<0x0ba>), ((CPU::Handler)&CPU::instruction<0x0bb>),
    ((CPU::Handler)&CPU::instruction<0x0bc>), ((CPU::Handler)&CPU::instruction<0x0bd>), ((CPU::Handler)&CPU::instruction<0x0be>), ((CPU::Handler)&CPU::instruction<0x0bf>),
    ((CPU::Handler)&CPU::instruction<0x0c0>), ((CPU::Handler)&CPU::instruction<0x0c1>), ((CPU::Handler)&CPU::instruction<0x0c2>), ((CPU::Handler)&CPU::instruction<0x0c3>),
    ((CPU::Handler)&CPU::instruction<0x0c4>), ((CPU::Handler)&CPU::instruction<0x0c5>), ((CPU::Handler)&CPU::instruction<0x0c6>), ((CPU::Handler)&CPU::instruction<0x0c7>),
    ((CPU::Handler)&CPU::instruction<0x0c8>), ((CPU::Handler)&CPU::instruction<0x0c9>), ((CPU::Handler)&CPU::instruction<0x0ca>), ((CPU::Handler)&CPU::instruction<0x0cb>),
    ((CPU::Handler)&CPU::instruction<0x0cc>), ((CPU::Handler)&CPU::instruction<0x0cd>), ((CPU::Handler)&CPU::instruction<0x0ce>), ((CPU::Handler)&CPU::instruction<0x0cf>),
    ((CPU::Handler)&CPU::instruction<0x0d0>), ((CPU::Handler)&CPU::instruction<0x0d1>), ((CPU::Handler)&CPU::instruction<0x0d2>), ((CPU::Handler)&CPU::instruction<0x0d3>),
    ((CPU::Handler)&CPU::instruction<0x0d4>), ((CPU::Handler)&CPU::instruction<0x0d5>), ((CPU::Handler)&CPU::instruction<0x0d6>), ((CPU::Handler)&CPU::instruction<0x0d7>),
    ((CPU::Handler)&CPU::instruction<0x0d8>), ((CPU::Handler)&CPU::instruction<0x0d9>), ((CPU::Handler)&CPU::instruction<0x0da>), ((CPU::Handler)&CPU::instruction<0x0db>),
    ((CPU::Handler)&CPU::instruction<0x0dc>), ((CPU::Handler)&CPU::instruction<0x0dd>), ((CPU::Handler)&CPU::instruction<0x0de>), ((CPU::Handler)&CPU::instruction<0x0df>),
    ((CPU::Handler)&CPU::instruction<0x0e0>), ((CPU::Handler)&CPU::instruction<0x0e1>), ((CPU::Handler)&CPU::instruction<0x0e2>), ((CPU::Handler)&CPU::instruction<0x0e3>),
    ((CPU::Handler)&CPU::instruction<0x0e4>), ((CPU::Handler)&CPU::instruction<0x0e5>), ((CPU::Handler)&CPU::instruction<0x0e6>), ((CPU::Handler)&CPU::instruction<0x0e7>),
    ((CPU::Handler)&CPU::instruction<0x0e8>), ((CPU::Handler)&CPU::instruction<0x0e9>), ((CPU::Handler)&CPU::instruction<0x0ea>), ((CPU::Handler)&CPU::instruction<0x0eb>),
    ((CPU::Handler)&CPU::instruction<0x0ec>), ((CPU::Handler)&CPU::instruction<0x0ed>), ((CPU::Handler)&CPU::instruction<0x0ee>), ((CPU::Handler)&CPU::instruction<0x0ef>),
    ((CPU::Handler)&CPU::instruction<0x0f0>), ((CPU::Handler)&CPU::instruction<0x0f1>), ((CPU::Handler)&CPU::instruction<0x0f2>), ((CPU::Handler)&CPU::instruction<0x0f3>),
    ((CPU::Handler)&CPU::instruction<0x0f4>), ((CPU::Handler)&CPU::instruction<0x0f5>), ((CPU::Handler)&CPU::instruction<0x0f6>), ((CPU::Handler)&CPU::instruction<0x0f7>),
    ((CPU::Handler)&CPU::instruction<0x0f8>), ((CPU::Handler)&CPU::instruction<0x0f9>), ((CPU::Handler)&CPU::instruction<0x0fa>), ((CPU::Handler)&CPU::instruction<0x0fb>),
    ((CPU::Handler)&CPU::instruction<0x0fc>), ((CPU::Handler)&CPU::instruction<0x0fd>), ((CPU::Handler)&CPU::instruction<0x0fe>), ((CPU::Handler)&CPU::instruction<0x0ff>),
    ((CPU::Handler)&CPU::instruction<0x100>), ((CPU::Handler)&CPU::instruction<0x101>), ((CPU::Handler)&CPU::instruction<0x102>), ((CPU::Handler)&CPU::instruction<0x103>),
    ((CPU::Handler)&CPU::instruction<0x104>), ((CPU::Handler)&CPU::instruction<0x105>), ((CPU::Handler)&CPU::instruction<0x106>), ((CPU::Handler)&CPU::instruction<0x107>),
    ((CPU::Handler)&CPU::instruction<0x108>), ((CPU::Handler)&CPU::instruction<0x109>), ((CPU::Handler)&CPU::instruction<0x10a>), ((CPU::Handler)&CPU::instruction<0x10b>),
    ((CPU::Handler)&CPU::instruction<0x10c>), ((CPU::Handler)&CPU::instruction<0x10d>), ((CPU::Handler)&CPU::instruction<0x10e>), ((CPU::Handler)&CPU::instruction<0x10f>),
    ((CPU::Handler)&CPU::instruction<0x110>), ((CPU::Handler)&CPU::instruction<0x111>), ((CPU::Handler)&CPU::instruction<0x112>), ((CPU::Handler)&CPU::instruction<0x113>),
    ((CPU::Handler)&CPU::instruction<0x114>), ((CPU::Handler)&CPU::instruction<0x115>), ((CPU::Handler)&CPU::instruction<0x116>), ((CPU::Handler)&CPU::instruction<0x117>),
    ((CPU::Handler)&CPU::instruction<0x118>), ((CPU::Handler)&CPU::instruction<0x119>), ((CPU::Handler)&CPU::instruction<0x11a>), ((CPU::Handler)&CPU::instruction<0x11b>),
    ((CPU::Handler)&CPU::instruction<0x11c>), ((CPU::Handler)&CPU::instruction<0x11d>), ((CPU::Handler)&CPU::instruction<0x11e>), ((CPU::Handler)&CPU::instruction<0x11f>),
    ((CPU::Handler)&CPU::instruction<0x120>), ((CPU::Handler)&CPU::instruction<0x121>), ((CPU::Handler)&CPU::instruction<0x122>), ((CPU::Handler)&CPU::instruction<0x123>),
    ((CPU::Handler)&CPU::instruction<0x124>), ((CPU::Handler)&CPU::instruction<0x125>), ((CPU::Handler)&CPU::instruction<0x126>), ((CPU::Handler)&CPU::instruction<0x127>),
    ((CPU::Handler)&CPU::instruction<0x128>), ((CPU::Handler)&CPU::instruction<0x129>), ((CPU::Handler)&CPU::instruction<0x12a>), ((CPU::Handler)&CPU::instruction<0x12b>),
    ((CPU::Handler)&CPU::instruction<0x12c>), ((CPU::Handler)&CPU::instruction<0x12d>), ((CPU::Handler)&CPU::instruction<0x12e>), ((CPU::Handler)&CPU::instruction<0x12f>),
    ((CPU::Handler)&CPU::instruction<0x130>), ((CPU::Handler)&CPU::instruction<0x131>), ((CPU::Handler)&CPU::instruction<0x132>), ((CPU::Handler)&CPU::instruction<0x133>),
    ((CPU::Handler)&CPU::instruction<0x134>), ((CPU::Handler)&CPU::instruction<0x135>), ((CPU::Handler)&CPU::instruction<0x136>), ((CPU::Handler)&CPU::instruction<0x137>),
    ((CPU::Handler)&CPU::instruction<0x138>), ((CPU::Handler)&CPU::instruction<0x139>), ((CPU::Handler)&CPU::instruction<0x13a>), ((CPU::Handler)&CPU::instruction<0x13b>),
    ((CPU::Handler)&CPU::instruction<0x13c>), ((CPU::Handler)&CPU::instruction<0x13d>), ((CPU::Handler)&CPU::instruction<0x13e>), ((CPU::Handler)&CPU::instruction<0x13f>),
    ((CPU::Handler)&CPU::instruction<0x140>), ((CPU::Handler)&CPU::instruction<0x141>), ((CPU::Handler)&CPU::instruction<0x142>), ((CPU::Handler)&CPU::instruction<0x143>),
    ((CPU::Handler)&CPU::instruction<0x144>), ((CPU::Handler)&CPU::instruction<0x145>), ((CPU::Handler)&CPU::instruction<0x146>), ((CPU::Handler)&CPU::instruction<0x147>),
    ((CPU::Handler)&CPU::instruction<0x148>), ((CPU::Handler)&CPU::instruction<0x149>), ((CPU::Handler)&CPU::instruction<0x14a>), ((CPU::Handler)&CPU::instruction<0x14b>),
    ((CPU::Handler)&CPU::instruction<0x14c>), ((CPU::Handler)&CPU::instruction<0x14d>), ((CPU::Handler)&CPU::instruction<0x14e>), ((CPU::Handler)&CPU::instruction<0x14f>),
    ((CPU::Handler)&CPU::instruction<0x150>), ((CPU::Handler)&CPU::instruction<0x151>), ((CPU::Handler)&CPU::instruction<0x152>), ((CPU::Handler)&CPU::instruction<0x153>),
    ((CPU::Handler)&CPU::instruction<0x154>), ((CPU::Handler)&CPU::instruction<0x155>), ((CPU::Handler)&CPU::instruction<0x156>), ((CPU::Handler)&CPU::instruction<0x157>),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x159>), ((CPU::Handler)&CPU::instruction<0x15a>), ((CPU::Handler)&CPU::instruction<0x15b>),
    ((CPU::Handler)&CPU::instruction<0x15c>), ((CPU::Handler)&CPU::instruction<0x15d>), ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x15f>),
    ((CPU::Handler)&CPU::instruction<0x160>), ((CPU::Handler)&CPU::instruction<0x161>), ((CPU::Handler)&CPU::instruction<0x162>), ((CPU::Handler)&CPU::instruction<0x163>),
    ((CPU::Handler)&CPU::instruction<0x164>), ((CPU::Handler)&CPU::instruction<0x165>), ((CPU::Handler)&CPU::instruction<0x166>), ((CPU::Handler)&CPU::instruction<0x167>),
    ((CPU::Handler)&CPU::instruction<0x168>), ((CPU::Handler)&CPU::instruction<0x169>), ((CPU::Handler)&CPU::instruction<0x16a>), ((CPU::Handler)&CPU::instruction<0x16b>),
    ((CPU::Handler)&CPU::instruction<0x16c>), ((CPU::Handler)&CPU::instruction<0x16d>), ((CPU::Handler)&CPU::instruction<0x16e>), ((CPU::Handler)&CPU::instruction<0x16f>),
    ((CPU::Handler)&CPU::instruction<0x170>), ((CPU::Handler)&CPU::instruction<0x171>), ((CPU::Handler)&CPU::instruction<0x172>), ((CPU::Handler)&CPU::instruction<0x173>),
    ((CPU::Handler)&CPU::instruction<0x174>), ((CPU::Handler)&CPU::instruction<0x175>), ((CPU::Handler)&CPU::instruction<0x176>), ((CPU::Handler)&CPU::instruction<0x177>),
    ((CPU::Handler)&CPU::instruction<0x178>), ((CPU::Handler)&CPU::instruction<0x179>), ((CPU::Handler)&CPU::instruction<0x17a>), ((CPU::Handler)&CPU::instruction<0x17b>),
    ((CPU::Handler)&CPU::instruction<0x17c>), ((CPU::Handler)&CPU::instruction<0x17d>), ((CPU::Handler)&CPU::instruction<0x17e>), ((CPU::Handler)&CPU::instruction<0x17f>),
    ((CPU::Handler)&CPU::instruction<0x180>), ((CPU::Handler)&CPU::instruction<0x181>), ((CPU::Handler)&CPU::instruction<0x182>), ((CPU::Handler)&CPU::instruction<0x183>),
    ((CPU::Handler)&CPU::instruction<0x184>), ((CPU::Handler)&CPU::instruction<0x185>), ((CPU::Handler)&CPU::instruction<0x186>), ((CPU::Handler)&CPU::instruction<0x187>),
    ((CPU::Handler)&CPU::instruction<0x188>), ((CPU::Handler)&CPU::instruction<0x189>), ((CPU::Handler)&CPU::instruction<0x18a>), ((CPU::Handler)&CPU::instruction<0x18b>),
    ((CPU::Handler)&CPU::instruction<0x18c>), ((CPU::Handler)&CPU::instruction<0x18d>), ((CPU::Handler)&CPU::instruction<0x18e>), ((CPU::Handler)&CPU::instruction<0x18f>),
    ((CPU::Handler)&CPU::instruction<0x190>), ((CPU::Handler)&CPU::instruction<0x191>), ((CPU::Handler)&CPU::instruction<0x192>), ((CPU::Handler)&CPU::instruction<0x193>),
    ((CPU::Handler)&CPU::instruction<0x194>), ((CPU::Handler)&CPU::instruction<0x195>), ((CPU::Handler)&CPU::instruction<0x196>), ((CPU::Handler)&CPU::instruction<0x197>),
    ((CPU::Handler)&CPU::instruction<0x198>), ((CPU::Handler)&CPU::instruction<0x199>), ((CPU::Handler)&CPU::instruction<0x19a>), ((CPU::Handler)&CPU::instruction<0x19b>),
    ((CPU::Handler)&CPU::instruction<0x19c>), ((CPU::Handler)&CPU::instruction<0x19d>), ((CPU::Handler)&CPU::instruction<0x19e>), ((CPU::Handler)&CPU::instruction<0x19f>),
    ((CPU::Handler)&CPU::instruction<0x1a0>), ((CPU::Handler)&CPU::instruction<0x1a1>), ((CPU::Handler)&CPU::instruction<0x1a2>), ((CPU::Handler)&CPU::instruction<0x1a3>),
    ((CPU::Handler)&CPU::instruction<0x1a4>), ((CPU::Handler)&CPU::instruction<0x1a5>), ((CPU::Handler)&CPU::instruction<0x1a6>), ((CPU::Handler)&CPU::instruction<0x1a7>),
    ((CPU::Handler)&CPU::instruction<0x1a8>), ((CPU::Handler)&CPU::instruction<0x1a9>), ((CPU::Handler)&CPU::instruction<0x1aa>), ((CPU::Handler)&CPU::instruction<0x1ab>),
    ((CPU::Handler)&CPU::instruction<0x1ac>), ((CPU::Handler)&CPU::instruction<0x1ad>), ((CPU::Handler)&CPU::instruction<0x1ae>), ((CPU::Handler)&CPU::instruction<0x1af>),
    ((CPU::Handler)&CPU::instruction<0x1b0>), ((CPU::Handler)&CPU::instruction<0x1b1>), ((CPU::Handler)&CPU::instruction<0x1b2>), ((CPU::Handler)&CPU::instruction<0x1b3>),
    ((CPU::Handler)&CPU::instruction<0x1b4>), ((CPU::Handler)&CPU::instruction<0x1b5>), ((CPU::Handler)&CPU::instruction<0x1b6>), ((CPU::Handler)&CPU::instruction<0x1b7>),
    ((CPU::Handler)&CPU::instruction<0x1b8>), ((CPU::Handler)&CPU::instruction<0x1b9>), ((CPU::Handler)&CPU::instruction<0x1ba>), ((CPU::Handler)&CPU::instruction<0x1bb>),
    ((CPU::Handler)&CPU::instruction<0x1bc>), ((CPU::Handler)&CPU::instruction<0x1bd>), ((CPU::Handler)&CPU::instruction<0x1be>),
};
