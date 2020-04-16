#ifndef LIBNIN_MAPPER_GXROM_H
#define LIBNIN_MAPPER_GXROM_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperGxROM : public Mapper
{
public:
    using Mapper::Mapper;

    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
};

}

#endif
