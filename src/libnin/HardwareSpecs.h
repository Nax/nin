#ifndef LIBNIN_HARDWARE_SPECS_H
#define LIBNIN_HARDWARE_SPECS_H 1

#include <cstdint>

namespace libnin
{

class HardwareSpecs
{
public:
    std::uint32_t clockRate;
    std::uint32_t frameCycles;
    std::uint32_t frameDelay;
    std::uint32_t cycleExtraIncrement;
    std::uint32_t firstVisibleScanline;
    std::uint16_t vblank;
    std::uint16_t apuFrameCycles[5];
    std::uint16_t apuNoisePeriod[16];
    std::uint16_t apuDmcPeriod[16];

    static const HardwareSpecs NTSC;
    static const HardwareSpecs PAL;
};

}

#endif
