#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    unsigned int version : 4;
    unsigned int ihl : 4;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int ihl : 4;
    unsigned int version : 4;
#else
    #error "Endianness not defined!"
#endif
    uint8_t tos; // Type of service
    uint16_t total_length;
    uint16_t id;
    uint16_t fragment_offset; // TODO: flags
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_address;
    uint32_t dst_address;
} ip_header;

void ip_print_header(ip_header *header);
char* ip_extract_header(ip_header *header, char *buffer, size_t buffer_size);
char* ip_write_header(ip_header *header, char *buffer, size_t buffer_size);
bool ip_verify_checksum(ip_header *header);
uint16_t ip_calculate_checksum(ip_header* header);
