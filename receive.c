#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {

    int sockfd;
    printf("Creating socket...\n");
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 0;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7777);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    printf("Binding...\n");
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        return 0;
    }

    char buffer[1024];
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);

    printf("Waiting for data...\n");
    int bytes = recvfrom(
        sockfd, buffer, 1024, 0, (struct sockaddr*)&peer_addr, &peer_addr_len
    );
    if (bytes < 0) {
        perror("recvfrom failed");
        return 0;
    }
    printf(
        "Received from: %s:%d\n",
        inet_ntoa(peer_addr.sin_addr),
        ntohs(peer_addr.sin_port)
    );
    printf("Received: %.*s\n", bytes, buffer);
    return 0;
}
