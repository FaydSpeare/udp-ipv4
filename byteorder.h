#include <stdint.h>

uint16_t swap_bytes16(uint16_t value);
uint32_t swap_bytes32(uint32_t value);
uint16_t identity16(uint16_t value);
uint32_t identity32(uint32_t value);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define MAYBE_SWAP16 swap_bytes16
#define MAYBE_SWAP32 swap_bytes32
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define MAYBE_SWAP16 identity16
#define MAYBE_SWAP32 identity32
#else
#error "Unknown byte order"
#endif
