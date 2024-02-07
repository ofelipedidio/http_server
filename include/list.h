#ifndef LIST
#define LIST

#include <stdlib.h>

#define list_definition(T) \
    struct { \
        T *items; \
        size_t length; \
        size_t capacity; \
    }

#define list_init(T, list) \
    (list).capacity = 1; \
    (list).items = (T*) calloc((list).capacity, sizeof(T)); \
    (list).length = 0;

#define list_insert(T, list, item) \
    while ((list).length + 1 >= (list).capacity) { \
        size_t new_capacity = (list).capacity * 2; \
        T *new_array = (T*) reallocarray((list).items, new_capacity, sizeof(T)); \
        (list).items = new_array; \
        (list).capacity = new_capacity; \
    } \
    (list).items[(list).length] = (item); \
    (list).length++;

#define list_get(list, index) \
    (((index) < (list).length) ? &(list).items[(index)] : NULL)

#define list_get_unsafe(list, index) \
    ((list).items[(index)])

#define list_free(list) \
    free((list).items);

#endif
