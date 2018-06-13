/*
 * socket_handler.c
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */
#include "socket_handler.h"

#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>

#include "socket_buffer.h"
#include "quic_engine_holder.h"

void init_local_addr(int is_ipv4, struct sockaddr_storage* local_addr_out_ref, int port_number)
{
	if (is_ipv4) {
		local_addr_out_ref->ss_family = AF_INET;
		struct sockaddr_in* in_v4 = (struct sockaddr_in*) local_addr_out_ref;
		in_v4->sin_family = AF_INET;
		in_v4->sin_addr.s_addr = INADDR_ANY;
		in_v4->sin_port = htons(port_number);
	} else {
		local_addr_out_ref->ss_family = AF_INET6;
	}
}

#define ERROR_MESSAGE_BUFFER_SIZE 50

int print_socket_error_message(char* destination, int is_ipv4)
{
    return sprintf(destination, "socket was not opened ipv4?=%d", is_ipv4);
}

int open_socket(int is_ipv4, struct sockaddr_storage* out_local_addr_ref, int* socket_id_out_ref,
        socklen_t* socket_buffer_size, int port_number, error_report_t** error_out)
{
	init_local_addr(is_ipv4, out_local_addr_ref, port_number);
	char error_message_buffer[ERROR_MESSAGE_BUFFER_SIZE];

	// maybe it's udp protocol can be used instead of 0 in socket(), but it works automatically
	//int udp_protocol = 17;
	int domain = is_ipv4? AF_INET : AF_INET6;
	int socket_id = socket(domain, SOCK_DGRAM , 0);
	if (socket_id == 0)
	{
        int error_size = print_socket_error_message(error_message_buffer, is_ipv4);
        *error_out = create_report(init_text(error_message_buffer, (size_t)error_size), errno, __FILE__, __LINE__);
		return -1;
	}
	socklen_t local_addr_len = is_ipv4? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
	int bind_status = bind(socket_id, (struct sockaddr*)out_local_addr_ref, local_addr_len);
	if (bind_status != 0)
	{
		int error_size = print_socket_error_message(error_message_buffer, is_ipv4);
		*error_out = create_report(init_text(error_message_buffer, (size_t)error_size), errno, __FILE__, __LINE__);
		return -1;
	}
	int reuse = 1;
	if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) == -1)
	{
        *error_out = create_report(init_text_from_const("Can not set reuse on socket"), errno, __FILE__, __LINE__);
		return -1;
	}
	if (0 != getsockname(socket_id, (struct sockaddr*)out_local_addr_ref, &local_addr_len))
	{
		int error_size = print_socket_error_message(error_message_buffer, is_ipv4);
		*error_out = create_report(init_text(error_message_buffer, (size_t)error_size), errno, __FILE__, __LINE__);
		return -1;
	}
	int non_blocking_flag = fcntl(socket_id, F_GETFL);
	if (non_blocking_flag == -1)
	{
		int error_size = print_socket_error_message(error_message_buffer, is_ipv4);
		*error_out = create_report(init_text(error_message_buffer, (size_t)error_size), errno, __FILE__, __LINE__);
		return -1;
	}
	non_blocking_flag = fcntl(socket_id, F_SETFL, non_blocking_flag | O_NONBLOCK);
	if (non_blocking_flag != 0){
		int error_size = print_socket_error_message(error_message_buffer, is_ipv4);
		*error_out = create_report(init_text(error_message_buffer, (size_t)error_size), errno, __FILE__, __LINE__);
		return -1;
	}
	socklen_t socket_buffer_size_len = sizeof(*socket_buffer_size);
	if (0 != getsockopt(socket_id, SOL_SOCKET, SO_RCVBUF, (void*)socket_buffer_size,
						&socket_buffer_size_len))
	{
        *error_out = create_report(init_text_from_const("error on read socket buffer size"), errno, __FILE__, __LINE__);
		return -1;
	}
    *socket_id_out_ref = socket_id;

	return socket_id;
}


void close_socket(int socket_id)
{
	shutdown(socket_id, SHUT_RDWR);
}

void read_socket_message_sequence(int socket_id, struct socket_data* socket_data_ref,
		struct sockaddr_storage* local_addr_ref, size_t max_packet_size, error_report_t** error_out)
{
    socket_data_ref->data_size = 0;
    socket_data_ref->messages_len = 0;

    ssize_t message_len = 0;
    do
    {
        struct buffered_socket_message* current_message_ref =
        		&socket_data_ref->messages[socket_data_ref->messages_len];

        memcpy(&current_message_ref->local_addr, local_addr_ref, sizeof(*local_addr_ref));
        current_message_ref->message.iov_base = socket_data_ref->data_block + socket_data_ref->data_size;
        current_message_ref->message.iov_len = max_packet_size;
        struct msghdr incoming_message = {
                .msg_name = &current_message_ref->peer_addr,
                .msg_namelen = sizeof(struct sockaddr_storage),
                .msg_flags = 0,
                .msg_iov = &current_message_ref->message,
                .msg_iovlen = 1
        };
        message_len = recvmsg(socket_id, &incoming_message, 0);
        if (message_len == -1)
		{
			if (!(EAGAIN == errno || EWOULDBLOCK == errno))
			{
				*error_out = create_report(init_text_from_const("socket recvmsg error"), errno, __FILE__, __LINE__);
				return;
			}
		}
        if (message_len > 0)
        {
            current_message_ref->message.iov_len = message_len;
            socket_data_ref->data_size += message_len;
            socket_data_ref->messages_len++;
        }
    } while (message_len > 0);
}

static int errors_in_row = 0;
#define MAX_ERRORS_NUMBER 100

void read_socket_packet(quic_engine_parameters* engine_ref)
{
	size_t max_packet_size = engine_ref->program_args.is_ipv4? QUIC_IPV4_MAX_PACKET_SIZE : QUIC_IPV6_MAX_PACKET_SIZE;
	error_report_t* error = NULL;
	read_socket_message_sequence(engine_ref->socket_id, &engine_ref->socket_buffer, &engine_ref->local_addr,
								 max_packet_size, &error);
	if (error != NULL)
	{
		report_engine_error_with_report(engine_ref, error);
        if (errors_in_row++ > MAX_ERRORS_NUMBER)
            lsquic_engine_destroy(engine_ref->engine_ref);
        return;
	}
    int decrypt_status = 1;
    for(size_t i = 0; i < engine_ref->socket_buffer.messages_len && decrypt_status != -1; i++)
	{
		struct buffered_socket_message message = engine_ref->socket_buffer.messages[i];
		if (get_list_size(engine_ref->connections) > 0)
			decrypt_status = lsquic_engine_packet_in(engine_ref->engine_ref, (unsigned char*)message.message.iov_base,
				message.message.iov_len, (struct sockaddr *)&message.local_addr, (struct sockaddr *)&message.peer_addr,
						NULL);
		// test
		char* ip = inet_ntoa(((struct sockaddr_in*)&message.peer_addr)->sin_addr);
		printf("received packets from ip: %s\n", ip);
	}



	if (decrypt_status == -1)
	{
	    report_engine_error(init_text_from_const("can not decrypt the packet"), errno, engine_ref,
							init_text_from_const(__FILE__), __LINE__);
	    if (errors_in_row++ > MAX_ERRORS_NUMBER)
		{
			destroy_engine(engine_ref);
			return;
		}


	}
	errors_in_row = 0;
	lsquic_engine_process_conns(engine_ref->engine_ref);
}



void read_socket_event(int fd, short flags, void *ctx)
{
	quic_engine_parameters* quic_engine_ref = (quic_engine_parameters*) ctx;
	read_socket_packet(quic_engine_ref);
}