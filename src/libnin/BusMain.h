#ifndef LIBNIN_BUS_MAIN_H
#define LIBNIN_BUS_MAIN_H 1

#include <cstdlib>
#include <libnin/NonCopyable.h>

namespace libnin
{

enum class WriteAction
{
    None,
    DMA
};

class Memory;
class Cart;
class Mapper;
class PPU;
class APU;
class Input;
class BusMain : private NonCopyable
{
public:
    BusMain(Memory& memory, Cart& cart, Mapper& mapper, PPU& ppu, APU& apu, Input& input);

    std::uint8_t    read(std::uint16_t addr);
    WriteAction     write(std::uint16_t addr, std::uint8_t value);

    void dump(std::uint8_t* dst, std::uint16_t start, std::size_t len);

private:
    Memory&     _memory;
    Cart&       _cart;
    Mapper&     _mapper;
    PPU&        _ppu;
    APU&        _apu;
    Input&      _input;
};

};

#endif
