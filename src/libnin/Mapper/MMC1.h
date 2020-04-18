#ifndef LIBNIN_MAPPER_MMC1_H
#define LIBNIN_MAPPER_MMC1_H 1

#include <cstdint>

namespace libnin
{

struct MapperMMC1
{
    std::uint8_t shift;
    std::uint8_t count;
    std::uint8_t mirroring:2;
    std::uint8_t prgBankMode:2;
    std::uint8_t chrBankMode:1;
    std::uint8_t chrBank0:5;
    std::uint8_t chrBank1:5;
    std::uint8_t prgBank:4;
};

}

#endif
