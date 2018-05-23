/*
 * connection_read_handler.c
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#include "connection_read_handler.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "engine_structs.h"

ssize_t read_to_buffer(stream_parameters_t* stream)
{
	ssize_t nread = lsquic_stream_read(stream->stream_ref, stream->data_pointer->data_ref +
			stream->data_pointer->data_size, stream->buffer_left);
	stream->buffer_left -= nread;
	stream->data_pointer->data_size += nread;
	return nread;
}

void http_client_on_read (lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h)
{
	stream_parameters_t* stream_parameters = (stream_parameters_t*) st_h;
	// todo manage errors
    ssize_t read_len = read_to_buffer(stream_parameters);

    if (read_len == 0)
	{
		connection_parameters* conn = stream_parameters->connection_params_ref;
		quic_engine_parameters* engine = conn->quic_engine_params_ref;
		lsquic_stream_shutdown(stream, 0);
		engine->reading_streams_count--;
		put_stack_element(engine->output_stack_ref, stream_parameters->data_pointer);
		engine->urls_left--;
	}
    while (read_len > 0)
    {
		read_len = read_to_buffer(stream_parameters);
    }
}


int on_packets_out(void *packets_out_ctx, const struct lsquic_out_spec *out_spec, unsigned n_packets_out)
{
	struct iovec iov;
	quic_engine_parameters* engine_params_ref = (quic_engine_parameters*)packets_out_ctx;
	struct msghdr message_header;

	message_header.msg_iov = &iov;

	struct sockaddr destination_addr;
	for(unsigned int i = 0; i < n_packets_out; i++)
	{
	    memcpy(&destination_addr, out_spec[i].dest_sa, sizeof(&out_spec[i].dest_sa));
		iov.iov_base = (void *) out_spec[i].buf;
		iov.iov_len = out_spec[i].sz;
		message_header.msg_name       = (void*)&destination_addr;
		message_header.msg_namelen    = out_spec[i].dest_sa->sa_family == AF_INET?
										sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
		message_header.msg_iov        = &iov;
		message_header.msg_iovlen     = 1;
		message_header.msg_flags      = 0;
		message_header.msg_control 	  = NULL;
		message_header.msg_controllen = 0;

		ssize_t send_status = sendmsg(engine_params_ref->socket_id, &message_header, 0);
		// packet sending was failed
		if (send_status < 0)
		{
			puts(strerror(errno));
		}
	}
	return n_packets_out;
}
