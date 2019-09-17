#ifndef NIN_H
#define NIN_H

#include <nin/config.h>

#include <stdio.h>
#include <stdint.h>

#if defined(__cplusplus)
# define NIN_API extern "C"
#else
# define NIN_API
#endif

typedef enum {
    NIN_OK = 0,
    NIN_ERROR_IO,
    NIN_ERROR_BAD_FILE,
    NIN_ERROR_BAD_MAPPER,
    NIN_ERROR_UNKNOWN_MAPPER
} NinError;

typedef struct NinState_ NinState;
typedef void (*NINAUDIOCALLBACK)(void*, const int16_t*);

#define NIN_AUDIO_SAMPLE_SIZE   1024

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
    NIN_INFO_CLOCK_RATE,
    NIN_INFO_FRAME_CYCLES,
    NIN_INFO_FRAME_DELAY,
} NinInfo;

typedef int32_t NinInt32;

NIN_API NinError        ninCreateState(NinState** state, const char* path);
NIN_API NinError        ninLoadRom(NinState* state, const char* path);
NIN_API void            ninSetSaveFile(NinState* state, const char* path);
NIN_API void            ninSyncSave(NinState* state);
NIN_API void            ninDestroyState(NinState* state);
NIN_API int             ninRunCycles(NinState* state, size_t cycles, size_t* cyc);
NIN_API const uint32_t* ninGetScreenBuffer(NinState* state);
NIN_API void            ninSetInput(NinState* state, uint8_t input);
NIN_API void            ninAudioSetCallback(NinState* state, NINAUDIOCALLBACK callback, void* arg);
NIN_API void            ninDumpMemory(NinState* state, uint8_t* dst, uint16_t start, size_t len);

NIN_API void            ninInfoQueryInteger(NinState* state, NinInt32* dst, NinInfo info);

#endif
