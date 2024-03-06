cmake_minimum_required(VERSION 3.28)
project(redis)

set(CMAKE_CXX_STANDARD 23)

# If the build type is not specified, default to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Additional checks for the Debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

add_executable(redis main.cpp)
