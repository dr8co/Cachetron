#include <gtest/gtest.h>
#include "../data_structures/queue/deque_c.h"

TEST(DequeTest, CreateShouldInitializeEmptyDeque) {
    Deque *deque = create_deque();
    ASSERT_NE(deque, nullptr);
    ASSERT_TRUE(deque_empty(deque));
    ASSERT_EQ(deque_size(deque), 0);
    destroy_deque(deque);
}

TEST(DequeTest, PushFrontShouldAddElementToFront) {
    Deque *deque = create_deque();
    int data = 5;
    ASSERT_TRUE(deque_push_front(deque, &data));
    ASSERT_EQ(deque_size(deque), 1);
    ASSERT_EQ(*(int *) deque_front(deque), data);
    destroy_deque(deque);
}

TEST(DequeTest, PushBackShouldAddElementToBack) {
    Deque *deque = create_deque();
    int data = 5;
    ASSERT_TRUE(deque_push_back(deque, &data));
    ASSERT_EQ(deque_size(deque), 1);
    ASSERT_EQ(*(int *) deque_back(deque), data);
    destroy_deque(deque);
}

TEST(DequeTest, PopFrontShouldRemoveAndReturnFrontElement) {
    Deque *deque = create_deque();
    int data = 5;
    deque_push_front(deque, &data);
    ASSERT_EQ(*(int *) deque_pop_front(deque), data);
    ASSERT_TRUE(deque_empty(deque));
    destroy_deque(deque);
}

TEST(DequeTest, PopBackShouldRemoveAndReturnBackElement) {
    Deque *deque = create_deque();
    int data = 5;
    deque_push_back(deque, &data);
    ASSERT_EQ(*(int *) deque_pop_back(deque), data);
    ASSERT_TRUE(deque_empty(deque));
    destroy_deque(deque);
}

TEST(DequeTest, ResizeShouldDoubleCapacityWhenFull) {
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
