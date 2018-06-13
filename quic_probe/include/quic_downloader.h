//
// Created by sergei on 09.05.18.
//

#ifndef QUIC_PROBE_QUIC_DOWNLOADER_H
#define QUIC_PROBE_QUIC_DOWNLOADER_H

#include <stdio.h>

#include "engine_structs.h"
#include "args_data.h"

extern int create_client(quic_engine_parameters** engine_parameters_ref, quic_args* quic_args_ref);
extern void destroy_client(quic_engine_parameters* engine_ref_out);

extern int start_downloading(quic_engine_parameters* engine_ref, struct download_request* requests, int request_count);

// don`t forget to free the memory as it's not static
struct output_data* get_download_result(quic_engine_parameters* engine_ref);

#endif //QUIC_PROBE_QUIC_DOWNLOADER_H
