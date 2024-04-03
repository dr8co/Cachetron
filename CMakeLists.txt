cmake_minimum_required(VERSION 3.28)
project(Cachetron C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Check if the system is Linux
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "This program only targets Linux.")
endif ()

# GCC and Clang are (probably) the only supported compilers due to use of
# GNU statement expression extension from macro expansion ('container_of' macro)
if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "GNU" AND NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only GCC and Clang are supported.")
endif ()

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Additional options for the Debug build
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

# Sanitizers for debugging (Disabled by default, slows down the program. Uncomment to enable)
# GCC does not support all sanitizers, so Clang is recommended for this purpose. Requires llvm-symbolizer
# The build type should be RelWithDebInfo (or Debug) for sanitizers to work properly
if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    # Address, leak, undefined, integer, nullability sanitizers
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -fsanitize=address,leak,undefined,integer,nullability -fno-omit-frame-pointer -g")

    # Thread sanitizer, cannot be used with address sanitizer
    #  set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -fsanitize=thread -fPIE -fno-omit-frame-pointer -g -O1")

    # Memory sanitizer, cannot be used with address sanitizer.
    # set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -fsanitize=memory -fPIE -fno-optimize-sibling-calls -fno-omit-frame-pointer -g -O1")
endif ()

# Tests
add_subdirectory(tests)

add_executable(server
        server.c
        common.h
        thread_pool.c
        thread_pool.h
        data_structures/trees/avl.c
        data_structures/trees/avl.h
        data_structures/vector/vector_c.c
        data_structures/vector/vector_c.h
        data_structures/string/string_c.c
        data_structures/string/string_c.h
        data_structures/hashmap/hashtable.c
        data_structures/hashmap/hashtable.h
        data_structures/set/zset.c
        data_structures/set/zset.h
        data_structures/list/list.c
        data_structures/list/list.h
        data_structures/trees/heap.c
        data_structures/trees/heap.h
        data_structures/queue/deque_c.c
        data_structures/queue/deque_c.h)

add_executable(client
        client.c
        data_structures/vector/vector_c.c
        data_structures/vector/vector_c.h
        data_structures/string/string_c.c
        data_structures/string/string_c.h
        common.h)