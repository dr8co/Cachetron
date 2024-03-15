#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "vector.h"

// Create a new vector, with initial capacity of 16
vector *vector_new(const size_t elem_size) {
    vector *v = malloc(sizeof(vector));
    if (v == nullptr) return nullptr;

    v->data = malloc(16 * elem_size);
    if (v->data == nullptr) {
        free(v);
        return nullptr;
    }
    v->size = 0;
    v->capacity = 16;
    v->element_size = elem_size;

    return v;
}

// Free the memory used by the vector
void vector_free(vector *const restrict v) {
    if (v) {
        if (v->data) {
            free(v->data);
            v->data = nullptr;
        }
        v->size = 0;
        v->capacity = 0;
        v->element_size = 0;

        free(v);
    }
}

// Round up x to the next power of 2
static unsigned clp2(unsigned x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return ++x;
}

// Resize a vector
bool vector_resize(vector *const restrict v, size_t size) {
    if (v) {
        if (size <= 16) return true;

        errno = 0;
        size = clp2(size);

        if (size >= v->size) {
            void *temp = realloc(v->data, size * v->element_size);
            if (temp == nullptr || errno) {
                return false;
            }
            v->data = temp;
            v->capacity = size;

            return true;
        }
    }

    return false;
}

// Add a range of elements to the end of the vector
bool vector_append(vector *const restrict v, const void *const restrict elems, const size_t count) {
    if (v && elems) {
        if (count == 0) return true;

        const size_t new_size = v->size + count;

        if (new_size >= v->capacity) {
            if (!vector_resize(v, new_size)) return false;
        }

        memcpy(v->data + v->size * v->element_size, elems, v->element_size * count);
        v->size = new_size;
        return true;
    }
    return false;
}

// Add a new element to the end of the vector
bool vector_push_back(vector *const restrict v, const void *const restrict elem) {
    return v && elem ? vector_append(v, elem, 1) : false;
}

// Get the element at the given index
void *vector_at(const vector *const restrict v, const size_t index) {
    if (v)
        if (index < v->size) return v->data + index * v->element_size;
    return nullptr;
}

// Remove the last element from the vector
void vector_pop_back(vector *const restrict v) {
    if (v)
        if (v->size > 0) --v->size;
}

// Check if the vector is empty
bool vector_empty(const vector *const restrict v) {
    return v ? v->size == 0 : true;
}

// Remove the element at the given index
bool vector_erase(vector *const restrict v, const size_t index) {
    if (v) {
        if (index < v->size) {
            memmove(v->data + index * v->element_size, v->data + (index + 1) * v->element_size,
                    v->element_size * (v->size - index));
            --v->size;
            return true;
        }
    }
    return false;
}

// Insert a new element at the given index
bool vector_insert(vector *const restrict v, const size_t index, const void *const restrict elem) {
    if (v && elem) {
        if (index == 0 && v->size == 0) return vector_push_back(v, elem);

        if (index < v->size) {
            if (v->size + 1 >= v->capacity) {
                v->capacity *= 2;

                if (!vector_resize(v, v->capacity)) return false;
            }
            memmove(v->data + (index + 1) * v->element_size, v->data + index * v->element_size,
                    v->element_size * (v->size - index));

            memcpy(v->data + index * v->element_size, elem, v->element_size);
            ++v->size;

            return true;
        }
    }
    return false;
}

__inline static unsigned max(const unsigned x, const unsigned y) {
    return x ^ ((x ^ y) & -(x < y));
}

bool vector_set_range(vector *const restrict v, const void *const restrict elem, const size_t index,
                      const size_t count) {
    if (v && elem) {
        if (index == 0 && v->size == 0)
            return vector_append(v, elem, count);

        if (index < v->size) {
            const size_t new_size = max(v->size, count + index);

            if (new_size > v->size) {
                v->size = new_size;
                v->capacity = new_size;

                if (!vector_resize(v, v->capacity)) return false;
            }

            memcpy(v->data + index * v->element_size, elem, v->element_size * count);
            return true;
        }
    }
    return false;
}

// Modify the element at the given index
bool vector_set(vector *const restrict v, const size_t index, const void *const restrict elem) {
    if (v == nullptr || elem == nullptr) return false;
    return vector_set_range(v, elem, index, 1);
}

bool vector_resize_expand(vector *const restrict v, const size_t new_size) {
    if (v == nullptr) return false;
    if (new_size <= v->size) return true;
    if (!vector_resize(v, new_size)) return false;

    v->size = new_size;
    return true;
}

// Returns the underlying array
void *vector_data(const vector *const restrict v) {
    return v ? v->data : nullptr;
}

// Get the number of elements in the vector
size_t vector_size(const vector *const restrict v) {
    return v ? v->size : 0;
}

// Get the capacity of the vector
size_t vector_capacity(const vector *const restrict v) {
    return v ? v->capacity : 0;
}

// Remove all elements from the vector
void vector_clear(vector *const restrict v) {
    if (v) v->size = 0;
}

///////////////////////// ptr_vector /////////////////////////

// Create a new pointer vector, with initial capacity of 16
ptr_vector *ptr_vector_new() {
    ptr_vector *v = malloc(sizeof(ptr_vector));
    if (v == nullptr) return nullptr;

    v->data = malloc(16 * sizeof(void *));
    if (v->data == nullptr) {
        free(v);
        return nullptr;
    }
    v->size = 0;
    v->capacity = 16;

    return v;
}

// Free the memory used by the pointer vector
void ptr_vector_free(ptr_vector *const restrict v) {
    if (v) {
        if (v->data) {
            free(v->data);
            v->data = nullptr;
        }
        v->size = 0;
        v->capacity = 0;

        free(v);
    }
}

// Resize a pointer vector
bool ptr_vector_resize(ptr_vector *const restrict v, size_t size) {
    if (v == nullptr) return false;
    if (size <= 16) return true;

    errno = 0;
    size = clp2(size);

    if (size >= v->size) {
        void *temp = realloc(v->data, size * sizeof(void *));
        if (temp == nullptr || errno) {
            return false;
        }
        v->data = temp;
        v->capacity = size;
    }

    return true;
}

// Add a new element to the end of the pointer vector
bool ptr_vector_push_back(ptr_vector *const restrict v, void *const restrict elem) {
    if (v == nullptr) return false;

    if (v->size >= v->capacity) {
        v->capacity *= 2;

        if (!ptr_vector_resize(v, v->capacity)) return false;
    }

    v->data[v->size++] = elem;
    return true;
}

// Get the element at the given index
void *ptr_vector_at(const ptr_vector *const restrict v, const size_t index) {
    if (v)
        if (index < v->size) return v->data[index];

    return nullptr;
}

// Remove the last element from the pointer vector
void ptr_vector_pop_back(ptr_vector *const restrict v) {
    if (v)
        if (v->size > 0) --v->size;
}

// Check if the pointer vector is empty
bool ptr_vector_empty(const ptr_vector *const restrict v) {
    return v ? v->size : false;
}

// Remove the element at the given index
bool ptr_vector_erase(ptr_vector *const restrict v, const size_t index) {
    if (v) {
        if (index < v->size) {
            memmove(v->data + index, v->data + index + 1, (v->size - index) * sizeof(void *));
            --v->size;
            return true;
        }
    }
    return false;
}

// Modify the element at the given index
bool ptr_vector_set(const ptr_vector *const restrict v, const size_t index, void *const restrict elem) {
    if (v) {
        if (index < v->size) {
            v->data[index] = elem;
            return true;
        }
    }
    return false;
}

// Resize the vector to the given size, and default construct the new elements to nullptr
bool ptr_vector_resize_expand(ptr_vector *const restrict v, const size_t new_size) {
    if (v == nullptr) return false;

    if (new_size <= v->size) return true;
    if (!ptr_vector_resize(v, new_size)) return false;

    // Default construct the new elements to point to nullptr
    for (size_t i = v->size; i < new_size; ++i) {
        ptr_vector_push_back(v, nullptr);
    }

    v->size = new_size;
    return true;
}

// Returns the underlying array
void **ptr_vector_data(const ptr_vector *const restrict v) {
    return v ? v->data : nullptr;
}

// Get the number of elements in the vector
size_t ptr_vector_size(const ptr_vector *const restrict v) {
    return v ? v->size : 0;
}

// Get the capacity of the vector
size_t ptr_vector_capacity(const ptr_vector *const restrict v) {
    return v ? v->capacity : 0;
}

// Remove all elements from the vector
void ptr_vector_clear(ptr_vector *const restrict v) {
    if (v) v->size = 0;
}
