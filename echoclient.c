#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUF_SIZE];

    // buat socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

    // konek ke server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }

    printf("Terhubung ke server. Ketik pesan:\n");

    while (1) {
        printf("Anda: ");
        fgets(buffer, BUF_SIZE, stdin);
        write(sockfd, buffer, strlen(buffer));

        int n = read(sockfd, buffer, BUF_SIZE);
        buffer[n] = '\0';
        printf("Echo dari server: %s", buffer);
    }

    close(sockfd);
    return 0;
}
