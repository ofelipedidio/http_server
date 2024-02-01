#ifndef SERVER
#define SERVER
#include <stdint.h>
#include "../include/bool.h"

/*
 * Starts a TCP server that listens on {port}.
 */
bool_t start_server(uint16_t port);

void *start_http_server();

#endif
