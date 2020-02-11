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
            std::getchar();
            addr = value << 8;
            _ppu.tick(3);
            _apu.tick(1);
            for (int i = 0; i < 256; ++i)
            {
                tmp = read(addr | i);
                _ppu.tick(3);
                _apu.tick(1);
                _ppu.oamWrite(tmp);
                _ppu.tick(3);
                _apu.tick(1);
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

