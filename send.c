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
    char* msg = udp_send(0x7f000001, PORT, message, strlen(message)); 

    for (int i = 0; i < 20 + 8 + strlen(message); i++) {
        print_bits(msg[i]);
    }

    if (sendto(
            sockfd, msg, 20 + 8 + strlen(message), 0,
            (struct sockaddr*)&serveraddr, sizeof(serveraddr)
        ) < 0) {
        perror("sendto error");
        return EXIT_FAILURE;
    }

    printf("Message sent!\n");
    free(msg);
    return EXIT_SUCCESS;
}
