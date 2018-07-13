cmake_minimum_required(VERSION 3.10)
project(JsToC)

set(CMAKE_C_STANDARD 11)

include_directories(. ./lib)

set(EXECUTABLE_OUTPUT_PATH ./out)

set(COMMON_SOURCES
        environments.c
        environments.h
        exceptions.c
        exceptions.h
        language.c
        language.h
        objects.c
        objects.h
        strings.c
        strings.h

        lib/uthash.h
)


add_executable(environments_test
        environments.test.c
        ${COMMON_SOURCES}
)

add_executable(objects_test
        objects.test.c
        ${COMMON_SOURCES}
)

add_executable(language_test
        language.test.c
        ${COMMON_SOURCES}
)

add_executable(strings_test
        strings.test.c
        ${COMMON_SOURCES}
)
