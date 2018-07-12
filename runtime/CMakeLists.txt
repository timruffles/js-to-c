cmake_minimum_required(VERSION 3.10)
project(JsToC)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(runtime_test
        environments.c
        environments.test.c
)
