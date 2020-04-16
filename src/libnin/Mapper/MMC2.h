#ifndef LIBNIN_MAPPER_MMC2_H
#define LIBNIN_MAPPER_MMC2_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC2 : public Mapper
{
public:
    MapperMMC2(Memory& memory, Cart& cart, IRQ& irq);

    void handleReset() override;
    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
    void handleVideoRead(std::uint16_t addr) override;

private:
    void apply();

    std::uint8_t _bankLatch0Lo;
    std::uint8_t _bankLatch0Hi;
    std::uint8_t _bankLatch1Lo;
    std::uint8_t _bankLatch1Hi;
    std::uint8_t _latch0:1;
    std::uint8_t _latch1:1;
};

}

#endif
