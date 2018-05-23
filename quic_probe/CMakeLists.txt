cmake_minimum_required(VERSION 2.10)
project(quic_probe)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")

set(GENERAL_SOURCES
        src/address_finder.c
        src/connection_establisher.c
        src/connection_init_handler.c
        src/connection_read_handler.c
        src/connection_write_handler.c
        src/engine_structs.c
        src/quic_engine_holder.c
        src/socket_handler.c
        src/connection_params.c
        src/socket_buffer.c
        src/quic_downloader.c
        src/stack.c
        src/cycle_buffer.c
        src/simple_list.c src/event_handlers.c src/event_handlers.h)
set(GENERAL_LIBS lsquic event pthread ssl crypto z m)

include_directories(src)
include_directories(src/test)
include_directories(/home/sergei/Documents/git/lsquic-client/include)
include_directories(/home/sergei/Documents/git/boringssl/include)

link_directories("/usr/local/lib")
link_directories("/home/sergei/Documents/git/lsquic-client/src/liblsquic")

add_executable(quic_probe
        src/main.c  ${GENERAL_SOURCES})
target_link_libraries(quic_probe ${GENERAL_LIBS})

