// Stdlib
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Network
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

// Extra
#include <semaphore.h>
#include <pthread.h>

#include "../include/bool.h"
#include "../include/thread_pool.h"

/*
 * Starts a TCP server that listens on {port}.
 */
bool_t start_server(
        uint16_t in_port,
        int in_listen_count,
        int *out_listen_sockfd) {
    struct sockaddr_in server_address;

    *out_listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*out_listen_sockfd == -1) {
        return false;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(in_port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_address.sin_zero), 8);

    if (bind(*out_listen_sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        return false;
    }

    listen(*out_listen_sockfd, in_listen_count);
    return true;
}



void *start_http_server() {
    int listen_sockfd;
    int connection_sockfd;
    socklen_t client_length;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    uint16_t port = 8080;

    if (!start_server(port, 15, &listen_sockfd)) {
        fprintf(stderr, "ERROR: Could not listen on port %d", port);
        perror(NULL);
    }

    fprintf(stdout, "[HTTP] Setup successful\n");

    // Handle clients
    {
        client_length = sizeof(struct sockaddr_in);

        while (1) {
            // Accept connecting clients and break on 
            connection_sockfd = accept(listen_sockfd, (struct sockaddr *) &client_address, &client_length);
            if (connection_sockfd == -1) {
                perror("ERROR: [DNS] Call to 'accept' failed");
                // break;
            } else {

            allocate_to_thread((thread_input_t) {
                    .server = server_address,
                    .client = client_address,
                    .sockfd = connection_sockfd,
                    });
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
