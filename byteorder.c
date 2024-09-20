#include "byteorder.h"

uint16_t swap_bytes16(uint16_t value) { return (value << 8) | (value >> 8); }

uint32_t swap_bytes32(uint32_t value) {
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) |
           (value >> 24);
}

uint16_t identity16(uint16_t value) { return value; }
uint32_t identity32(uint32_t value) { return value; }
