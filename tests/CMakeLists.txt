# Test executables
add_executable(test_vector
        test_vector.cpp
        ../data_structures/vector/vector_c.c
        ../data_structures/vector/vector_c.h)

add_executable(test_string
        test_string.cpp
        ../data_structures/string/lite_string.c
        ../data_structures/string/lite_string.h)

add_executable(test_deque
        test_deque.cpp
        ../data_structures/queue/deque_c.c
        ../data_structures/queue/deque_c.h)

# Link test executables with gtest
target_link_libraries(test_vector gtest gtest_main)
target_link_libraries(test_string gtest gtest_main)
target_link_libraries(test_deque gtest gtest_main)

include(GoogleTest)

# Discover tests
gtest_discover_tests(test_vector)
gtest_discover_tests(test_string)
gtest_discover_tests(test_deque)
