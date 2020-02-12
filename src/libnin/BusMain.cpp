#include <cstring>
#include <libnin/APU.h>
#include <libnin/BusMain.h>
#include <libnin/Input.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/PPU.h>

using namespace libnin;

BusMain::BusMain(Memory& memory, Mapper& mapper, PPU& ppu, APU& apu, Input& input)
: _memory{memory}
, _mapper{mapper}
, _ppu{ppu}
, _apu{apu}
, _input{input}
{

}

std::uint8_t BusMain::read(std::uint16_t addr)
{
    switch (addr >> 12)
    {
    case 0x0:
    case 0x1:
        return _memory.ram[addr & 0x7ff];
    case 0x2:
    case 0x3:
        return _ppu.regRead(addr);
    case 0x4:
        switch (addr)
        {
        case 0x4000:
        case 0x4001:
        case 0x4002:
        case 0x4003:
        case 0x4004:
        case 0x4005:
        case 0x4006:
        case 0x4007:
        case 0x4008:
        case 0x4009:
        case 0x400a:
        case 0x400b:
        case 0x400c:
        case 0x400d:
        case 0x400e:
        case 0x400f:
        case 0x4010:
        case 0x4011:
        case 0x4012:
        case 0x4013:
        case 0x4015:
            return _apu.regRead(addr);
        case 0x4016:
            // JOY1
            return _input.read();
        case 0x4017:
            // JOY2
            return 0x00;
        case 0x4018:
        case 0x4019:
        case 0x401a:
        case 0x401b:
        case 0x401c:
        case 0x401d:
        case 0x401e:
        case 0x401f:
            return 0x00;
        default:
            return 0x00;
        }
    case 0x5:
    case 0x6:
    case 0x7:
        return 0x00;
    case 0x8:
    case 0x9:
    case 0xa:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
        return _mapper.prg((addr >> 13) & 0x3)[addr & 0x1fff];
    default:
        return 0x00;
    }
}

#include <cstdio>

void BusMain::write(std::uint16_t addr, std::uint8_t value)
{
    std::uint8_t tmp;

    switch (addr >> 12)
    {
    case 0x0:
    case 0x1:
        _memory.ram[addr & 0x7ff] = value;
        return;
    case 0x2:
    case 0x3:
        _ppu.regWrite(addr, value);
        return;
    case 0x4:
        switch (addr)
        {
        case 0x4000:
        case 0x4001:
        case 0x4002:
        case 0x4003:
        case 0x4004:
        case 0x4005:
        case 0x4006:
        case 0x4007:
        case 0x4008:
        case 0x4009:
        case 0x400a:
        case 0x400b:
        case 0x400c:
        case 0x400d:
        case 0x400e:
        case 0x400f:
        case 0x4010:
        case 0x4011:
        case 0x4012:
        case 0x4013:
        case 0x4015:
        case 0x4017:
            _apu.regWrite(addr, value);
            return;
        case 0x4014:
            // DMA
            // FIXME: Hack
            addr = value << 8;
            //_ppu.tick(3);
            //_apu.tick(1);
            for (int i = 0; i < 256; ++i)
            {
                tmp = read(addr | i);
                //_ppu.tick(3);
                //_apu.tick(1);
                _ppu.oamWrite(tmp);
                //_ppu.tick(3);
                //_apu.tick(1);
            }
            return;
        case 0x4016:
            // JOY STROBE
            _input.poll(!!(value & 0x01));
            return;
        case 0x4018:
        case 0x4019:
        case 0x401a:
        case 0x401b:
        case 0x401c:
        case 0x401d:
        case 0x401e:
        case 0x401f:
            return;
        default:
            return;
        }
    case 0x5:
    case 0x6:
    case 0x7:
        return;
    case 0x8:
    case 0x9:
    case 0xa:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
        _mapper.write(addr, value);
        return;
    }
}

static bool memoryExtractOverlap(std::uint16_t start, std::size_t len, std::uint16_t regionStart, std::size_t regionLen, std::uint16_t* overlapOffset, std::size_t* overlapLen, std::uint16_t* overlapOffsetInDest)
{
    std::size_t offset;
    std::size_t regionEnd;
    std::size_t transferEnd;
    std::size_t end;

    /* Check for non overlapping memory regions */
    if (start >= regionStart + regionLen)
        return false;
    if (start + len < regionStart)
        return false;

    /* Now we know that the regions DO overlap, check the first region byte that overlap */
    if (regionStart > start)
        offset = regionStart;
    else
        offset = start;

    *overlapOffset = offset - regionStart;
    regionEnd = regionStart + regionLen;
    transferEnd = start + len;
    end = regionEnd > transferEnd ? transferEnd : regionEnd;
    *overlapLen = end - offset;
    *overlapOffsetInDest = offset - start;

    return true;
}

void BusMain::dump(std::uint8_t* dst, std::uint16_t start, std::size_t len)
{
    std::size_t oLen;
    std::uint16_t oOff;
    std::uint16_t dOff;

    /* RAM */
    for (int i = 0; i < 4; ++i)
    {
        if (memoryExtractOverlap(start, len, i * 0x800, 0x800, &oOff, &oLen, &dOff))
        {
            std::memcpy(dst + dOff, _memory.ram + oOff, oLen);
        }
    }

    /* 0x2000-0x7fff, not implemented */
    if (memoryExtractOverlap(start, len, 0x2000, 0x6000, &oOff, &oLen, &dOff))
    {
        std::memset(dst + dOff, 0xff, oLen);
    }

    /* 0x8000-0xffff, cart via mapper */
    for (int i = 0; i < 4; ++i)
    {
        if (memoryExtractOverlap(start, len, 0x8000 + i * 0x2000, 0x2000, &oOff, &oLen, &dOff))
        {
            std::memcpy(dst + dOff, _mapper.prg(i) + oOff, oLen);
        }
    }
}
