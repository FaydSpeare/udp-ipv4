#include "../udp.h"
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);

    int sockfd;
    struct sockaddr_in serveraddr;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket error");
        return EXIT_FAILURE;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
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

        printf(
            "Received %d bytes from %s\n", bytes_received,
            inet_ntoa(peeraddr.sin_addr)
        );

        ip_header iph;
        udp_header udph;

        ip_extract_header(&iph, buffer, 100);
        ip_print_header(&iph);

        udp_extract_header(&udph, buffer + 20, 100);
        udp_print_header(&udph);

        uint16_t ip_checksum = ip_calculate_checksum(&iph);
        printf(" IP checksum: %hu\n", ip_checksum);

        uint16_t udp_checksum =
            udp_calculate_checksum(&iph, &udph, buffer + 28);
        printf("UDP checksum: %hu\n", udp_checksum);
        printf("\n\n");
    }

    return EXIT_SUCCESS;
}
