#ifndef LIBNIN_MAPPER_CNROM_H
#define LIBNIN_MAPPER_CNROM_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperCNROM : public Mapper
{
public:
    using Mapper::Mapper;

    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
};

}

#endif
