#ifndef LIBNIN_MAPPER_MMC5_H
#define LIBNIN_MAPPER_MMC5_H 1

#include <cstdint>

namespace libnin
{

struct MapperMMC5
{
    std::uint8_t    bankModePrg:2;
    std::uint8_t    bankModeChr:2;
    std::uint8_t    bankSelectPrg[5];
    std::uint8_t    bankSelectChr[12];
    std::uint8_t    nametable;
    std::uint8_t    fillNT;
    std::uint8_t    fillAT;
    std::uint8_t    mul[2];
    std::uint8_t*   chr2[8];

    std::uint16_t   ppuAddr;
    std::uint8_t    ppuAddrCount;
    std::uint8_t    ppuIdleCount;
    std::uint8_t    fetchCount;
    std::uint8_t    scanline;
    std::uint8_t    scanlineTarget;

    bool ppuReading:1;
    bool ppuSpriteFlag:1;
    bool ppuRenderFlag:1;
    bool mode8x16:1;
    bool inFrame:1;
    bool scanlineEnabled:1;
    bool scanlinePending:1;
};

}

#endif
