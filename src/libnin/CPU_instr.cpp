#include <libnin/CPU.h>
#include <libnin/Memory.h>

using namespace libnin;

template<> CPU::Handler CPU::instruction<0x000>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _pc = ((_pc & 0x00ff) | (((std::uint16_t)read(_addr + 1)) << 8)); return next; }
template<> CPU::Handler CPU::instruction<0x001>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x000>); _pc = ((_pc & 0xff00) | read(_addr)); return next; }
template<> CPU::Handler CPU::instruction<0x002>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x001>); _memory.ram[0x100 | _s] = _p | PFLAG_1 | PFLAG_B; _s--; _p |= PFLAG_I; return next; }
template<> CPU::Handler CPU::instruction<0x003>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x002>); _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x004>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x003>); _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x005>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x004>); _addr = 0xfffe; read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x006>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x001>); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x007>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x006>); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x008>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x007>); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x009>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x008>); _addr = 0xfffc; read(_pc); _p |= PFLAG_I; return next; }
template<> CPU::Handler CPU::instruction<0x00a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x001>); _memory.ram[0x100 | _s] = _p | PFLAG_1; _s--; _p |= PFLAG_I; return next; }
template<> CPU::Handler CPU::instruction<0x00b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x00a>); _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x00c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x00b>); _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x00d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x00c>); _addr = 0xfffe; read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x00e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x001>); _memory.ram[0x100 | _s] = _p | PFLAG_1; _s--; return next; }
template<> CPU::Handler CPU::instruction<0x00f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x00e>); _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x010>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x00f>); _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x011>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x010>); _addr = 0xfffa; read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x012>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _memory.ram[0x100 | _s] = _p | PFLAG_1 | PFLAG_B; _s--; return next; }
template<> CPU::Handler CPU::instruction<0x013>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x012>); read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x014>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p = _memory.ram[0x100 | _s] & ~(PFLAG_1 | PFLAG_B); return next; }
template<> CPU::Handler CPU::instruction<0x015>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x014>); _s++; return next; }
template<> CPU::Handler CPU::instruction<0x016>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x015>); read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x017>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _memory.ram[0x100 | _s] = _a; _s--; return next; }
template<> CPU::Handler CPU::instruction<0x018>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x017>); read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x019>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _a = _memory.ram[0x100 | _s]; flagNZ(_a); return next; }
template<> CPU::Handler CPU::instruction<0x01a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x019>); _s++; return next; }
template<> CPU::Handler CPU::instruction<0x01b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x01a>); read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x01c>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _pc = (_addr | ((std::uint16_t)read(_pc) << 8)); return next; }
template<> CPU::Handler CPU::instruction<0x01d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x01c>); _memory.ram[0x100 | _s] = (_pc & 0xff); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x01e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x01d>); _memory.ram[0x100 | _s] = (_pc >> 8); _s--; return next; }
template<> CPU::Handler CPU::instruction<0x01f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x01e>); /* nop */; return next; }
template<> CPU::Handler CPU::instruction<0x020>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x01f>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x021>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _pc = (_pc & 0x00ff) | ((std::uint16_t)_memory.ram[0x100 | _s] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x022>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x021>); _pc = (_pc & 0xff00) | _memory.ram[0x100 | _s]; _s++; return next; }
template<> CPU::Handler CPU::instruction<0x023>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x022>); _p = _memory.ram[0x100 | _s] & ~(PFLAG_1 | PFLAG_B); _s++; return next; }
template<> CPU::Handler CPU::instruction<0x024>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x023>); _s++; return next; }
template<> CPU::Handler CPU::instruction<0x025>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x024>); read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x026>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _pc++; return next; }
template<> CPU::Handler CPU::instruction<0x027>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x026>); _pc = (_pc & 0x00ff) | ((std::uint16_t)_memory.ram[0x100 | _s] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x028>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x027>); _pc = (_pc & 0xff00) | _memory.ram[0x100 | _s]; _s++; return next; }
template<> CPU::Handler CPU::instruction<0x029>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x028>); _s++; return next; }
template<> CPU::Handler CPU::instruction<0x02a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x029>); read(_pc); return next; }
template<> CPU::Handler CPU::instruction<0x02b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x01c>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x02c>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _pc = ((_pc & 0x00ff) | ((std::uint16_t)read((_addr & 0xff00) | ((_addr + 1) & 0xff))) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x02d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x02c>); _pc = ((_pc & 0xff00) | read(_addr)); return next; }
template<> CPU::Handler CPU::instruction<0x02e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x02d>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x02f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x02e>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x030>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p &= ~PFLAG_C; return next; }
template<> CPU::Handler CPU::instruction<0x031>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p &= ~PFLAG_I; return next; }
template<> CPU::Handler CPU::instruction<0x032>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p &= ~PFLAG_V; return next; }
template<> CPU::Handler CPU::instruction<0x033>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p &= ~PFLAG_D; return next; }
template<> CPU::Handler CPU::instruction<0x034>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p |= PFLAG_C; return next; }
template<> CPU::Handler CPU::instruction<0x035>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p |= PFLAG_I; return next; }
template<> CPU::Handler CPU::instruction<0x036>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _p |= PFLAG_D; return next; }
template<> CPU::Handler CPU::instruction<0x037>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); /* nop */; return next; }
template<> CPU::Handler CPU::instruction<0x038>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x037>); if (!_addrCarry) return (Handler)&CPU::dispatch; _pc = _addr; return next; }
template<> CPU::Handler CPU::instruction<0x039>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (_p & PFLAG_N) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x03a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (!(_p & PFLAG_N)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x03b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (_p & PFLAG_V) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x03c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (!(_p & PFLAG_V)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x03d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (_p & PFLAG_C) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x03e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (!(_p & PFLAG_C)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x03f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (_p & PFLAG_Z) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x040>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x038>); _addr = read(_pc++); if (!(_p & PFLAG_Z)) { return (Handler)&CPU::dispatch; } _addrCarry = (((_pc + (std::int8_t)_addr) ^ _pc) & 0xff00) ? 1 : 0; _addr = _pc + (std::int8_t)_addr; _pc = ((_pc & 0xff00) | (_addr & 0xff)); return next; }
template<> CPU::Handler CPU::instruction<0x041>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _x = _a; flagNZ(_a); return next; }
template<> CPU::Handler CPU::instruction<0x042>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _y = _a; flagNZ(_a); return next; }
template<> CPU::Handler CPU::instruction<0x043>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _x = _s; flagNZ(_s); return next; }
template<> CPU::Handler CPU::instruction<0x044>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _a = _x; flagNZ(_x); return next; }
template<> CPU::Handler CPU::instruction<0x045>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _s = _x; return next; }
template<> CPU::Handler CPU::instruction<0x046>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); _a = _y; flagNZ(_y); return next; }
template<> CPU::Handler CPU::instruction<0x047>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _memory.ram[_addr] = _regs[_selSrc]; return next; }
template<> CPU::Handler CPU::instruction<0x048>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x047>); _selSrc = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x049>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x047>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x04a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x049>); _selSrc = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x04b>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); next = write(_addr, _regs[_selSrc], next); return next; }
template<> CPU::Handler CPU::instruction<0x04c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04b>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x04d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04c>); _selSrc = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x04e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04b>); read(_addr); _addr += (((std::uint16_t)_addrCarry) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x04f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04e>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x050>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04f>); _selSrc = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x051>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04e>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x052>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x051>); _selSrc = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x053>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04b>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x054>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x053>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x055>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x054>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x056>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x055>); _selSrc = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x057>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04e>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x058>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x057>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x059>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x058>); _selSrc = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x05a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x047>); _selSrc = REG_X; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x05b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x047>); _addr = (_addr + _y) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x05c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x05b>); _selSrc = REG_X; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x05d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04c>); _selSrc = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x05e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x047>); _selSrc = REG_Y; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x05f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x049>); _selSrc = REG_Y; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x060>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x04c>); _selSrc = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x061>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _a = read(_pc++); flagNZ(_a); return next; }
template<> CPU::Handler CPU::instruction<0x062>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _regs[_selDst] = _memory.ram[_addr]; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x063>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x062>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x064>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x062>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x065>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x064>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x066>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _regs[_selDst] = read(_addr); flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x067>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x066>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x068>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x067>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x069>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x066>); _regs[_selDst] = read(_addr); flagNZ(_regs[_selDst]); _addr += ((std::uint16_t)_addrCarry << 8); if (!_addrCarry) return ((Handler)&CPU::dispatch); return next; }
template<> CPU::Handler CPU::instruction<0x06a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x069>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x06b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x06a>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x06c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x069>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x06d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x06c>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x06e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x066>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x06f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x06e>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x070>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x06f>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x071>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x070>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x072>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x069>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x073>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x072>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x074>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x073>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x075>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _x = read(_pc++); flagNZ(_x); return next; }
template<> CPU::Handler CPU::instruction<0x076>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x062>); _selDst = REG_X; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x077>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x062>); _addr = (_addr + _y) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x078>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x077>); _selDst = REG_X; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x079>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x067>); _selDst = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x07a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x06c>); _selDst = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x07b>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _y = read(_pc++); flagNZ(_y); return next; }
template<> CPU::Handler CPU::instruction<0x07c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x062>); _selDst = REG_Y; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x07d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x064>); _selDst = REG_Y; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x07e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x067>); _selDst = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x07f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x06a>); _selDst = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x080>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_A; std::uint8_t tmp = read(_pc++); _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x081>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x082>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x081>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x083>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x081>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x084>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x083>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x085>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x086>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x085>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x087>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x086>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x088>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x085>); std::uint8_t tmp = read(_addr); if (!_addrCarry) { _regs[_selDst] |= tmp; flagNZ(_regs[_selDst]); return (Handler)&CPU::dispatch; } _addr += 0x100; return next; }
template<> CPU::Handler CPU::instruction<0x089>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x088>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x08a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x089>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x08b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x088>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x08c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x08b>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x08d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x085>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x08e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x08d>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x08f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x08e>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x090>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x08f>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x091>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x088>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x092>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x091>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x093>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x092>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x094>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_A; std::uint8_t tmp = read(_pc++); _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x095>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x096>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x095>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x097>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x095>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x098>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x097>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x099>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x09a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x099>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x09b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x09a>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x09c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x099>); std::uint8_t tmp = read(_addr); if (!_addrCarry) { _regs[_selDst] &= tmp; flagNZ(_regs[_selDst]); return (Handler)&CPU::dispatch; } _addr += 0x100; return next; }
template<> CPU::Handler CPU::instruction<0x09d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x09c>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x09e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x09d>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x09f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x09c>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0a0>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x09f>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0a1>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x099>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0a2>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a1>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0a3>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a2>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0a4>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a3>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0a5>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x09c>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x0a6>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a5>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0a7>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a6>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0a8>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_A; std::uint8_t tmp = read(_pc++); _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x0a9>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x0aa>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a9>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0ab>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0a9>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0ac>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ab>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0ad>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return next; }
template<> CPU::Handler CPU::instruction<0x0ae>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ad>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0af>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ae>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0b0>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ad>); std::uint8_t tmp = read(_addr); if (!_addrCarry) { _regs[_selDst] ^= tmp; flagNZ(_regs[_selDst]); return (Handler)&CPU::dispatch; } _addr += 0x100; return next; }
template<> CPU::Handler CPU::instruction<0x0b1>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b0>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0b2>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b1>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0b3>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b0>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0b4>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b3>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0b5>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ad>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0b6>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b5>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0b7>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b6>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0b8>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b7>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0b9>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b0>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x0ba>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0b9>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0bb>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ba>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0bc>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_A; std::uint8_t tmp = read(_pc++); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0bd>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0be>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0bd>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0bf>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0bd>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0c0>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0bf>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0c1>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0c2>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c1>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0c3>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c2>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0c4>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c1>); std::uint8_t tmp = read(_addr); if (!_addrCarry) { _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return (Handler)&CPU::dispatch; } _addr += 0x100; return next; }
template<> CPU::Handler CPU::instruction<0x0c5>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c4>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0c6>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c5>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0c7>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c4>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0c8>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c7>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0c9>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c1>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0ca>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c9>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0cb>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ca>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0cc>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0cb>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0cd>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c4>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x0ce>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0cd>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0cf>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ce>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0d0>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_A; std::uint8_t tmp = read(_pc++); _a = adc(_a, tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0d1>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _a = adc(_a, tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0d2>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d1>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0d3>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d1>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0d4>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d3>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0d5>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _a = adc(_a, tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0d6>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d5>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0d7>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d6>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0d8>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d5>); std::uint8_t tmp = read(_addr); if (!_addrCarry) { _a = adc(_a, tmp); return (Handler)&CPU::dispatch; } _addr += 0x100; return next; }
template<> CPU::Handler CPU::instruction<0x0d9>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d8>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0da>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d9>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0db>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d8>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0dc>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0db>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0dd>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d5>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0de>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0dd>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0df>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0de>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0e0>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0df>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0e1>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0d8>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x0e2>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e1>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0e3>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e2>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0e4>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_A; std::uint8_t tmp = read(_pc++); _a = adc(_a, ~tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0e5>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _a = adc(_a, ~tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0e6>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e5>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0e7>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e5>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0e8>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e7>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0e9>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _a = adc(_a, ~tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0ea>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e9>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0eb>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ea>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0ec>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e9>); std::uint8_t tmp = read(_addr); if (!_addrCarry) { _a = adc(_a, ~tmp); return (Handler)&CPU::dispatch; } _addr += 0x100; return next; }
template<> CPU::Handler CPU::instruction<0x0ed>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ec>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0ee>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ed>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0ef>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ec>); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (((_addr + _y) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0f0>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ef>); _selDst = REG_A; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0f1>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0e9>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0f2>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0f1>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0f3>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0f2>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x0f4>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0f3>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0f5>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ec>); _addr = (_addr >> 8) | ((std::uint16_t)_memory.ram[(_addr + 1) & 0xff] << 8); _addrCarry = (((_addr & 0xff) + _y) > 0xff) ? 1 : 0; _addr = (_addr & 0xff00) | ((_addr + _y) & 0xff); return next; }
template<> CPU::Handler CPU::instruction<0x0f6>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0f5>); _addr = _addr | ((std::uint16_t)_memory.ram[_addr] << 8); return next; }
template<> CPU::Handler CPU::instruction<0x0f7>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0f6>); _selDst = REG_A; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0f8>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_X; std::uint8_t tmp = read(_pc++); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0f9>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0bd>); _selDst = REG_X; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0fa>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c2>); _selDst = REG_X; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0fb>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _selDst = REG_Y; std::uint8_t tmp = read(_pc++); _p &= ~PFLAG_C; _p |= (_regs[_selDst] >= tmp) ? PFLAG_C : 0; flagNZ(_regs[_selDst] - tmp); return next; }
template<> CPU::Handler CPU::instruction<0x0fc>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0bd>); _selDst = REG_Y; _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x0fd>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0c2>); _selDst = REG_Y; _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x0fe>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); std::uint8_t tmp = _a; _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp <<= 1; flagNZ(tmp); _a = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x0ff>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); _memory.ram[_addr] = _rmw; return next; }
template<> CPU::Handler CPU::instruction<0x100>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ff>); std::uint8_t tmp = _rmw; _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp <<= 1; flagNZ(tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x101>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x100>); _rmw = _memory.ram[_addr]; return next; }
template<> CPU::Handler CPU::instruction<0x102>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x101>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x103>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x101>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x104>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x103>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x105>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); next = write(_addr, _rmw, next); return next; }
template<> CPU::Handler CPU::instruction<0x106>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x105>); next = write(_addr, _rmw, next); std::uint8_t tmp = _rmw; _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp <<= 1; flagNZ(tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x107>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x106>); _rmw = read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x108>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x107>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x109>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x108>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x10a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x107>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x10b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x10a>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x10c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x10b>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x10d>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); std::uint8_t tmp = _a; std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp = ((tmp << 1) | c); flagNZ(tmp); _a = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x10e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ff>); std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp = ((tmp << 1) | c); flagNZ(tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x10f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x10e>); _rmw = _memory.ram[_addr]; return next; }
template<> CPU::Handler CPU::instruction<0x110>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x10f>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x111>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x10f>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x112>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x111>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x113>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x105>); next = write(_addr, _rmw, next); std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x01 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x80) ? PFLAG_C : 0); tmp = ((tmp << 1) | c); flagNZ(tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x114>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x113>); _rmw = read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x115>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x114>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x116>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x115>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x117>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x114>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x118>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x117>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x119>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x118>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x11a>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); std::uint8_t tmp = _a; _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp >>= 1; _p |= (tmp ? 0 : PFLAG_Z); _a = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x11b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ff>); std::uint8_t tmp = _rmw; _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp >>= 1; _p |= (tmp ? 0 : PFLAG_Z); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x11c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x11b>); _rmw = _memory.ram[_addr]; return next; }
template<> CPU::Handler CPU::instruction<0x11d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x11c>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x11e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x11c>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x11f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x11e>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x120>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x105>); next = write(_addr, _rmw, next); std::uint8_t tmp = _rmw; _p &= ~(PFLAG_N | PFLAG_Z | PFLAG_C); _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp >>= 1; _p |= (tmp ? 0 : PFLAG_Z); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x121>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x120>); _rmw = read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x122>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x121>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x123>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x122>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x124>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x121>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x125>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x124>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x126>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x125>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x127>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc); std::uint8_t tmp = _a; std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp = ((tmp >> 1) | c); flagNZ(tmp); _a = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x128>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ff>); std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp = ((tmp >> 1) | c); flagNZ(tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x129>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x128>); _rmw = _memory.ram[_addr]; return next; }
template<> CPU::Handler CPU::instruction<0x12a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x129>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x12b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x129>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x12c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x12b>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x12d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x105>); next = write(_addr, _rmw, next); std::uint8_t tmp = _rmw; std::uint8_t c = (_p & PFLAG_C ? 0x80 : 0x00); _p &= ~PFLAG_C; _p |= ((tmp & 0x01) ? PFLAG_C : 0); tmp = ((tmp >> 1) | c); flagNZ(tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x12e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x12d>); _rmw = read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x12f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x12e>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x130>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x12f>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x131>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x12e>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x132>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x131>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x133>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x132>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x134>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ff>); std::uint8_t tmp = _rmw; flagNZ(++tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x135>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x134>); _rmw = _memory.ram[_addr]; return next; }
template<> CPU::Handler CPU::instruction<0x136>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x135>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x137>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x135>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x138>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x137>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x139>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x105>); next = write(_addr, _rmw, next); std::uint8_t tmp = _rmw; flagNZ(++tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x13a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x139>); _rmw = read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x13b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x13a>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x13c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x13b>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x13d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x13a>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x13e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x13d>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x13f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x13e>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x140>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x0ff>); std::uint8_t tmp = _rmw; flagNZ(--tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x141>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x140>); _rmw = _memory.ram[_addr]; return next; }
template<> CPU::Handler CPU::instruction<0x142>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x141>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x143>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x141>); _addr = (_addr + _x) & 0xff; return next; }
template<> CPU::Handler CPU::instruction<0x144>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x143>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x145>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x105>); next = write(_addr, _rmw, next); std::uint8_t tmp = _rmw; flagNZ(--tmp); _rmw = tmp; return next; }
template<> CPU::Handler CPU::instruction<0x146>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x145>); _rmw = read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x147>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x146>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x148>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x147>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x149>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x146>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x14a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x149>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x14b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x14a>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x14c>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = _memory.ram[_addr]; _p &= ~(PFLAG_Z | PFLAG_N | PFLAG_V); _p |= (tmp & 0xc0); _p |= ((_a & tmp) ? 0 : PFLAG_Z); return next; }
template<> CPU::Handler CPU::instruction<0x14d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x14c>); _addr = read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x14e>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); std::uint8_t tmp = read(_addr); _p &= ~(PFLAG_Z | PFLAG_N | PFLAG_V); _p |= (tmp & 0xc0); _p |= ((_a & tmp) ? 0 : PFLAG_Z); return next; }
template<> CPU::Handler CPU::instruction<0x14f>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x14e>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x150>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x14f>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x151>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); flagNZ(++_x); return next; }
template<> CPU::Handler CPU::instruction<0x152>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); flagNZ(++_y); return next; }
template<> CPU::Handler CPU::instruction<0x153>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); flagNZ(--_x); return next; }
template<> CPU::Handler CPU::instruction<0x154>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); flagNZ(--_y); return next; }
template<> CPU::Handler CPU::instruction<0x155>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_pc++); /* nop */; return next; }
template<> CPU::Handler CPU::instruction<0x156>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x037>); read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x157>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x037>); /* nop */; return next; }
template<> CPU::Handler CPU::instruction<0x158>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x157>); read(_pc++); return next; }
template<> CPU::Handler CPU::instruction<0x159>(void) { Handler next = ((CPU::Handler)&CPU::dispatch); read(_addr); return next; }
template<> CPU::Handler CPU::instruction<0x15a>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x159>); _addr = ((_addr & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x15b>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x15a>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }
template<> CPU::Handler CPU::instruction<0x15c>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x159>); read(_addr); _addr += ((std::uint16_t)_addrCarry << 8); return next; }
template<> CPU::Handler CPU::instruction<0x15d>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x15c>); _addrCarry = (((_addr & 0xff) + _x) > 0xff) ? 1 : 0; _addr = (((_addr + _x) & 0x00ff) | ((std::uint16_t)read(_pc++)) << 8); return next; }
template<> CPU::Handler CPU::instruction<0x15e>(void) { Handler next = ((CPU::Handler)&CPU::instruction<0x15d>); _addr = ((_addr & 0xff00) | read(_pc++)); return next; }

const CPU::Handler CPU::kOps[] = {
    ((CPU::Handler)&CPU::instruction<0x005>), ((CPU::Handler)&CPU::instruction<0x090>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x156>), ((CPU::Handler)&CPU::instruction<0x082>), ((CPU::Handler)&CPU::instruction<0x102>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x013>), ((CPU::Handler)&CPU::instruction<0x080>), ((CPU::Handler)&CPU::instruction<0x0fe>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15b>), ((CPU::Handler)&CPU::instruction<0x087>), ((CPU::Handler)&CPU::instruction<0x109>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x039>), ((CPU::Handler)&CPU::instruction<0x093>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x084>), ((CPU::Handler)&CPU::instruction<0x104>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x030>), ((CPU::Handler)&CPU::instruction<0x08c>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x08a>), ((CPU::Handler)&CPU::instruction<0x10c>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x020>), ((CPU::Handler)&CPU::instruction<0x0a4>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x14d>), ((CPU::Handler)&CPU::instruction<0x096>), ((CPU::Handler)&CPU::instruction<0x110>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x016>), ((CPU::Handler)&CPU::instruction<0x094>), ((CPU::Handler)&CPU::instruction<0x10d>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x150>), ((CPU::Handler)&CPU::instruction<0x09b>), ((CPU::Handler)&CPU::instruction<0x116>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x03a>), ((CPU::Handler)&CPU::instruction<0x0a7>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x098>), ((CPU::Handler)&CPU::instruction<0x112>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x034>), ((CPU::Handler)&CPU::instruction<0x0a0>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x09e>), ((CPU::Handler)&CPU::instruction<0x119>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x025>), ((CPU::Handler)&CPU::instruction<0x0b8>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x156>), ((CPU::Handler)&CPU::instruction<0x0aa>), ((CPU::Handler)&CPU::instruction<0x11d>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x018>), ((CPU::Handler)&CPU::instruction<0x0a8>), ((CPU::Handler)&CPU::instruction<0x11a>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x02b>), ((CPU::Handler)&CPU::instruction<0x0af>), ((CPU::Handler)&CPU::instruction<0x123>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x03b>), ((CPU::Handler)&CPU::instruction<0x0bb>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x0ac>), ((CPU::Handler)&CPU::instruction<0x11f>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x031>), ((CPU::Handler)&CPU::instruction<0x0b4>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x0b2>), ((CPU::Handler)&CPU::instruction<0x126>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x02a>), ((CPU::Handler)&CPU::instruction<0x0e0>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x156>), ((CPU::Handler)&CPU::instruction<0x0d2>), ((CPU::Handler)&CPU::instruction<0x12a>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x01b>), ((CPU::Handler)&CPU::instruction<0x0d0>), ((CPU::Handler)&CPU::instruction<0x127>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x02f>), ((CPU::Handler)&CPU::instruction<0x0d7>), ((CPU::Handler)&CPU::instruction<0x130>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x03c>), ((CPU::Handler)&CPU::instruction<0x0e3>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x0d4>), ((CPU::Handler)&CPU::instruction<0x12c>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x035>), ((CPU::Handler)&CPU::instruction<0x0dc>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x0da>), ((CPU::Handler)&CPU::instruction<0x133>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x155>), ((CPU::Handler)&CPU::instruction<0x056>), ((CPU::Handler)&CPU::instruction<0x155>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x05e>), ((CPU::Handler)&CPU::instruction<0x048>), ((CPU::Handler)&CPU::instruction<0x05a>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x154>), ((CPU::Handler)&CPU::instruction<0x155>), ((CPU::Handler)&CPU::instruction<0x044>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x060>), ((CPU::Handler)&CPU::instruction<0x04d>), ((CPU::Handler)&CPU::instruction<0x05d>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x03d>), ((CPU::Handler)&CPU::instruction<0x059>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x05f>), ((CPU::Handler)&CPU::instruction<0x04a>), ((CPU::Handler)&CPU::instruction<0x05c>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x046>), ((CPU::Handler)&CPU::instruction<0x052>), ((CPU::Handler)&CPU::instruction<0x045>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::instruction<0x050>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x07b>), ((CPU::Handler)&CPU::instruction<0x071>), ((CPU::Handler)&CPU::instruction<0x075>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x07c>), ((CPU::Handler)&CPU::instruction<0x063>), ((CPU::Handler)&CPU::instruction<0x076>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x042>), ((CPU::Handler)&CPU::instruction<0x061>), ((CPU::Handler)&CPU::instruction<0x041>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x07e>), ((CPU::Handler)&CPU::instruction<0x068>), ((CPU::Handler)&CPU::instruction<0x079>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x03e>), ((CPU::Handler)&CPU::instruction<0x074>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x07d>), ((CPU::Handler)&CPU::instruction<0x065>), ((CPU::Handler)&CPU::instruction<0x078>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x032>), ((CPU::Handler)&CPU::instruction<0x06d>), ((CPU::Handler)&CPU::instruction<0x043>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x07f>), ((CPU::Handler)&CPU::instruction<0x06b>), ((CPU::Handler)&CPU::instruction<0x07a>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x0fb>), ((CPU::Handler)&CPU::instruction<0x0cc>), ((CPU::Handler)&CPU::instruction<0x155>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x0fc>), ((CPU::Handler)&CPU::instruction<0x0be>), ((CPU::Handler)&CPU::instruction<0x142>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x152>), ((CPU::Handler)&CPU::instruction<0x0bc>), ((CPU::Handler)&CPU::instruction<0x153>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x0fd>), ((CPU::Handler)&CPU::instruction<0x0c3>), ((CPU::Handler)&CPU::instruction<0x148>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x03f>), ((CPU::Handler)&CPU::instruction<0x0cf>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x0c0>), ((CPU::Handler)&CPU::instruction<0x144>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x033>), ((CPU::Handler)&CPU::instruction<0x0c8>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x0c6>), ((CPU::Handler)&CPU::instruction<0x14b>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x0f8>), ((CPU::Handler)&CPU::instruction<0x0f4>), ((CPU::Handler)&CPU::instruction<0x155>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x0f9>), ((CPU::Handler)&CPU::instruction<0x0e6>), ((CPU::Handler)&CPU::instruction<0x136>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x151>), ((CPU::Handler)&CPU::instruction<0x0e4>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::instruction<0x0e4>),
    ((CPU::Handler)&CPU::instruction<0x0fa>), ((CPU::Handler)&CPU::instruction<0x0eb>), ((CPU::Handler)&CPU::instruction<0x13c>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x040>), ((CPU::Handler)&CPU::instruction<0x0f7>), ((CPU::Handler)&CPU::kil), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x158>), ((CPU::Handler)&CPU::instruction<0x0e8>), ((CPU::Handler)&CPU::instruction<0x138>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x036>), ((CPU::Handler)&CPU::instruction<0x0f0>), ((CPU::Handler)&CPU::instruction<0x037>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x15e>), ((CPU::Handler)&CPU::instruction<0x0ee>), ((CPU::Handler)&CPU::instruction<0x13f>), ((CPU::Handler)&CPU::kil),
    ((CPU::Handler)&CPU::instruction<0x009>), ((CPU::Handler)&CPU::instruction<0x00d>), ((CPU::Handler)&CPU::instruction<0x011>),
};

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
    ((CPU::Handler)&CPU::instruction<0x15c>), ((CPU::Handler)&CPU::instruction<0x15d>), ((CPU::Handler)&CPU::instruction<0x15e>),
};
