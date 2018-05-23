/*
 * connection_establisher.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#include "connection_establisher.h"

#include <lsquic.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <event.h>
#include <string.h>

#include "address_finder.h"
#include "socket_handler.h"
#include "simple_list.h"
#include "event_handlers.h"

connection_parameters* create_connection(parsed_url_t* parsed_url, int is_ipv4, quic_engine_parameters* engine_ref)
{
    connection_parameters* connection = malloc(sizeof(connection_parameters));

    connection->hostname = calloc(sizeof(char), parsed_url->hostname_len);
    strcpy(connection->hostname, parsed_url->hostname);

    connection->hostname_len = parsed_url->hostname_len;
    connection->stream_list = init_simple_list();
    get_sockaddr(parsed_url->hostname, &connection->destination_addr, is_ipv4);
    connection->quic_engine_params_ref = (void*) engine_ref;
    return connection;
}

stream_parameters_t* create_connection_stream(parsed_url_t* parsed_url)
{
    stream_parameters_t* stream_ref = malloc(sizeof(stream_parameters_t));
    stream_ref->is_header_sent = 0;
    stream_ref->is_created = 0;
    stream_ref->path = malloc(parsed_url->path_len);
    strcpy(stream_ref->path, parsed_url->path);
    stream_ref->path_len = parsed_url->path_len;
    return stream_ref;
}

void prepare_stream_output(stream_parameters_t* stream, const char* url, struct cycle_buffer* memory_buffer)
{
    stream->data_pointer = malloc(sizeof(struct output_data));
    stream->data_pointer->url_ref = &url;
    stream->data_pointer->url_size = strlen(url);
    stream->data_pointer->data_ref = get_memory_pointer(memory_buffer);
    stream->data_pointer->data_size = 0;
    stream->data_pointer->has_error = 0;
    stream->data_pointer->error_str = NULL;
    stream->buffer_left = memory_buffer->partition_size;
}

void quic_connect(quic_engine_parameters* quic_engine_ref, const char* url)
{
    parsed_url_t parsed_url;
    parse_url(url, &parsed_url);
    connection_parameters* connection = find_connection(quic_engine_ref->connections, parsed_url.hostname);
    if (connection == NULL)
    {
        unsigned short max_packet_size = quic_engine_ref->program_args.is_ipv4?
                QUIC_IPV4_MAX_PACKET_SIZE : QUIC_IPV6_MAX_PACKET_SIZE;
        connection = create_connection(&parsed_url, quic_engine_ref->program_args.is_ipv4, quic_engine_ref);
        insert_list_element(quic_engine_ref->connections, 0, (void*)connection);
        connection->connection_ref = lsquic_engine_connect(quic_engine_ref->engine_ref,
                (struct sockaddr*) &connection->destination_addr, NULL, (lsquic_conn_ctx_t*)connection,
                        connection->hostname, max_packet_size);
    }
    stream_parameters_t* new_stream_ref = create_connection_stream(&parsed_url);
    new_stream_ref->connection_params_ref = (void*)connection;
    insert_list_element(connection->stream_list, 0, (void*)new_stream_ref);

    prepare_stream_output(new_stream_ref, url, &quic_engine_ref->output_result_buffer);
    quic_engine_ref->urls_left++;
    lsquic_conn_make_stream(connection->connection_ref);
}



// more complex context is destroyed on close events
void destroy_stream(stream_parameters_t* stream_ref)
{
    lsquic_stream_close(stream_ref->stream_ref);
}

void destroy_connections(quic_engine_parameters* quic_engine_ref)
{
    struct enumerator* conn_enumerator = get_enumerator(quic_engine_ref->connections);
    for(connection_parameters* connection = (connection_parameters*)get_next_element(conn_enumerator);
            connection != NULL; connection = (connection_parameters*)get_next_element(conn_enumerator))
    {
        struct enumerator* stream_enumerator = get_enumerator(connection->stream_list);
        for(stream_parameters_t* stream = (stream_parameters_t*)get_next_element(stream_enumerator);
            stream != NULL; stream = (stream_parameters_t*)get_next_element(stream_enumerator))
        {
            destroy_stream(stream);
        }
        lsquic_conn_close(connection->connection_ref);
        free(stream_enumerator);
    }
    free(conn_enumerator);
}


