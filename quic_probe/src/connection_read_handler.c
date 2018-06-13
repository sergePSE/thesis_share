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

#include "timedifference.h"
#include "engine_structs.h"
#include "html_header_parser.h"
#include "connection_establisher.h"

#define ERROR_MESSAGE_SIZE 200

ssize_t read_to_buffer(stream_parameters_t* stream)
{
	ssize_t nread = lsquic_stream_read(stream->stream_ref, (void*)stream->data_ref,	stream->buffer_left);
	stream->data_pointer->request_args->buffer.used_size += nread;
	stream->buffer_left -= nread;
	stream->data_ref += nread;
	if (stream->buffer_left <= 0)
	{
		report_stream_error(init_text_from_const("Buffer output overflow"), errno, stream,
							init_text_from_const(__FILE__), __LINE__);
		lsquic_stream_close(stream->stream_ref);
		return 0;
	}
	return nread;
}

void fix_time(stream_parameters_t* stream_parameters)
{
	struct timespec now_time;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now_time);
	connection_parameters* connection = (connection_parameters*) stream_parameters->connection_params_ref;
	fill_time_difference_with_now(&stream_parameters->start_time, &stream_parameters->data_pointer->download_time);
	stream_parameters->data_pointer->connection_time = connection->handshake_time;
}

void post_ready_data(stream_parameters_t* stream_parameters, quic_engine_parameters* engine)
{
	fix_time(stream_parameters);
	put_stack_element(engine->output_stack_ref, stream_parameters->data_pointer);
	// remove output container out of scope of stream destructor (on_stream_close)
	stream_parameters->data_pointer = NULL;
	// if there are no streams, then all stream results were posted
	if (engine->reading_streams_count == 0)
	{
		event_base_loopbreak(engine->events.event_base_ref);
		if (get_stack_length(engine->output_stack_ref) == 0)
		{
			int a = 10;
		}
	}

}

void report_http_code_error(stream_parameters_t *stream_parameters, struct header_info *http_result)
{
	char error[ERROR_MESSAGE_SIZE];
	int actual_error_size = sprintf(error, "http response code %d", http_result->response_code);
	report_stream_error(init_text(error, actual_error_size), 0, stream_parameters, init_text_from_const(__FILE__),
						__LINE__);
	if (http_result->redirect_url != NULL)
		free(http_result->redirect_url);
}

void process_finished_stream(stream_parameters_t* stream_parameters)
{
	connection_parameters* conn = stream_parameters->connection_params_ref;
	quic_engine_parameters* engine = conn->quic_engine_params_ref;
	engine->reading_streams_count--;

	error_report_t* error_report = NULL;
	struct header_info* http_result = get_response_header(stream_parameters->data_pointer->request_args->buffer.buffer,
			stream_parameters->data_pointer->request_args->buffer.used_size, &error_report);
	if (error_report != NULL)
	{
		report_engine_error_with_report(engine, error_report);
		return;
	}
	// success 2xx codes
	if (http_result->response_code >= 200 && http_result->response_code < 300)
	{
		post_ready_data(stream_parameters, engine);

	} else if (http_result->response_code >= 300 && http_result->response_code < 400) // 3xx redirect codes
	{
	    charge_request(engine, stream_parameters->data_pointer->request_args, http_result->redirect_url,
                       stream_parameters->start_time);
		stream_parameters->is_closed = 1;
		destroy_output_data_container(stream_parameters->data_pointer);
		stream_parameters->data_pointer = NULL;
	} else
	{
		// else there is an error
		report_http_code_error(stream_parameters, http_result);
	}
//  destroyed by charge_request function in case of redirect
//	if (http_result->redirect_url != NULL)
//		destroy_text(http_result->redirect_url);
	free(http_result);
}

void http_client_on_read (lsquic_stream_t *stream, lsquic_stream_ctx_t *st_h)
{
	stream_parameters_t* stream_parameters = (stream_parameters_t*) st_h;
    // test
    printf("stream on read:%s\n", stream_parameters->data_pointer->final_url.url_data);
    ssize_t read_len = read_to_buffer(stream_parameters);
	while (read_len > 0)
	{
		read_len = read_to_buffer(stream_parameters);
	}
	if (read_len < 0)
	{
	    if (errno == EWOULDBLOCK)
        {
            return;
        }
	    else
        {
            report_stream_error(init_text_from_const("read error"), errno, stream_parameters,
                                init_text_from_const(__FILE__), __LINE__);
            stream_parameters->is_read = 1;
            lsquic_stream_close(stream);
            return;
        }
	}
    lsquic_stream_close(stream);
    stream_parameters->is_read = 1;
    process_finished_stream(stream_parameters);
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
        //test
        char *ip = inet_ntoa(((struct sockaddr_in*)&destination_addr)->sin_addr);
        printf("out packets to ip: %s\n", ip);

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
			report_engine_error(init_text_from_const("cannot send packets out"), errno, engine_params_ref,
								init_text_from_const(__FILE__), __LINE__);
		}
	}
	return (int)n_packets_out;
}
