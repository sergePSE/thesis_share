//
// Created by sergei on 14.05.18.
//

#ifndef QUIC_PROBE_OUTPUT_DATA_H
#define QUIC_PROBE_OUTPUT_DATA_H

#include <stdio.h>

struct output_data {
    // reference to the buffer pointer
    char* data_ref;
    ssize_t data_size;

    int has_error;
    char* error_str;

    const char** url_ref;
    int url_size;
};


#endif //QUIC_PROBE_OUTPUT_DATA_H
