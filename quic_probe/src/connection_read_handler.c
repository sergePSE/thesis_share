/*
 * connection_read_handler.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#include "connection_read_handler.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include "engine_structs.h"

void http_client_on_read (lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h)
{

}

int on_packets_out(void *packets_out_ctx, const struct lsquic_out_spec  *out_spec, unsigned n_packets_out)
{
	struct iovec iov;
	struct packets_out_ctx_def* ctx = packets_out_ctx;
	struct msghdr message_header;
	message_header.msg_iov = &iov;
	int send_status;
	for(int i = 0; i < n_packets_out; i++)
	{
		iov.iov_base = (void *) out_spec[i].buf;
		iov.iov_len = out_spec[i].sz;
		message_header.msg_name       = (void *) out_spec[i].dest_sa;
		message_header.msg_namelen    = out_spec[i].dest_sa->sa_family == AF_INET? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
		message_header.msg_iov        = &iov;
		message_header.msg_iovlen     = 1;
		message_header.msg_flags      = 0;
		message_header.msg_control 	  = NULL;
		message_header.msg_controllen = 0;
		send_status = sendmsg(ctx->socket_id, &message_header, 0);
	}

	return n_packets_out;
}
