#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 12345
#define BUF_SIZE 1024

// Handler untuk zombie process
void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t addr_len;

    // Buat socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Atur alamat server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Binding
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Tangani zombie process
    signal(SIGCHLD, sigchld_handler);

    printf("Echo Server T3 berjalan di port %d...\n", PORT);

    // Loop utama
    while (1) {
        addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        // Fork untuk client baru
        if (fork() == 0) {
            close(server_fd);
            printf("Client terhubung dari %s:%d\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

            while (1) {
                memset(buffer, 0, BUF_SIZE);
                int n = read(client_fd, buffer, BUF_SIZE);
                if (n <= 0) {
                    printf("Client %s:%d disconnect\n",
                           inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));
                    break;
                }
                printf("Dari client: %s\n", buffer);
                write(client_fd, buffer, strlen(buffer));
            }
            close(client_fd);
            exit(0);
        } else {
            close(client_fd); // Parent tidak pakai client_fd
        }
    }

    close(server_fd);
    return 0;
}
