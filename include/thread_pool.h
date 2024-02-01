#ifndef THREAD_POOL
#define THREAD_POOL

#include <netinet/in.h>

#define THREAD_COUNT 10

typedef struct {
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sockfd;
} thread_input_t;

void init_thread_pool();

void allocate_to_thread(thread_input_t input);

void *thread_function(void *arg);

void close_thread_pool();

#endif
