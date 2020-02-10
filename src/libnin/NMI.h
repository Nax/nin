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

#ifndef LIBNIN_NMI_H
#define LIBNIN_NMI_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

#define NMI_OCCURED 0x01
#define NMI_OUTPUT  0x02

namespace libnin
{

class NMI : private NonCopyable
{
public:
    NMI() : _status{}, _latch{} {}

    bool high() const { return _latch; }
    bool check(std::uint8_t flag) const { return !!(_status & flag); }

    void set(std::uint8_t flag)
    {
        std::uint8_t prev;

        prev = _status;
        _status |= flag;

        if ((prev != _status) && (_status == (NMI_OCCURED | NMI_OUTPUT)))
            _latch = true;
    }

    void unset(std::uint8_t flag)
    {
        _status &= ~flag;
    }

    void ack()
    {
        _latch = false;
    }

private:
    std::uint8_t    _status:2;
    bool            _latch:1;
};

};

#endif
