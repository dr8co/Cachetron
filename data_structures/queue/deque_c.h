#pragma once

#if __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief A structure representing a deque.
 *
 */
struct Deque {
    size_t capacity; ///< The capacity of the deque.
    size_t size;     ///< The number of elements in the deque.
    size_t front;    ///< The index of the front element.
    size_t back;     ///< The index of the back element.
    void **data;     ///< The array storing the elements of the deque.
};

typedef struct Deque Deque;

Deque *create_deque();

bool deque_empty(const Deque *deque);

size_t deque_size(const Deque *deque);

bool deque_full(const Deque *deque);

bool deque_resize(Deque *deque);

bool deque_push_front(Deque *deque, void *data);

bool deque_push_back(Deque *deque, void *data);

void *deque_pop_front(Deque *deque);

void *deque_pop_back(Deque *deque);

void *deque_front(const Deque *deque);

void *deque_back(const Deque *deque);

void destroy_deque(Deque *deque);

#if __cplusplus
    }
#endif
