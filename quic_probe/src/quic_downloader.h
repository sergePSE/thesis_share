//
// Created by sergei on 09.05.18.
//

#ifndef QUIC_PROBE_QUIC_DOWNLOADER_H
#define QUIC_PROBE_QUIC_DOWNLOADER_H

#include <stdio.h>

#include "engine_structs.h"
#include "output_data.h"

struct buffer {
    char* buffer;
    size_t max_size;
    size_t current_size;
};

struct url {
    char* url_data;
    size_t url_len;
};

extern void create_client(quic_engine_parameters* engine_ref_out, quic_args* quic_args_ref);
extern void destroy_client(quic_engine_parameters* engine_ref_out);

extern void start_downloading(quic_engine_parameters* engine_ref, const struct url* urls, int url_count);

// don`t forget to free the memory as it's not static
struct output_data* get_download_result(quic_engine_parameters* engine_ref);

#endif //QUIC_PROBE_QUIC_DOWNLOADER_H
