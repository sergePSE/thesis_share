/*
 * connection_init_handler.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#include "connection_init_handler.h"

#include <stdio.h>

void http_client_on_close (lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h)
{

}

void http_client_on_conn_closed (lsquic_conn_t *conn)
{

}

lsquic_stream_ctx_t * http_client_on_new_stream (void *stream_if_ctx, lsquic_stream_t *stream)
{

    return NULL;
}

lsquic_conn_ctx_t * http_client_on_new_conn (void* stream_if_ctx, lsquic_conn_t* conn)
{
	lsquic_conn_make_stream(conn);
	lsquic_conn_ctx_t* conn_context = lsquic_conn_get_ctx(conn);
	return conn_context;

}
