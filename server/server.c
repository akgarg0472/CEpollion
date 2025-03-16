#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <pthread.h>
#include "parser.h"
#include "logger.h"
#include "utils.h"
#include "command_handler.h"

#define PORT 2318
#define BACKLOG 100
#define READ_BUFFER_SIZE 1024
#define MAX_EVENTS 10000
#define MAX_CLIENTS 10000

int total_clients_connected = 0;
int total_queries_processed = 0;
int active_clients = 0;
int server_fd;
int epoll_fd;

/**
 * @brief Prints server statistics before shutdown.
 */
void print_statistics()
{
    printf("\n+------------------------+------------------------+\n");
    printf("| Total Clients Connected | Total Queries Processed |\n");
    printf("+------------------------+------------------------+\n");
    printf("| %22d | %22d |\n", total_clients_connected, total_queries_processed);
    printf("+------------------------+------------------------+\n");
}

/**
 * @brief Sets a socket file descriptor to non-blocking mode.
 *
 * In non-blocking mode, system calls like `read()` and `write()`
 * will return immediately if no data is available, instead of blocking.
 *
 * @param fd The socket file descriptor to modify.
 *
 * @note If `fcntl` fails at any step, the function prints an error message
 *       using `perror` and exits the program with `EXIT_FAILURE`.
 */
void set_socket_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl GETFL");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl SETFL O_NONBLOCK");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Handles termination signals (e.g., SIGINT, SIGTERM)
 *        and gracefully shuts down the server cleaning up server resources.
 *
 * @param signal The signal number received (e.g., SIGINT = 2, SIGTERM = 15).
 */
void cleanup_and_close_server(int signal)
{
    if (server_fd != -1)
    {
        close(server_fd);
    }

    if (epoll_fd != -1)
    {
        close(epoll_fd);
    }

    print_statistics();
    exit(signal);
}

int main()
{
    signal(SIGINT, cleanup_and_close_server);  // Handle Ctrl+C
    signal(SIGTERM, cleanup_and_close_server); // Handle termination using kill
    pthread_setname_np(pthread_self(), "main");

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(server_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) == -1)
    {
        perror("setsockopt TCP_NODELAY");
        exit(EXIT_FAILURE);
    }

    // Enable SO_REUSEADDR to allow immediate reuse of the port
    int opt = 1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    set_socket_nonblocking(server_fd);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event server_event;
    server_event.events = EPOLLIN;
    server_event.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &server_event) == -1)
    {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }

    log_message("INFO", "CEpollion Server started:\n"
                        "{\n"
                        "  \"server_socket_fd\": %d,\n"
                        "  \"port\": %d,\n"
                        "  \"max_clients\": %d\n"
                        "}",
                server_fd, ntohs(server_addr.sin_port), MAX_CLIENTS);

    struct epoll_event events[MAX_EVENTS];
    initialize_command_handler();

    while (true)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;

            // New client trying to connect
            if (fd == server_fd)
            {
                if (active_clients >= MAX_CLIENTS)
                {
                    log_message("ERROR", "Max clients reached (%d). Rejecting connection...", MAX_CLIENTS);
                    int tmp_fd = accept(server_fd, NULL, NULL);
                    if (tmp_fd != -1)
                        close(tmp_fd);
                    continue;
                }

                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

                if (client_fd == -1)
                {
                    perror("accept");
                    continue;
                }

                set_socket_nonblocking(client_fd);

                struct epoll_event client_event;
                client_event.events = EPOLLIN | EPOLLET;
                client_event.data.fd = client_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
                {
                    perror("epoll_ctl EPOLL_CTL_ADD");
                    close(client_fd);
                    continue;
                }

                active_clients++;
                total_clients_connected++;
            }
            else
            {
                // Existing client has sent some data
                char buffer[READ_BUFFER_SIZE];
                memset(buffer, 0, READ_BUFFER_SIZE);
                ssize_t bytes_read = read(fd, buffer, READ_BUFFER_SIZE);

                if (bytes_read <= 0)
                {
                    if (bytes_read < 0)
                    {
                        perror("sock_read_err");
                    }
                    close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    active_clients--;
                }
                else
                {
                    remove_trailing_newline(buffer);
                    Command *cmd = malloc(sizeof(Command));
                    parse_client_input(buffer, cmd);
                    char *resp = execute_command(cmd);
                    write(fd, resp, strlen(resp));
                    free(resp);
                    free(cmd);
                    total_queries_processed++;
                }
            }
        }
    }

    cleanup_and_close_server(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
