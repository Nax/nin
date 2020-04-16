#ifndef LIBNIN_MAPPER_UXROM_H
#define LIBNIN_MAPPER_UXROM_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperUxROM : public Mapper
{
public:
    using Mapper::Mapper;

    void handleReset() override;
    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
};

}

#endif
