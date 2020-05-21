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

#include <libnin/Cart.h>
#include <libnin/Disk.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

using namespace libnin;

template <>
void Mapper::handleReset<MapperID::FDS>()
{
    _diskSystem = MapperDiskSystem{};
    bankPrg32k(1, CART_PRG_RAM, 0);
    bankPrg8k(5, CART_PRG_ROM, 0);
}

template <>
std::uint8_t Mapper::handleRead<MapperID::FDS>(std::uint16_t addr)
{
    std::uint8_t value{};

    switch (addr)
    {
    case 0x4030: // Disk Status
        value = 0x80;
        if (_irq.check(IRQ_MAPPER1)) value |= 0x01;
        if (_diskSystem.transfered)
            value |= 0x02;
        _diskSystem.transfered = 0;
        if (_diskSystem.endOfDisk)
            value |= 0x40;
        _irq.unset(IRQ_MAPPER1 | IRQ_MAPPER2);
        break;
    case 0x4031: // Disk Read
        value = _diskSystem.latchRead;
        _diskSystem.transfered = 0;
        _irq.unset(IRQ_MAPPER2);
        break;
    case 0x4032: // Disk Drive Status
        value = 0x40;
        if (_disk.inserted())
            value |= 0x00;
        else
            value |= 0x07;
        if (!_diskSystem.scanning)
            value |= 0x02;
        break;
    case 0x4033: // Ext Read
        value = 0x80;
        break;
    }
    return value;
}

template <>
void Mapper::handleWrite<MapperID::FDS>(std::uint16_t addr, std::uint8_t value)
{
    switch (addr)
    {
    case 0x4020:
        _diskSystem.irqReloadValue &= 0xff00;
        _diskSystem.irqReloadValue |= value;
        break;
    case 0x4021:
        _diskSystem.irqReloadValue &= 0x00ff;
        _diskSystem.irqReloadValue |= ((uint16_t)value << 8);
        break;
    case 0x4022:
        _diskSystem.irqReloadFlag = !!(value & 0x01);
        _diskSystem.irqEnabledTimer = !!(value & 0x02);
        _diskSystem.irqTimer = _diskSystem.irqReloadValue;
        if (!_diskSystem.irqEnabledTimer)
            _irq.unset(IRQ_MAPPER1);
        break;
    case 0x4023:
        if (!(value & 0x01))
            _irq.unset(IRQ_MAPPER1);
        break;
    case 0x4024: // Disk write
        _irq.unset(IRQ_MAPPER2);
        _diskSystem.transfered = 0;
        _diskSystem.latchWrite = value;
        break;
    case 0x4025: // FDS Control
        _diskSystem.motor = !!(value & 0x01);
        _diskSystem.noScan = !!(value & 0x02);
        _diskSystem.write = !(value & 0x04);
        _diskSystem.inData = !!(value & 0x40);
        _diskSystem.irqEnabledTransfer = !!(value & 0x80);
        mirror((value & 0x08) ? NIN_MIRROR_V : NIN_MIRROR_H);
        break;
    case 0x4026: // External
        _diskSystem.extPort = value;
        break;
    }
}

#if 0

Register bits :
$4025 bit 0 : DontStopMotor
$4025 bit 1 : DontScanMedia
$4025 bit 2 : DontWrite
$4025 bit 4 : TransmitCRC
$4025 bit 6 : InData
$4025 bit 7 : DiskIRQ_enable
$4030 bit 0 : TIMER_IRQ
$4030 bit 1 : DataReady
$4030 bit 4 : CRC[0..15] != 0x0000
$4030 bit 6 : EndOfDisk
$4032 bit 0 : DiskNotInserted
$4032 bit 1 : DiskNotInserted || !Scanning
$4032 bit 2 : DiskNotInserted
$4031 8 bits : ReadData
$4024 8 bits : WriteData
Writing or reading any of these registers does no immediate action aside from reading / storing the relevant bits in memory, and possibly reprogramming nametable mirroringand /or acknowledging an IRQ.

Internal bits :
.....bit . : DiskNotInserted(0 if disk is inserted, 1 otherwise)
.....bit . : Scanning(used internally, indicates whether the motor has awakened)
.....bit . : GapCovered(used internally, indicates whether the end of gap was found)
.....bit . : PreviousCRCflag(used internally, stores last value of TransmitCRC)
.....bit . : DoIRQ(used internally as temporary)
..... 8 bits : ShiftRegister(used internally to store current disk - transfer byte)
..... 16 bits : CRCaccumulator(used internally for CRC calculation)
.....integer : DiskPosition(indicates current byte - position in the raw disk data)
.....integer : DELAY(indicates amount of remaining wait in CPU cycles)

FDS algorithm(invoked on every CPU cycle) :
    --Do nothing if motor is stopped, or if disk is not inserted.
    IF DontStopMotor = 0 OR DiskNotInserted = 1,
    Scanning : = 0
    EndOfDisk : = 1
    END CYCLE
    -- At this point, DontStopMotor = 1, and DiskNotInserted = 0.
    --Do nothing if scanning is not started and has not been started.
    IF DontScanMedia = 1 AND Scanning = 0,
    END CYCLE
    -- At this point, DontStopMotor = 1, and DiskNotInserted = 0, and (DontScanMedia = 0 OR Scanning = 1).
    --So motorand scanning are permitted.Are they not started yet ?
    IF EndOfDisk = 1,
    --Start with delay.
    DELAY : = 50000   (arbitrary number)
    EndOfDisk : = 0
    --Set internal variables for starting from the beginning of disk.
    DiskPosition : = 0
    GapCovered : = 0
    --Elapse any pending delay(read - delay, motor spin - up delay)
    IF DELAY > 0,
    DECREMENT DELAY
    END CYCLE

    -- Motor is now running and disk is being scanned.Set the flag thus indicating,
    --so that we don't get knocked back into halt, when the game sets DontScanMedia=1.
    --Also this reports in $4032 bit 1.
    --TODO: Figure out what happens on real hardware, if you set DontScanMedia = 1
    --a few cycles after setting DontScanMedia = 0, but before $4032 bit 1 indicates Scanning = 1.
    --In this emulator, such action resets the disk drive without starting scanning,
    --but there is a possibility that the real hardware would ignore the flag setting.
    Scanning : = 1

    --At this point, DontStopMotor = 1, and DiskNotInserted = 0, and Scanning = 1, and DontScanMedia = irrelevant.
    --READING ?
    IF DontWrite = 1,
    --Begin read - cycle :
    READ DISK-- > ShiftReg
    IF PreviousCRCflag = 0,
    UPDATE CRCaccumulator WITH ShiftReg
    -- End read - cycle:
DoIRQ: = DiskIRQ_enable
-- If InData = 0, we are to think we are in a gap.
IF InData = 0,
GapCovered : = 0
CRCaccumulator : = 0
ELSE, IF ShiftReg != 0x00,
--InData = 1. If we found a nonzero byte, we're no longer in the gap.
GapCovered : = 1
DoIRQ : = 0  --Don't signal IRQ for data-begin byte
--If we are not in gap, signal the read data.
IF GapCovered = 1,
DataReady : = 1
ReadData : = ShiftReg
If DoIRQ, SIGNAL IRQ

-- WRITING ?
IF DontWrite = 0,
IF TransmitCRC = 0,
--If we are not writing CRC, signal the readiness for data.
--TODO : Figure out whether DataReady should also be signalled
--       when InData = 0. In this emulator, it will.
--The FDS BIOS never polls $4030 when InData = 0, nor
--       never has DiskIRQ_enable = 1 when InData = 0, but a game
--       may do direct disk access differently.
DataReady : = 1
ShiftReg : = WriteData
DoIRQ : = DiskIRQ_enable
If DoIRQ, SIGNAL IRQ

-- ShiftReg now contains the value of $4025 that was
-- given to us in the beginning of the write cycle.
IF InData = 0,
--If InData = 0, we are to produce a gap.
ShiftReg       : = 0x00
IF TransmitCRC = 0,
UPDATE CRCaccumulator WITH ShiftReg
-- TODO : Figure out what happens on real hardware if you have
--       InData = 0 and TransmitCRC = 1 simultaneously.
--Will the CRC(first two bytes thereof anyway) be written ?
--In this emulator, they will.
ELSE,
IF PreviousCRCflag = 0,
--If CRC was RAISED, finish the CRC calculation with TWO zero - bytes.
UPDATE CRCaccumulator WITH 0x00
UPDATE CRCaccumulator WITH 0x00
ShiftReg       : = CRCaccumulator low 8 bits
DOWNSHIFT CRCaccumulator by 8 bits
WRITE DISK < --ShiftReg
    -- These flags may be useless at the moment :
GapCovered: = 0

--After processing this byte, go to next byte.
PreviousCRCflag : = TransmitCRC
INCREMENT DiskPosition
IF DiskPosition >= DISK_SIZE,
--End of disk reached.Stop motor.
--I don't know if this is correct, or if the motor should rewind automatically, or something.
--In any case, this will also set the EndOfDisk flag at next cycle.
DontStopMotor: = 0
    ELSE,
    DELAY : = 149
    END CYCLE

#endif

template<>
void Mapper::handleTick<MapperID::FDS>()
{
    std::uint8_t tmp;
    int skip{};

    _disk.tick();
    if (_diskSystem.irqEnabledTimer)
    {
        if (_diskSystem.irqTimer)
            _diskSystem.irqTimer--;
        else
        {
            _irq.set(IRQ_MAPPER1);
            if (_diskSystem.irqReloadFlag)
                _diskSystem.irqTimer = _diskSystem.irqReloadValue;
            else
                _diskSystem.irqEnabledTimer = 0;
        }
    }

    if (!_diskSystem.motor)
    {
        _diskSystem.scanning = 0;
        _diskSystem.endOfDisk = 1;
        return;
    }
    else if (_diskSystem.noScan && !_diskSystem.scanning)
    {
        return;
    }
    else if (_diskSystem.endOfDisk)
    {
        _diskSystem.endOfDisk = 0;
        _diskSystem.delay = 10000;
        _diskSystem.headPos = 0;
        _diskSystem.skippedGap = 0;
        return;
    }
    else if (_diskSystem.delay)
    {
        _diskSystem.delay--;
        return;
    }

    _diskSystem.scanning = 1;

    if (!_diskSystem.write)
    {
        /* Read */
        tmp = _disk.read(_diskSystem.headPos);

        if (!_diskSystem.inData)
        {
            _diskSystem.skippedGap = 0;
        }
        else if (tmp && !_diskSystem.skippedGap)
        {
            _diskSystem.skippedGap = 1;
            skip = 1;
        }

        if (_diskSystem.skippedGap && !skip)
        {
            _diskSystem.transfered = 1;
            _diskSystem.latchRead = tmp;
            if (_diskSystem.irqEnabledTransfer)
            {
                _irq.set(IRQ_MAPPER2);
            }
        }
    }
    else
    {
        /* Write */
        _diskSystem.transfered = 1;
        if (!_diskSystem.inData)
            tmp = 0x00;
        else
            tmp = _diskSystem.latchWrite;
        _disk.write(_diskSystem.headPos, tmp);
        _diskSystem.skippedGap = 1;
        if (_diskSystem.irqEnabledTransfer)
        {
            _irq.set(IRQ_MAPPER2);
        }
    }

    _diskSystem.headPos++;
    if (_diskSystem.headPos >= Disk::DiskSize)
    {
        _diskSystem.motor = 0;
    }
    else
    {
        _diskSystem.delay = 149;
    }
}

template <>
void Mapper::init<MapperID::FDS>()
{
    _handleReset = &Mapper::handleReset<MapperID::FDS>;
    _handleRead = &Mapper::handleRead<MapperID::FDS>;
    _handleWrite = &Mapper::handleWrite<MapperID::FDS>;
    _handleTick = &Mapper::handleTick<MapperID::FDS>;
}
