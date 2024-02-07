#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/thread_pool.h"
#include "../include/server.h"
#include "../include/string.h"
#include "../include/http.h"


void sigint_handler(int val) {
    // TODO: close listening socket
    fprintf(stderr, "signal val = %d\n", val);
    close_thread_pool();
    exit(1);
}

void *start_http_server(uint16_t port) {
    int listen_sockfd;
    int connection_sockfd;
    socklen_t client_length;
    struct sockaddr_in client_address;
    connection_t connection;

    if (!start_server(port, 15, &listen_sockfd)) {
        fprintf(stderr, "ERROR: Could not listen on port %d", port);
        perror(" ");
        return NULL;
    }

    fprintf(stdout, "[HTTP] Setup successful\n");

    // Handle clients
    {
        client_length = sizeof(struct sockaddr_in);

        while (1) {
            // Accept connecting clients and break on 
            connection_sockfd = accept(listen_sockfd, (struct sockaddr *) &client_address, &client_length);
            if (connection_sockfd != -1) {
                connection.server_port = port;
                connection.client = client_address;
                connection.sockfd = connection_sockfd;

                allocate_to_thread((thread_input_t) { .connection = connection, });
            } else {
                perror("ERROR: [HTTP] Call to 'accept' failed");
                break;
            }
        }
    }

    // Thread teardown
    {
        fprintf(stdout, "[HTTP] Closing\n");

        close(listen_sockfd);
        exit(EXIT_SUCCESS);
        return NULL; 
    }
}

uint8_t aaa[] = ": \r\r\n";
size_t indx = 0;

uint8_t next_byte() {
    return aaa[indx++];
}

void print_char(FILE *stream, char c) {
    switch (c) {
        case '\0':
            fprintf(stream, "\033[33m0\033[0m");
            break;
        case '\r':
            fprintf(stream, "\033[33mr\033[0m");
            break;
        case '\n':
            fprintf(stream, "\033[33mn\033[0m");
            break;
        case '\t':
            fprintf(stream, "\033[33mt\033[0m");
            break;
        case ' ':
            fprintf(stream, "\033[33ms\033[0m");
            break;
        default:
            putc(c, stream);
            break;
    }
}

void handle_input(size_t thread_number, thread_input_t input) {
    fprintf(stderr, "Received request from %hu\n", ntohs(input.connection.client.sin_port));
    handle_connection(&input.connection);
}

int main(int argc, char **argv) {
    uint16_t port = 8080;
    if (argc >= 2) {
        port = (uint16_t) atoi(argv[1]);
    }

    if (true) {
        signal(SIGINT, sigint_handler);
        init_thread_pool();
        start_http_server(port);
        close_thread_pool();
    }

    // parse_header();

    return 0;
}

