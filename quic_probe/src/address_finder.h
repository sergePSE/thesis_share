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
#include "connection_params.h"

void parse_url(const char *url, parsed_url_t* parsed_url_out_ref);
void get_sockaddr(const char* host_name, struct sockaddr_storage * address_ref_out, int is_ipv4);

#endif /* SRC_ADDRESS_FINDER_H_ */
