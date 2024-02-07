#ifndef HTTP
#define HTTP

#include "../include/thread_pool.h"
#include "../include/string.h"
#include "../include/list.h"

typedef struct {
    string_t key;
    string_t value;
} http_header_entry_t;

typedef struct {
    http_header_entry_t *items;
    size_t length;
    size_t capacity;
} http_headers_t;

typedef struct {
    // Connection data
    uint16_t server_port;
    uint32_t client_ip;
    uint16_t client_port;

    // Request data
    string_t method;
    string_t path;
    string_t http_version;
    http_headers_t headers;
} http_request_t;

typedef struct {
    uint32_t status_code;
    string_t status_message;
    uint32_t content_length;
    uint8_t *content;
    http_headers_t headers;
} http_response_t;

void handle_request(http_request_t *request, http_response_t *response);

void handle_connection(connection_t *connection);

#endif
