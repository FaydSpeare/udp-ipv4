#include "ip.h"
#include <stdio.h>
#include <string.h>

uint16_t swap_bytes16(uint16_t value) { return (value << 8) | (value >> 8); }

uint32_t swap_bytes32(uint32_t value) {
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) |
           (value >> 24);
}

uint16_t identity16(uint16_t value) { return value; }

uint32_t identity32(uint32_t value) { return value; }

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define MAYBE_SWAP16 swap_bytes16
#define MAYBE_SWAP32 swap_bytes32

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define MAYBE_SWAP16 identity16
#define MAYBE_SWAP32 identity32

#else
#error "Unknown byte order"
#endif

void _print_addr(uint32_t addr) {
    printf(
        "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
        (addr >> 8) & 0xFF, addr & 0xFF
    );
}

void ip_print_header(ip_header* header) {
    printf("IP Header\n");
    printf("  Version: %d\n", header->version);
    printf("  IHL: %d\n", header->ihl);
    printf("  TOS: %d\n", header->tos);
    printf("  Total Length: %d\n", header->total_length);
    printf("  ID: %d\n", header->id);
    printf("  Fragment Offset: %d\n", header->fragment_offset);
    printf("  TTL: %d\n", header->ttl);
    printf("  Protocol: %d\n", header->protocol);
    printf("  Checksum: %d\n", header->checksum);
    printf("  Source Address: ");
    _print_addr(header->src_address);
    printf("\n");
    printf("  Destination Address: ");
    _print_addr(header->dst_address);
    printf("\n");
}

char* ip_extract_header(ip_header* header, char* buffer, size_t buffer_size) {
    if (buffer_size < 20) {
        return NULL;
    }
    header->version = (buffer[0] & 0xF0) >> 4;
    header->ihl = buffer[0] & 0x0F;
    header->tos = buffer[1];
    header->total_length = MAYBE_SWAP16(*(uint16_t*)(buffer + 2));
    header->id = MAYBE_SWAP16(*(uint16_t*)(buffer + 4));
    header->fragment_offset = MAYBE_SWAP16(*(uint16_t*)(buffer + 6));
    header->ttl = buffer[8];
    header->protocol = buffer[9];
    header->checksum = MAYBE_SWAP16(*(uint16_t*)(buffer + 10));
    header->src_address = MAYBE_SWAP32(*(uint32_t*)(buffer + 12));
    header->dst_address = MAYBE_SWAP32(*(uint32_t*)(buffer + 16));
    return buffer + 20;
}

char* ip_write_header(ip_header* header, char* buffer, size_t buffer_size) {
    if (buffer_size < 20) {
        return NULL;
    }
    buffer[0] = (header->version << 4) | header->ihl;
    buffer[1] = header->tos;

    uint16_t tmp;
    tmp = MAYBE_SWAP16(header->total_length);
    memcpy(buffer + 2, &tmp, 2);

    tmp = MAYBE_SWAP16(header->id);
    memcpy(buffer + 4, &tmp, 2);

    tmp = MAYBE_SWAP16(header->fragment_offset);
    memcpy(buffer + 6, &tmp, 2);

    buffer[8] = header->ttl;
    buffer[9] = header->protocol;

    tmp = MAYBE_SWAP16(header->checksum);
    memcpy(buffer + 10, &tmp, 2);

    uint32_t tmp32;
    tmp32 = MAYBE_SWAP32(header->src_address);
    memcpy(buffer + 12, &tmp32, 4);

    tmp32 = MAYBE_SWAP32(header->dst_address);
    memcpy(buffer + 16, &tmp32, 4);
    return buffer + 20;
}

bool ip_verify_checksum(ip_header* header) {
    return ip_calculate_checksum(header) == header->checksum;
}

// The checksum is calculated by summing the 16-bit values of the header
uint16_t ip_calculate_checksum(ip_header* header) {
    uint32_t sum = 0;

    // We want to keep these fields in their original network order.
    // Since none of these fields are larger than a byte, they are not
    // affected by endianness for the purpose of the checksum calculation.
    sum += (header->version << 12) | (header->ihl << 8) | (header->tos << 0);

    // These fields are affected by endianness as they are 2 byte values,
    // however, they are stored in the struct with host byte order, so we
    // can simply add them as they are.
    sum += header->total_length;
    sum += header->id;
    sum += header->fragment_offset;

    // We want to keep these fields in their original network order.
    // Since none of these fields are larger than a byte, they are not
    // affected by endianness for the purpose of the checksum calculation.
    sum += (header->ttl << 8) | header->protocol;

    // Network byte order: A B C D
    // If the host is little-endian we have: D C B A
    // We need to add the 16 bits values given by A B and C D (big-endian)
    // If the host is little-endian, these values are B A and D C.
    // Thus we can simply add the first 2 bytes and the second 2 bytes
    // regardless of endianness.
    for (int i = 0; i < 2; i++) {
        sum += header->src_address >> (16 * i) & 0xFFFF;
    }
    for (int i = 0; i < 2; i++) {
        sum += header->dst_address >> (16 * i) & 0xFFFF;
    }

    // Perform the 1's complement sum we need to wrap overflow
    if (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum;
}
