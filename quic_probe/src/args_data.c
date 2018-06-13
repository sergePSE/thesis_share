//
// Created by sergei on 24.05.18.
//

#include "../include/args_data.h"
#include "text.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>


struct output_data* init_output_data_container(char* url, size_t url_size)
{
    struct output_data* container = malloc(sizeof(struct output_data));
    container->has_error = 0;
    container->error_str = NULL;

    copy_char_sequence(url, url_size, &container->final_url.url_data, &container->final_url.url_len);
    return container;
}

void destroy_output_data_container(struct output_data* data_ref)
{
    if (data_ref->error_str != NULL)
        free(data_ref->error_str);
    if (data_ref->final_url.url_data != NULL)
        free(data_ref->final_url.url_data);
    free(data_ref);
}

