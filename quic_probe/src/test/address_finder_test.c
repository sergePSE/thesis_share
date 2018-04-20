/*
 * address_finder_test.c
 *
 *  Created on: Apr 13, 2018
 *      Author: sergei
 */
#include "../address_finder.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void test_str(char* expected, char* result, int are_different)
{
	int is_different = strcmp(expected, result)? 1 : 0;
	if (is_different != are_different)
	{
		printf("Test failed\n");
		exit(-1);
	}
}

void test_address_finder()
{
	char normal_url[] = "http://en.wikibooks.org/wiki/C_Programming/Error_handling";
	char double_http_url[] = "http://en.wikibooks.org/wiki";
	char no_http_url[] = "en.wikibooks.org/wiki";
	char https_url[] = "https://en.wikibooks.org/wiki/C_Programming/Error_handling";
	char http_crashed_url[] = "htpddd://en.wikibooks.org/wiki";
	char host_source[] = "en.wikibooks.org";

	char* host_name;
	size_t host_size;
	get_host_name(normal_url, &host_name, &host_size);
	test_str(host_source, host_name, 0);
	free(host_name);
	get_host_name(double_http_url, &host_name, &host_size);
	test_str(host_source, host_name, 0);
	free(host_name);
	get_host_name(no_http_url, &host_name, &host_size);
	test_str(host_source, host_name, 1);
	free(host_name);
	get_host_name(https_url, &host_name, &host_size);
	test_str(host_source, host_name, 0);
	free(host_name);
	get_host_name(http_crashed_url, &host_name, &host_size);
	test_str(host_source, host_name, 1);
	free(host_name);
	printf("Address parse test passed\n");
}



