#ifndef LIBNIN_MAPPER_ACTION52_H
#define LIBNIN_MAPPER_ACTION52_H 1

#include <libnin/Cart.h>
#include <libnin/Mapper.h>

namespace libnin
{

class MapperAction52 : public Mapper
{
public:
    using Mapper::Mapper;

    void handleReset() override;
    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
};

inline void MapperAction52::handleReset()
{
    bankPrg32k(2, CART_PRG_ROM, 0);
    bankChr8k(0);
}

inline void MapperAction52::handleWrite(std::uint16_t addr, std::uint8_t value)
{
    std::uint16_t prgBank;
    std::uint16_t chrBank;

    if (addr >= 0x8000)
    {
        chrBank = ((addr & 0x000f) << 2) | (value & 0x03);
        prgBank = (addr >> 6) & 0x1f;
        switch ((addr >> 0xb) & 0x3)
        {
        case 0:
            prgBank += 0;
            break;
        case 1:
            prgBank += 32;
            break;
        case 2:
            break;
        case 3:
            prgBank += 64;
        }

        bankChr8k(chrBank);
        if (addr & 0x20)
        {
            bankPrg16k(2, CART_PRG_ROM, prgBank);
            bankPrg16k(4, CART_PRG_ROM, prgBank);
        }
        else
        {
            bankPrg16k(2, CART_PRG_ROM, (prgBank & 0xfffe) | 0);
            bankPrg16k(4, CART_PRG_ROM, (prgBank & 0xfffe) | 1);
        }
        mirror(addr & 0x2000 ? NIN_MIRROR_V : NIN_MIRROR_H);
    }
}

}

#endif
