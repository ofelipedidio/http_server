#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/thread_pool.h"
#include "../include/server.h"

void sigint_handler(int val) {
    // TODO: close listening socket
    fprintf(stderr, "signal val = %d\n", val);
    close_thread_pool();
    exit(1);
}

void handle_input(size_t thread_number, thread_input_t input) {
    fprintf(stderr, "Received request from %hu\n", ntohs(input.connection.client.sin_port));
}

int main(int argc, char **argv) {
    signal(SIGINT, sigint_handler);

    init_thread_pool();

    start_http_server();

    close_thread_pool();

    return 0;
}
