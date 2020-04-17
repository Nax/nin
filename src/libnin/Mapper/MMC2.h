#ifndef LIBNIN_MAPPER_MMC2_H
#define LIBNIN_MAPPER_MMC2_H 1

#include <cstdint>

namespace libnin
{

struct MapperMMC2
{
    std::uint8_t bankLatch0Lo;
    std::uint8_t bankLatch0Hi;
    std::uint8_t bankLatch1Lo;
    std::uint8_t bankLatch1Hi;
    std::uint8_t latch0:1;
    std::uint8_t latch1:1;
};

}

#endif
