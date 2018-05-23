/*
 * quic_engine_holder.c
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */


#include "quic_engine_holder.h"

#include <stdio.h>
#include <lsquic.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "address_finder.h"
#include "connection_init_handler.h"
#include "connection_read_handler.h"
#include "connection_write_handler.h"
#include "socket_handler.h"
#include "connection_params.h"
#include "socket_buffer.h"
#include "connection_establisher.h"
#include "event_handlers.h"

const struct lsquic_stream_if http_client_if = {
    .on_new_conn            = http_client_on_new_conn,
    .on_conn_closed         = http_client_on_conn_closed,
    .on_new_stream          = http_client_on_new_stream,
    .on_read                = http_client_on_read,
    .on_write               = http_client_on_write,
    .on_close               = http_client_on_stream_close,
};

#define QUIC_PORT_NUMBER 443

int printlog(void *logger_ctx, const char *fmt, va_list args)
{
	return printf(fmt, args);
}

void allocate_output(quic_engine_parameters* quic_engine_ref)
{
    init_memory_buffer(&quic_engine_ref->output_result_buffer, quic_engine_ref->program_args.buffer_size);
	quic_engine_ref->output_stack_ref = init_stack();
}


void init_engine(quic_engine_parameters* quic_engine_ref)
{
	lsquic_engine_init_settings(&quic_engine_ref->engine_settings, LSENG_HTTP);
	quic_engine_ref->engine_api.ea_settings = &quic_engine_ref->engine_settings;
	quic_engine_ref->engine_api.ea_stream_if = &http_client_if;
	quic_engine_ref->engine_api.ea_stream_if_ctx = (void*)quic_engine_ref;
	//	// set a parameters for each connection event
	quic_engine_ref->engine_api.ea_packets_out = on_packets_out;
	quic_engine_ref->engine_api.ea_packets_out_ctx = (void*)quic_engine_ref;
	quic_engine_ref->engine_api.ea_pmi = NULL;
	quic_engine_ref->engine_api.ea_pmi_ctx = NULL;
	lsquic_global_init(LSQUIC_GLOBAL_CLIENT);

	quic_engine_ref->engine_ref = lsquic_engine_new(LSENG_HTTP, &quic_engine_ref->engine_api);
		// 443 - quic udp port

	if (open_socket(quic_engine_ref->program_args.is_ipv4, &quic_engine_ref->local_addr, &quic_engine_ref->socket_id,
					&quic_engine_ref->socket_buffer_size, quic_engine_ref->program_args.local_port_number) == -1)
	{
		printf("socket %d was opened", QUIC_PORT_NUMBER);
	}
	int max_packet_size = quic_engine_ref->program_args.is_ipv4? QUIC_IPV4_MAX_PACKET_SIZE : QUIC_IPV6_MAX_PACKET_SIZE;
	init_socket_buffer(&quic_engine_ref->socket_buffer, quic_engine_ref->socket_id, max_packet_size,
					   quic_engine_ref->socket_buffer_size);
	allocate_output(quic_engine_ref);
	quic_engine_ref->connections = init_simple_list();
	quic_engine_ref->urls_left = 0;

	// create event base and hang 2 events upon
	quic_engine_ref->events.event_base_ref = event_base_new();

	quic_engine_ref->events.event_time_ref = event_new(quic_engine_ref->events.event_base_ref, -1, 0, timer_handler,
			(void*)quic_engine_ref);
	quic_engine_ref->events.event_socket_read_ref =	event_new(quic_engine_ref->events.event_base_ref,
			quic_engine_ref->socket_id, EV_READ|EV_PERSIST, read_socket_event, (void*)quic_engine_ref);

	event_add(quic_engine_ref->events.event_socket_read_ref, NULL);
	quic_engine_ref->loop_thread = 0;

#ifdef DEBUG
	// test
	// init log
	quic_engine_ref->logger.vprintf = printlog;
	lsquic_logger_init(&quic_engine_ref->logger, NULL, LLTS_HHMMSSMS);
	lsquic_set_log_level("info");
#endif
}

void destroy_events(event_timer_t* events)
{
	event_free(events->event_time_ref);
	event_free(events->event_socket_read_ref);
	event_base_free(events->event_base_ref);
}

void destroy_engine(quic_engine_parameters* quic_engine_ref)
{
	destroy_connections(quic_engine_ref);
	if (quic_engine_ref->loop_thread)
		pthread_cancel(quic_engine_ref->loop_thread);
	destroy_events(&quic_engine_ref->events);
	lsquic_engine_destroy(quic_engine_ref->engine_ref);


	destroy_socket_buffer(&quic_engine_ref->socket_buffer);
	destroy_stack(quic_engine_ref->output_stack_ref);
	destroy_memory_buffer(&quic_engine_ref->output_result_buffer);

	close_socket(quic_engine_ref->socket_id);


	lsquic_global_cleanup();

}
