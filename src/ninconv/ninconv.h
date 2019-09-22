#ifndef NINCONV_H
#define NINCONV_H 1

#include <stdio.h>
#include <stdint.h>

typedef struct {
    const char* name;
    uint32_t    crc32;
    uint8_t     region:1;
    uint16_t    mapper;
    uint8_t     mirror:1;
    uint8_t     battery:1;
    uint32_t    prgRomSize;
    uint32_t    prgRamSize;
    uint32_t    chrRomSize;
    uint32_t    chrRamSize;
} NinDbGame;

uint32_t crc32(FILE* f);

#endif
