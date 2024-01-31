// Stdlib
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Extra
#include <semaphore.h>
#include <pthread.h>

#include "../include/bool.h"
#include "../include/thread_pool.h"


pthread_t      threads[THREAD_COUNT];
size_t         thread_ids[THREAD_COUNT];
sem_t          thread_input_lock[THREAD_COUNT];
bool_t         thread_free[THREAD_COUNT];
thread_input_t thread_input[THREAD_COUNT];

sem_t thread_lock;
sem_t global_mutex;

bool_t should_close;

void init_thread_pool() {
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        sem_init(&thread_input_lock[i], 0, 0);
        thread_free[i] = true;
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }

    sem_init(&thread_lock, 0, THREAD_COUNT);
    sem_init(&global_mutex, 0, 1);
    should_close = false;
}

void allocate_to_thread(thread_input_t input) {
    sem_wait(&thread_lock);
    sem_wait(&global_mutex);
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        if (thread_free[i]) {
            thread_free[i] = false;
            thread_input[i] = input;
            sem_post(&thread_input_lock[i]);
            break;
        }
    }
    sem_post(&global_mutex);
}

void close_thread_pool() {
    sem_wait(&global_mutex);
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        sem_post(&thread_input_lock[i]);
    }
    sem_post(&global_mutex);

    void *ret;
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        fprintf(stderr, "Joined thread %zd\n", i);
        if (pthread_join(threads[i], &ret) == 0) {
            fprintf(stderr, "Thread %zd closed\n", i);
        } else {
            fprintf(stderr, "Thread %zd failed", i);
            perror(NULL);
        }
    }
}

void *thread_function(void *arg) {
    // TODO: receive thread index
    size_t i = *((size_t*) arg);

    while (true) {
        // Start
        sem_wait(&thread_input_lock[i]);
        sem_wait(&global_mutex);
        if (thread_free[i]) {
            fprintf(stdout, "[thread %zu] closing\n", i);
            sem_post(&global_mutex);
            break;
        }
        sem_post(&global_mutex);

        // Process
        fprintf(stdout, "[thread %zu] processing\n", i);
        {
            thread_input_t input = thread_input[i];

            uint8_t buf[] = "HTTP/1.1 200 Success\r\nContent-Length: 14\r\n\r\nHello, world!\n";
            write(input.sockfd, buf, 58);

            close(input.sockfd);
        }
        fprintf(stdout, "[thread %zu] done\n", i);

        // End
        sem_wait(&global_mutex);
        thread_free[i] = true;
        sem_post(&thread_lock);
        sem_post(&global_mutex);
    }

    fprintf(stdout, "[thread %zu] stopping\n", i);
    return NULL;
}

