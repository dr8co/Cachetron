#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "vector.h"

// Create a new vector, with initial capacity of 16
vector vector_new() {
    const vector v = {
        .data = malloc(16 * sizeof(void *)),
        .size = 0,
        .capacity = 16
    };
    return v;
}

// Free the memory used by the vector
void vector_free(vector *const restrict v) {
    if (v->data) {
        free(v->data);
        v->data = nullptr;
    }
    v->size = 0;
    v->capacity = 0;
}

// Add a new element to the end of the vector
void vector_push_back(vector *const restrict v, void *const restrict elem) {
    errno = 0;
    if (v->size == v->capacity) {
        v->capacity *= 2;
        void *temp = realloc(v->data, v->capacity * sizeof(void *));
        if (temp == nullptr) {
            return;
        }
        v->data = temp;
    }
    v->data[v->size++] = elem;
}

// Get the element at the given index
void *vector_at(const vector *const restrict v, const size_t index) {
    if (index < v->size) {
        return v->data[index];
    }
    return nullptr;
}

// Remove the last element from the vector
inline void vector_pop_back(vector *const restrict v) {
    if (v->size > 0) {
        --v->size;
    }
}

// Check if the vector is empty
inline bool vector_empty(const vector *const restrict v) {
    return v->size;
}

// Remove the element at the given index
void vector_erase(vector *const restrict v, const size_t index) {
    if (index < v->size) {
        for (size_t i = index; i < v->size - 1; ++i) {
            v->data[i] = v->data[i + 1];
        }
        --v->size;
    }
}

// Insert a new element at the given index
void vector_insert(vector *const restrict v, const size_t index, void *const restrict elem) {
    errno = 0;
    if (index < v->size) {
        if (v->size == v->capacity) {
            v->capacity *= 2;
            void *temp = realloc(v->data, v->capacity * sizeof(void *));
            if (temp == nullptr) {
                return;
            }
            v->data = temp;
        }
        for (size_t i = v->size; i > index; --i) {
            v->data[i] = v->data[i - 1];
        }
        v->data[index] = elem;
        ++v->size;
    }
}

// Modify the element at the given index
void vector_set(const vector *const restrict v, const size_t index, void *const restrict elem) {
    if (index < v->size) {
        v->data[index] = elem;
    }
}

// Get the number of elements in the vector
inline size_t vector_size(const vector *const restrict v) {
    return v->size;
}

// Get the capacity of the vector
inline size_t vector_capacity(const vector *const restrict v) {
    return v->capacity;
}

// Remove all elements from the vector
inline void vector_clear(vector *const restrict v) {
    v->size = 0;
}
