#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    // Buat socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Atur alamat server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect ke server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Terhubung ke Echo Server\n");

    while (1) {
        printf("Ketik pesan: ");
        fgets(buffer, BUF_SIZE, stdin);

        // Kirim pesan
        send(sockfd, buffer, strlen(buffer), 0);

        // Terima balasan
        memset(buffer, 0, BUF_SIZE);
        int n = recv(sockfd, buffer, BUF_SIZE, 0);
        if (n <= 0) {
            printf("Server putus\n");
            break;
        }
        printf("Dari server: %s", buffer);
    }

    close(sockfd);
    return 0;
}
