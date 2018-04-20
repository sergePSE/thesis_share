/*
 * socket_hander.h
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */

#ifndef SRC_SOCKET_HANDLER_H_
#define SRC_SOCKET_HANDLER_H_

#include <sys/socket.h>

// opens a UPD socket and returns socket id
int open_socket(int is_ipv4);
void close_socket(int socket_id);

#endif /* SRC_SOCKET_HANDLER_H_ */
