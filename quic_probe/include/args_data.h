//
// Created by sergei on 14.05.18.
//

#ifndef QUIC_PROBE_OUTPUT_DATA_H
#define QUIC_PROBE_OUTPUT_DATA_H

#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct quic_args {
    int is_ipv4;

    const char* quic_ver_str;
    size_t quic_ver_str_size;

    size_t timeout_ms;
    int use_prev_conn_data;

    uint local_port_number;
    unsigned char max_streams;
} quic_args;

struct url {
    char* url_data;
    size_t url_len;
};

struct download_buffer {
    char* buffer;
    size_t allocated_size;
    size_t used_size;
};

struct download_request{
    struct url url_request;
    struct download_buffer buffer;
};

struct output_data {
    int has_error;

    char* error_str;
    size_t error_size;

    struct download_request* request_args;
    struct url final_url;

    struct timespec connection_time;
    struct timespec download_time;
};

extern void destroy_output_data_container(struct output_data* data_ref);
extern struct output_data* init_output_data_container(char* url, size_t url_size);


#endif //QUIC_PROBE_OUTPUT_DATA_H
