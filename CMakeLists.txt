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

# GCC and Clang and compatible compilers only
if (NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    message(FATAL_ERROR "Only GCC and Clang derivatives are supported.")
endif ()

# Options
include(CMakeDependentOption)
option(BUILD_TESTING "Build tests" ON)
# GCC does not support all sanitizers
cmake_dependent_option(ENABLE_SANITIZERS
        "Enable sanitizers (Ignored if not using Clang compiler)" OFF
        "${CMAKE_C_COMPILER_ID} STREQUAL \"Clang\"" OFF)

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Find pthreads
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

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

# Expose GNU (and other) C extensions
target_compile_definitions(server PUBLIC _GNU_SOURCE)
target_compile_definitions(client PUBLIC _GNU_SOURCE)

# Additional compile options for the Debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
            -Wall
            -Wextra
            -Werror
            -Wpedantic
            -g
    )

endif ()

# Sanitizers for debugging and testing
# Requires llvm-symbolizer and sanitizer libraries (asan, ubsan, msan, tsan)
if (ENABLE_SANITIZERS)
    # Common flags for all sanitizers
    set(sanitizer_common_flags "-fno-omit-frame-pointer -g -O1")

    # Address, leak, undefined, integer, nullability sanitizers
    set(address_sanitizer_flags "-fsanitize=address,leak,undefined,integer,nullability")

    # Thread sanitizer, cannot be used with address sanitizer
    set(thread_sanitizer_flags "-fsanitize=thread -fPIE")

    # Memory sanitizer, cannot be used with address sanitizer.
    set(memory_sanitizer_flags "-fsanitize=memory -fPIE -fno-optimize-sibling-calls")

    # Add compile options
    add_compile_options(
            "SHELL:${sanitizer_common_flags}"
            "SHELL:${address_sanitizer_flags}"
    )

    # Link the enabled sanitizers.
    target_link_libraries(client PRIVATE asan ubsan)
    target_link_libraries(server PRIVATE asan ubsan)
endif ()

# Link the threading libraries explicitly
target_link_libraries(client PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE Threads::Threads)

# Tests
if (BUILD_TESTING)
    # Google Test
    find_package(GTest QUIET)
    if (NOT GTest_FOUND)
        message(STATUS "Google Test not found. Fetching...")

        include(FetchContent)

        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG a7f443b80b105f940225332ed3c31f2790092f47
                EXCLUDE_FROM_ALL
        )

        FetchContent_MakeAvailable(googletest)

        set(GTest_ManualFetch ON)

        # Alias the Google Test targets
        if (NOT TARGET GTest::gtest)
            add_library(GTest::gtest ALIAS gtest)
            add_library(GTest::gtest_main ALIAS gtest_main)
            add_library(GTest::gmock ALIAS gmock)
        endif ()

    endif ()

    enable_testing()
    add_subdirectory(tests)
endif ()

