#include "deque_c.h"
#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Creates a new Deque.
 *
 * This function allocates memory for a new Deque structure and initializes its members.\n
 * The initial capacity of the Deque is set to 16. The size is set to 0, and the front and back
 * indices are set to SIZE_MAX, indicating that the Deque is empty.
 *
 * If memory allocation fails at any point, the function cleans up any previously allocated memory
 * and returns nullptr.
 *
 * @return A pointer to the newly created Deque, or nullptr if memory allocation failed.
 */
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

/**
 * @brief Resizes the Deque.
 *
 * @param deque A pointer to the Deque.
 * @return True if the resizing was successful, false otherwise.
 */
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
        // Update the dequeue data and capacity
        deque->data = new_data;
        deque->capacity = new_capacity;
        return true;
    }
    return false;
}

/**
 * @brief Pushes an element to the front of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @param data The data to be pushed to the front of the Deque.
 * @return True if the push operation was successful, false otherwise.
 */
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

/**
 * @brief Appends an element to the back of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @param data The data to be pushed to the back of the Deque.
 * @return True if the push operation was successful, false otherwise.
 */
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

/**
 * @brief Removes an element from the front of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @return The front element of the Deque, or nullptr if the Deque is empty.
 */
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

/**
 * @brief Removes an element from the back of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @return The back element of the Deque, or nullptr if the Deque is empty.
 */
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

/**
 * @brief Destroys the Deque.
 *
 * This function frees the memory allocated for the Deque's data array and the Deque structure itself.
 *
 * @param deque A pointer to the Deque.
 */
void destroy_deque(Deque *deque) {
    free(deque->data);
    free(deque);
}

/**
 * @brief Checks if the Deque is empty.
 *
 * @param deque A pointer to the Deque.
 * @return True if the Deque is empty, false otherwise.
 */
bool deque_empty(const Deque *deque) {
    return deque->size == 0;
}

/**
 * @brief Gets the size of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @return The size of the Deque.
 */
size_t deque_size(const Deque *deque) {
    return deque->size;
}

/**
 * @brief Checks if the Deque is full.
 *
 * @param deque A pointer to the Deque.
 * @return True if the Deque is full, false otherwise.
 */
bool deque_full(const Deque *deque) {
    return deque->size == deque->capacity;
}

/**
 * @brief Gets the front element of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @return The front element of the Deque, or nullptr if the Deque is empty.
 */
void *deque_front(const Deque *deque) {
    return deque_empty(deque) ? nullptr : deque->data[deque->front];
}

/**
 * @brief Gets the back element of the Deque.
 *
 * @param deque A pointer to the Deque.
 * @return The back element of the Deque, or nullptr if the Deque is empty.
 */
void *deque_back(const Deque *deque) {
    return deque_empty(deque) ? nullptr : deque->data[deque->back];
}
