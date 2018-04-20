/*
 * address_finder.h
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */

#ifndef SRC_ADDRESS_FINDER_H_
#define SRC_ADDRESS_FINDER_H_

#include <stddef.h>
#include <sys/socket.h>

void get_host_name(const char* url, char** host_name_ref, size_t* host_len_ref_out);
void get_sockaddr_for_url(const char* url, struct sockaddr * address_ref_out, int is_ipv4);


#endif /* SRC_ADDRESS_FINDER_H_ */
