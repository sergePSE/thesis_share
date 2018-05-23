/*
 * address_finder.c
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */

#include "address_finder.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#include <regex.h>
#include <sys/types.h>

#define MAXMATCH 2
#define QUIC_PORT 443

void set_quic_port(struct sockaddr_storage* addr_ref, int is_ipv4)
{
	if (is_ipv4)
		((struct sockaddr_in*) addr_ref)->sin_port = htons(QUIC_PORT);
	else
		((struct sockaddr_in6*) addr_ref)->sin6_port = htons(QUIC_PORT);
}

void get_sockaddr(const char* host_name, struct sockaddr_storage * address_ref_out, int is_ipv4)
{
	int required_family = is_ipv4? AF_INET : AF_INET6;
	struct addrinfo* addresses;
	struct addrinfo hint;
	hint.ai_family = required_family;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_ALL;
	hint.ai_protocol = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	int get_addr_info_status = getaddrinfo(host_name, NULL, &hint, &addresses);
	if (get_addr_info_status != 0)
	{
		printf("For host %s ipv%d address was not found, error %s", host_name, is_ipv4? 4 : 6,
			   gai_strerror(get_addr_info_status));
		exit(-1);
	}
	struct addrinfo* addr_i_ref = addresses;

	memcpy(address_ref_out, addr_i_ref->ai_addr, addr_i_ref->ai_addrlen);
	set_quic_port(address_ref_out, is_ipv4);
	freeaddrinfo(addresses);
}

void copy_str(const char* url, char** out_str_ref, size_t * len_out_ref, size_t from, size_t to)
{
	*len_out_ref = to - from;
    *out_str_ref = calloc(*len_out_ref, sizeof *url);
	strncpy(*out_str_ref, url + from, *len_out_ref);
}

void parse_url(const char * url, parsed_url_t* parsing_out_ref)
{
    regex_t regex;
    regmatch_t matches[MAXMATCH];
    int status;
    // extract http://www.google.com/some_other_url_part_which_does_not_matter
    status = regcomp(&regex, "https?://([^/]+)/", REG_EXTENDED);
    status |= regexec(&regex, url, MAXMATCH, matches, 0);
    if (status)
    {
        printf("Host not found for url %s\n", url);
        return;
    }

    // first match is the general match and not a specific one for the grouped symbols ()
	copy_str(url, &parsing_out_ref->hostname, &parsing_out_ref->hostname_len, (size_t)matches[1].rm_so,
			 (size_t)matches[1].rm_eo);
	copy_str(url, &parsing_out_ref->path, &parsing_out_ref->path_len, (size_t)matches[1].rm_eo, strlen(url));
    regfree(&regex);
}
