#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/Cart.h>

using namespace libnin;

Mapper::Mapper(Memory& memory, Cart& cart)
: _memory(memory)
, _cart(cart)
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
    case 2:
        /* UxROM (180) */
        _writeHandler = &Mapper::write_UXROM;
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

/* ROM CONFIG */
/* Mapper-specific logic */

#if 0
switch (state->mapper)
{
default:
    return NIN_ERROR_UNKNOWN_MAPPER;
case 0:
    /* NROM */
    break;
case 1:
    /* MMC1 */
    state->prgWriteHandler = &ninPrgWriteHandlerMMC1;
    break;
case 2:
    /* UxROM */
    state->prgWriteHandler = &ninPrgWriteHandlerUXROM;
    break;
case 3:
    /* CNROM */
    state->prgWriteHandler = &ninPrgWriteHandlerCNROM;
    break;
case 4:
    /* MMC3 */
    state->prgWriteHandler = &ninPrgWriteHandlerMMC3;
    state->ppuMonitorHandler = &ninPpuMonitorHandlerMMC3;
    state->mapperRegs.mmc3.bank[6] = 0;
    state->mapperRegs.mmc3.bank[7] = 1;
    break;
case 7:
    /* AXROM */
    ninBankSwitchPrgRom8k(state, 1, 2);
    for (int i = 0; i < 4; ++i)
        state->nametables[i] = state->vram;
    state->prgWriteHandler = &ninPrgWriteHandlerAXROM;
    break;
case 9:
    /* MMC2 */
    state->prgWriteHandler = &ninPrgWriteHandlerMMC2;
    state->ppuMonitorHandler = &ninPpuMonitorHandlerMMC2;
    ninBankSwitchPrgRom8k(state, 1, -3);
    ninBankSwitchPrgRom8k(state, 2, -2);
    ninBankSwitchPrgRom8k(state, 3, -1);
    break;
case 10:
    /* MMC4 */
    state->prgWriteHandler = &ninPrgWriteHandlerMMC4;
    state->ppuMonitorHandler = &ninPpuMonitorHandlerMMC2;
    break;
case 11:
    /* ColorDreams */
    state->prgWriteHandler = &ninPrgWriteHandlerColorDreams;
    break;
case 66:
    /* GXROM */
    state->prgWriteHandler = &ninPrgWriteHandlerGXROM;
    break;
case 180:
    /* UxROM (180) */
    state->prgWriteHandler = &ninPrgWriteHandlerUXROM_180;
    break;
}

#endif
