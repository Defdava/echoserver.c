#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 12345
#define BUF_SIZE 1024

void handle_client(int connfd) {
    char buffer[BUF_SIZE];
    int n;

    while ((n = read(connfd, buffer, BUF_SIZE)) > 0) {
        buffer[n] = '\0';
        printf("Pesan dari client: %s", buffer);
        write(connfd, buffer, strlen(buffer)); // echo balik
    }
    close(connfd);
    exit(0);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    // ignore zombie process
    signal(SIGCHLD, SIG_IGN);

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

    printf("Server berjalan di port %d...\n", PORT);

    while (1) {
        len = sizeof(cliaddr);
        connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        if (connfd < 0) {
            perror("accept error");
            continue;
        }

        if (fork() == 0) { // proses anak
            close(sockfd);
            handle_client(connfd);
        }
        close(connfd); // proses induk menutup koneksi client
    }

    close(sockfd);
    return 0;
}
