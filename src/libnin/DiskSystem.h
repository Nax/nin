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

#ifndef LIBNIN_DISK_SYSTEM_H
#define LIBNIN_DISK_SYSTEM_H 1

#include <cstdio>
#include <cstdint>
#include <libnin/NonCopyable.h>

class HardwareInfo;
class IRQ;
class DiskSystem : private NonCopyable
{
public:
    DiskSystem(const HardwareInfo& info, IRQ& irq);
    ~DiskSystem();

    std::uint8_t    regRead(std::uint16_t addr);
    void            regWrite(std::uint16_t addr, std::uint8_t value);

    void tick();

    void loadDisk(FILE* f);
    void changeSide(std::uint8_t side);

private:
    void loadDiskSide(FILE* f, std::uint8_t side);

    const HardwareInfo& _info;
    IRQ&                _irq;

    std::uint8_t*   _data;
    std::uint8_t*   _dataCurrentDisk;

    std::uint32_t   _headPos;
    std::uint16_t   _headClock;
    std::uint16_t   _delay;
    std::uint16_t   _irqReloadValue;
    std::uint16_t   _irqTimer;
    std::uint8_t    _extPort;
    std::uint8_t    _latchRead;
    std::uint8_t    _sideCount;

    std::uint8_t    _motor:1;
    std::uint8_t    _noScan:1;
    std::uint8_t    _noWrite:1;
    std::uint8_t    _inData:1;
    std::uint8_t    _irqEnabledTransfer:1;
    std::uint8_t    _irqEnabledTimer:1;
    std::uint8_t    _irqReloadFlag:1;
    std::uint8_t    _transfered:1;
    std::uint8_t    _scanning:1;
    std::uint8_t    _skippedGap:1;
    std::uint8_t    _endOfDisk:1;
};

#endif
