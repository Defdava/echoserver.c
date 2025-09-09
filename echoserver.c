#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUF_SIZE];

    // buat socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    // set alamat server
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // bind
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // listen
    if (listen(sockfd, 5) < 0) {
        perror("listen error");
        exit(1);
    }

    printf("Echo Server berjalan di port %d...\n", PORT);

    // tunggu client
    len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    if (connfd < 0) {
        perror("accept error");
        exit(1);
    }

    // komunikasi
    while (1) {
        int n = read(connfd, buffer, BUF_SIZE);
        if (n <= 0) break;
        buffer[n] = '\0';
        printf("Dari Client: %s", buffer);
        write(connfd, buffer, strlen(buffer)); // echo balik
    }

    close(connfd);
    close(sockfd);
    return 0;
}
