# Test executables
add_executable(test_vector)
target_sources(test_vector PRIVATE
        testVector.cpp
        ../data_structures/vector/vector_c.c
        ../data_structures/vector/vector_c.h)

add_executable(test_string)
target_sources(test_string PRIVATE
        testLiteString.cpp
        ../data_structures/string/lite_string.c
        ../data_structures/string/lite_string.h)

add_executable(test_deque)
target_sources(test_deque PRIVATE
        testDeque.cpp
        ../data_structures/queue/deque_c.c
        ../data_structures/queue/deque_c.h)

add_executable(test_avl)
target_sources(test_avl PRIVATE
        test_avl.cpp
        ../data_structures/trees/avl.c
        ../data_structures/trees/avl.h)

# Add GTest include directories, if manual fetch is enabled
if (GTest_ManualFetch)
    target_include_directories(test_vector PRIVATE ${GTEST_INCLUDE_DIRS})
    target_include_directories(test_string PRIVATE ${GTEST_INCLUDE_DIRS})
    target_include_directories(test_deque PRIVATE ${GTEST_INCLUDE_DIRS})
    target_include_directories(test_avl PRIVATE ${GTEST_INCLUDE_DIRS})
endif ()

# Link test executables with gtest
target_link_libraries(test_vector PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_string PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_deque PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_avl PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)

# Discover tests
gtest_discover_tests(test_vector)
gtest_discover_tests(test_string)
gtest_discover_tests(test_deque)
gtest_discover_tests(test_avl)
