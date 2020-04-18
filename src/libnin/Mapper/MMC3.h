#ifndef LIBNIN_MAPPER_MMC3_H
#define LIBNIN_MAPPER_MMC3_H 1

#include <cstdint>

namespace libnin
{

struct MapperMMC3
{
    std::uint8_t    bankSelect:3;
    std::uint8_t    bank[8];
    std::uint8_t    bankModePrgRom:1;
    std::uint8_t    bankModeChrRom:1;

    std::uint8_t    irqScanlineEnabled:1;
    std::uint8_t    irqScanlineReload:1;
    std::uint8_t    irqScanlineCounter;
    std::uint8_t    irqScanlineReloadValue;
    std::uint16_t   irqScanlineFilterShifter;
    std::uint16_t   oldVmemAddr;
};

}

#endif
