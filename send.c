#include "byteorder.h"
#include "udp.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define PEER_IP "127.0.0.1"
#define PORT 7777

void print_bits(char c) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (c >> i) & 1);
    }
    printf("'%c'\n", c > 31 ? c : ' ');
}

int main(void) {
    printf("host endianness: %d\n", __BYTE_ORDER__);
    printf("little endian: %d\n", __ORDER_LITTLE_ENDIAN__);
    printf("big endian: %d\n", __ORDER_BIG_ENDIAN__);

    int sockfd;
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(PEER_IP);

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        perror("socket error");
        return EXIT_FAILURE;
    }

    char buffer[1024];
    char message[] = "Szia Dorka :)!";

    ip_header iph;
    iph.version = 4;
    iph.ihl = 5;
    iph.tos = 0;
    iph.total_length = MAYBE_SWAP16(20 + 8 + strlen(message));
    iph.id = MAYBE_SWAP16(0);
    iph.fragment_offset = MAYBE_SWAP16(0);
    iph.ttl = 48;
    iph.protocol = 17;
    iph.src_address = MAYBE_SWAP32(0x7f000001);
    iph.dst_address = MAYBE_SWAP32(0x7f000001);
    iph.checksum = MAYBE_SWAP16(ip_calculate_checksum(&iph));

    udp_header udph;
    udph.src_port = MAYBE_SWAP16(0);
    udph.dst_port = MAYBE_SWAP16(PORT);
    udph.length = MAYBE_SWAP16(22);
    udph.checksum = MAYBE_SWAP16(udp_calculate_checksum(&iph, &udph, message));

    ip_write_header(&iph, buffer, 100);
    udp_write_header(&udph, buffer + 20, 100);
    memcpy(buffer + 28, message, strlen(message));

    for (int i = 0; i < 20 + 8 + strlen(message); i++) {
        print_bits(buffer[i]);
    }

    if (sendto(
            sockfd, buffer, 20 + 8 + strlen(message), 0,
            (struct sockaddr*)&serveraddr, sizeof(serveraddr)
        ) < 0) {
        perror("sendto error");
        return EXIT_FAILURE;
    }

    printf("Message sent!\n");
    return EXIT_SUCCESS;
}
