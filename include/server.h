#ifndef SERVER
#define SERVER

#include <inttypes.h>
#include <netinet/in.h>
#include <stdint.h>

#include "../include/bool.h"

typedef struct {
    uint16_t server_port;
    struct sockaddr_in client;
    int sockfd;
} connection_t;


/*
 * Starts a TCP server that listens on {port}.
 */
bool_t start_server( uint16_t in_port, int in_listen_count, int *out_listen_sockfd);

#endif
