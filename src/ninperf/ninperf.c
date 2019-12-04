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
#include <stdlib.h>
#include <time.h>
#include <nin/nin.h>

#if defined(WIN32) || defined(_WIN32)
# define LF "\r\n"
#else
# define LF "\n"
#endif

int main(int argc, char** argv)
{
    NinState* state;
    clock_t lastFrame;
    clock_t now;
    size_t cyc;

    if (argc != 2)
        return 1;

    ninCreateState(&state, argv[1]);
    if (!state)
        return 1;

    srand((unsigned)time(NULL));

    cyc = 0;
    lastFrame = clock();
    for (;;)
    {
        ninSetInput(state, (rand() & 0xff));
        ninRunCycles(state, 5000, NULL);
        cyc += 5000;
        now = clock();
        if (now - lastFrame > CLOCKS_PER_SEC)
        {
            printf("%llu cycles/s" LF, (unsigned long long)cyc);
            fflush(stdout);
            cyc = 0;
            lastFrame = now;
        }
    }

    return 0;
}
