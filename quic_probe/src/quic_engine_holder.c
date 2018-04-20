/*
 * quic_engine_holder.c
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */


#include "quic_engine_holder.h"

#include <stdio.h>
#include <lsquic.h>

#include "address_finder.h"
#include "connection_init_handler.h"
#include "connection_read_handler.h"
#include "connection_write_handler.h"
#include "socket_handler.h"

const struct lsquic_stream_if http_client_if = {
    .on_new_conn            = http_client_on_new_conn,
    .on_conn_closed         = http_client_on_conn_closed,
    .on_new_stream          = http_client_on_new_stream,
    .on_read                = http_client_on_read,
    .on_write               = http_client_on_write,
    .on_close               = http_client_on_close,
};

void init_engine(quic_engine* quic_engine_ref, enum lsquic_version version)
{
	struct lsquic_engine_api api;
	// 443 - quic udp port
	quic_engine_ref->connection_parameters.socket_id = open_socket(443);
	api.ea_stream_if = &http_client_if;

	struct lsquic_engine_settings engine_settings;
	lsquic_engine_init_settings(&engine_settings, LSENG_HTTP);
	api.ea_settings = &engine_settings;
	lsquic_global_init(LSQUIC_GLOBAL_CLIENT);

	// set a parameters for each connection event
	api.ea_packets_out_ctx = (void*)&quic_engine_ref->connection_parameters;
	api.ea_packets_out = on_packets_out;

	quic_engine_ref->engine_ref = lsquic_engine_new(LSENG_HTTP, &api);
}

void destroy_engine(quic_engine* quic_engine_ref)
{
	lsquic_engine_destroy(quic_engine_ref->engine_ref);
}
