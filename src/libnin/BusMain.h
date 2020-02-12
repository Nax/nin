#ifndef LIBNIN_BUS_MAIN_H
#define LIBNIN_BUS_MAIN_H 1

#include <cstdlib>
#include <libnin/NonCopyable.h>

namespace libnin
{


class Memory;
class Mapper;
class PPU;
class APU;
class Input;
class BusMain : private NonCopyable
{
public:
    BusMain(Memory& memory, Mapper& mapper, PPU& ppu, APU& apu, Input& input);

    std::uint8_t    read(std::uint16_t addr);
    std::uint16_t   read16(std::uint16_t addr) { return read(addr) | (std::uint16_t(read(addr + 1)) << 8); }
    void            write(std::uint16_t addr, std::uint8_t value);
    void            write16(std::uint16_t addr, std::uint16_t value) { write(addr, value & 0xff); write(addr + 1, value >> 8); }

    void dump(std::uint8_t* dst, std::uint16_t start, std::size_t len);

private:
    Memory&     _memory;
    Mapper&     _mapper;
    PPU&        _ppu;
    APU&        _apu;
    Input&      _input;
};

};

#endif
