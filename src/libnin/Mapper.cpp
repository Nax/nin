#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/Cart.h>
#include <libnin/Util.h>

#include <libnin/Mapper/Action52.h>
#include <libnin/Mapper/AxROM.h>
#include <libnin/Mapper/CNROM.h>
#include <libnin/Mapper/ColorDreams.h>
#include <libnin/Mapper/GxROM.h>
#include <libnin/Mapper/MMC1.h>
#include <libnin/Mapper/MMC2.h>
#include <libnin/Mapper/MMC3.h>
#include <libnin/Mapper/MMC4.h>
#include <libnin/Mapper/MMC5.h>
#include <libnin/Mapper/UxROM.h>

#define MAPPER(major, minor, klass, ...)  case ((major << 16) | minor): mapper = new klass(memory, cart, irq, __VA_ARGS__); break

using namespace libnin;

Mapper::Mapper(Memory& memory, Cart& cart, IRQ& irq)
: _memory{ memory }
, _cart{ cart }
, _irq{ irq }
, _prg{}
, _prgWriteFlag{}
, _chr{}
, _nametables{}
{
    mirror(NIN_MIRROR_H);
    bankChr8k(0);
    bankPrg8k(1, CART_PRG_RAM, 0);
    bankPrg8k(2, CART_PRG_ROM, 0);
    bankPrg8k(3, CART_PRG_ROM, 1);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);
}

Mapper::~Mapper()
{
}

Mapper* Mapper::create(Memory& memory, Cart& cart, IRQ& irq, int mapperMajor, int mapperMinor)
{
    Mapper* mapper{};

    switch ((mapperMajor << 16) | mapperMinor)
    {
        MAPPER(0,   0, Mapper);
        MAPPER(1,   0, MapperMMC1);
        MAPPER(2,   0, MapperUxROM);
        MAPPER(3,   0, MapperCNROM);
        MAPPER(4,   0, MapperMMC3);
        MAPPER(5,   0, MapperMMC5);
        MAPPER(7,   0, MapperAxROM);
        MAPPER(9,   0, MapperMMC2);
        MAPPER(10,  0, MapperMMC4);
        MAPPER(11,  0, MapperColorDreams);
        MAPPER(66,  0, MapperGxROM);
        MAPPER(228, 0, MapperAction52);
    default:
        break;
    }

    return mapper;
}

const std::uint8_t* Mapper::bank(int slot) const
{
    return _prg[slot];
}

std::uint8_t Mapper::read(std::uint16_t addr)
{
    std::uint8_t value = handleRead(addr);
    int slot = ((addr - 0x4000) / 0x2000);

    return _prg[slot] ? _prg[slot][addr & 0x1fff] : value;
}

void Mapper::write(std::uint16_t addr, std::uint8_t value)
{
    int slot = ((addr - 0x4000) / 0x2000);

    if (_prg[slot] && _prgWriteFlag[slot])
    {
        _prg[slot][addr & 0x1fff] = value;
    }
    handleWrite(addr, value);
}

void Mapper::mirror(int mirrorMode)
{
    switch (mirrorMode)
    {
    case NIN_MIRROR_A:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x000;
        _nametables[2] = _memory.vram + 0x000;
        _nametables[3] = _memory.vram + 0x000;
        break;
    case NIN_MIRROR_B:
        _nametables[0] = _memory.vram + 0x400;
        _nametables[1] = _memory.vram + 0x400;
        _nametables[2] = _memory.vram + 0x400;
        _nametables[3] = _memory.vram + 0x400;
        break;
    case NIN_MIRROR_H:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x400;
        _nametables[2] = _memory.vram + 0x000;
        _nametables[3] = _memory.vram + 0x400;
        break;
    case NIN_MIRROR_V:
        _nametables[0] = _memory.vram + 0x000;
        _nametables[1] = _memory.vram + 0x000;
        _nametables[2] = _memory.vram + 0x400;
        _nametables[3] = _memory.vram + 0x400;
        break;
    }
}

void Mapper::bankPrg8k(std::uint8_t slot, int domain, std::int16_t bank)
{
    const CartSegment& seg = _cart.segment(domain);
    bank += seg.bankCount;
    if (seg.base)
    {
        _prg[slot] = seg.base + std::uintptr_t(std::uint16_t(bank) % seg.bankCount) * 0x2000;
        _prgWriteFlag[slot] = (domain == CART_PRG_RAM);
    }
    else
    {
        _prg[slot] = nullptr;
        _prgWriteFlag[slot] = false;
    }
}

void Mapper::bankPrg16k(std::uint8_t slot, int domain, std::int16_t bank)
{
    bankPrg8k(slot + 0, domain, bank * 2 + 0);
    bankPrg8k(slot + 1, domain, bank * 2 + 1);
}

void Mapper::bankPrg32k(std::uint8_t slot, int domain, std::int16_t bank)
{
    bankPrg8k(slot + 0, domain, bank * 4 + 0);
    bankPrg8k(slot + 1, domain, bank * 4 + 1);
    bankPrg8k(slot + 2, domain, bank * 4 + 2);
    bankPrg8k(slot + 3, domain, bank * 4 + 3);
}

void Mapper::bankChr1k(std::uint8_t slot, std::int16_t bank)
{
    const CartSegment& segRam = _cart.segment(CART_CHR_RAM);
    const CartSegment& segRom = _cart.segment(CART_CHR_ROM);

    if (segRam.base)
    {
        bank += segRam.bankCount;
        _chr[slot] = segRam.base + std::uintptr_t(bank % segRam.bankCount) * 0x400;
    }
    else
    {
        bank += segRom.bankCount;
        _chr[slot] = segRom.base + std::uintptr_t(bank % segRom.bankCount) * 0x400;
    }
}

void Mapper::bankChr2k(std::uint8_t slot, std::int16_t bank)
{
    bankChr1k(slot * 2 + 0, bank * 2 + 0);
    bankChr1k(slot * 2 + 1, bank * 2 + 1);
}

void Mapper::bankChr4k(std::uint8_t slot, std::int16_t bank)
{
    bankChr1k(slot * 4 + 0, bank * 4 + 0);
    bankChr1k(slot * 4 + 1, bank * 4 + 1);
    bankChr1k(slot * 4 + 2, bank * 4 + 2);
    bankChr1k(slot * 4 + 3, bank * 4 + 3);
}

void Mapper::bankChr8k(std::int16_t bank)
{
    bankChr1k(0, bank * 8 + 0);
    bankChr1k(1, bank * 8 + 1);
    bankChr1k(2, bank * 8 + 2);
    bankChr1k(3, bank * 8 + 3);
    bankChr1k(4, bank * 8 + 4);
    bankChr1k(5, bank * 8 + 5);
    bankChr1k(6, bank * 8 + 6);
    bankChr1k(7, bank * 8 + 7);
}

void Mapper::handleReset()
{

}

void Mapper::handleTick()
{

}

std::uint8_t Mapper::handleRead(std::uint16_t addr)
{
    return 0x00;
}

void Mapper::handleWrite(std::uint16_t addr, std::uint8_t value)
{

}

void Mapper::handleVideoRead(std::uint16_t addr)
{

}

std::uint8_t Mapper::handleNtRead(int nametable, std::uint16_t offset)
{
    return _nametables[nametable][offset];
}

void Mapper::handleNtWrite(int nametable, std::uint16_t offset, std::uint8_t value)
{
    _nametables[nametable][offset] = value;
}

std::uint8_t Mapper::handleChrRead(int bank, std::uint16_t offset)
{
    return _chr[bank][offset];
}

void Mapper::handleChrWrite(int bank, std::uint16_t offset, std::uint8_t value)
{
    if (_cart.segment(CART_CHR_RAM).base)
    {
        _chr[bank][offset] = value;
    }
}
