#include "../data_structures/queue/deque_c.h"
#include <assert.h>

void create_deque_should_initialize_empty_deque() {
    Deque *deque = create_deque();
    assert(deque != nullptr);
    assert(deque_empty(deque));
    assert(deque_size(deque) == 0);
    destroy_deque(deque);
}

void deque_push_front_should_add_element_to_front() {
    Deque *deque = create_deque();
    int data = 5;
    assert(deque_push_front(deque, &data));
    assert(deque_size(deque) == 1);
    assert(*(int *)deque_front(deque) == data);
    destroy_deque(deque);
}

void deque_push_back_should_add_element_to_back() {
    Deque *deque = create_deque();
    int data = 5;
    assert(deque_push_back(deque, &data));
    assert(deque_size(deque) == 1);
    assert(*(int *)deque_back(deque) == data);
    destroy_deque(deque);
}

void deque_pop_front_should_remove_and_return_front_element() {
    Deque *deque = create_deque();
    int data = 5;
    deque_push_front(deque, &data);
    assert(*(int *)deque_pop_front(deque) == data);
    assert(deque_empty(deque));
    destroy_deque(deque);
}

void deque_pop_back_should_remove_and_return_back_element() {
    Deque *deque = create_deque();
    int data = 5;
    deque_push_back(deque, &data);
    assert(*(int *)deque_pop_back(deque) == data);
    assert(deque_empty(deque));
    destroy_deque(deque);
}

void deque_resize_should_double_capacity_when_full() {
    Deque *deque = create_deque();
    for (int i = 0; i < 16; ++i) {
        deque_push_back(deque, &i);
    }
    assert(deque_size(deque) == 16);
    int data = 17;
    assert(deque_push_back(deque, &data));
    assert(deque_size(deque) == 17);
    destroy_deque(deque);
}

int main() {
    create_deque_should_initialize_empty_deque();
    deque_push_front_should_add_element_to_front();
    deque_push_back_should_add_element_to_back();
    deque_pop_front_should_remove_and_return_front_element();
    deque_pop_back_should_remove_and_return_back_element();
    deque_resize_should_double_capacity_when_full();
    return 0;
}
