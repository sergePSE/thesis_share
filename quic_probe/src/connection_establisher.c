/*
 * connection_establisher.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#include "connection_establisher.h"

#include <lsquic.h>
#include <sys/socket.h>

#include "address_finder.h"

void quic_connect(quic_engine* quic_engine_ref, const char* url, int is_ipv4)
{
	struct sockaddr destination;
	get_sockaddr_for_url(url, &destination, is_ipv4);
	quic_engine_ref->connection_ref = lsquic_engine_connect(quic_engine_ref->engine_ref, &destination, NULL, NULL, url, 0);
	lsquic_engine_process_conns(quic_engine_ref->engine_ref);
}

void destroy(quic_engine* quic_engine_ref)
{
	lsquic_conn_close(quic_engine_ref->connection_ref);
}


