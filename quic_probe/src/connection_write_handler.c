/*
 * connection_write_handler.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */
#include <memory.h>
#include <stdlib.h>
#include "connection_write_handler.h"
#include "engine_structs.h"

void send_header(stream_parameters_t* stream_ref, lsquic_stream_t *stream)
{
    connection_parameters* connection = (connection_parameters*)stream_ref->connection_params_ref;
    lsquic_http_header_t headers_arr[] = {
            {
                    .name  = { ":authority",     10, },
                    .value = { .iov_base = (void *) connection->hostname,
                            .iov_len = connection->hostname_len },
            },
            {
                    .name  = { .iov_base = ":method",       .iov_len = 7, },
                    .value = { .iov_base = (void *)"GET",          .iov_len = 3, },
            },
            {
                    .name  = { .iov_base = ":path",         .iov_len = 5, },
                    .value = { .iov_base = (void *) stream_ref->path,
                            .iov_len = stream_ref->path_len },
            },
            {
                    .name  = { .iov_base = ":scheme",       .iov_len = 7, },
                    .value = { .iov_base = (void *)"https",          .iov_len = 5, }
            },
            {
                    .name  = { .iov_base = "accept",         .iov_len = 6, },
                    .value = { .iov_base = (void *) "*/*",   .iov_len = 3 },
            },
            // always redirects to youtube.com
            {
                    .name  = { .iov_base = "origin",         .iov_len = 6, },
                    .value = { .iov_base = (void *) "https://www.youtube.com",   .iov_len = 23 },
            },
            {
                    .name  = { .iov_base = "referer",         .iov_len = 7, },
                    .value = { .iov_base = (void *) "https://www.youtube.com/",   .iov_len = 24 },
            }
    };
    lsquic_http_headers_t headers = {
            .count = sizeof(headers_arr) / sizeof(headers_arr[0]),
            .headers = headers_arr,
    };
    if (0 != lsquic_stream_send_headers(stream, &headers, 1))
    {
        printf("cannot send headers: %s", strerror(errno));
        return;
    }
}

void increment_engine_reading_streams(stream_parameters_t* stream)
{
    connection_parameters* connection = (connection_parameters*)stream->connection_params_ref;
    quic_engine_parameters* engine = (quic_engine_parameters*)connection->quic_engine_params_ref;
    engine->reading_streams_count++;
}

void http_client_on_write (lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h)
{
    stream_parameters_t* stream_ref = (stream_parameters_t*) st_h;
    if (stream_ref->is_header_sent)
    {
        // close stream and read all incoming packets
        lsquic_stream_shutdown(stream, 1);
        lsquic_stream_wantread(stream, 1);
        increment_engine_reading_streams(stream_ref);
    }
    else
    {
        send_header(stream_ref, stream);
        stream_ref->is_header_sent = 1;
    }
}
