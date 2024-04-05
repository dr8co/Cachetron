#include <gtest/gtest.h>
#include "../data_structures/vector/vector_c.h"

TEST(VectorCTest, NewCreatesEmptyVector) {
    vector_c *v = vector_new(sizeof(int));
    ASSERT_NE(v, nullptr);
    ASSERT_EQ(vector_size(v), 0);
    ASSERT_EQ(vector_capacity(v), 16);
    vector_free(v);
}

TEST(VectorCTest, PushBackIncreasesSize) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    ASSERT_EQ(vector_size(v), 1);
    vector_free(v);
}

TEST(VectorCTest, PushBackStoresCorrectValue) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    ASSERT_EQ(*(int *) vector_at(v, 0), value);
    vector_free(v);
}

TEST(VectorCTest, PopBackDecreasesSize) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    vector_pop_back(v);
    ASSERT_EQ(vector_size(v), 0);
    vector_free(v);
}

TEST(VectorCTest, AppendIncreasesSize) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int values[] = {1, 2, 3, 4, 5};
    vector_append(v, values, 5);
    ASSERT_EQ(vector_size(v), 5);
    vector_free(v);
}

TEST(VectorCTest, AppendStoresCorrectValues) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int values[] = {1, 2, 3, 4, 5};
    vector_append(v, values, 5);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(*(int *) vector_at(v, i), values[i]);
    }
    vector_free(v);
}

TEST(VectorCTest, InsertIncreasesSize) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_insert(v, 0, &value);
    ASSERT_EQ(vector_size(v), 1);
    vector_free(v);
}

TEST(VectorCTest, InsertStoresCorrectValue) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_insert(v, 0, &value);
    ASSERT_EQ(*(int *) vector_at(v, 0), value);
    vector_free(v);
}

TEST(VectorCTest, EraseDecreasesSize) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    vector_erase(v, 0);
    ASSERT_EQ(vector_size(v), 0);
    vector_free(v);
}

TEST(VectorCTest, ClearResetsSize) {
    vector_c *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    vector_clear(v);
    ASSERT_EQ(vector_size(v), 0);
    vector_free(v);
}

TEST(VectorCTest, SizeReturnsZeroForNullptr) {
    ASSERT_EQ(vector_size(nullptr), 0);
}

TEST(VectorCTest, CapacityReturnsZeroForNullptr) {
    ASSERT_EQ(vector_capacity(nullptr), 0);
}

TEST(VectorCTest, PushBackDoesNotCrashForNullptr) {
    constexpr int value = 5;
    vector_push_back(nullptr, &value);
}

TEST(VectorCTest, AtReturnsNullForNullptr) {
    ASSERT_EQ(vector_at(nullptr, 0), nullptr);
}

TEST(VectorCTest, PopBackDoesNotCrashForNullptr) {
    vector_pop_back(nullptr);
}

TEST(VectorCTest, AppendDoesNotCrashForNullptr) {
    constexpr int values[] = {1, 2, 3, 4, 5};
    vector_append(nullptr, values, 5);
}

TEST(VectorCTest, InsertDoesNotCrashForNullptr) {
    constexpr int value = 5;
    vector_insert(nullptr, 0, &value);
}

TEST(VectorCTest, EraseDoesNotCrashForNullptr) {
    vector_erase(nullptr, 0);
}

TEST(VectorCTest, ClearDoesNotCrashForNullptr) {
    vector_clear(nullptr);
}
