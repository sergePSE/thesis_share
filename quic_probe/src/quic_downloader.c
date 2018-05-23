//
// Created by sergei on 09.05.18.
//

#include <pthread.h>
#include "quic_downloader.h"
#include "quic_engine_holder.h"
#include "connection_establisher.h"
#include "cycle_buffer.h"
#include "event_handlers.h"

/*
 * engine_ref_out - empty instance for memory allocation simplification
 * quic_args_ref - quic engine parameters for creation
 */
void create_client(quic_engine_parameters* engine_ref_out, quic_args* quic_args_ref)
{
    engine_ref_out->program_args = *quic_args_ref;
    init_engine(engine_ref_out);
}
void destroy_client(quic_engine_parameters* engine_ref_out)
{
    destroy_engine(engine_ref_out);
}

void *thread_event_loop(void* event_base)
{
    event_base_loop((struct event_base*)event_base, 0);
    return NULL;
}

void start_loop(pthread_t* thread_ref, struct event_base* event_base_ref)
{
    // test
    thread_event_loop(event_base_ref);
    int a = 10;
//    if (*thread_ref != NULL)
//        pthread_exit(thread_ref);
//    pthread_create(thread_ref, NULL, thread_event_loop, event_base_ref);
}

void start_downloading(quic_engine_parameters* engine_ref, const struct url* urls, int url_count)
{
    reset_memory_buffer_allocations(&engine_ref->output_result_buffer, url_count);
    for(int i = 0; i < url_count; i++)
        quic_connect(engine_ref, urls[i].url_data);
    schedule_engine(engine_ref);
    start_loop(&engine_ref->loop_thread, engine_ref->events.event_base_ref);
}

struct output_data* get_download_result(quic_engine_parameters* engine_ref)
{
    return (struct output_data*) pull_stack_element(engine_ref->output_stack_ref);
}





