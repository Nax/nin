#ifndef LIBNIN_MAPPER_DISK_SYSTEM_H
#define LIBNIN_MAPPER_DISK_SYSTEM_H 1

#include <cstdint>

namespace libnin
{

struct MapperDiskSystem
{
    std::uint32_t   headPos;
    std::uint16_t   delay;
    std::uint16_t   irqReloadValue;
    std::uint16_t   irqTimer;
    std::uint8_t    extPort;
    std::uint8_t    latchRead;
    std::uint8_t    latchWrite;
    std::uint8_t    sideCount;

    bool    motor:1;
    bool    noScan:1;
    bool    inData:1;
    bool    write:1;
    bool    irqEnabledTransfer:1;
    bool    irqEnabledTimer:1;
    bool    irqReloadFlag:1;
    bool    transfered:1;
    bool    scanning:1;
    bool    skippedGap:1;
    bool    endOfDisk:1;
};

}

#endif
