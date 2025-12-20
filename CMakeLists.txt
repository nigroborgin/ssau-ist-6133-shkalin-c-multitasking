cmake_minimum_required(VERSION 3.20)
project(multitasking C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)


# Lab 1: multitasking
add_executable(lab_1_multitasking
        src/lab_01/main.c
        src/lab_01/arithmetic_processes.c
)

target_include_directories(lab_1_multitasking
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include/lab_01
)

target_link_libraries(lab_1_multitasking
        PRIVATE
        pthread   # подключить библиотеку для POSIX-потоков
        m         # подключить математическую библиотеку libm
)


# Lab 2: Xorg rocket
add_executable(lab_2_xorg
        src/lab_02/rocket.c
)

target_include_directories(lab_2_xorg
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include/lab_02
)

target_link_libraries(lab_2_xorg
        PRIVATE
        X11
)
