cmake_minimum_required(VERSION 3.28)
project(Cachetron C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Additional checks for the Debug build
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

add_subdirectory(tests)

add_executable(server server.c vector.c vector.h)

add_executable(client client.c vector.h)
