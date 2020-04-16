#ifndef LIBNIN_MAPPER_MMC1_H
#define LIBNIN_MAPPER_MMC1_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC1 : public Mapper
{
public:
    MapperMMC1(Memory& memory, Cart& cart, IRQ& irq);

    void handleWrite(std::uint16_t addr, std::uint8_t value) override;

private:
    void mapperRegWrite(std::uint16_t addr, std::uint8_t value);
    void mapperBankPrg();
    void mapperBankChr();
    void mapperMirror();

    std::uint8_t _shift;
    std::uint8_t _count;
    std::uint8_t _mirroring:2;
    std::uint8_t _prgBankMode:2;
    std::uint8_t _chrBankMode:1;
    std::uint8_t _chrBank0:5;
    std::uint8_t _chrBank1:5;
    std::uint8_t _prgBank:4;
};

}

#endif
