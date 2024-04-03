#include "deque_c.h"
#include <stdlib.h>
#include <stdint.h>

Deque *create_deque() {
    Deque *deque = malloc(sizeof(Deque));
    if (deque) {
        deque->data = malloc(16 * sizeof(void *));
        if (deque->data) {
            deque->capacity = 16;
            deque->size = 0;
            deque->front = SIZE_MAX;
            deque->back = SIZE_MAX;
            return deque;
        }
        free(deque);
    }
    return nullptr;
}

bool deque_empty(const Deque *deque) {
    return deque->size == 0;
}

size_t deque_size(const Deque *deque) {
    return deque->size;
}

bool deque_full(const Deque *deque) {
    return deque->size == deque->capacity;
}

bool deque_resize(Deque *deque) {
    const size_t new_capacity = deque->capacity * 2;
    void **new_data = realloc(deque->data, new_capacity * sizeof(void *));
    if (new_data) {
        // Adjust elements if wraparound occurs
        if (deque->front > deque->back) {
            size_t i, j;
            for (i = 0, j = deque->front; j < deque->capacity; i++, j++) {
                new_data[i] = deque->data[j];
            }
            for (j = 0; j <= deque->back; i++, j++) {
                new_data[i] = deque->data[j];
            }
            deque->front = 0;
            deque->back = deque->capacity - 1;
        }
        deque->data = new_data;
        deque->capacity = new_capacity;
        return true;
    }
    return false;
}

bool deque_push_front(Deque *deque, void *data) {
    if (deque_full(deque) && !deque_resize(deque)) {
        return false;
    }
    if (deque_empty(deque)) {
        deque->front = deque->back = 0;
    } else {
        deque->front = deque->front == 0 ? deque->capacity - 1 : deque->front - 1;
    }
    deque->data[deque->front] = data;
    deque->size++;
    return true;
}

bool deque_push_back(Deque *deque, void *data) {
    if (deque_full(deque) && !deque_resize(deque)) {
        return false;
    }
    if (deque_empty(deque)) {
        deque->front = deque->back = 0;
    } else {
        deque->back = (deque->back + 1) % deque->capacity;
    }
    deque->data[deque->back] = data;
    ++deque->size;
    return true;
}

void *deque_pop_front(Deque *deque) {
    if (deque_empty(deque)) {
        return nullptr;
    }
    void *data = deque->data[deque->front];
    if (deque->front == deque->back) {
        deque->front = deque->back = SIZE_MAX;
    } else {
        deque->front = (deque->front + 1) % deque->capacity;
    }
    --deque->size;
    return data;
}

void *deque_pop_back(Deque *deque) {
    if (deque_empty(deque)) {
        return nullptr;
    }
    void *data = deque->data[deque->back];
    if (deque->front == deque->back) {
        deque->front = deque->back = SIZE_MAX;
    } else {
        deque->back = deque->back == 0 ? deque->capacity - 1 : deque->back - 1;
    }
    --deque->size;
    return data;
}

void *deque_front(const Deque *deque) {
    return deque_empty(deque) ? nullptr : deque->data[deque->front];
}

void *deque_back(const Deque *deque) {
    return deque_empty(deque) ? nullptr : deque->data[deque->back];
}

void destroy_deque(Deque *deque) {
    free(deque->data);
    free(deque);
}
