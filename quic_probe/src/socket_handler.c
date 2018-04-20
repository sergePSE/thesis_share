/*
 * socket_handler.c
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */


#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "socket_handler.h"

int open_socket(int is_ipv4)
{
	int udp_protocol = 17;
	int domain = is_ipv4? AF_INET : AF_INET6;
	int socket_id = socket(domain, SOCK_DGRAM , udp_protocol);
	if (socket_id == 0)
	{
		printf("socket was not opened %d ipv4?", is_ipv4);
	}
	return socket_id;
}

void close_socket(int socket_id)
{
	shutdown(socket_id, SHUT_RDWR);
}

