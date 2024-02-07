#include "../include/http.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/string.h"

typedef struct {
    int sockfd;
    uint8_t buf[256];
    ssize_t length;
    ssize_t index;
} tcp_buffer_t;

/*
 * After running this functions, buffer->length should be checked.
 * [buffer->length == 0] indicates EOF
 * [buffer->length < 0] indicates Error
 */
void tcp_buffer_read(tcp_buffer_t *buffer, uint8_t *out_byte) {
    if (buffer->index >= buffer->length) {
        buffer->length = read(buffer->sockfd, buffer->buf, 256);
        if (buffer->length <= 0) {
            return;
        }
        buffer->index = 0;
    }
    *out_byte = buffer->buf[buffer->index++];
}

ssize_t read_until(tcp_buffer_t *buffer, char match, string_t *out_string) {
    uint8_t byte;
    while (true) {
        tcp_buffer_read(buffer, &byte);
        if (buffer->length == 0) {
            perror("[HTTP] Could not read the input (1)");
            return buffer->length;
        } else if (buffer->length < 0) {
            perror("[HTTP] Could not read the input (2)");
            return buffer->length;
        }
        if (byte == match) {
            break;
        } else {
            str_pushf(out_string, str_char(byte));
        }
    }
    return 1;
}

ssize_t read_until2(tcp_buffer_t *buffer, char match1, char match2, string_t *out_string) {
    uint8_t byte;
    while (true) {
        tcp_buffer_read(buffer, &byte);
        if (buffer->length == 0) {
            perror("[HTTP] Could not read the input (1)");
            return buffer->length;
        } else if (buffer->length < 0) {
            perror("[HTTP] Could not read the input (2)");
            return buffer->length;
        }
        if (byte == match1) {
            tcp_buffer_read(buffer, &byte);
            if (buffer->length == 0) {
                perror("[HTTP] Could not read the input (1)");
                return buffer->length;
            } else if (buffer->length < 0) {
                perror("[HTTP] Could not read the input (2)");
                return buffer->length;
            }
            if (byte == match2) {
                break;
            }

            str_pushf(out_string, str_char(match1));
            str_pushf(out_string, str_char(match2));
        } else {
            str_pushf(out_string, str_char(byte));
        }
    }
    return 1;
}

/*
 * Returns:
 * - 0 - when a filled line is recognized (key and value are set)
 * - 1 - when an empty line is recognized (key and value are unchanged)
 * - 2 - when a valid input is not recognized (key and value might be changed)
 */
int parse_header_line(tcp_buffer_t *buffer, string_t *key, string_t *value) {
    uint8_t byte;
    size_t state = 0;

    while (state < 7) {
        tcp_buffer_read(buffer, &byte);
        if (buffer->length == 0) {
            // EOF
        } else if (buffer->length == 1) {
            // EOF
        }

        switch (state) {
            case 0:
                // fprintf(stderr, "[state = 0, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case ':':
                        state = 2;
                        break;
                    case '\r':
                        state = 5;
                        break;
                    default:
                        str_pushf(key, str_char(byte));
                        state = 1;
                        break;
                }
                break;

            case 1:
                // fprintf(stderr, "[state = 1, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case ':':
                        state = 2;
                        break;
                    case '\r':
                        state = 6;
                        break;
                    default:
                        str_pushf(key, str_char(byte));
                        state = 1;
                        break;
                }
                break;

            case 2:
                // fprintf(stderr, "[state = 2, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case ' ':
                        state = 3;
                        break;
                    case ':':
                        str_pushf(key, str_char(':'));
                        state = 2;
                        break;
                    case '\r':
                        str_pushf(key, str_char(':'));
                        state = 6;
                        break;
                    default:
                        str_pushf(key, str_char(byte));
                        state = 1;
                        break;
                }
                break;

            case 3:
                // fprintf(stderr, "[state = 3, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case '\r':
                        state = 4;
                        break;
                    default:
                        str_pushf(value, str_char(byte));
                        state = 3;
                        break;
                }
                break;

            case 4:
                // fprintf(stderr, "[state = 4, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case '\n':
                        state = 7;
                        break;
                    case '\r':
                        str_pushf(value, str_char('\r'));
                        state = 4;
                        break;
                    default:
                        str_pushf(value, str_char('\r'));
                        str_pushf(value, str_char(byte));
                        state = 3;
                        break;
                }
                break;

            case 5:
                // fprintf(stderr, "[state = 5, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case '\n':
                        state = 8;
                        break;
                    case '\r':
                        str_pushf(key, str_char('\r'));
                        state = 6;
                        break;
                    default:
                        str_pushf(key, str_char('\r'));
                        state = 1;
                        break;
                }
                break;

            case 6:
                // fprintf(stderr, "[state = 6, char = %hhx (", byte);
                // print_char(stderr, byte);
                // fprintf(stderr, ")]\n");
                switch (byte) {
                    case '\n':
                        state = 9;
                        break;
                    case '\r':
                        str_pushf(key, str_char('\r'));
                        state = 6;
                        break;
                    case ':':
                        str_pushf(key, str_char('\r'));
                        state = 2;
                        break;
                    default:
                        str_pushf(key, str_char('\r'));
                        str_pushf(key, str_char(byte));
                        state = 1;
                        break;
                }
                break;
        }
    }

    if (state == 7) {
        // fprintf(stderr, "Read header (keylen = %zd) (vallen = %zd)\n", key.length, value.length);
        // str_print(key, stdout);
        // str_printf(str_char('\n'), stdout);
        // str_print(value, stdout);
        // str_printf(str_char('\n'), stdout);
        // str_free(key);
        // str_free(value);
        return 0;
    } else if (state == 8) {
        // fprintf(stderr, "No header (ok)\n");
        // str_free(key);
        // str_free(value);
        return 1;
    } else if (state == 9) {
        // fprintf(stderr, "Input error\n");
        // str_free(key);
        // str_free(value);
        return 2;
    } else {
        // fprintf(stderr, "Input was not recognized\n");
        // str_free(key);
        // str_free(value);
        return 2;
    }
}

bool_t parse_request(tcp_buffer_t *buffer, http_request_t *request) {
    if (read_until(buffer, ' ', &request->method) <= 0) {
        return false;
    }

    if (read_until(buffer, ' ', &request->path) <= 0) {
        return false;
    }

    if (read_until2(buffer, '\r', '\n', &request->http_version) <= 0) {
        return false;
    }

    while (true) {
        string_t key = string_new();
        string_t value = string_new();

        int res = parse_header_line(buffer, &key, &value);
        if (res == 0) {
            // TODO: handle header
            str_printf(str_cstr("Key: "), stdout);
            str_print(key, stdout);
            str_printf(str_char('\n'), stdout);

            str_printf(str_cstr("Value: "), stdout);
            str_print(value, stdout);
            str_printf(str_char('\n'), stdout);
        } else if (res == 1) {
            break;
        } else {
            str_printf(str_cstr("Could not recognize header line\n"), stdout);
            return false;
        }

        str_free(key);
        str_free(value);
    }

    return true;
}

#define sock_write(sockfd, string_literal) { char buf[] = string_literal; write(sockfd, buf, sizeof(buf)-1); }
bool_t write_response(int sockfd, http_response_t *response) {
    // Header
    sock_write(sockfd, "HTTP/1.1 ");
    str_writef(str_uint(response->status_code, 10), sockfd);
    sock_write(sockfd, " ");
    str_writef(response->status_message, sockfd);
    sock_write(sockfd, "\r\n");

    // Content-Length
    str_writef(str_cstr("Content-Length: "), sockfd);
    str_writef(str_uint(response->content_length, 10), sockfd);
    str_writef(str_cstr("\r\n"), sockfd);

    // End headers
    str_writef(str_cstr("\r\n"), sockfd);

    // Content
    write(sockfd, response->content, response->content_length);

    return true;
}

void handle_connection(connection_t *connection) {
    http_request_t request;
    http_response_t response;
    tcp_buffer_t buffer;
    uint8_t byte;

    buffer.sockfd = connection->sockfd;
    buffer.index = 0;
    buffer.length = 0;

    while (true) {
        request.method = string_new();
        request.path = string_new();
        request.http_version = string_new();

        if (!parse_request(&buffer, &request)) {
            // TODO: Handle request parse error
            str_printf(str_cstr("An error happened while parsing the request\n"), stderr);
            uint8_t buf[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 16\r\nConnection: close\r\n\r\nCould not parse\n";
            write(connection->sockfd, buf, sizeof(buf)-1);
            break;
        }

        handle_request(&request, &response);

        str_free(request.method);
        str_free(request.path);
        str_free(request.http_version);

        if (!write_response(connection->sockfd, &response)) {
            // TODO: handle error
            str_printf(str_cstr("An error happened while writing the response\n"), stderr);
            break;
        }

        uint8_t buf[] = "HTTP/1.1 200 Ok\r\nContent-Length: 14\r\nConnection: close\r\n\r\nHello, World!\n";
        write(connection->sockfd, buf, sizeof(buf)-1);
    }
}

void handle_request(http_request_t *request, http_response_t *response) {
    response->status_code = 200;
    response->status_message = str_cstr("Ok");

    char buf[] = "Hello from handle_request(...)\n";
    response->content = calloc(sizeof(buf)-1, sizeof(char));
    memcpy(response->content, buf, sizeof(buf)-1);
    response->content_length = sizeof(buf)-1;
}
