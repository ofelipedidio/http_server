#ifndef THREAD_POOL
#define THREAD_POOL

#include "../include/server.h"

#define THREAD_COUNT 10

/*
 * Thread payload
 */
typedef struct {
    connection_t connection;
} thread_input_t;

/*
 * Payload handler. This function should be implemented elsewhere
 */
void handle_input(size_t thread_number, thread_input_t input);

/*
 * Sets up the environment to start allocating tasks to threads
 */
void init_thread_pool();

/*
 * Assigns the input to one of the threads
 */
void allocate_to_thread(thread_input_t input);

/*
 * Waits for all threads to end their tasks and closes the pool
 *
 * After this function is called, calls to 'allocate_to_thread' are UB
 */
void close_thread_pool();

#endif
