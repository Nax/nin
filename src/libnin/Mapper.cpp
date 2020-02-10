#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/Cart.h>
#include <libnin/Util.h>

using namespace libnin;

Mapper::Mapper(Memory& memory, Cart& cart, IRQ& irq)
: _memory(memory)
, _cart(cart)
, _irq(irq)
, _videoReadHandler(&Mapper::videoRead_NULL)
{
    mirror(NIN_MIRROR_H);
    configure(0, 0);
}

Mapper::~Mapper()
{

}

void Mapper::configure(std::uint16_t mapperMajor, std::uint8_t mapperMinor)
{
    switch (mapperMajor)
    {
    case 0:
        /* NROM */
        _writeHandler = &Mapper::write_NROM;
        break;
    case 1:
        /* MMC1 */
        _mmc1 = MMC1{};
        _writeHandler = &Mapper::write_MMC1;
        break;
    case 2:
        /* UxROM (180) */
        _writeHandler = &Mapper::write_UXROM;
        break;
    case 3:
        /* CNROM */
        _writeHandler = &Mapper::write_CNROM;
        break;
    case 4:
        /* MMC3 */
        _mmc3 = MMC3{};
        _writeHandler = &Mapper::write_MMC3;
        _videoReadHandler = &Mapper::videoRead_MMC3;
        _mmc3.bank[6] = 0;
        _mmc3.bank[7] = 1;
        break;
    case 7:
        /* AXROM */
        bankPrg8k(1, 2);
        mirror(NIN_MIRROR_A);
        _writeHandler = &Mapper::write_AXROM;
        break;
    case 9:
        /* MMC2 */
        _mmc2 = MMC2{};
        _writeHandler = &Mapper::write_MMC2;
        _videoReadHandler = &Mapper::videoRead_MMC2;
        bankPrg8k(1, -3);
        bankPrg8k(2, -2);
        bankPrg8k(3, -1);
        break;
    case 10:
        /* MMC4 */
        /* Implemented as a MMC2 variant */
        _mmc2 = MMC2{};
        _writeHandler = &Mapper::write_MMC4;
        _videoReadHandler = &Mapper::videoRead_MMC2;
        break;
    case 11:
        /* ColorDreams */
        _writeHandler = &Mapper::write_ColorDreams;
        break;
    case 66:
        /* GXROM */
        _writeHandler = &Mapper::write_GXROM;
        break;
    case 180:
        /* UxROM (180) */
        _writeHandler = &Mapper::write_UXROM180;
        break;
    }
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

void Mapper::bankPrg8k(std::uint8_t slot, std::int16_t bank)
{
    const CartSegment& seg = _cart.segment(CART_PRG_ROM);
    bank += seg.bankCount;
    _prg[slot] = seg.base + (bank & (seg.bankCount - 1)) * 0x2000;
}

void Mapper::bankPrg16k(std::uint8_t slot, std::int16_t bank)
{
    bankPrg8k(slot * 2 + 0, bank * 2 + 0);
    bankPrg8k(slot * 2 + 1, bank * 2 + 1);
}

void Mapper::bankPrg32k(std::int16_t bank)
{
    bankPrg8k(0, bank * 4 + 0);
    bankPrg8k(1, bank * 4 + 1);
    bankPrg8k(2, bank * 4 + 2);
    bankPrg8k(3, bank * 4 + 3);
}

void Mapper::bankChr1k(std::uint8_t slot, std::int16_t bank)
{
    const CartSegment& segRam = _cart.segment(CART_CHR_RAM);
    const CartSegment& segRom = _cart.segment(CART_CHR_ROM);

    if (segRam.base)
    {
        bank += segRam.bankCount;
        _chr[slot] = segRam.base + (bank & (segRam.bankCount - 1)) * 0x400;
    }
    else
    {
        bank += segRom.bankCount;
        _chr[slot] = segRom.base + (bank & (segRom.bankCount - 1)) * 0x400;
    }
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

void Mapper::videoRead_NULL(std::uint16_t addr)
{
    UNUSED(addr);
}
