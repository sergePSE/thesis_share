//
// Created by sergei on 07.05.18.
//

#ifndef QUIC_PROBE_CONNECTION_PARAMS_H
#define QUIC_PROBE_CONNECTION_PARAMS_H

#include <lsquic_types.h>
#include <stddef.h>
#include <sys/socket.h>

#include "simple_list.h"
#include "output_data.h"

typedef struct parsed_url {
    char* hostname;
    size_t hostname_len;
    char* path;
    size_t path_len;
} parsed_url_t;

typedef struct stream_parameters{
    int is_header_sent;
    char* path;
    size_t path_len;
    lsquic_stream_t *stream_ref;
    void* connection_params_ref;
    int is_created;
    struct output_data* data_pointer;
    size_t buffer_left;
} stream_parameters_t;

typedef struct connection_params {
    struct sockaddr_storage destination_addr;
    struct simple_list* stream_list;
    lsquic_conn_t* connection_ref;
    char* hostname;
    size_t hostname_len;

    void* quic_engine_params_ref;
} connection_parameters;

connection_parameters* find_connection(struct simple_list* connections, const char* hostname);

#endif //QUIC_PROBE_CONNECTION_PARAMS_H
