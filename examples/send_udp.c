#include "../byteorder.h"
#include "../udp.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <address> <port> <message>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* peer_ip = argv[1];
    int peer_port = atoi(argv[2]);
    char* message = argv[3];    

    printf("Address: %s\n", peer_ip);
    printf("Port: %d\n", peer_port);
    printf("Message: %s\n", message);

    int result = udp_send(peer_ip, peer_port, message, strlen(message)); 

    if (result == 1) {
        printf("Failed to send message!\n");
        return EXIT_FAILURE;
    }

    printf("Message sent!\n");
    return EXIT_SUCCESS;
}
