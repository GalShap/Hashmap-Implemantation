cmake_minimum_required(VERSION 3.19)
project(ex4_galshaffir C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ex4_galshaffir
        hashmap.c
        pair.c
        vector.c
        main.c
        test_suite.c

        )