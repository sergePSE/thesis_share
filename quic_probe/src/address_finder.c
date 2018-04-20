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

void get_host_name(const char* url, char** host_name_ref, size_t* host_len_ref_out)
{
	regex_t regex;
	regmatch_t matches[MAXMATCH];

	int status;
	char message_buffer[100];
	// extract http://www.google.com/some_other_url_part_which_does_not_matter
	status = regcomp(&regex, "https?://([^/]+)/", REG_EXTENDED);
	status = regexec(&regex, url, MAXMATCH, matches, 0);
	if (status)
	{
		// printf("Host not found for url %s\n", url);
		return;
	}
	*host_len_ref_out = (int)matches[1].rm_eo - (int)matches[1].rm_so;
	*host_name_ref = calloc(*host_len_ref_out, sizeof *url);
	strncpy(*host_name_ref, url + matches[1].rm_so, *host_len_ref_out);
	regfree(&regex);
}


void get_sockaddr_for_url(const char* url, struct sockaddr * address_ref_out, int is_ipv4)
{
	int required_family = is_ipv4? AF_INET : AF_INET6;
	size_t host_name_len;
	char* host_name;
	get_host_name(url, &host_name, &host_name_len);
	struct addrinfo* addresses;
	getaddrinfo(host_name, NULL, NULL, &addresses);

	int is_found = 0;

	for(struct addrinfo* addr_i_ref = addresses; addr_i_ref != NULL; addr_i_ref = addr_i_ref->ai_next)
	{
		if (addr_i_ref->ai_family == required_family)
		{
			memcpy(address_ref_out, addr_i_ref->ai_addr, addr_i_ref->ai_addrlen);
			is_found = 1;
			break;
		}
		//Return the first one;
	}
	freeaddrinfo(addresses);
	free(host_name);
	if (!is_found)
		printf("For host %s ipv%d address was not found", url, is_ipv4? 4 : 6);
}
