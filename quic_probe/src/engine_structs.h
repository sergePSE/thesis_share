/*
 * engine_structs.h
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#ifndef SRC_ENGINE_STRUCTS_H_
#define SRC_ENGINE_STRUCTS_H_

#include <sys/socket.h>
#include <event.h>
#include <pthread.h>

#include "lsquic.h"
#include "connection_params.h"
#include "socket_buffer.h"
#include "output_data.h"
#include "stack.h"
#include "cycle_buffer.h"
#include "simple_list.h"

// standard default value to the library documentation
#define QUIC_IPV4_MAX_PACKET_SIZE 1370
#define QUIC_IPV6_MAX_PACKET_SIZE 1350

typedef struct quic_args {
	int is_ipv4;
	char* quic_ver_str;
	size_t quic_ver_str_size;
	size_t timeout_ms;
	int use_prev_conn_data;
	size_t buffer_size;
	int local_port_number;
} quic_args;

typedef struct event_timer {
    struct event* event_time_ref;
    struct event_base* event_base_ref;
    struct event* event_socket_read_ref;
} event_timer_t;

typedef struct quic_engine_shell {
	quic_args program_args;

	lsquic_engine_t* engine_ref;
	struct lsquic_engine_api engine_api;
	struct lsquic_engine_settings engine_settings;

	struct sockaddr_storage local_addr;

	struct simple_list* connections;

	event_timer_t events;

	struct lsquic_logger_if logger;

	struct socket_data socket_buffer;
	int socket_id;
	socklen_t socket_buffer_size;

	struct primitive_stack* output_stack_ref;
	struct cycle_buffer output_result_buffer;

	unsigned char urls_left;
    pthread_t loop_thread;

    unsigned int reading_streams_count;

} quic_engine_parameters;

#endif /* SRC_ENGINE_STRUCTS_H_ */
