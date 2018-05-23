/*
 * main.c
 *
 *  Created on: Apr 4, 2018
 *      Author: sergei
 */


#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "lsquic.h"
#include "quic_downloader.h"

int main()
{
	quic_engine_parameters quic_engine;
	quic_args quic_args_ref =
	{
		.is_ipv4 = 1,
		.buffer_size = 5*1024*1024,
		.quic_ver_str = "Q039",
		.quic_ver_str_size = strlen("Q039"),
		.timeout_ms = 10000,
		.use_prev_conn_data = 0,
		.local_port_number = 3039
	};
 	create_client(&quic_engine, &quic_args_ref);

 	char* url1 = "https://www.youtube.com/watch?v=3xmH2GxduTA";
 	char* url2 = "https://www.youtube.com/watch?v=DgLxs7cjFnY";
 	struct url urls[2];
 	urls[0].url_data = url1;
	urls[0].url_len = strlen(url1);

	urls[1].url_data = url2;
	urls[1].url_len = strlen(url2);

 	start_downloading(&quic_engine, urls, 2);

	//sleep(1);// 10 sec
	struct output_data* data = (struct output_data*)pull_stack_element(quic_engine.output_stack_ref);
	while (data != NULL)
	{
		puts(data->data_ref);
		puts("\n\n\n");
		free(data);
		data = (struct output_data*)pull_stack_element(quic_engine.output_stack_ref);
	}

	destroy_client(&quic_engine);
	printf("Hello %d times according to Freud!!\n", 2);

	return 0;
}
