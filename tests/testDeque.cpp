#include <gtest/gtest.h>
#include "../data_structures/queue/deque_c.h"

TEST(DequeCTest, CreateDequeShouldInitializeEmptyDeque) {
    Deque *deque = create_deque();
    ASSERT_NE(deque, nullptr);
    ASSERT_TRUE(deque_empty(deque));
    ASSERT_EQ(deque_size(deque), 0);
    destroy_deque(deque);
}

TEST(DequeCTest, DequePushFrontShouldAddElementToFront) {
    Deque *deque = create_deque();
    int data = 5;
    ASSERT_TRUE(deque_push_front(deque, &data));
    ASSERT_EQ(deque_size(deque), 1);
    ASSERT_EQ(*(int *) deque_front(deque), data);
    destroy_deque(deque);
}

TEST(DequeCTest, DequePushBackShouldAddElementToBack) {
    Deque *deque = create_deque();
    int data = 5;
    ASSERT_TRUE(deque_push_back(deque, &data));
    ASSERT_EQ(deque_size(deque), 1);
    ASSERT_EQ(*(int *) deque_back(deque), data);
    destroy_deque(deque);
}

TEST(DequeCTest, DequePopFrontShouldRemoveAndReturnFrontElement) {
    Deque *deque = create_deque();
    int data = 5;
    deque_push_front(deque, &data);
    ASSERT_EQ(*(int *) deque_pop_front(deque), data);
    ASSERT_TRUE(deque_empty(deque));
    destroy_deque(deque);
}

TEST(DequeCTest, DequePopBackShouldRemoveAndReturnBackElement) {
    Deque *deque = create_deque();
    int data = 5;
    deque_push_back(deque, &data);
    ASSERT_EQ(*(int *) deque_pop_back(deque), data);
    ASSERT_TRUE(deque_empty(deque));
    destroy_deque(deque);
}

TEST(DequeCTest, DequeResizeShouldDoubleCapacityWhenFull) {
    Deque *deque = create_deque();
    for (int i = 0; i < 16; ++i) {
        deque_push_back(deque, &i);
    }
    ASSERT_EQ(deque_size(deque), 16);
    int data = 17;
    ASSERT_TRUE(deque_push_back(deque, &data));
    ASSERT_EQ(deque_size(deque), 17);
    destroy_deque(deque);
}
