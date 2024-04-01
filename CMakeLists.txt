cmake_minimum_required(VERSION 3.28)
project(Cachetron C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# This program only targets Linux
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "This program only targets Linux.")
endif ()

# GCC and Clang are the only supported compilers
# Due to use of GNU statement expression extension from macro expansion ('container_of' macro)
if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "GNU" AND NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only GCC and Clang are supported.")
endif ()

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Additional checks for the Debug build
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

# Tests
add_subdirectory(tests)

add_executable(server
        server.c
        common.h
        data_structures/avl.c
        data_structures/avl.h
        data_structures/vector_c.c
        data_structures/vector_c.h
        data_structures/string_c.c
        data_structures/string_c.h
        data_structures/hashtable.c
        data_structures/hashtable.h
        data_structures/zset.c
        data_structures/zset.h)

add_executable(client
        client.c
        data_structures/vector_c.c
        data_structures/vector_c.h
        data_structures/string_c.c
        data_structures/string_c.h
        common.h)