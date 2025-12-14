cmake_minimum_required(VERSION 4.0)
project(multitasking C)

set(CMAKE_C_STANDARD 17)
include_directories(include)

add_executable(multitasking
        src/lab_01/main.c
        include/lab_01/arithmetic_processes.h
        src/lab_01/arithmetic_processes.c
        include/lab_01/task_statuses.h
        include/lab_01/file_paths.h
)

add_executable(1_processes
        src/1_processes/main.c
        src/1_processes/hello.c
)

add_executable(2_1_system_v_threads
        src/2_threads/system_v_threads.c
)

add_executable(2_2_posix_threads_1
        src/2_threads/posix_threads_1.c
)

add_executable(2_3_posix_threads_2
        src/2_threads/posix_threads_2.c
)
