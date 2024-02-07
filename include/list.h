#ifndef LIST
#define LIST

#include <stdlib.h>

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

// TODO: decide if it's worth it to keep T in the signature for consistency
#define list_get(T, list, index) \
    (((index) < (list).length) ? &(list).items[(index)] : NULL)

// TODO: decide if it's worth it to keep T in the signature for consistency
#define list_get_unsafe(T, list, index) \
    ((list).items[(index)])

// TODO: decide if it's worth it to keep T in the signature for consistency
#define list_free(T, list) \
    free((list).items);

#endif
