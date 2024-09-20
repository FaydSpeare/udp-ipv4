#include "udp.h"
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define PORT 7777

int main(void) {
    printf("host endianness: %d\n", __BYTE_ORDER__);
    printf("little endian: %d\n", __ORDER_LITTLE_ENDIAN__);
    printf("big endian: %d\n", __ORDER_BIG_ENDIAN__);

    int sockfd;
    struct sockaddr_in serveraddr;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket error");
        return EXIT_FAILURE;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    char buffer[1024];
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len;
    while (1) {
        int bytes_received = recvfrom(
            sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&peeraddr,
            &peeraddr_len
        );

        if (bytes_received < 0) {
            perror("recvfrom error");
            return EXIT_FAILURE;
        }

        printf("\n");
        printf("Received %d bytes from %s:%d\n", bytes_received,
               inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

        ip_header iph;
        udp_header udph;

        ip_extract_header(&iph, buffer, 100);
        ip_print_header(&iph);

        udp_extract_header(&udph, buffer + 20, 100);
        udp_print_header(&udph);

        uint16_t ip_checksum = ip_calculate_checksum(&iph);
        printf(" IP checksum: %hu\n", ip_checksum);

        uint16_t udp_checksum = udp_calculate_checksum(&iph, &udph, buffer + 28);
        printf("UDP checksum: %hu\n", udp_checksum);
    }

    return EXIT_SUCCESS;
}
