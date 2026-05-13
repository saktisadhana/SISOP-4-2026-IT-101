// Project MOO — TCP Client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000
#define MAX_BUFFER 4096
/*
XoXoXoXoXoXoXoXo
Global Variables
XoXoXoXoXoXoXoXo
*/
int sock_fd;
/*
XoXoXoXoXoXoXoXo
Helper: kirim command ke server dan terima respon
Baca sampai server selesai mengirim data
XoXoXoXoXoXoXoXo
*/
void send_and_receive(const char *cmd)
{
    // Kirim command + newline ke server
    char send_buf[MAX_BUFFER];
    snprintf(send_buf, sizeof(send_buf), "%s\n", cmd);
    send(sock_fd, send_buf, strlen(send_buf), 0);

    // Terima respon dari server
    char recv_buf[MAX_BUFFER];
    int bytes;

    // Baca sampai tidak ada data lagi
    while ((bytes = recv(sock_fd, recv_buf, sizeof(recv_buf) - 1, 0)) > 0)
    {
        recv_buf[bytes] = '\0';
        printf("%s", recv_buf);

        // Jika bytes lebih kecil dari buffer, berarti sudah selesai
        if (bytes < (int)(sizeof(recv_buf) - 1))
            break;
    }
}

int main()
{
    /*
    _o_o_o_o_o_o_o
    Connect ke Server
    _o_o_o_o_o_o_o
    */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to DB Server on port %d\n", PORT);
    printf("Type HELP for available commands\n");
    printf("Type EXIT to quit\n");

    /*
    _o_o_o_o_o_o_o
    Main Loop — baca input user,
    kirim ke server, tampilkan respon
    _o_o_o_o_o_o_o
    */
    char input[MAX_BUFFER];

    while (1)
    {
        printf("\ndb > ");
        fflush(stdout);

        // Baca input dari user
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        // Hapus newline
        input[strcspn(input, "\n")] = '\0';

        // Cek apakah user mau keluar
        if (strcmp(input, "EXIT") == 0 || strcmp(input, "exit") == 0)
        {
            printf("Disconnecting from server...\n");
            break;
        }

        // Skip input kosong
        if (strlen(input) == 0)
            continue;

        // Kirim command dan terima respon
        send_and_receive(input);
    }

    /*
    _o_o_o_o_o_o_o
    Cleanup: tutup socket
    _o_o_o_o_o_o_o
    */
    close(sock_fd);
    return 0;
}
