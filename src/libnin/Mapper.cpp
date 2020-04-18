#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/Cart.h>
#include <libnin/Util.h>

using namespace libnin;

Mapper::Mapper(Memory& memory, Cart& cart, IRQ& irq)
: _memory{memory}
, _cart{cart}
, _irq{irq}
, _handleReset{&Mapper::handleReset<MapperID::NROM>}
, _handleTick{&Mapper::handleTick<MapperID::NROM>}
, _handleRead{&Mapper::handleRead<MapperID::NROM>}
, _handleWrite{&Mapper::handleWrite<MapperID::NROM>}
, _handleVideoRead{&Mapper::handleVideoRead<MapperID::NROM>}
, _handleNtRead{&Mapper::handleNtRead<MapperID::NROM>}
, _handleNtWrite{&Mapper::handleNtWrite<MapperID::NROM>}
, _handleChrRead{&Mapper::handleChrRead<MapperID::NROM>}
, _handleChrWrite{&Mapper::handleChrWrite<MapperID::NROM>}
, _prg{}
, _prgWriteFlag{}
, _chr{}
, _nametables{}
{

}

NinError Mapper::configure(int mapper, int submapper)
{
    MapperID mapperID;

    mapperID = getMapperID(mapper, submapper);
    switch (mapperID)
    {
    case MapperID::Bad:
        return NIN_ERROR_BAD_MAPPER;
    case MapperID::Unknown:
        return NIN_ERROR_UNKNOWN_MAPPER;
    default:
        break;
    }

    mirror(NIN_MIRROR_H);
    bankChr8k(0);
    bankPrg8k(1, CART_PRG_RAM, 0);
    bankPrg8k(2, CART_PRG_ROM, 0);
    bankPrg8k(3, CART_PRG_ROM, 1);
    bankPrg8k(4, CART_PRG_ROM, -2);
    bankPrg8k(5, CART_PRG_ROM, -1);
    initMatching<MapperID(0)>(mapperID);
    return NIN_OK;
}

std::uint8_t Mapper::read(std::uint16_t addr)
{
    std::uint8_t value = (this->*_handleRead)(addr);
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
    (this->*_handleWrite)(addr, value);
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

template <>
void Mapper::initMatching<MapperID::MAX>(MapperID id2)
{
    UNUSED(id2);
}

template <MapperID id>
void Mapper::initMatching(MapperID id2)
{
    if (id == id2)
        init<id>();
    else
        initMatching<MapperID((int)id + 1)>(id2);
}

template <MapperID id>
void Mapper::handleReset()
{

}

template <MapperID id>
void Mapper::handleTick()
{

}

template <MapperID id>
std::uint8_t Mapper::handleRead(std::uint16_t addr)
{
    return 0x00;
}

template <MapperID id>
void Mapper::handleWrite(std::uint16_t addr, std::uint8_t value)
{

}

template <MapperID id>
void Mapper::handleVideoRead(std::uint16_t addr)
{

}

template <MapperID id>
std::uint8_t Mapper::handleNtRead(int nametable, std::uint16_t offset)
{
    return _nametables[nametable][offset];
}

template <MapperID id>
void Mapper::handleNtWrite(int nametable, std::uint16_t offset, std::uint8_t value)
{
    _nametables[nametable][offset] = value;
}

template <MapperID id>
std::uint8_t Mapper::handleChrRead(int bank, std::uint16_t offset)
{
    return _chr[bank][offset];
}

template <MapperID id>
void Mapper::handleChrWrite(int bank, std::uint16_t offset, std::uint8_t value)
{
    if (_cart.segment(CART_CHR_RAM).base)
    {
        _chr[bank][offset] = value;
    }
}
