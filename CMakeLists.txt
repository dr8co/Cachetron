cmake_minimum_required(VERSION 3.27)

project(Cachetron C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Google Test requires at least C++14
set(CMAKE_CXX_STANDARD 20)
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

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Additional options for the Debug build
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

# Sanitizers for debugging (Disabled by default, slows down the program. Uncomment to enable.)
# GCC does not support all sanitizers, so Clang is recommended for this purpose. Requires llvm-symbolizer.
# The build type should be RelWithDebInfo (or Debug, adjust the variables appropriately) for sanitizers to work properly.
if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    # Address, leak, undefined, integer, nullability sanitizers
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -fsanitize=address,leak,undefined,integer,nullability -fno-omit-frame-pointer -g")

    # Thread sanitizer, cannot be used with address sanitizer
    #  set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -fsanitize=thread -fPIE -fno-omit-frame-pointer -g -O1")

    # Memory sanitizer, cannot be used with address sanitizer.
    # set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -fsanitize=memory -fPIE -fno-optimize-sibling-calls -fno-omit-frame-pointer -g -O1")
endif ()

# Google Test
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/f10e11fb27301fba21caa71030bb5024e67aa135.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Data structures
file(GLOB_RECURSE DS_SOURCES
        data_structures/*.c
        data_structures/*.h)

# server executable
add_executable(server
        server.c
        common.h
        thread_pool.c
        thread_pool.h
        commands.h
        ${DS_SOURCES})

# client executable
add_executable(client
        client.c
        common.h
        data_structures/vector/vector_c.c
        data_structures/vector/vector_c.h
        data_structures/string/string_c.c
        data_structures/string/string_c.h)

# Tests
add_subdirectory(tests)
