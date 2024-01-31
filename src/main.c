#include "../include/thread_pool.h"

int main(int argc, char **argv) {
    init_thread_pool();

    thread_input_t in;
    for (int i = 0; i < 30; i++) {
        in.num = i;
        allocate_to_thread(in);
    }

    close_thread_pool();

    return 0;
}
