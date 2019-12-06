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
#include <libnin/libnin.h>

#define FDS             (state->fds)
#define FDS_CLOCK_MAX   (149)

NIN_API uint8_t ninFdsRegRead(NinState* state, uint16_t addr)
{
    uint8_t value;

    value = 0;
    switch (addr)
    {
    case 0x4030: // Disk Status
        value = 0x80;
        if (state->irqFlags & IRQ_FDS_TIMER) value |= 0x01;
        if (FDS.transfered) value |= 0x02;
        FDS.transfered = 0;
        if (FDS.endOfDisk)
            value |= 0x40;
        ninClearIRQ(state, IRQ_FDS_TRANSFER);
        ninClearIRQ(state, IRQ_FDS_TIMER);
        break;
    case 0x4031: // Disk Read
        value = FDS.latchRead;
        FDS.transfered = 0;
        ninClearIRQ(state, IRQ_FDS_TRANSFER);
        break;
    case 0x4032: // Disk Drive Status
        value = 0x40;
        if (!FDS.scanning)
            value |= 0x02;
        break;
    case 0x4033: // Ext Read
        value = 0x80;
        break;
    }
    return value;
}

NIN_API void ninFdsRegWrite(NinState* state, uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case 0x4020:
        FDS.irqReloadValue &= 0xff00;
        FDS.irqReloadValue |= value;
        break;
    case 0x4021:
        FDS.irqReloadValue &= 0x00ff;
        FDS.irqReloadValue |= ((uint16_t)value << 8);
        break;
    case 0x4022:
        FDS.irqReloadFlag = !!(value & 0x01);
        FDS.irqEnabledTimer = !!(value & 0x02);
        FDS.irqTimer = FDS.irqReloadValue;
        if (!FDS.irqEnabledTimer)
            ninClearIRQ(state, IRQ_FDS_TIMER);
        break;
    case 0x4023:
        if (!(value & 0x01))
            ninClearIRQ(state, IRQ_FDS_TIMER);
        break;
    case 0x4024: // Disk write
        ninClearIRQ(state, IRQ_FDS_TRANSFER);
        FDS.transfered = 0;
        break;
    case 0x4025: // FDS Control
        FDS.motor = !!(value & 0x01);
        FDS.noScan = !!(value & 0x02);
        FDS.inData = !!(value & 0x40);
        FDS.irqEnabledTransfer = !!(value & 0x80);

        if (value & 0x08)
            ninMirrorV(state);
        else
            ninMirrorH(state);
        break;
    case 0x4026: // External
        FDS.extPort = value;
        break;
    }
}

NIN_API void ninFdsCycle(NinState* state)
{
    uint8_t tmp;
    int skip;

    if (state->system != NIN_SYSTEM_FDS)
        return;

    skip = 0;

    if (FDS.irqEnabledTimer)
    {
        if (FDS.irqTimer)
            FDS.irqTimer--;
        else
        {
            ninSetIRQ(state, IRQ_FDS_TIMER);
            if (FDS.irqReloadFlag)
                FDS.irqTimer = FDS.irqReloadValue;
            else
                FDS.irqEnabledTimer = 0;
        }
    }

    if (!FDS.motor)
    {
        FDS.scanning = 0;
        FDS.endOfDisk = 1;
        return;
    }
    else if (FDS.noScan && !FDS.scanning)
    {
        return;
    }
    else if (FDS.endOfDisk)
    {
        FDS.endOfDisk = 0;
        FDS.delay = 10000;
        FDS.headPos = 0;
        FDS.skippedGap = 0;
        return;
    }
    else if (FDS.delay)
    {
        FDS.delay--;
        return;
    }

    FDS.scanning = 1;
    tmp = state->diskData[FDS.headPos++];

    if (!FDS.inData)
    {
        FDS.skippedGap = 0;
    }
    else if (tmp && !FDS.skippedGap)
    {
        FDS.skippedGap = 1;
        skip = 1;
    }

    if (FDS.skippedGap && !skip)
    {
        FDS.transfered = 1;
        FDS.latchRead = tmp;
        if (FDS.irqEnabledTransfer)
        {
            ninSetIRQ(state, IRQ_FDS_TRANSFER);
        }
    }

    if (FDS.headPos >= 0x14000)
    {
        FDS.motor = 0;
    }
    else
    {
        FDS.delay = FDS_CLOCK_MAX;
    }
}
