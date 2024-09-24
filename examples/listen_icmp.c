#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int port = atoi(argv[1]);

    int sockfd;
    struct sockaddr_in serveraddr;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
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
        printf(
            "Received %d bytes from %s:%d\n", bytes_received,
            inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port)
        );

        for (int i = 20; i < bytes_received; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
            if (buffer[i] >= 32) {
                printf("'%c' ", buffer[i]);
            }
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
