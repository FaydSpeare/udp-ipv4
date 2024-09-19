
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} udp_header;

void udp_print_header(udp_header* header);
char* udp_extract_header(udp_header* header, char* buffer, size_t buffer_size);
char* udp_write_header(udp_header* header, char* buffer, size_t buffer_size);
bool udp_verify_checksum(udp_header* header);
uint16_t udp_calculate_checksum(udp_header* header);
