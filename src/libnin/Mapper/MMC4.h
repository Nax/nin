#ifndef LIBNIN_MAPPER_MMC4_H
#define LIBNIN_MAPPER_MMC4_H 1

#include <libnin/Mapper/MMC2.h>

namespace libnin
{

class MapperMMC4 : public MapperMMC2
{
public:
    using MapperMMC2::MapperMMC2;

    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
};

}

#endif
