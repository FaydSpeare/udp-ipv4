#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

    struct sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(7777);

    char* peer_ip = "127.0.0.1";
    int result = inet_pton(AF_INET, peer_ip, &peer_addr.sin_addr);
    if (result <= 0) {
        perror("inet_pton failed");
        return 0;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    char* buffer = "Hello, world!";
    int bytes = sendto(
        sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&peer_addr,
        sizeof(peer_addr)
    );
    if (bytes < 0) {
        perror("sendto failed");
        return 0;
    }
    printf("Sent %d bytes\n", bytes);
    close(sockfd);
    return 0;
}
