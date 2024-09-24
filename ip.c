#include "ip.h"
#include "byteorder.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>

uint32_t get_local_ipv4_address(void) {
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return 0;
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
    return 0;
}

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
    printf("  Total Length: %d\n", MAYBE_SWAP16(header->total_length));
    printf("  ID: %d\n", MAYBE_SWAP16(header->id));
    printf("  Fragment Offset: %d\n", MAYBE_SWAP16(header->fragment_offset));
    printf("  TTL: %d\n", header->ttl);
    printf("  Protocol: %d\n", header->protocol);
    printf("  Checksum: %d\n", MAYBE_SWAP16(header->checksum));
    printf("  Source Address: ");
    _print_addr(MAYBE_SWAP32(header->src_address));
    printf("\n");
    printf("  Destination Address: ");
    _print_addr(MAYBE_SWAP32(header->dst_address));
    printf("\n");
}

char* ip_extract_header(ip_header* header, char* buffer, size_t buffer_size) {
    if (buffer_size < 20) {
        return NULL;
    }
    memcpy(header, buffer, 20);
    return buffer + 20;
}

char* ip_write_header(ip_header* header, char* buffer, size_t buffer_size) {
    if (buffer_size < 20) {
        return NULL;
    }
    memcpy(buffer, header, 20);
    return buffer + 20;
}

bool ip_verify_checksum(ip_header* header) {
    return ip_calculate_checksum(header) == header->checksum;
}

uint16_t ip_calculate_checksum(ip_header* header) {
    uint32_t sum = 0;
    for (int i = 0; i < 10; i++) {
        // Exclude checksum
        if (i == 5) {
            continue;
        }
        uint16_t value = ((uint16_t*)header)[i];
        sum += MAYBE_SWAP16(value);
    }
    // Perform the 1's complement sum we need to wrap overflow
    if (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum;
}
