#include "../include/string.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char digits[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    'l',
    'm',
    'n',
    'o',
    'p',
    'q',
    'r',
    's',
    't',
    'u',
    'v',
};

/*
 * Creates an empty string
 */
string_t string_new() {
    string_t string;
    string.text = malloc(0);
    string.length = 0;
    string.capacity = 0;
    return string;
}

string_t string_new_with_capacity(size_t initial_capacity) {
    string_t string;
    string.text = calloc(initial_capacity, sizeof(char));
    string.length = 0;
    string.capacity = initial_capacity;
    return string;
}

/*
 * Converts a C-style string to a string_t
 */
string_t str_cstr(char *str) {
    string_t string;
    // TODO: is strdup the right function to call? I think I want called and not malloc here
    string.text = strdup(str);
    string.length = strlen(string.text);
    string.capacity = string.length;
    return string;
}

/*
 * Converts a char string to a string_t
 */
string_t str_char(char c) {
    string_t string;
    string.text = calloc(1, sizeof(char));
    string.text[0] = c;
    string.length = 1;
    string.capacity = 1;
    return string;
}

/*
 * Converts an unsigned integer to a string_t
 */
string_t str_int(uint64_t value, size_t base) {
    if (value == 0) {
        return str_cstr("0");
    }
    size_t digit_count = 0;                   
    uint64_t value_copy = value;
    while (value_copy != 0) {
        digit_count++;
        value_copy = value_copy / base;
    }
    string_t string = string_new_with_capacity(digit_count);    
    string.length = string.capacity;
    while (value > 0) {
        char c = digits[value % base];
        string.text[--digit_count] = c;
        value = value / base;
    }
    return string;
}

/*
 * Converts a signed integer to a string_t
 */
string_t str_uint(int64_t value, size_t base) {
    if (value == 0) {
        return str_cstr("0");
    }
    size_t digit_count = (value < 0) ? 1 : 0;                   
    int64_t value_copy = value;
    while (value_copy != 0) {
        digit_count++;
        value_copy = value_copy / base;
    }
    string_t string = string_new_with_capacity(digit_count);    
    if (value < 0) {
        string.text[0] = '-';
        value = -value;
    }
    string.length = string.capacity;
    while (value > 0) {
        char c = digits[value % base];
        string.text[--digit_count] = c;
        value = value / base;
    }
    return string;
}

/*
 * Appends a string_t (source) into another string_t (destination)
 */
string_t *str_push(string_t *destination, string_t source) {
    if (destination->length + source.length >= destination->capacity) {
        destination->capacity = destination->length + source.length;
        char *new_text = reallocarray(destination->text, destination->capacity, sizeof(char));
        destination->text = new_text;
    }
    memcpy(destination->text + destination->length, source.text, source.length * sizeof(char));
    destination->length = destination->length + source.length;
    return destination;
}

/*
 * Appends a string_t (source) into another string_t (destination).
 * Also frees the appended string_t.
 */
string_t *str_pushf(string_t *destination, string_t source) {
    string_t *res = str_push(destination, source);
    str_free(source);
    return res;
}

/*
 * Prints the {string} to {stream}
 */
void str_print(string_t string, FILE *stream) {
    for (size_t i = 0; i < string.length; i++) {
        putc(string.text[i], stream);
    }
}

/*
 * Prints the {string} to {stream}.
 * Also frees the printed string_t.
 */
void str_printf(string_t string, FILE *stream) {
    str_print(string, stream);
    str_free(string);
}

/*
 * Prints the {string} to {stream}
 */
ssize_t str_write(string_t string, int fd) {
    return write(fd, string.text, string.length);
}

/*
 * Prints the {string} to {stream}
 */
ssize_t str_writef(string_t string, int fd) {
    ssize_t ret = str_write(string, fd);
    str_free(string);
    return ret;
}

/*
 * Frees the allocated memory from a string_t
 */
void str_free(string_t string) {
    free(string.text);
}

