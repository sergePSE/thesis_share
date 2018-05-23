//
// Created by sergei on 16.05.18.
//


#include "engine_structs.h"
#include "lsquic_types.h"

#define ERROR_BUFFER_SIZE 100

void remove_connection(connection_parameters* connection)
{
    free(connection->hostname);
    destroy_simple_list(connection->stream_list);
    quic_engine_parameters* engine = (quic_engine_parameters*)connection->quic_engine_params_ref;
    remove_list_element_by_data(engine->connections, connection);
}

void clean_connections(struct simple_list* connection_list)
{
    char error_buffer[ERROR_BUFFER_SIZE];
    struct enumerator* list_enumerator = get_enumerator(connection_list);
    for(connection_parameters* conn_parameters = get_next_element(list_enumerator); conn_parameters != NULL;
            conn_parameters = get_next_element(list_enumerator))
    {
        enum LSQUIC_CONN_STATUS conn_status = lsquic_conn_status(conn_parameters->connection_ref, error_buffer,
                ERROR_BUFFER_SIZE);
        if(conn_status != LSCONN_ST_CONNECTED || conn_status != LSCONN_ST_HSK_IN_PROGRESS)
        {
            remove_connection(conn_parameters);
        }
    }
    free(list_enumerator);
}

void schedule_engine(quic_engine_parameters* quic_engine_ref)
{
    if (quic_engine_ref->urls_left == 0)
    {
        event_base_loopbreak(quic_engine_ref->events.event_base_ref);
        return;
    }

    lsquic_engine_process_conns(quic_engine_ref->engine_ref);
    int diff_ns;
    struct timeval timeout;

    if (lsquic_engine_earliest_adv_tick(quic_engine_ref->engine_ref, &diff_ns))
    {
        timeout.tv_sec = (unsigned) diff_ns / 1000000;
        timeout.tv_usec = (unsigned) diff_ns % 1000000;

        event_add(quic_engine_ref->events.event_time_ref, &timeout);
    }
    //clean_connections(quic_engine_ref->connections);
};

void timer_handler (int fd, short what, void *arg)
{
    quic_engine_parameters* quic_engine_ref = (quic_engine_parameters*)arg;
    schedule_engine(quic_engine_ref);
}

#include "event_handlers.h"
