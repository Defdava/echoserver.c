#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket, client_socket[MAX_CLIENTS];
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int max_sd, sd, activity, valread;

    // Inisialisasi semua client_socket ke 0
    for (int i = 0; i < MAX_CLIENTS; i++)
        client_socket[i] = 0;

    // Buat socket utama
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Gagal membuat socket");
        exit(EXIT_FAILURE);
    }

    // Setup alamat server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind ke port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind gagal");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen koneksi masuk
    if (listen(server_fd, 3) < 0) {
        perror("Listen gagal");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server berjalan di port %d...\n", PORT);

    while (1) {
        // Kosongkan set dan tambahkan server_fd
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Tambahkan client socket ke set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        // Tunggu aktivitas pada socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Kesalahan pada select()\n");
        }

        // Jika ada koneksi baru
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("Accept gagal");
                exit(EXIT_FAILURE);
            }

            printf("Client baru terhubung -> socket fd: %d | IP: %s | PORT: %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Tambahkan ke daftar client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }

            // Tampilkan semua client yang sedang terhubung
            printf("Client socket descriptor aktif: ");
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] > 0)
                    printf("%d ", client_socket[i]);
            }
            printf("\n");
        }

        // Cek aktivitas dari masing-masing client
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                // Jika client menutup koneksi
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Client terputus -> IP: %s | PORT: %d | fd: %d\n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port), sd);
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    // Pesan dari client
                    buffer[valread] = '\0';
                    printf("Pesan dari client fd %d: %s", sd, buffer);

                    // Kirim ke semua client lain
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socket[j] != 0 && client_socket[j] != sd) {
                            send(client_socket[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
