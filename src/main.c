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

int main(int argc, char **argv) {
    signal(SIGINT, sigint_handler);

    init_thread_pool();

    start_http_server();

    close_thread_pool();

    return 0;
}
