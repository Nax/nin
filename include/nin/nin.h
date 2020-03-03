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

#ifndef NIN_H
#define NIN_H

#include <nin/config.h>

#include <stdio.h>
#include <stdint.h>

#if defined(__cplusplus)
# define NIN_EXTERN extern "C"
#else
# define NIN_EXTERN
#endif

#if defined(WIN32) || defined(_WIN32)
# if defined(NIN_STATIC)
#  define NIN_DECLSPEC
# elif defined(NIN_DLL)
#  define NIN_DECLSPEC __declspec(dllexport)
# else
#  define NIN_DECLSPEC __declspec(dllimport)
# endif
#else
# if defined(NIN_STATIC)
#  define NIN_DECLSPEC
# else
#  define NIN_DECLSPEC __attribute__ ((visibility("default")))
# endif
#endif

#define NIN_API NIN_EXTERN NIN_DECLSPEC

typedef enum {
    NIN_OK = 0,
    NIN_ERROR_IO,
    NIN_ERROR_BAD_FILE,
    NIN_ERROR_BAD_MAPPER,
    NIN_ERROR_UNKNOWN_MAPPER
} NinError;

typedef struct NinState NinState;
typedef void (*NINAUDIOCALLBACK)(void*, const float*);

#define NIN_AUDIO_SAMPLE_SIZE   1024
#define NIN_FRAME_SIZE          (256 * 240 * 4)

#define NIN_BUTTON_A        0x01
#define NIN_BUTTON_B        0x02
#define NIN_BUTTON_SELECT   0x04
#define NIN_BUTTON_START    0x08
#define NIN_BUTTON_UP       0x10
#define NIN_BUTTON_DOWN     0x20
#define NIN_BUTTON_LEFT     0x40
#define NIN_BUTTON_RIGHT    0x80

typedef enum {
    NIN_REGION_NTSC     = 0,
    NIN_REGION_PAL      = 1,
    NIN_REGION_WORLD    = 2,
    NIN_REGION_DENDY    = 3
} NinRegion;

typedef enum {
    NIN_SYSTEM_NES      = 0,
    NIN_SYSTEM_FDS      = 1
} NinSystem;

typedef enum {
    NIN_INFO_SYSTEM,
    NIN_INFO_CLOCK_RATE,
    NIN_INFO_FRAME_CYCLES,
    NIN_INFO_FRAME_DELAY,
    NIN_INFO_PC,
    NIN_INFO_REG_A,
    NIN_INFO_REG_X,
    NIN_INFO_REG_Y,
    NIN_INFO_REG_S
} NinInfo;

typedef int32_t NinInt32;

NIN_API NinError        ninCreateState(NinState** state, const char* path);
NIN_API void            ninSetSaveFile(NinState* state, const char* path);
NIN_API void            ninSyncSave(NinState* state);
NIN_API void            ninDestroyState(NinState* state);
NIN_API int             ninRunCycles(NinState* state, size_t cycles, size_t* cyc);
NIN_API const uint32_t* ninGetScreenBuffer(NinState* state);
NIN_API void            ninSetInput(NinState* state, uint8_t input);
NIN_API void            ninAudioSetFrequency(NinState* state, uint32_t frequency);
NIN_API void            ninAudioSetCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg);
NIN_API void            ninLoadBiosFDS(NinState* state, const char* path);

NIN_API void            ninInfoQueryInteger(NinState* state, NinInt32* dst, NinInfo info);

NIN_API int             ninStepInstruction(NinState* state);

/* Dump */
NIN_API void            ninDumpMemory(NinState* state, uint8_t* dst, uint16_t start, size_t len);
NIN_API void            ninDumpNametable(NinState* state, uint8_t* dst, int nametable);

#endif
