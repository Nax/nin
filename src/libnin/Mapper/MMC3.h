#ifndef LIBNIN_MAPPER_MMC3_H
#define LIBNIN_MAPPER_MMC3_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC3 : public Mapper
{
public:
    MapperMMC3(Memory& memory, Cart& cart, IRQ& irq);

    void handleReset() override;
    void handleWrite(std::uint16_t addr, std::uint8_t value) override;
    void handleVideoRead(std::uint16_t addr) override;

private:
    void apply();

    std::uint8_t _bankSelect:3;
    std::uint8_t _bank[8];
    std::uint8_t _bankModePrgRom:1;
    std::uint8_t _bankModeChrRom:1;

    std::uint8_t    _irqScanlineEnabled:1;
    std::uint8_t    _irqScanlineReload:1;
    std::uint8_t    _irqScanlineCounter;
    std::uint8_t    _irqScanlineReloadValue;
    std::uint16_t   _irqScanlineFilterShifter;
    std::uint16_t   _oldVmemAddr;
};

}

#endif
