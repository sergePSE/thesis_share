/*
 * connection_init_handler.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#include "connection_init_handler.h"
#include "engine_structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

void http_client_on_stream_close(lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h)
{
    if (st_h == NULL)
        return;
    stream_parameters_t* stream_params = (stream_parameters_t*)st_h;
    connection_parameters* conn_parameters = stream_params->connection_params_ref;
    remove_list_element_by_data(conn_parameters->stream_list, (void*)stream_params);
    free(stream_params->path);
    free(stream_params);
}

void http_client_on_conn_closed (lsquic_conn_t *conn)
{
    connection_parameters* connection = (connection_parameters*)lsquic_conn_get_ctx(conn);
    free(connection->hostname);
    destroy_simple_list(connection->stream_list);
    quic_engine_parameters* engine = (quic_engine_parameters*)connection->quic_engine_params_ref;
    remove_list_element_by_data(engine->connections, connection);
    free(conn);
}

stream_parameters_t* find_not_created_stream_parameters(connection_parameters* connection)
{
    struct enumerator* enum_conn = get_enumerator(connection->stream_list);
    stream_parameters_t* stream_params = (stream_parameters_t*)get_next_element(enum_conn);
    while (stream_params != NULL)
    {
        if (!stream_params->is_created)
            break;
        stream_params = (stream_parameters_t*)get_next_element(enum_conn);
    }
    free(enum_conn);
    return stream_params;
}

lsquic_stream_ctx_t * http_client_on_new_stream (void* stream_if_ctx, lsquic_stream_t *stream)
{
    lsquic_conn_t* stream_connection = lsquic_stream_conn(stream);
    connection_parameters* conn_params = (connection_parameters*)lsquic_conn_get_ctx(stream_connection);
    if (conn_params == NULL)
        return NULL;
    stream_parameters_t* free_stream = find_not_created_stream_parameters(conn_params);
    free_stream->is_created = 1;
    free_stream->is_header_sent = 0;
    free_stream->stream_ref = stream;
    lsquic_stream_wantwrite(stream, 1);
    return (void*)free_stream;
}

lsquic_conn_ctx_t * http_client_on_new_conn (void* conn_if_ctx, lsquic_conn_t* conn)
{
	return lsquic_conn_get_ctx(conn);
}