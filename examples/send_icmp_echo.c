#include "../byteorder.h"
#include "../ip.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <peer-ip>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char* peer_ip = argv[1];

    int sockfd;
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(peer_ip);

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        perror("socket error");
        return EXIT_FAILURE;
    }

    char message[] = "Szia";
    ip_header iph = {
        .version = 4,
        .ihl = 5,
        .tos = 0,
        .total_length = MAYBE_SWAP16(20 + 8 + 4),
        .id = 0,
        .fragment_offset = 0,
        .ttl = 64,
        .protocol = IPPROTO_ICMP,
        .checksum = 0,
        .src_address = get_local_ipv4_address(),
        .dst_address = inet_addr(peer_ip),
    };
    iph.checksum = MAYBE_SWAP16(ip_calculate_checksum(&iph));

    char buffer[1024];
    memcpy(buffer, &iph, 20);
    buffer[20] = 0x08;         // ICMP type
    buffer[21] = 0x00;         // ICMP code
    buffer[22] = 0x00;         // ICMP checksum
    buffer[23] = 0x00;         // ICMP checksum
    memset(buffer + 24, 0, 2); // id
    memset(buffer + 26, 0, 2); // sequence
    memcpy(buffer + 28, message, 4);

    uint32_t checksum = 0;
    checksum += (8 << 8);
    for (int i = 0; i < 2; i++) {
        uint16_t word = ((uint16_t*)message)[i];
        checksum += MAYBE_SWAP16(word);
    }
    checksum = (checksum & 0xFFFF) + (checksum >> 16);
    uint16_t checksum16 = ~checksum;
    buffer[22] = checksum16 >> 8;
    buffer[23] = checksum16 & 0xFF;

    if (sendto(
            sockfd, buffer, 20 + 8 + 4, 0, (struct sockaddr*)&serveraddr,
            sizeof(serveraddr)
        ) < 0) {
        perror("sendto error");
        return EXIT_FAILURE;
    }

    printf("Message sent!\n");
    return EXIT_SUCCESS;
}
