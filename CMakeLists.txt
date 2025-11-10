cmake_minimum_required(VERSION 4.0)
project(multitasking C)

set(CMAKE_C_STANDARD 17)
include_directories(include)

add_executable(multitasking
        src/1_processes/main.c
        src/1_processes/hello.c
        src/2_threads/system_v_threads.c
        src/2_threads/posix_threads_1.c
        src/2_threads/posix_threads_2.c
)
