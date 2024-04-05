#pragma once

#if __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief A structure representing a heap item.
 *
 */
struct HeapItem {
    uint64_t val;  ///< The value of the heap item.
    size_t *ref;   ///< A pointer to the position of the item in the heap array.
};

typedef struct HeapItem HeapItem;

/**
 * @brief Initializes a heap item.
 *
 * @param item A pointer to the heap item that needs to be initialized.
 */
static inline void init_heap_item(HeapItem *item) {
    item->val = 0;
    item->ref = nullptr;
}

/**
 * @brief Calculates the parent's index of a heap item.
 *
 * @param i The index of the heap item.
 * @return The index of the parent of the heap item.
 */
static inline size_t heap_parent(const size_t i) {
    return (i + 1) / 2 - 1;
}

/**
 * @brief Calculates the left child's index of a heap item.
 *
 * @param i The index of the heap item.
 * @return The index of the left child of the heap item.
 */
static inline size_t heap_left(const size_t i) {
    return i * 2 + 1;
}

/**
 * @brief Calculates the right child's index of a heap item.
 *
 * @param i The index of the heap item.
 * @return The index of the right child of the heap item.
 */
static inline size_t heap_right(const size_t i) {
    return i * 2 + 2;
}

void heap_update(HeapItem *item, size_t pos, size_t len);

#if __cplusplus
}
#endif
