#include <libnin/Disk.h>

using namespace libnin;

Disk::Disk()
: _data{}
, _sideCount{}
{

}

Disk::~Disk()
{
    delete[] _data;
}

void Disk::load(std::FILE* f)
{
    std::uint8_t header[16];

    std::fseek(f, 0, SEEK_SET);
    std::fread(header, 16, 1, f);

    _sideCount = header[4];
    delete[] _data;
    _data = new std::uint8_t[(std::size_t)DiskSize * _sideCount]();

    for (std::uint8_t i = 0; i < _sideCount; ++i)
    {
        loadSide(f, i);
    }
}

void Disk::loadSide(std::FILE* f, int side)
{
    std::uint8_t* dst;
    std::uint32_t head;
    std::uint16_t fileSize;
    std::uint8_t fileCount;

    dst = _data + (std::size_t)DiskSize * side;
    std::fseek(f, 16 + side * DiskSizeArchive, SEEK_SET);

    /* First, large gap */
    head = Gap0;
    dst[head - 1] = 0x80;

    /* Load block 1 */
    std::fread(dst + head, 0x38, 1, f);
    head += 0x3a;
    head += Gap1;
    dst[head - 1] = 0x80;

    /* Load block 2 */
    std::fread(dst + head, 0x02, 1, f);
    fileCount = dst[head + 1];
    head += 0x04;

    for (int i = 0; i < fileCount; ++i)
    {
        head += Gap1;
        dst[head - 1] = 0x80;

        /* Block 3 */
        std::fread(dst + head, 0x10, 1, f);
        fileSize = *(std::uint16_t*)(dst + head + 13);
        head += 0x12;
        head += Gap1;
        dst[head - 1] = 0x80;

        /* Block 4 */
        std::fread(dst + head, (std::size_t)fileSize + 1, 1, f);
        head += (fileSize + 3);
    }
}
