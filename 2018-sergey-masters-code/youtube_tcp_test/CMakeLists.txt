cmake_minimum_required(VERSION 3.10)
project(youtube_tcp_test)

set(CMAKE_C_STANDARD 11)

include_directories(../build/boringssl/include src)
include_directories(../build/curl-7.58.0/build/include src)
include_directories(../build/ffmpeg-4.0/build/include src)

link_directories(../build/boringssl/lib)
link_directories(../build/curl-7.58.0/build/lib)
link_directories(../build/ffmpeg-4.0/build/lib)

add_executable(youtube_tcp_test
        src/adaptive.c
        src/coro.c
        src/curlops.c
        src/getinfo.c
        src/helper.c
        src/metrics.c
        src/mm_parser.c
        src/youtube-dl.c
        src/arguments_parser.c
        src/arguments_parser.h
        src/network_addresses.c
        src/network_addresses.h)

target_link_libraries(youtube_tcp_test curl avformat avcodec avutil ssl crypto z m pthread dl)
