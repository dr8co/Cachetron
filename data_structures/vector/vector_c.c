#include <stdlib.h>
#include <string.h>
#include "vector_c.h"

/**
 * @brief Creates a new vector with an initial capacity of 16.
 *
 * @param elem_size The size of the elements that the vector will hold.
 * @return A pointer to the newly created vector, or NULL if memory allocation failed.
 */
vector_c *vector_new(const size_t elem_size) {
    // Allocate memory for the vector
    vector_c *v = malloc(sizeof(vector_c));
    if (v) {
        // Allocate memory for the data array
        v->data = malloc(16 * elem_size);
        if (v->data) {
            // Initialize the vector fields
            v->size = 0;
            v->capacity = 16;
            v->element_size = elem_size;
            return v;
        }
        // Free the vector if memory allocation failed
        free(v);
    }
    return nullptr;
}

/**
 * @brief Frees the memory used by the vector.
 *
 * @param v A pointer to the vector to be freed.
 */
void vector_free(vector_c *const restrict v) {
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

/**
 * @brief Computes the smallest power of 2 greater than or equal to the input integer.
 *
 * @param x The input integer.
 * @return The smallest power of 2 greater than or equal to the input integer.
 */
__attribute_pure__ static size_t clp2(size_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return ++x;
}

/**
 * @brief Resizes the vector to the given size.
 *
 *
 * @param v A pointer to the vector to be resized.
 * @param size The new size for the vector.
 * @return true if the operation was successful, false otherwise.
 *
 * @note The function resizes the vector to the next power of 2 greater than or equal to the given size.\n
 * Resizing the vector to a smaller size than the current size has no effect.\n \n
 * The function returns false if the resizing was necessary and failed, true otherwise.
 * @warning This function is for internal use only.
 */
static bool vector_resize(vector_c *const restrict v, size_t size) {
    if (size <= 16) return true;

    // Resize the vector, if necessary
    size = clp2(size);
    if (size >= v->capacity) {
        void *temp = realloc(v->data, size * v->element_size);
        // If the resizing failed, the existing data is preserved
        if (temp == nullptr) return false;
        // Update the vector fields
        v->data = temp;
        v->capacity = size;
    }
    return true;
}

/**
 * @brief Adds a range of elements to the end of the vector.
 *
 * @param v A pointer to the vector to which the elements will be added.
 * @param elems A pointer to the elements to be added.
 * @param count The number of elements to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool vector_append(vector_c *const restrict v, const void *const restrict elems, const size_t count) {
    if (v && elems) {
        if (count == 0) return true;

        const size_t new_size = v->size + count;

        // Resize the vector, if necessary
        if (new_size >= v->capacity) {
            if (!vector_resize(v, new_size)) return false;
        }
        // Add the elements to the end of the vector
        memcpy((char *) v->data + v->size * v->element_size, elems, v->element_size * count);
        v->size = new_size;
        return true;
    }
    return false;
}

/**
 * @brief Adds a new element to the end of the vector.
 *
 * Both the vector and the element must not be NULL for the operation to be successful.
 *
 * @param v A pointer to the vector to which the element will be added.
 * @param elem A pointer to the element to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool vector_push_back(vector_c *const restrict v, const void *const restrict elem) {
    return v && elem && vector_append(v, elem, 1);
}

/**
 * @brief Retrieves the element at the given index in the vector.
 *
 * @param v A pointer to the vector.
 * @param index The index of the element to be retrieved.
 * @return A pointer to the element at the given index, or NULL if the index is out of bounds or the vector is NULL.
 */
void *vector_at(const vector_c *const restrict v, const size_t index) {
    return v && index < v->size ? (char *) v->data + index * v->element_size : nullptr;
}

/**
 * @brief Retrieves the last element of the vector.
 *
 * @param v A pointer to the vector.
 * @return A pointer to the last element in the vector, or nullptr if the vector is empty or is nullptr.
 */
void *vector_back(const vector_c *restrict v) {
    return v && v->size ? (char *) v->data + (v->size - 1) * v->element_size : nullptr;
}

/**
 * @brief Retrieves the first element of the vector.
 *
 * @param v A pointer to the vector.
 * @return A pointer to the first element in the vector, or nullptr if the vector is empty or is nullptr.
 */
void *vector_front(const vector_c *restrict v) {
    return v && v->size ? v->data : nullptr;
}

/**
 * @brief Removes the last element from the vector.
 *
 * @param v A pointer to the vector from which the last element will be removed.
 */
void vector_pop_back(vector_c *const restrict v) {
    if (v && v->size) --v->size;
}

/**
 * @brief Checks if the vector is empty.
 *
 * @param v A pointer to the vector to be checked.
 * @return true if the vector is empty or NULL, false otherwise.
 */
bool vector_empty(const vector_c *const restrict v) {
    return v == nullptr || v->size == 0;
}

/**
 * @brief Removes the element at the given index from the vector.
 *
 * @param v A pointer to the vector from which the element will be removed.
 * @param index The index of the element to be removed.
 * @return true if the operation was successful, false otherwise.
 */
bool vector_erase(vector_c *const restrict v, const size_t index) {
    // Check if the vector and the index are valid
    if (v && index < v->size) {
        // Move the elements that come after the index one position to the left
        memmove((char *) v->data + index * v->element_size, (char *) v->data + (index + 1) * v->element_size,
                v->element_size * (v->size - index));
        --v->size;
        return true;
    }
    return false;
}

/**
 * @brief Inserts a new element at the given index in the vector.
 *
 * @param v A pointer to the vector in which the element will be inserted.
 * @param index The index at which the element will be inserted.
 * @param elem A pointer to the element to be inserted.
 * @return true if the operation was successful, false otherwise.
 */
bool vector_insert(vector_c *const restrict v, const size_t index, const void *const restrict elem) {
    if (v && elem) {
        if (index < v->size) {
            // Resize the vector, if necessary
            if (v->size + 1 >= v->capacity) {
                if (!vector_resize(v, v->capacity * 2)) return false;
            }
            // Move the elements that come after the index one position to the right
            memmove((char *) v->data + (index + 1) * v->element_size, (char *) v->data + index * v->element_size,
                    v->element_size * (v->size - index));

            // Insert the element at the given index
            memcpy((char *) v->data + index * v->element_size, elem, v->element_size);
            ++v->size;

            return true;
        }
        if (index == 0 && v->size == 0) return vector_push_back(v, elem);
    }
    return false;
}

/**
 * @brief Returns the maximum of two unsigned integers.
 *
 * @param x, y unsigned integers.
 * @return The maximum of \p x and \p y.
 *
 * @note This function uses bitwise operations to calculate the maximum of two unsigned integers.\n
 * It does not use conditional statements, which makes it faster in some cases.
 */
static inline size_t max(const size_t x, const size_t y) {
    return x ^ ((x ^ y) & -(x < y));
}

/**
 * @brief Sets a range of elements in the vector to a specific value.
 *
 * @param v A pointer to the vector in which the range will be set.
 * @param elem A pointer to the element to be set.
 * @param index The index at which the range starts.
 * @param count The number of elements in the range.
 * @return true if the operation was successful, false otherwise.
 */
bool vector_set_range(vector_c *const restrict v, const void *const restrict elem, const size_t index,
                      const size_t count) {
    if (v && elem) {
        if (index < v->size) {
            const size_t new_size = max(v->size, count + index);

            // Resize the vector, if necessary
            if (new_size > v->size)
                if (!vector_resize(v, new_size)) return false;

            // Set the range to the given value
            memcpy((char *) v->data + index * v->element_size, elem, v->element_size * count);
            v->size = new_size;

            return true;
        }
        // Special case: if the index is 0 and the vector is empty, append the range to the vector
        if (index == 0 && v->size == 0) return vector_append(v, elem, count);
    }
    return false;
}

/**
 * @brief Modifies the element at the given index in the vector.
 *
 * @param v A pointer to the vector in which the element will be modified.
 * @param index The index of the element to be modified.
 * @param elem A pointer to the new element.
 * @return true if the operation was successful, false otherwise.
 */
bool vector_set(vector_c *const restrict v, const size_t index, const void *const restrict elem) {
    // The element and the vector must not be NULL
    if (v == nullptr || elem == nullptr) return false;
    return vector_set_range(v, elem, index, 1);
}

/**
 * @brief Returns the underlying array of the vector.
 *
 * @param v A pointer to the vector.
 * @return A pointer to the underlying array of the vector, or NULL if the vector is NULL.
 */
void *vector_data(const vector_c *const restrict v) {
    return v ? v->data : nullptr;
}

/**
 * @brief Returns the number of elements in the vector.
 *
 * @param v A pointer to the vector.
 * @return The number of elements in the vector, or 0 if the vector is NULL.
 */
size_t vector_size(const vector_c *const restrict v) {
    return v ? v->size : 0;
}

/**
 * @brief Returns the capacity of the vector.
 *
 * @param v A pointer to the vector.
 * @return The capacity of the vector, or 0 if the vector is NULL.
 */
size_t vector_capacity(const vector_c *const restrict v) {
    return v ? v->capacity : 0;
}

/**
 * @brief Removes all elements from the vector.
 *
 * This function does not free the memory used by the vector, it only resets the size to 0.
 *
 * @param v A pointer to the vector to be cleared.
 */
void vector_clear(vector_c *const restrict v) {
    if (v) v->size = 0;
}

/*//////////////////////// ptr_vector ////////////////////////*/

/**
 * @brief Creates a new pointer vector with an initial capacity of 16.
 *
 * @return A pointer to the newly created pointer vector, or NULL if memory allocation failed.
 */
ptr_vector *ptr_vector_new() {
    // Allocate memory for the pointer vector
    ptr_vector *v = malloc(sizeof(ptr_vector));
    if (v) {
        // Allocate memory for the data array
        if ((v->data = (void **) malloc(16 * sizeof(void *)))) {
            // Initialize the vector fields
            v->size = 0;
            v->capacity = 16;
            return v;
        }
        // Free the vector if memory allocation failed
        free(v);
    }
    return nullptr;
}

/**
 * @brief Frees the memory used by the pointer vector.
 *
 * @param v A pointer to the pointer vector to be freed.
 */
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

/**
 * @brief Resizes a pointer vector to a given size.
 *
 * @param v A pointer to the pointer vector to be resized.
 * @param size The new size for the vector.
 * @return true if the operation was successful, false otherwise.
 *
 * @note The function resizes the vector to the next power of 2 greater than or equal to the given size.\n
 * Resizing the vector to a smaller size than the current size has no effect.\n \n
 * The function returns false if the resizing was necessary and failed, true otherwise.
 *
 * @warning This function is for internal use only.
 * Use \p ptr_vector_expand to increase the number of elements in the vector.
 */
static bool ptr_vector_resize(ptr_vector *const restrict v, size_t size) {
    if (size <= 16) return true;

    // Resize the vector, if necessary
    size = clp2(size);
    if (size >= v->capacity) {
        void *temp = realloc(v->data, size * sizeof(void *));
        // If the resizing failed, the existing data is preserved
        if (temp == nullptr) return false;
        // Update the vector fields
        v->data = (void **) temp;
        v->capacity = size;
    }
    return true;
}

/**
 * @brief Adds a new element to the end of the pointer vector.
 *
 * @param v A pointer to the pointer vector to which the element will be added.
 * @param elem A pointer to the element to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool ptr_vector_push_back(ptr_vector *const restrict v, void *const restrict elem) {
    if (v) {
        // Resize the vector, if necessary
        if (v->size == v->capacity)
            if (!ptr_vector_resize(v, v->capacity * 2)) return false;

        v->data[v->size++] = elem;
        return true;
    }

    return false;
}

/**
 * @brief Retrieves the element at the given index in the pointer vector.
 *
 * @param v A pointer to the pointer vector.
 * @param index The index of the element to be retrieved.
 * @return A pointer to the element at the given index, or NULL if the index is out of bounds or the vector is NULL.
 */
void *ptr_vector_at(const ptr_vector *const restrict v, const size_t index) {
    return v && index < v->size ? v->data[index] : nullptr;
}

/**
 * @brief Removes the last element from the pointer vector.
 *
 * @param v A pointer to the pointer vector from which the last element will be removed.
 */
void ptr_vector_pop_back(ptr_vector *const restrict v) {
    if (v && v->size) --v->size;
}

/**
 * @brief Checks if the pointer vector is empty.
 *
 * @param v A pointer to the pointer vector to be checked.
 * @return true if the vector is empty or NULL, false otherwise.
 */
bool ptr_vector_empty(const ptr_vector *const restrict v) {
    return v == nullptr || v->size == 0;
}

/**
 * @brief Removes the element at the given index from the pointer vector.
 *
 * @param v A pointer to the pointer vector from which the element will be removed.
 * @param index The index of the element to be removed.
 * @return true if the operation was successful, false otherwise.
 */
bool ptr_vector_erase(ptr_vector *const restrict v, const size_t index) {
    // Check if the vector and the index are valid
    if (v && index < v->size) {
        // Move the elements that come after the index one position to the left
        memmove(v->data + index, v->data + index + 1, (v->size - index) * sizeof(void *));
        --v->size;
        return true;
    }
    return false;
}

/**
 * @brief Modifies the element at the given index in the pointer vector.
 *
 * @param v A pointer to the pointer vector in which the element will be modified.
 * @param index The index of the element to be modified.
 * @param elem A pointer to the new element.
 * @return true if the operation was successful, false otherwise.
 */
bool ptr_vector_set(const ptr_vector *const restrict v, const size_t index, void *const restrict elem) {
    if (v && index < v->size) {
        v->data[index] = elem;
        return true;
    }
    return false;
}

/**
 * @brief Resizes the pointer vector to a given size and initializes the new elements to NULL.
 *
 * @param v A pointer to the pointer vector to be resized.
 * @param new_size The new size for the vector.
 * @return true if the operation was successful, false otherwise.
 *
 * @note This function differs from \p ptr_vector_resize in that it increases the number of elements
 * (and the capacity, if necessary) in the vector, while \p ptr_vector_resize only changes the capacity of the vector.
 */
bool ptr_vector_expand(ptr_vector *restrict v, const size_t new_size) {
    if (v) {
        if (new_size <= v->size) return true;
        if (ptr_vector_resize(v, new_size)) {
            // Initialize the new elements
            for (size_t i = v->size; i < new_size; ++i)
                ptr_vector_push_back(v, nullptr);

            v->size = new_size;
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns the underlying data array of the pointer vector.
 *
 * @param v A pointer to the pointer vector.
 * @return A pointer to the underlying data array of the pointer vector, or NULL if the pointer vector is NULL.
 */
void **ptr_vector_data(const ptr_vector *const restrict v) {
    return v ? v->data : nullptr;
}

/**
 * @brief Returns the number of elements in the pointer vector.
 *
 * @param v A pointer to the pointer vector.
 * @return The number of elements in the pointer vector, or 0 if the pointer vector is NULL.
 */
size_t ptr_vector_size(const ptr_vector *const restrict v) {
    return v ? v->size : 0;
}

/**
 * @brief Returns the capacity of the pointer vector.
 *
 * @param v A pointer to the pointer vector.
 * @return The capacity of the pointer vector, or 0 if the pointer vector is NULL.
 */
size_t ptr_vector_capacity(const ptr_vector *const restrict v) {
    return v ? v->capacity : 0;
}

/**
 * @brief Removes all elements from the pointer vector.
 *
 * @param v A pointer to the pointer vector to be cleared.
 * @note This function does not free the memory used by the pointer vector, it only resets the size to 0.
 */
void ptr_vector_clear(ptr_vector *const restrict v) {
    if (v) v->size = 0;
}

/**
 * @brief Retrieves the last element of the pointer vector.
 *
 * @param v A pointer to the pointer vector.
 * @return A pointer to the last element in the pointer vector, or nullptr if the vector is empty or is nullptr.
 */
void *ptr_vector_back(const ptr_vector *restrict v) {
    return v && v->size ? v->data[v->size - 1] : nullptr;
}

/**
 * @brief Retrieves the first element of the pointer vector.
 *
 * @param v A pointer to the pointer vector.
 * @return A pointer to the first element in the pointer vector, or nullptr if the vector is empty or is nullptr.
 */
void *ptr_vector_front(const ptr_vector *restrict v) {
    return v && v->size ? v->data[0] : nullptr;
}
