#include <libnin/Util.h>
#include <libnin/Cart.h>

using namespace libnin;

void Cart::load(int id, std::uint16_t bankCount, std::FILE* file)
{
    CartSegment& seg = _segments[id];

    std::uint32_t size;
    std::uint32_t bankSize;

    delete[] seg.base;

    switch (id)
    {
    case CART_PRG_ROM:
    case CART_PRG_RAM:
        bankSize = 0x2000;
        break;
    case CART_CHR_ROM:
    case CART_CHR_RAM:
        bankSize = 0x400;
        break;
    default:
        UNREACHABLE();
    }

    size = bankCount * bankSize;
    seg.bankCount = bankCount;

    if (size == 0)
    {
        seg.base = nullptr;
    }
    else if (file)
    {
        seg.base = new std::uint8_t[size];
        std::fread(seg.base, size, 1, file);
    }
    else
    {
        seg.base = new std::uint8_t[size]();
    }
}
