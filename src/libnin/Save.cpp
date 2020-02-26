#include <libnin/Cart.h>
#include <libnin/Save.h>

using namespace libnin;

Save::Save(Cart& cart)
: _cart{ cart}
, _file{}
, _battery{}
{

}

Save::~Save()
{
    sync();
    if (_file)
        std::fclose(_file);
}

void Save::setBattery(bool battery)
{
    _battery = battery;
}

void Save::setSaveFile(const char* path)
{
    if (!_battery)
        return;

    _file = std::fopen(path, "ab");
    if (_file)
        std::fclose(_file);

    _file = std::fopen(path, "r+b");
    if (_file)
    {
        _cart.load(CART_PRG_RAM, _cart.segment(CART_PRG_RAM).bankCount, _file);
    }
}

void Save::sync()
{
    const CartSegment& prgRam = _cart.segment(CART_PRG_RAM);

    if (!_file)
        return;

    std::fseek(_file, 0, SEEK_SET);
    std::fwrite(prgRam.base, prgRam.bankCount * 0x2000, 1, _file);
    std::fflush(_file);
}
