#include "udp.h"
#include "byteorder.h"
#include <string.h>
#include <stdio.h>

void udp_print_header(udp_header* header) {
    printf("UDP Header\n");
    printf("  Source Port: %hu\n", MAYBE_SWAP16(header->src_port));
    printf("  Destination Port: %hu\n", MAYBE_SWAP16(header->dst_port));
    printf("  Length: %hu\n", MAYBE_SWAP16(header->length));
    printf("  Checksum: %hu\n", MAYBE_SWAP16(header->checksum));
}

char* udp_extract_header(udp_header* header, char* buffer, size_t buffer_size) {
    if (buffer_size < 8) {
        return NULL;
    }
    memcpy(header, buffer, 8);
    return buffer + 8;
}

char* udp_write_header(udp_header* header, char* buffer, size_t buffer_size) {
    if (buffer_size < 8) {
        return NULL;
    }
    memcpy(buffer, header, 8);
    return buffer + 8;
}

uint16_t udp_calculate_checksum(ip_header *iphdr, udp_header *udphdr, char *data) {
    uint32_t sum = 0;

    // UDP header
    sum += MAYBE_SWAP16(udphdr->src_port);
    sum += MAYBE_SWAP16(udphdr->dst_port);
    sum += MAYBE_SWAP16(udphdr->length);

    // IP Pseudo header
    sum += MAYBE_SWAP16(iphdr->src_address & 0xFFFF);
    sum += MAYBE_SWAP16(iphdr->src_address >> 16);
    sum += MAYBE_SWAP16(iphdr->dst_address & 0xFFFF);
    sum += MAYBE_SWAP16(iphdr->dst_address >> 16);
    sum += 17; // Protocol
    sum += MAYBE_SWAP16(udphdr->length);

    // Data
    uint16_t data_length = MAYBE_SWAP16(udphdr->length) - 8;
    uint16_t sections = data_length / 2;
    for (int i = 0; i < sections; i++) {
        sum += MAYBE_SWAP16(((uint16_t*)data)[i]);
    }

    // If the data length is odd, we need to pad the last section with 0s
    if (data_length % 2 == 1) {
        sum += data[data_length - 1] << 8;
    }
    
    if (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum; 
}
