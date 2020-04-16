#ifndef LIBNIN_MAPPER_MMC5_H
#define LIBNIN_MAPPER_MMC5_H 1

#include <libnin/Mapper.h>

namespace libnin
{

class MapperMMC5 : public Mapper
{
public:
    MapperMMC5(Memory& memory, Cart& cart, IRQ& irq);

    void            handleReset() override;
    void            handleTick() override;
    std::uint8_t    handleRead(std::uint16_t addr) override;
    void            handleWrite(std::uint16_t addr, std::uint8_t value) override;
    void            handleVideoRead(std::uint16_t addr) override;
    std::uint8_t    handleNtRead(int nametable, std::uint16_t offset) override;
    void            handleNtWrite(int nametable, std::uint16_t offset, std::uint8_t value) override;
    std::uint8_t    handleChrRead(int bank, std::uint16_t offset) override;

private:
    void applyPrg();
    void applyChr();

    std::uint8_t    _bankModePrg:2;
    std::uint8_t    _bankModeChr:2;
    std::uint8_t    _bankSelectPrg[5];
    std::uint8_t    _bankSelectChr[12];
    std::uint8_t    _nametable;
    std::uint8_t    _fillNT;
    std::uint8_t    _fillAT;
    std::uint8_t    _mul[2];
    std::uint8_t*   _chr2[8];

    std::uint16_t   _ppuAddr;
    std::uint8_t    _ppuAddrCount;
    std::uint8_t    _ppuIdleCount;
    std::uint8_t    _fetchCount;
    std::uint8_t    _scanline;
    std::uint8_t    _scanlineTarget;

    bool _ppuReading:1;
    bool _ppuSpriteFlag:1;
    bool _ppuRenderFlag:1;
    bool _mode8x16:1;
    bool _inFrame:1;
    bool _scanlineEnabled:1;
    bool _scanlinePending:1;
};

}

#endif
