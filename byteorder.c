#include "byteorder.h"

uint16_t swap_bytes16(uint16_t value) { return (value << 8) | (value >> 8); }

uint32_t swap_bytes32(uint32_t value) {
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) |
           (value >> 24);
}

uint16_t identity16(uint16_t value) { return value; }
uint32_t identity32(uint32_t value) { return value; }

void print_endianess(void) {
    printf("host endianness: %d\n", __BYTE_ORDER__);
    printf("little endian: %d\n", __ORDER_LITTLE_ENDIAN__);
    printf("big endian: %d\n", __ORDER_BIG_ENDIAN__);
}
