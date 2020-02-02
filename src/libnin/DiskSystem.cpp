/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <libnin/DiskSystem.h>
#include <libnin/HardwareInfo.h>
#include <libnin/IRQ.h>

#define FDS_CLOCK_MAX           149
#define FDS_DISK_SIZE           0x14000
#define FDS_DISK_SIZE_ARCHIVE   65500
#define FDS_DISK_GAP0           3538
#define FDS_DISK_GAP1           122

DiskSystem::DiskSystem(const HardwareInfo& info, IRQ& irq)
: _info(info)
, _irq(irq)
, _data{}
, _dataCurrentDisk{}
, _headPos{}
, _headClock{}
, _delay{}
, _irqReloadValue{}
, _irqTimer{}
, _extPort{}
, _latchRead{}
, _sideCount{}
, _motor{}
, _noScan{}
, _noWrite{}
, _inData{}
, _irqEnabledTransfer{}
, _irqEnabledTimer{}
, _irqReloadFlag{}
, _transfered{}
, _scanning{}
, _skippedGap{}
, _endOfDisk{}
{

}

DiskSystem::~DiskSystem()
{
    delete[] _data;
}

std::uint8_t DiskSystem::regRead(std::uint16_t addr)
{
    std::uint8_t value;

    value = 0;
    switch (addr)
    {
    case 0x4030: // Disk Status
        value = 0x80;
        if (_irq.check(IRQ_FDS_TIMER)) value |= 0x01;
        if (_transfered)
            value |= 0x02;
        _transfered = 0;
        if (_endOfDisk)
            value |= 0x40;
        _irq.unset(IRQ_FDS_TRANSFER | IRQ_FDS_TIMER);
        break;
    case 0x4031: // Disk Read
        value = _latchRead;
        _transfered = 0;
        _irq.unset(IRQ_FDS_TRANSFER);
        break;
    case 0x4032: // Disk Drive Status
        value = 0x40;
        if (!_scanning)
            value |= 0x02;
        break;
    case 0x4033: // Ext Read
        value = 0x80;
        break;
    }
    return value;
}

void DiskSystem::regWrite(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x4020:
        _irqReloadValue &= 0xff00;
        _irqReloadValue |= value;
        break;
    case 0x4021:
        _irqReloadValue &= 0x00ff;
        _irqReloadValue |= ((uint16_t)value << 8);
        break;
    case 0x4022:
        _irqReloadFlag = !!(value & 0x01);
        _irqEnabledTimer = !!(value & 0x02);
        _irqTimer = _irqReloadValue;
        if (!_irqEnabledTimer)
            _irq.unset(IRQ_FDS_TIMER);
        break;
    case 0x4023:
        if (!(value & 0x01))
            _irq.unset(IRQ_FDS_TIMER);
        break;
    case 0x4024: // Disk write
        _irq.unset(IRQ_FDS_TRANSFER);
        _transfered = 0;
        break;
    case 0x4025: // FDS Control
        _motor = !!(value & 0x01);
        _noScan = !!(value & 0x02);
        _inData = !!(value & 0x40);
        _irqEnabledTransfer = !!(value & 0x80);

        // TODO: Fix that once we have the bus
        //if (value & 0x08)
        //    ninMirrorV(state);
        //else
        //    ninMirrorH(state);
        break;
    case 0x4026: // External
        _extPort = value;
        break;
    }
}

void DiskSystem::tick()
{
    std::uint8_t tmp;
    int skip;

    if (_info.system() != NIN_SYSTEM_FDS)
        return;

    skip = 0;

    if (_irqEnabledTimer)
    {
        if (_irqTimer)
            _irqTimer--;
        else
        {
            _irq.set(IRQ_FDS_TIMER);
            if (_irqReloadFlag)
                _irqTimer = _irqReloadValue;
            else
                _irqEnabledTimer = 0;
        }
    }

    if (!_motor)
    {
        _scanning = 0;
        _endOfDisk = 1;
        return;
    }
    else if (_noScan && !_scanning)
    {
        return;
    }
    else if (_endOfDisk)
    {
        _endOfDisk = 0;
        _delay = 10000;
        _headPos = 0;
        _skippedGap = 0;
        return;
    }
    else if (_delay)
    {
        _delay--;
        return;
    }

    _scanning = 1;
    tmp = _dataCurrentDisk[_headPos++];

    if (!_inData)
    {
        _skippedGap = 0;
    }
    else if (tmp && !_skippedGap)
    {
        _skippedGap = 1;
        skip = 1;
    }

    if (_skippedGap && !skip)
    {
        _transfered = 1;
        _latchRead = tmp;
        if (_irqEnabledTransfer)
        {
            _irq.set(IRQ_FDS_TRANSFER);
        }
    }

    if (_headPos >= FDS_DISK_SIZE)
    {
        _motor = 0;
    }
    else
    {
        _delay = FDS_CLOCK_MAX;
    }
}

void DiskSystem::loadDisk(FILE* f)
{
    std::uint8_t header[16];

    std::fseek(f, 0, SEEK_SET);
    std::fread(header, 16, 1, f);

    _sideCount = header[4];
    delete[] _data;
    _data = new std::uint8_t[FDS_DISK_SIZE * _sideCount]();

    for (std::uint8_t i = 0; i < _sideCount; ++i)
    {
        loadDiskSide(f, i);
    }

    _dataCurrentDisk = _data;
}

void DiskSystem::changeSide(std::uint8_t side)
{
    side %= _sideCount;
    _dataCurrentDisk = _data + side * FDS_DISK_SIZE;
}

void DiskSystem::loadDiskSide(FILE* f, std::uint8_t side)
{
    std::uint8_t* dst;
    std::uint32_t head;
    std::uint16_t fileSize;
    std::uint8_t fileCount;

    dst = _data + FDS_DISK_SIZE * side;
    std::fseek(f, 16 + side * FDS_DISK_SIZE_ARCHIVE, SEEK_SET);

    /* First, large gap */
    head = FDS_DISK_GAP0;
    dst[head - 1] = 0x80;

    /* Load block 1 */
    std::fread(dst + head, 0x38, 1, f);
    head += 0x3a;
    head += FDS_DISK_GAP1;
    dst[head - 1] = 0x80;

    /* Load block 2 */
    std::fread(dst + head, 0x02, 1, f);
    fileCount = dst[head + 1];
    head += 0x04;

    for (int i = 0; i < fileCount; ++i)
    {
        head += FDS_DISK_GAP1;
        dst[head - 1] = 0x80;

        /* Block 3 */
        std::fread(dst + head, 0x10, 1, f);
        fileSize = *(std::uint16_t*)(dst + head + 13);
        head += 0x12;
        head += FDS_DISK_GAP1;
        dst[head - 1] = 0x80;

        /* Block 4 */
        std::fread(dst + head, fileSize + 1, 1, f);
        head += (fileSize + 3);
    }
}
