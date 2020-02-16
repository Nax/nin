#include <libnin/CPU.h>
#include <libnin/Memory.h>

using namespace libnin;

template<> CPU::Handler CPU::instruction<0x103>(void) { _pc = ((_pc & 0x00ff) | ((std::uint16_t)read(_addr + 1)) << 8); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x104>(void) { _pc = ((_pc & 0xff00) | read(_addr)); return ((Handler)&CPU::instruction<0x103>); }
template<> CPU::Handler CPU::instruction<0x105>(void) { _memory.ram[0x100 | _s--] = _p; return ((Handler)&CPU::instruction<0x104>); }
template<> CPU::Handler CPU::instruction<0x106>(void) { _memory.ram[0x100 | _s--] = (_pc & 0xff); return ((Handler)&CPU::instruction<0x105>); }
template<> CPU::Handler CPU::instruction<0x107>(void) { _memory.ram[0x100 | _s--] = (_pc >> 8); return ((Handler)&CPU::instruction<0x106>); }
template<> CPU::Handler CPU::instruction<0x108>(void) { _s--; return ((Handler)&CPU::instruction<0x104>); }
template<> CPU::Handler CPU::instruction<0x109>(void) { _s--; return ((Handler)&CPU::instruction<0x108>); }
template<> CPU::Handler CPU::instruction<0x10a>(void) { _s--; return ((Handler)&CPU::instruction<0x109>); }
template<> CPU::Handler CPU::instruction<0x000>(void) { _addr = 0xfffe; read(_pc++); return ((Handler)&CPU::instruction<0x107>); }
template<> CPU::Handler CPU::instruction<0x001>(void) { return debug_not_impl(0x001); }
template<> CPU::Handler CPU::instruction<0x002>(void) { return debug_not_impl(0x002); }
template<> CPU::Handler CPU::instruction<0x003>(void) { return debug_not_impl(0x003); }
template<> CPU::Handler CPU::instruction<0x004>(void) { return debug_not_impl(0x004); }
template<> CPU::Handler CPU::instruction<0x005>(void) { return debug_not_impl(0x005); }
template<> CPU::Handler CPU::instruction<0x006>(void) { return debug_not_impl(0x006); }
template<> CPU::Handler CPU::instruction<0x007>(void) { return debug_not_impl(0x007); }
template<> CPU::Handler CPU::instruction<0x008>(void) { return debug_not_impl(0x008); }
template<> CPU::Handler CPU::instruction<0x009>(void) { return debug_not_impl(0x009); }
template<> CPU::Handler CPU::instruction<0x00a>(void) { return debug_not_impl(0x00a); }
template<> CPU::Handler CPU::instruction<0x00b>(void) { return debug_not_impl(0x00b); }
template<> CPU::Handler CPU::instruction<0x00c>(void) { return debug_not_impl(0x00c); }
template<> CPU::Handler CPU::instruction<0x00d>(void) { return debug_not_impl(0x00d); }
template<> CPU::Handler CPU::instruction<0x00e>(void) { return debug_not_impl(0x00e); }
template<> CPU::Handler CPU::instruction<0x00f>(void) { return debug_not_impl(0x00f); }
template<> CPU::Handler CPU::instruction<0x010>(void) { return debug_not_impl(0x010); }
template<> CPU::Handler CPU::instruction<0x011>(void) { return debug_not_impl(0x011); }
template<> CPU::Handler CPU::instruction<0x012>(void) { return debug_not_impl(0x012); }
template<> CPU::Handler CPU::instruction<0x013>(void) { return debug_not_impl(0x013); }
template<> CPU::Handler CPU::instruction<0x014>(void) { return debug_not_impl(0x014); }
template<> CPU::Handler CPU::instruction<0x015>(void) { return debug_not_impl(0x015); }
template<> CPU::Handler CPU::instruction<0x016>(void) { return debug_not_impl(0x016); }
template<> CPU::Handler CPU::instruction<0x017>(void) { return debug_not_impl(0x017); }
template<> CPU::Handler CPU::instruction<0x018>(void) { _p &= ~PFLAG_C; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x019>(void) { return debug_not_impl(0x019); }
template<> CPU::Handler CPU::instruction<0x01a>(void) { return debug_not_impl(0x01a); }
template<> CPU::Handler CPU::instruction<0x01b>(void) { return debug_not_impl(0x01b); }
template<> CPU::Handler CPU::instruction<0x01c>(void) { return debug_not_impl(0x01c); }
template<> CPU::Handler CPU::instruction<0x01d>(void) { return debug_not_impl(0x01d); }
template<> CPU::Handler CPU::instruction<0x01e>(void) { return debug_not_impl(0x01e); }
template<> CPU::Handler CPU::instruction<0x01f>(void) { return debug_not_impl(0x01f); }
template<> CPU::Handler CPU::instruction<0x020>(void) { return debug_not_impl(0x020); }
template<> CPU::Handler CPU::instruction<0x021>(void) { return debug_not_impl(0x021); }
template<> CPU::Handler CPU::instruction<0x022>(void) { return debug_not_impl(0x022); }
template<> CPU::Handler CPU::instruction<0x023>(void) { return debug_not_impl(0x023); }
template<> CPU::Handler CPU::instruction<0x024>(void) { return debug_not_impl(0x024); }
template<> CPU::Handler CPU::instruction<0x025>(void) { return debug_not_impl(0x025); }
template<> CPU::Handler CPU::instruction<0x026>(void) { return debug_not_impl(0x026); }
template<> CPU::Handler CPU::instruction<0x027>(void) { return debug_not_impl(0x027); }
template<> CPU::Handler CPU::instruction<0x028>(void) { return debug_not_impl(0x028); }
template<> CPU::Handler CPU::instruction<0x029>(void) { return debug_not_impl(0x029); }
template<> CPU::Handler CPU::instruction<0x02a>(void) { return debug_not_impl(0x02a); }
template<> CPU::Handler CPU::instruction<0x02b>(void) { return debug_not_impl(0x02b); }
template<> CPU::Handler CPU::instruction<0x02c>(void) { return debug_not_impl(0x02c); }
template<> CPU::Handler CPU::instruction<0x02d>(void) { return debug_not_impl(0x02d); }
template<> CPU::Handler CPU::instruction<0x02e>(void) { return debug_not_impl(0x02e); }
template<> CPU::Handler CPU::instruction<0x02f>(void) { return debug_not_impl(0x02f); }
template<> CPU::Handler CPU::instruction<0x030>(void) { return debug_not_impl(0x030); }
template<> CPU::Handler CPU::instruction<0x031>(void) { return debug_not_impl(0x031); }
template<> CPU::Handler CPU::instruction<0x032>(void) { return debug_not_impl(0x032); }
template<> CPU::Handler CPU::instruction<0x033>(void) { return debug_not_impl(0x033); }
template<> CPU::Handler CPU::instruction<0x034>(void) { return debug_not_impl(0x034); }
template<> CPU::Handler CPU::instruction<0x035>(void) { return debug_not_impl(0x035); }
template<> CPU::Handler CPU::instruction<0x036>(void) { return debug_not_impl(0x036); }
template<> CPU::Handler CPU::instruction<0x037>(void) { return debug_not_impl(0x037); }
template<> CPU::Handler CPU::instruction<0x038>(void) { _p |= PFLAG_C; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x039>(void) { return debug_not_impl(0x039); }
template<> CPU::Handler CPU::instruction<0x03a>(void) { return debug_not_impl(0x03a); }
template<> CPU::Handler CPU::instruction<0x03b>(void) { return debug_not_impl(0x03b); }
template<> CPU::Handler CPU::instruction<0x03c>(void) { return debug_not_impl(0x03c); }
template<> CPU::Handler CPU::instruction<0x03d>(void) { return debug_not_impl(0x03d); }
template<> CPU::Handler CPU::instruction<0x03e>(void) { return debug_not_impl(0x03e); }
template<> CPU::Handler CPU::instruction<0x03f>(void) { return debug_not_impl(0x03f); }
template<> CPU::Handler CPU::instruction<0x040>(void) { return debug_not_impl(0x040); }
template<> CPU::Handler CPU::instruction<0x041>(void) { return debug_not_impl(0x041); }
template<> CPU::Handler CPU::instruction<0x042>(void) { return debug_not_impl(0x042); }
template<> CPU::Handler CPU::instruction<0x043>(void) { return debug_not_impl(0x043); }
template<> CPU::Handler CPU::instruction<0x044>(void) { return debug_not_impl(0x044); }
template<> CPU::Handler CPU::instruction<0x045>(void) { return debug_not_impl(0x045); }
template<> CPU::Handler CPU::instruction<0x046>(void) { return debug_not_impl(0x046); }
template<> CPU::Handler CPU::instruction<0x047>(void) { return debug_not_impl(0x047); }
template<> CPU::Handler CPU::instruction<0x048>(void) { return debug_not_impl(0x048); }
template<> CPU::Handler CPU::instruction<0x049>(void) { return debug_not_impl(0x049); }
template<> CPU::Handler CPU::instruction<0x04a>(void) { return debug_not_impl(0x04a); }
template<> CPU::Handler CPU::instruction<0x04b>(void) { return debug_not_impl(0x04b); }
template<> CPU::Handler CPU::instruction<0x04c>(void) { return debug_not_impl(0x04c); }
template<> CPU::Handler CPU::instruction<0x04d>(void) { return debug_not_impl(0x04d); }
template<> CPU::Handler CPU::instruction<0x04e>(void) { return debug_not_impl(0x04e); }
template<> CPU::Handler CPU::instruction<0x04f>(void) { return debug_not_impl(0x04f); }
template<> CPU::Handler CPU::instruction<0x050>(void) { return debug_not_impl(0x050); }
template<> CPU::Handler CPU::instruction<0x051>(void) { return debug_not_impl(0x051); }
template<> CPU::Handler CPU::instruction<0x052>(void) { return debug_not_impl(0x052); }
template<> CPU::Handler CPU::instruction<0x053>(void) { return debug_not_impl(0x053); }
template<> CPU::Handler CPU::instruction<0x054>(void) { return debug_not_impl(0x054); }
template<> CPU::Handler CPU::instruction<0x055>(void) { return debug_not_impl(0x055); }
template<> CPU::Handler CPU::instruction<0x056>(void) { return debug_not_impl(0x056); }
template<> CPU::Handler CPU::instruction<0x057>(void) { return debug_not_impl(0x057); }
template<> CPU::Handler CPU::instruction<0x058>(void) { _p &= ~PFLAG_I; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x059>(void) { return debug_not_impl(0x059); }
template<> CPU::Handler CPU::instruction<0x05a>(void) { return debug_not_impl(0x05a); }
template<> CPU::Handler CPU::instruction<0x05b>(void) { return debug_not_impl(0x05b); }
template<> CPU::Handler CPU::instruction<0x05c>(void) { return debug_not_impl(0x05c); }
template<> CPU::Handler CPU::instruction<0x05d>(void) { return debug_not_impl(0x05d); }
template<> CPU::Handler CPU::instruction<0x05e>(void) { return debug_not_impl(0x05e); }
template<> CPU::Handler CPU::instruction<0x05f>(void) { return debug_not_impl(0x05f); }
template<> CPU::Handler CPU::instruction<0x060>(void) { return debug_not_impl(0x060); }
template<> CPU::Handler CPU::instruction<0x061>(void) { return debug_not_impl(0x061); }
template<> CPU::Handler CPU::instruction<0x062>(void) { return debug_not_impl(0x062); }
template<> CPU::Handler CPU::instruction<0x063>(void) { return debug_not_impl(0x063); }
template<> CPU::Handler CPU::instruction<0x064>(void) { return debug_not_impl(0x064); }
template<> CPU::Handler CPU::instruction<0x065>(void) { return debug_not_impl(0x065); }
template<> CPU::Handler CPU::instruction<0x066>(void) { return debug_not_impl(0x066); }
template<> CPU::Handler CPU::instruction<0x067>(void) { return debug_not_impl(0x067); }
template<> CPU::Handler CPU::instruction<0x068>(void) { return debug_not_impl(0x068); }
template<> CPU::Handler CPU::instruction<0x069>(void) { return debug_not_impl(0x069); }
template<> CPU::Handler CPU::instruction<0x06a>(void) { return debug_not_impl(0x06a); }
template<> CPU::Handler CPU::instruction<0x06b>(void) { return debug_not_impl(0x06b); }
template<> CPU::Handler CPU::instruction<0x06c>(void) { return debug_not_impl(0x06c); }
template<> CPU::Handler CPU::instruction<0x06d>(void) { return debug_not_impl(0x06d); }
template<> CPU::Handler CPU::instruction<0x06e>(void) { return debug_not_impl(0x06e); }
template<> CPU::Handler CPU::instruction<0x06f>(void) { return debug_not_impl(0x06f); }
template<> CPU::Handler CPU::instruction<0x070>(void) { return debug_not_impl(0x070); }
template<> CPU::Handler CPU::instruction<0x071>(void) { return debug_not_impl(0x071); }
template<> CPU::Handler CPU::instruction<0x072>(void) { return debug_not_impl(0x072); }
template<> CPU::Handler CPU::instruction<0x073>(void) { return debug_not_impl(0x073); }
template<> CPU::Handler CPU::instruction<0x074>(void) { return debug_not_impl(0x074); }
template<> CPU::Handler CPU::instruction<0x075>(void) { return debug_not_impl(0x075); }
template<> CPU::Handler CPU::instruction<0x076>(void) { return debug_not_impl(0x076); }
template<> CPU::Handler CPU::instruction<0x077>(void) { return debug_not_impl(0x077); }
template<> CPU::Handler CPU::instruction<0x078>(void) { _p |= PFLAG_I; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x079>(void) { return debug_not_impl(0x079); }
template<> CPU::Handler CPU::instruction<0x07a>(void) { return debug_not_impl(0x07a); }
template<> CPU::Handler CPU::instruction<0x07b>(void) { return debug_not_impl(0x07b); }
template<> CPU::Handler CPU::instruction<0x07c>(void) { return debug_not_impl(0x07c); }
template<> CPU::Handler CPU::instruction<0x07d>(void) { return debug_not_impl(0x07d); }
template<> CPU::Handler CPU::instruction<0x07e>(void) { return debug_not_impl(0x07e); }
template<> CPU::Handler CPU::instruction<0x07f>(void) { return debug_not_impl(0x07f); }
template<> CPU::Handler CPU::instruction<0x080>(void) { return debug_not_impl(0x080); }
template<> CPU::Handler CPU::instruction<0x081>(void) { return debug_not_impl(0x081); }
template<> CPU::Handler CPU::instruction<0x082>(void) { return debug_not_impl(0x082); }
template<> CPU::Handler CPU::instruction<0x083>(void) { return debug_not_impl(0x083); }
template<> CPU::Handler CPU::instruction<0x084>(void) { return debug_not_impl(0x084); }
template<> CPU::Handler CPU::instruction<0x085>(void) { return debug_not_impl(0x085); }
template<> CPU::Handler CPU::instruction<0x086>(void) { return debug_not_impl(0x086); }
template<> CPU::Handler CPU::instruction<0x087>(void) { return debug_not_impl(0x087); }
template<> CPU::Handler CPU::instruction<0x088>(void) { return debug_not_impl(0x088); }
template<> CPU::Handler CPU::instruction<0x089>(void) { return debug_not_impl(0x089); }
template<> CPU::Handler CPU::instruction<0x08a>(void) { read(_pc); _a = _x; flagNZ(_x); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x08b>(void) { return debug_not_impl(0x08b); }
template<> CPU::Handler CPU::instruction<0x08c>(void) { return debug_not_impl(0x08c); }
template<> CPU::Handler CPU::instruction<0x08d>(void) { return debug_not_impl(0x08d); }
template<> CPU::Handler CPU::instruction<0x08e>(void) { return debug_not_impl(0x08e); }
template<> CPU::Handler CPU::instruction<0x08f>(void) { return debug_not_impl(0x08f); }
template<> CPU::Handler CPU::instruction<0x090>(void) { return debug_not_impl(0x090); }
template<> CPU::Handler CPU::instruction<0x091>(void) { return debug_not_impl(0x091); }
template<> CPU::Handler CPU::instruction<0x092>(void) { return debug_not_impl(0x092); }
template<> CPU::Handler CPU::instruction<0x093>(void) { return debug_not_impl(0x093); }
template<> CPU::Handler CPU::instruction<0x094>(void) { return debug_not_impl(0x094); }
template<> CPU::Handler CPU::instruction<0x095>(void) { return debug_not_impl(0x095); }
template<> CPU::Handler CPU::instruction<0x096>(void) { return debug_not_impl(0x096); }
template<> CPU::Handler CPU::instruction<0x097>(void) { return debug_not_impl(0x097); }
template<> CPU::Handler CPU::instruction<0x098>(void) { read(_pc); _a = _y; flagNZ(_y); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x099>(void) { return debug_not_impl(0x099); }
template<> CPU::Handler CPU::instruction<0x09a>(void) { read(_pc); _s = _x; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x09b>(void) { return debug_not_impl(0x09b); }
template<> CPU::Handler CPU::instruction<0x09c>(void) { return debug_not_impl(0x09c); }
template<> CPU::Handler CPU::instruction<0x09d>(void) { return debug_not_impl(0x09d); }
template<> CPU::Handler CPU::instruction<0x09e>(void) { return debug_not_impl(0x09e); }
template<> CPU::Handler CPU::instruction<0x09f>(void) { return debug_not_impl(0x09f); }
template<> CPU::Handler CPU::instruction<0x0a0>(void) { _y = read(_pc++); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0a1>(void) { return debug_not_impl(0x0a1); }
template<> CPU::Handler CPU::instruction<0x0a2>(void) { _x = read(_pc++); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0a3>(void) { return debug_not_impl(0x0a3); }
template<> CPU::Handler CPU::instruction<0x0a4>(void) { return debug_not_impl(0x0a4); }
template<> CPU::Handler CPU::instruction<0x0a5>(void) { return debug_not_impl(0x0a5); }
template<> CPU::Handler CPU::instruction<0x0a6>(void) { return debug_not_impl(0x0a6); }
template<> CPU::Handler CPU::instruction<0x0a7>(void) { return debug_not_impl(0x0a7); }
template<> CPU::Handler CPU::instruction<0x0a8>(void) { read(_pc); _y = _a; flagNZ(_a); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0a9>(void) { _a = read(_pc++); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0aa>(void) { read(_pc); _x = _a; flagNZ(_a); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0ab>(void) { return debug_not_impl(0x0ab); }
template<> CPU::Handler CPU::instruction<0x0ac>(void) { return debug_not_impl(0x0ac); }
template<> CPU::Handler CPU::instruction<0x0ad>(void) { return debug_not_impl(0x0ad); }
template<> CPU::Handler CPU::instruction<0x0ae>(void) { return debug_not_impl(0x0ae); }
template<> CPU::Handler CPU::instruction<0x0af>(void) { return debug_not_impl(0x0af); }
template<> CPU::Handler CPU::instruction<0x0b0>(void) { return debug_not_impl(0x0b0); }
template<> CPU::Handler CPU::instruction<0x0b1>(void) { return debug_not_impl(0x0b1); }
template<> CPU::Handler CPU::instruction<0x0b2>(void) { return debug_not_impl(0x0b2); }
template<> CPU::Handler CPU::instruction<0x0b3>(void) { return debug_not_impl(0x0b3); }
template<> CPU::Handler CPU::instruction<0x0b4>(void) { return debug_not_impl(0x0b4); }
template<> CPU::Handler CPU::instruction<0x0b5>(void) { return debug_not_impl(0x0b5); }
template<> CPU::Handler CPU::instruction<0x0b6>(void) { return debug_not_impl(0x0b6); }
template<> CPU::Handler CPU::instruction<0x0b7>(void) { return debug_not_impl(0x0b7); }
template<> CPU::Handler CPU::instruction<0x0b8>(void) { _p &= ~PFLAG_V; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0b9>(void) { return debug_not_impl(0x0b9); }
template<> CPU::Handler CPU::instruction<0x0ba>(void) { read(_pc); _x = _s; flagNZ(_s); return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0bb>(void) { return debug_not_impl(0x0bb); }
template<> CPU::Handler CPU::instruction<0x0bc>(void) { return debug_not_impl(0x0bc); }
template<> CPU::Handler CPU::instruction<0x0bd>(void) { return debug_not_impl(0x0bd); }
template<> CPU::Handler CPU::instruction<0x0be>(void) { return debug_not_impl(0x0be); }
template<> CPU::Handler CPU::instruction<0x0bf>(void) { return debug_not_impl(0x0bf); }
template<> CPU::Handler CPU::instruction<0x0c0>(void) { return debug_not_impl(0x0c0); }
template<> CPU::Handler CPU::instruction<0x0c1>(void) { return debug_not_impl(0x0c1); }
template<> CPU::Handler CPU::instruction<0x0c2>(void) { return debug_not_impl(0x0c2); }
template<> CPU::Handler CPU::instruction<0x0c3>(void) { return debug_not_impl(0x0c3); }
template<> CPU::Handler CPU::instruction<0x0c4>(void) { return debug_not_impl(0x0c4); }
template<> CPU::Handler CPU::instruction<0x0c5>(void) { return debug_not_impl(0x0c5); }
template<> CPU::Handler CPU::instruction<0x0c6>(void) { return debug_not_impl(0x0c6); }
template<> CPU::Handler CPU::instruction<0x0c7>(void) { return debug_not_impl(0x0c7); }
template<> CPU::Handler CPU::instruction<0x0c8>(void) { return debug_not_impl(0x0c8); }
template<> CPU::Handler CPU::instruction<0x0c9>(void) { return debug_not_impl(0x0c9); }
template<> CPU::Handler CPU::instruction<0x0ca>(void) { return debug_not_impl(0x0ca); }
template<> CPU::Handler CPU::instruction<0x0cb>(void) { return debug_not_impl(0x0cb); }
template<> CPU::Handler CPU::instruction<0x0cc>(void) { return debug_not_impl(0x0cc); }
template<> CPU::Handler CPU::instruction<0x0cd>(void) { return debug_not_impl(0x0cd); }
template<> CPU::Handler CPU::instruction<0x0ce>(void) { return debug_not_impl(0x0ce); }
template<> CPU::Handler CPU::instruction<0x0cf>(void) { return debug_not_impl(0x0cf); }
template<> CPU::Handler CPU::instruction<0x0d0>(void) { return debug_not_impl(0x0d0); }
template<> CPU::Handler CPU::instruction<0x0d1>(void) { return debug_not_impl(0x0d1); }
template<> CPU::Handler CPU::instruction<0x0d2>(void) { return debug_not_impl(0x0d2); }
template<> CPU::Handler CPU::instruction<0x0d3>(void) { return debug_not_impl(0x0d3); }
template<> CPU::Handler CPU::instruction<0x0d4>(void) { return debug_not_impl(0x0d4); }
template<> CPU::Handler CPU::instruction<0x0d5>(void) { return debug_not_impl(0x0d5); }
template<> CPU::Handler CPU::instruction<0x0d6>(void) { return debug_not_impl(0x0d6); }
template<> CPU::Handler CPU::instruction<0x0d7>(void) { return debug_not_impl(0x0d7); }
template<> CPU::Handler CPU::instruction<0x0d8>(void) { _p &= ~PFLAG_D; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0d9>(void) { return debug_not_impl(0x0d9); }
template<> CPU::Handler CPU::instruction<0x0da>(void) { return debug_not_impl(0x0da); }
template<> CPU::Handler CPU::instruction<0x0db>(void) { return debug_not_impl(0x0db); }
template<> CPU::Handler CPU::instruction<0x0dc>(void) { return debug_not_impl(0x0dc); }
template<> CPU::Handler CPU::instruction<0x0dd>(void) { return debug_not_impl(0x0dd); }
template<> CPU::Handler CPU::instruction<0x0de>(void) { return debug_not_impl(0x0de); }
template<> CPU::Handler CPU::instruction<0x0df>(void) { return debug_not_impl(0x0df); }
template<> CPU::Handler CPU::instruction<0x0e0>(void) { return debug_not_impl(0x0e0); }
template<> CPU::Handler CPU::instruction<0x0e1>(void) { return debug_not_impl(0x0e1); }
template<> CPU::Handler CPU::instruction<0x0e2>(void) { return debug_not_impl(0x0e2); }
template<> CPU::Handler CPU::instruction<0x0e3>(void) { return debug_not_impl(0x0e3); }
template<> CPU::Handler CPU::instruction<0x0e4>(void) { return debug_not_impl(0x0e4); }
template<> CPU::Handler CPU::instruction<0x0e5>(void) { return debug_not_impl(0x0e5); }
template<> CPU::Handler CPU::instruction<0x0e6>(void) { return debug_not_impl(0x0e6); }
template<> CPU::Handler CPU::instruction<0x0e7>(void) { return debug_not_impl(0x0e7); }
template<> CPU::Handler CPU::instruction<0x0e8>(void) { return debug_not_impl(0x0e8); }
template<> CPU::Handler CPU::instruction<0x0e9>(void) { return debug_not_impl(0x0e9); }
template<> CPU::Handler CPU::instruction<0x0ea>(void) { return debug_not_impl(0x0ea); }
template<> CPU::Handler CPU::instruction<0x0eb>(void) { return debug_not_impl(0x0eb); }
template<> CPU::Handler CPU::instruction<0x0ec>(void) { return debug_not_impl(0x0ec); }
template<> CPU::Handler CPU::instruction<0x0ed>(void) { return debug_not_impl(0x0ed); }
template<> CPU::Handler CPU::instruction<0x0ee>(void) { return debug_not_impl(0x0ee); }
template<> CPU::Handler CPU::instruction<0x0ef>(void) { return debug_not_impl(0x0ef); }
template<> CPU::Handler CPU::instruction<0x0f0>(void) { return debug_not_impl(0x0f0); }
template<> CPU::Handler CPU::instruction<0x0f1>(void) { return debug_not_impl(0x0f1); }
template<> CPU::Handler CPU::instruction<0x0f2>(void) { return debug_not_impl(0x0f2); }
template<> CPU::Handler CPU::instruction<0x0f3>(void) { return debug_not_impl(0x0f3); }
template<> CPU::Handler CPU::instruction<0x0f4>(void) { return debug_not_impl(0x0f4); }
template<> CPU::Handler CPU::instruction<0x0f5>(void) { return debug_not_impl(0x0f5); }
template<> CPU::Handler CPU::instruction<0x0f6>(void) { return debug_not_impl(0x0f6); }
template<> CPU::Handler CPU::instruction<0x0f7>(void) { return debug_not_impl(0x0f7); }
template<> CPU::Handler CPU::instruction<0x0f8>(void) { _p |= PFLAG_D; return ((Handler)&CPU::dispatch); }
template<> CPU::Handler CPU::instruction<0x0f9>(void) { return debug_not_impl(0x0f9); }
template<> CPU::Handler CPU::instruction<0x0fa>(void) { return debug_not_impl(0x0fa); }
template<> CPU::Handler CPU::instruction<0x0fb>(void) { return debug_not_impl(0x0fb); }
template<> CPU::Handler CPU::instruction<0x0fc>(void) { return debug_not_impl(0x0fc); }
template<> CPU::Handler CPU::instruction<0x0fd>(void) { return debug_not_impl(0x0fd); }
template<> CPU::Handler CPU::instruction<0x0fe>(void) { return debug_not_impl(0x0fe); }
template<> CPU::Handler CPU::instruction<0x0ff>(void) { return debug_not_impl(0x0ff); }
template<> CPU::Handler CPU::instruction<0x100>(void) { _addr = 0xfffc; read(_pc); return ((Handler)&CPU::instruction<0x10a>); }
template<> CPU::Handler CPU::instruction<0x101>(void) { return debug_not_impl(0x101); }
template<> CPU::Handler CPU::instruction<0x102>(void) { return debug_not_impl(0x102); }

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
    ((CPU::Handler)&CPU::instruction<0x108>), ((CPU::Handler)&CPU::instruction<0x109>), ((CPU::Handler)&CPU::instruction<0x10a>),
};
