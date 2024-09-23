#include "udp.h"
#include "byteorder.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

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

uint16_t
udp_calculate_checksum(ip_header* iphdr, udp_header* udphdr, char* data) {
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

uint32_t get_local_ipv4_address() {
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return NULL;
    }
    for (struct ifaddrs* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        struct sockaddr* addr = ifa->ifa_addr;
        struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
        if (addr_in->sin_family == AF_INET &&
            !(ifa->ifa_flags & IFF_LOOPBACK)) {
            return addr_in->sin_addr.s_addr;
        }
    }
    return NULL;
}

char* udp_send(
    uint32_t dst_address, uint16_t dst_port, char* message, size_t message_size
) {
    udp_header udph = {
        .src_port = MAYBE_SWAP16(rand() % 65536),
        .dst_port = MAYBE_SWAP16(dst_port),
        .length = MAYBE_SWAP16(8 + message_size),
        .checksum = 0
    };
    ip_header iph = {
        .version = 4,
        .ihl = 5,
        .tos = 0,
        .total_length = MAYBE_SWAP16(20 + 8 + message_size),
        .id = 0,
        .fragment_offset = 0,
        .ttl = 64,
        .protocol = 17,
        .checksum = 0,
        .src_address = get_local_ipv4_address(),
        .dst_address = MAYBE_SWAP32(dst_address)
    };
    iph.checksum = MAYBE_SWAP16(ip_calculate_checksum(&iph));
    udph.checksum = MAYBE_SWAP16(udp_calculate_checksum(&iph, &udph, message));

    char* segment = malloc(20 + 8 + message_size);
    ip_write_header(&iph, segment, 100);
    udp_write_header(&udph, segment + 20, 100);
    memcpy(segment + 28, message, message_size);
    return segment;
}
