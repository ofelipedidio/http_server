typedef struct {
    int num;
} thread_input_t;

void init_thread_pool();

void allocate_to_thread(thread_input_t input);

void *thread_function(void *arg);

void close_thread_pool();
