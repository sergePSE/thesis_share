/*
 * main.c
 *
 *  Created on: Apr 4, 2018
 *      Author: sergei
 */


#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "lsquic.h"
#include "quic_engine_holder.h"
#include "connection_establisher.h"
#include "address_finder.h"

int main()
{
	char* url = "https://www.youtube.com/watch?v=3xmH2GxduTA";
	size_t hostname_size;
	int is_ipv4 = 1;
	quic_engine quic_engine;
	init_engine(&quic_engine, LSQVER_039);
	quic_connect(&quic_engine, url, is_ipv4);
	printf("Hello %d times according to Freud!!\n", 2);
	return 0;
}
