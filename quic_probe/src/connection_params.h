//
// Created by sergei on 07.05.18.
//

#ifndef QUIC_PROBE_CONNECTION_PARAMS_H
#define QUIC_PROBE_CONNECTION_PARAMS_H

#include <lsquic_types.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "simple_list.h"
#include "args_data.h"
#include "text.h"

typedef struct parsed_url {
    text_t* hostname;
    text_t* path;
} parsed_url_t;

typedef struct stream_parameters {
    int is_header_sent;

    text_t* path;

    lsquic_stream_t *stream_ref;
    void* connection_params_ref;

    int is_created;
    struct output_data* data_pointer;
    size_t buffer_left;
    char* data_ref;
    struct timespec start_time;
    int is_closed;
    int is_read;

} stream_parameters_t;

typedef struct connection_params {
    struct sockaddr_storage destination_addr;
    struct simple_list* stream_list;
    lsquic_conn_t* connection_ref;

    text_t* hostname;

    struct timespec start_time;
    struct timespec handshake_time;

    void* quic_engine_params_ref;
} connection_parameters;

connection_parameters* find_connection(struct simple_list* connections, const char* hostname);

#endif //QUIC_PROBE_CONNECTION_PARAMS_H
