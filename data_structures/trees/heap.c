#include "heap.h"

/**
 * @brief A function to move a heap item upwards in the heap.
 *
 * This function is used to maintain the heap property of the heap data structure.
 * It is called when an item's value decreases and it may violate the heap property.
 *
 * @param item A pointer to the array of heap items.
 * @param pos The position of the item in the array that needs to be moved up.
 */
static void heap_up(HeapItem *item, size_t pos) {
    const HeapItem t = item[pos];
    while (pos > 0 && item[heap_parent(pos)].val > t.val) {
        // Swap with the parent
        item[pos] = item[heap_parent(pos)];
        // Update the position reference of the moved item
        *item[pos].ref = pos;
        // Move to the parent's position
        pos = heap_parent(pos);
    }
    // Place the item in its final position
    item[pos] = t;
    // Update the position reference of the item
    *item[pos].ref = pos;
}

/**
 * @brief A function to move a heap item downwards in the heap.
 *
 * This function is used to maintain the heap property of the heap data structure.
 * It is called when an item's value increases and it may violate the heap property.
 *
 * @param item A pointer to the array of heap items.
 * @param pos The position of the item in the array that needs to be moved down.
 * @param len The total number of items in the heap.
 */
static void heap_down(HeapItem *item, size_t pos, const size_t len) {
    const HeapItem t = item[pos];
    while (true) {
        // Find the smallest one among the parent and their kids
        const size_t l = heap_left(pos);
        const size_t r = heap_right(pos);
        size_t min_pos = -1;
        size_t min_val = t.val;
        if (l < len && item[l].val < min_val) {
            min_pos = l;
            min_val = item[l].val;
        }
        if (r < len && item[r].val < min_val)
            min_pos = r;

        if (min_pos == (size_t) -1) break;
        // Swap with the kid
        item[pos] = item[min_pos];
        *item[pos].ref = pos;
        pos = min_pos;
    }
    item[pos] = t;
    *item[pos].ref = pos;
}

/**
 * @brief A function to update a heap item's position in the heap.
 *
 * @param item A pointer to the array of heap items.
 * @param pos The position of the item in the array that needs to be updated.
 * @param len The total number of items in the heap.
 */
void heap_update(HeapItem *item, const size_t pos, const size_t len) {
    if (pos > 0 && item[heap_parent(pos)].val > item[pos].val)
        heap_up(item, pos);
    else heap_down(item, pos, len);
}
