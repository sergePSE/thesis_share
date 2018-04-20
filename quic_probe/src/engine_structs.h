/*
 * engine_structs.h
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#ifndef SRC_ENGINE_STRUCTS_H_
#define SRC_ENGINE_STRUCTS_H_

#include <lsquic.h>
#include <lsquic_types.h>
#include <sys/socket.h>

struct packets_out_ctx_def
{
	int socket_id;
	lsquic_conn_ctx_t* conn_ctx_ref;
};

typedef struct quic_engine {
	lsquic_engine_t* engine_ref;
	lsquic_conn_t* connection_ref;
	struct sockaddr local_addr;
	struct packets_out_ctx_def connection_parameters;
} quic_engine;

#endif /* SRC_ENGINE_STRUCTS_H_ */
