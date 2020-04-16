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

#ifndef LIBNIN_STATE_H
#define LIBNIN_STATE_H 1

#include <cstdlib>
#include <memory>
#include <nin/nin.h>
#include <libnin/APU.h>
#include <libnin/Audio.h>
#include <libnin/BusMain.h>
#include <libnin/BusVideo.h>
#include <libnin/Cart.h>
#include <libnin/CPU.h>
#include <libnin/DiskSystem.h>
#include <libnin/HardwareInfo.h>
#include <libnin/Input.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/NMI.h>
#include <libnin/NonCopyable.h>
#include <libnin/PPU.h>
#include <libnin/Save.h>
#include <libnin/Util.h>
#include <libnin/Video.h>

#define RAM_SIZE    0x800
#define VRAM_SIZE   0x800

#define BITMAP_X    256
#define BITMAP_Y    240

#define DEBUG_LEVEL 0

#define MIRRORING_HORIZONTAL    0
#define MIRRORING_VERTICAL      1
#define MIRRORING_QUAD          2

#define CLOCK_RATE_NTSC     1789773

namespace libnin
{

class State : private NonCopyable
{
public:
    static State* create(NinError& err, const char* path);

    std::unique_ptr<Memory>         memory;
    std::unique_ptr<HardwareInfo>   info;
    std::unique_ptr<Cart>           cart;
    std::unique_ptr<Save>           save;
    std::unique_ptr<Input>          input;
    std::unique_ptr<IRQ>            irq;
    std::unique_ptr<NMI>            nmi;
    std::unique_ptr<Video>          video;
    std::unique_ptr<Mapper>         mapper;
    std::unique_ptr<BusVideo>       busVideo;
    std::unique_ptr<Audio>          audio;
    std::unique_ptr<APU>            apu;
    std::unique_ptr<DiskSystem>     diskSystem;
    std::unique_ptr<PPU>            ppu;
    std::unique_ptr<BusMain>        busMain;
    std::unique_ptr<CPU>            cpu;

private:
    State() {}
};

}

/* Expose the state to the world */
struct NinState : public libnin::State {};

#endif
