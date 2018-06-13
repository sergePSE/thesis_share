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
#include <limits.h>

#include "../include/quic_downloader.h"

void write_to_file(char* start_pointer, size_t size, const char* file_name)
{
    FILE* file = fopen(file_name ,"w");
    for (; size > 0; size--) {
        fputc(*start_pointer, file);
        start_pointer++;
    }
    fclose(file);
}

void read_result(quic_engine_parameters* client, const char* file_name){
	struct output_data* result = get_download_result(client);
	write_to_file(result->request_args->buffer.buffer, result->request_args->buffer.used_size, file_name);
	if (result->has_error)
	    printf("error: %s, url: %s\n", result->error_str, result->final_url.url_data);
	else
        printf("%s\n%s was written : conn time %ld mcs, download time: %ld mcs\n",
           result->request_args->url_request.url_data,
           file_name, result->connection_time.tv_sec*1000000 + result->connection_time.tv_nsec / 1000,
		   result->download_time.tv_sec*1000000 + result->download_time.tv_nsec/1000);
	if (result != NULL)
		destroy_output_data_container(result);
}

#define BUFFER_SIZE 10 * 1024 * 1024

int main()
{
	quic_args quic_args_ref =
	{
		.is_ipv4 = 1,
		.quic_ver_str = "Q039",
		.quic_ver_str_size = strlen("Q039"),
		.timeout_ms = 5000,
		.use_prev_conn_data = 0,
		.local_port_number = 3039,
		.max_streams = 10
	};
    quic_engine_parameters* quic_engine_ref;
    int is_created = create_client(&quic_engine_ref, &quic_args_ref);
    if(is_created == -1)
        exit(-1);

    struct download_request download_requests4[2];
    download_requests4[0].url_request.url_data = "https://www.youtube.com";
    download_requests4[0].url_request.url_len = strlen(download_requests4[0].url_request.url_data);

    download_requests4[1].url_request.url_data = "https://www.google.com";
    download_requests4[1].url_request.url_len = strlen(download_requests4[1].url_request.url_data);

    download_requests4[0].buffer.buffer = malloc(BUFFER_SIZE);
    download_requests4[0].buffer.allocated_size = BUFFER_SIZE;
    download_requests4[0].buffer.used_size = 0;
    download_requests4[1].buffer.buffer = malloc(BUFFER_SIZE);
    download_requests4[1].buffer.allocated_size = BUFFER_SIZE;
    download_requests4[1].buffer.used_size = 0;

    start_downloading(quic_engine_ref, &download_requests4, 2);

    read_result(quic_engine_ref, "youtube_general.txt");
    read_result(quic_engine_ref, "google_general.txt");

    free(download_requests4[0].buffer.buffer);
    free(download_requests4[1].buffer.buffer);

//    struct download_request download_requests3[1];
//    download_requests3[0].url_request.url_data = "https://r1---sn-4g5e6nsk.googlevideo.com/videoplayback?aitags=133,134,135,136,137,160,242,243,244,247,248,278&lmt=1514963792803407&fvip=1&pcm2=yes&source=youtube&clen=30781446&mime=video%2Fmp4&c=WEB&itag=137&pl=16&gir=yes&id=o-APQKuCILru826xKhjGwJEBC2YZs0GfEmdF9tk1zj92qm&sparams=aitags,clen,dur,ei,expire,gir,id,initcwndbps,ip,ipbits,ipbypass,itag,keepalive,lmt,mime,mip,mm,mn,ms,mv,pcm2,pl,requiressl,source&requiressl=yes&ei=7XIfW6-4Ns2OgAeyt4_4Bw&dur=120.320&key=cms1&ip=131.159.24.7&keepalive=yes&ipbits=0&signature=56AAEBE28F4806C73667367BF785764CAE4FFB7E.38D6A557D693F438E12BA87CDBFB7BE0D8DB4160&expire=1528809294&range=0-9003905&redirect_counter=1&rm=sn-4g5ezd7e&req_id=d954dc3e686a7c30&cms_redirect=yes&ipbypass=yes&mip=131.159.193.203&mm=31&mn=sn-4g5e6nsk&ms=au&mt=1528805958&mv=m";
//    download_requests3[0].url_request.url_len = strlen(download_requests3[0].url_request.url_data);
//
//    download_requests3[0].buffer.buffer = malloc(BUFFER_SIZE);
//    download_requests3[0].buffer.allocated_size = BUFFER_SIZE;
//    download_requests3[0].buffer.used_size = 0;
//
//    start_downloading(quic_engine_ref, download_requests3, 1);
//
//    read_result(quic_engine_ref, "single_result.txt");
//
//    free(download_requests3[0].buffer.buffer);
//
//	struct download_request download_requests[2];
//	download_requests[0].url_request.url_data = "https://www.youtube.com/watch?v=rjp-JZif59M";
//	download_requests[0].url_request.url_len = strlen(download_requests[0].url_request.url_data);
////	// test redirect feature last p is wrong url
//	download_requests[1].url_request.url_data = "https://www.youtube.com/watch?v=3xmH2GxduTAp";
//	download_requests[1].url_request.url_len = strlen(download_requests[1].url_request.url_data);
//
//	download_requests[0].buffer.buffer = malloc(BUFFER_SIZE);
//	download_requests[0].buffer.allocated_size = BUFFER_SIZE;
//	download_requests[0].buffer.used_size = 0;
//	download_requests[1].buffer.buffer = malloc(BUFFER_SIZE);
//	download_requests[1].buffer.allocated_size = BUFFER_SIZE;
//	download_requests[1].buffer.used_size = 0;
//
//    start_downloading(quic_engine_ref, download_requests, 2);
//
//    read_result(quic_engine_ref, "youtube_result.txt");
//	read_result(quic_engine_ref, "google_result.txt");
//
//    free(download_requests[0].buffer.buffer);
//    free(download_requests[1].buffer.buffer);
//
//
//    struct download_request download_requests2[2];
//	download_requests2[0].url_request.url_data = "https://r1---sn-4g5e6nsk.googlevideo.com/videoplayback?aitags=133,134,135,136,137,160,242,243,244,247,248,278&lmt=1514963792803407&fvip=1&pcm2=yes&source=youtube&clen=30781446&mime=video%2Fmp4&c=WEB&itag=137&pl=16&gir=yes&id=o-APQKuCILru826xKhjGwJEBC2YZs0GfEmdF9tk1zj92qm&sparams=aitags,clen,dur,ei,gir,id,initcwndbps,ip,ipbits,itag,keepalive,lmt,mime,mm,mn,ms,mv,pcm2,pl,requiressl,source,expire&requiressl=yes&ei=7XIfW6-4Ns2OgAeyt4_4Bw&dur=120.320&key=yt6&ip=131.159.24.7&keepalive=yes&mm=31,29&mn=sn-4g5e6nsk,sn-4g5ednsk&mt=1528787565&initcwndbps=2616250&mv=m&ipbits=0&ms=au,rdu&signature=332B381B67B89E1436A37644AC2A692388925A17.5B57B7E62317584F2DF5DD6430161A13E62B8FB6&expire=1528809294&range=0-4003905";
//	download_requests2[0].url_request.url_len = strlen(download_requests2[0].url_request.url_data);
//	// test timeout
//	download_requests2[1].url_request.url_data = "https://r1---sn-4g5ednsk.googlevideo.com/videoplayback?mime=video%2Fmp4&requiressl=yes&clen=30781446&mn=sn-4g5ednsk,sn-4g5e6nsk&mm=31,29&mv=m&mt=1528730892&key=yt6&ms=au,rdu&source=youtube&keepalive=yes&signature=B4B67DC7D362A3DF9C817CDA876977927F898420.2F1BBC59432A2F4779DC731167AC2DA36A9CACC1&ipbits=0&itag=137&c=WEB&aitags=133,134,135,136,137,160,242,243,244,247,248,278&pl=16&expire=1528752619&dur=120.320&gir=yes&ip=131.159.24.7&sparams=aitags,clen,dur,ei,gir,id,initcwndbps,ip,ipbits,itag,keepalive,lmt,mime,mm,mn,ms,mv,pl,requiressl,source,expire&initcwndbps=1853750&id=o-AD-tdQopKtDaHefK1jvPydY2c9qjwassjQcV478L_a0h&ei=i5UeW8TTF8HL1wLhh7iQCg&lmt=1514963792803407&fvip=1&range=0-2773905";
//	download_requests2[1].url_request.url_len = strlen(download_requests2[1].url_request.url_data);
//
//    download_requests2[0].buffer.buffer = malloc(BUFFER_SIZE);
//    download_requests2[0].buffer.allocated_size = BUFFER_SIZE;
//    download_requests2[0].buffer.used_size = 0;
//    download_requests2[1].buffer.buffer = malloc(BUFFER_SIZE);
//    download_requests2[1].buffer.allocated_size = BUFFER_SIZE;
//    download_requests2[1].buffer.used_size = 0;
//
//    start_downloading(quic_engine_ref, download_requests2, 2);
//
//	read_result(quic_engine_ref, "polizei_result.txt");
//	read_result(quic_engine_ref, "happy_result.txt");
//
//    free(download_requests2[0].buffer.buffer);
//    free(download_requests2[1].buffer.buffer);



    destroy_client(quic_engine_ref);
	printf("Hello %d times according to Freud!!\n", 2);

	return 0;
}
