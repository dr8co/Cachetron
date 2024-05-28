cmake_minimum_required(VERSION 3.28)

project(Cachetron
        VERSION 0.1.0
        DESCRIPTION "A simple in-memory key-value store with a client-server architecture."
        HOMEPAGE_URL "https://github.com/dr8co/Cachetron"
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Google Test requires at least C++14
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if the system is Linux
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "This program only targets Linux.")
endif ()

# GCC and Clang are (probably) the only supported compilers due to use of
# GNU statement expression extension from macro expansion ('container_of' macro)
if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "GNU" AND NOT ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    message(FATAL_ERROR "Only GCC and Clang are supported.")
endif ()

# Options
option(BUILD_TESTING "Build tests" ON)

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Find pthreads
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Additional compile options for the Debug build
if (NOT MSVC AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(
                -Wall
                -Wextra
                -Werror
                -Wpedantic
                -g
        )
    endif ()
endif ()

# Sanitizers for debugging and testing
# GCC does not support all sanitizers, so Clang is recommended for this purpose. Requires llvm-symbolizer.
# The build type should be RelWithDebInfo (or Debug, adjust the variables appropriately) for sanitizers to work properly.
if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    # Common flags for all sanitizers
    set(sanitizer_common_flags "-fno-omit-frame-pointer -g -O1")

    # Address, leak, undefined, integer, nullability sanitizers
    set(address_sanitizer_flags "-fsanitize=address,leak,undefined,integer,nullability")

    # Thread sanitizer, cannot be used with address sanitizer
    set(thread_sanitizer_flags "-fsanitize=thread -fPIE")

    # Memory sanitizer, cannot be used with address sanitizer.
    set(memory_sanitizer_flags "-fsanitize=memory -fPIE -fno-optimize-sibling-calls")

    # Add flags for Release with Debug Info build
    if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(
                "SHELL:${sanitizer_common_flags}"
                "SHELL:${address_sanitizer_flags}"
        )
    endif ()
endif ()

# server executable
add_executable(server)
target_sources(server PRIVATE
        data_structures/hashmap/hashtable.c
        data_structures/list/list.c
        data_structures/queue/deque_c.c
        data_structures/set/zset.c
        data_structures/string/lite_string.c
        data_structures/trees/avl.c
        data_structures/trees/heap.c
        data_structures/vector/vector_c.c
        thread_pool.c
        server.c
)

# client executable
add_executable(client)
target_sources(client PRIVATE
        data_structures/vector/vector_c.c
        data_structures/string/lite_string.c
        client.c
)

target_link_libraries(client Threads::Threads)
target_link_libraries(server Threads::Threads)

# Link the enabled sanitizers.
if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_link_libraries(client asan ubsan)
    target_link_libraries(server asan ubsan)
endif ()

# Tests
if (BUILD_TESTING)
    # Google Test
    include(FetchContent)

    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/f10e11fb27301fba21caa71030bb5024e67aa135.zip
    )

    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif ()

