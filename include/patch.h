#pragma once

#include <stdint.h>

extern volatile uint32_t osTvType;
extern volatile uint32_t osRomType;
extern volatile uint32_t osRomBase;
extern volatile uint32_t osResetType;
extern volatile uint32_t osCicId;
extern volatile uint32_t osVersion;
extern volatile uint32_t osMemSize;
extern volatile uint8_t osAppNMIBuffer[64];

typedef struct {
    uint64_t crc;
    uint32_t entry;
} Patch_t;

#define DECLARE_PATCH(_crc, _entry) uint64_t crc [[gnu::section(".crc")]] = _crc; Patch_t patch [[gnu::section(".hdr")]] = { .crc = (_crc), .entry = (uint32_t)(_entry) };

#define RELOCATE(func, this) (((uint32_t)(func)) + ((uint32_t)(this)))
