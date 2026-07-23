#include "server.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "utils.h"

enum {
    RECEIVE_BUFFER_SIZE = 1024,
    LISTEN_BACKLOG = 5
};

static int create_listening_socket(uint16_t port);

static int send_all(
    int socket_fd,
    const void *buffer,
    size_t length
);

static void handle_client(int client_fd);

int server_run(uint16_t port)
{
    signal(SIGPIPE, SIG_IGN);

    int server_fd = create_listening_socket(port);

    printf("Сервер запущен на порту %u\n", (unsigned int)port);

    int active_clients = 0;

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);

        int client_fd = accept(
            server_fd,
            (struct sockaddr *)&client_address,
            &client_address_size
        );

        if (client_fd == -1) {
            if (errno == EINTR) {
                continue;
            }

            close(server_fd);
            fatal("accept");
        }

        char client_ip[INET_ADDRSTRLEN];

        const char *ip_result = inet_ntop(
            AF_INET,
            &client_address.sin_addr,
            client_ip,
            sizeof(client_ip)
        );

        if (ip_result == NULL) {
            strcpy(client_ip, "unknown");
        }

        unsigned int client_port =
            (unsigned int)ntohs(client_address.sin_port);

        active_clients++;

        printf(
            "[+] Клиент %s:%u подключился. Активных: %d\n",
            client_ip,
            client_port,
            active_clients
        );

        handle_client(client_fd);

        close(client_fd);

        active_clients--;

        printf(
            "[-] Клиент %s:%u отключился. Активных: %d\n",
            client_ip,
            client_port,
            active_clients
        );
    }

    close(server_fd);

    return 0;
}

static int create_listening_socket(uint16_t port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        fatal("socket");
    }

    int reuse_address = 1;

    if (setsockopt(
            server_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse_address,
            sizeof(reuse_address)
        ) == -1) {
        close(server_fd);
        fatal("setsockopt");
    }

    struct sockaddr_in server_address;

    memset(
        &server_address,
        0,
        sizeof(server_address)
    );

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(
            server_fd,
            (struct sockaddr *)&server_address,
            sizeof(server_address)
        ) == -1) {
        close(server_fd);
        fatal("bind");
    }

    if (listen(server_fd, LISTEN_BACKLOG) == -1) {
        close(server_fd);
        fatal("listen");
    }

    return server_fd;
}

static int send_all(
    int socket_fd,
    const void *buffer,
    size_t length
)
{
    const unsigned char *current =
        (const unsigned char *)buffer;

    size_t total_sent = 0;

    while (total_sent < length) {
        ssize_t sent_length = send(
            socket_fd,
            current + total_sent,
            length - total_sent,
            0
        );

        if (sent_length > 0) {
            total_sent += (size_t)sent_length;
            continue;
        }

        if (sent_length == -1 && errno == EINTR) {
            continue;
        }

        return -1;
    }

    return 0;
}

static void handle_client(int client_fd)
{
    const char greeting[] =
        "Echo server connected.\n"
        "Введите сообщение:\n";

    if (send_all(
            client_fd,
            greeting,
            sizeof(greeting) - 1
        ) == -1) {
        perror("send greeting");
        return;
    }

    unsigned char buffer[RECEIVE_BUFFER_SIZE];

    while (1) {
        ssize_t received_length = recv(
            client_fd,
            buffer,
            sizeof(buffer),
            0
        );

        if (received_length > 0) {
            printf(
                "Получено от клиента: %zd байтов\n",
                received_length
            );

            dump(
                buffer,
                (size_t)received_length
            );

            if (send_all(
                    client_fd,
                    buffer,
                    (size_t)received_length
                ) == -1) {
                perror("send echo");
                return;
            }

            printf(
                "Отправлено обратно: %zd байтов\n",
                received_length
            );

            continue;
        }

        if (received_length == 0) {
            printf("Клиент закрыл соединение\n");
            return;
        }

        if (errno == EINTR) {
            continue;
        }

        perror("recv");
        return;
    }
}