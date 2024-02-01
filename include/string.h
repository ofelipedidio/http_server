#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char *text;
    size_t length;
    size_t capacity;
} string_t;

/*
 * Creates an empty string
 */
string_t string_new();

/*
 * Creates an empty string
 */
string_t string_new_with_capacity(size_t initial_capacity);

/*
 * Converts a C-style string to a string_t
 */
string_t str_cstr(char *str);

/*
 * Converts a char string to a string_t
 */
string_t str_char(char c);

/*
 * Converts an unsigned integer to a string_t
 */
string_t str_int(uint64_t value, size_t base);

/*
 * Converts a signed integer to a string_t
 */
string_t str_uint(int64_t value, size_t base);

/*
 * Appends a string_t (source) into another string_t (destination).
 */
string_t *str_push(string_t *destination, string_t source);

/*
 * Appends a string_t (source) into another string_t (destination).
 * Also frees the appended string_t.
 */
string_t *str_pushf(string_t *destination, string_t source);

/*
 * Prints the {string} to {stream}
 */
void str_print(string_t string, FILE *stream);

/*
 * Prints the {string} to {stream}.
 * Also frees the printed string_t.
 */
void str_printf(string_t string, FILE *stream);

/*
 * Frees the allocated memory from a string_t
 */
void str_free(string_t string);
