#pragma once

#include <stdint.h>

/**
 * @brief A structure representing a node in the hash table.
 *
 * Each node has a pointer to the next node and a hash code.
 */
struct HNode {
    struct HNode *next; ///< A pointer to the next node in the hash table.
    uint64_t hcode;     ///< The hash code of the node.
};
typedef struct HNode HNode;

/**
 * @brief A structure representing a simple fixed-sized hash table.
 *
 * It contains an array of pointers to nodes, a mask for fast modulo operations,
 * and the size of the hash table.
 */
struct HTab {
    HNode **tab;  ///< An array of pointers to nodes in the hash table.
    size_t mask;  ///< A mask for fast modulo operations.
    size_t size;  ///< The number of nodes in the hash table.
};
typedef struct HTab HTab;

/**
 * @brief A structure representing a real hash table that uses two hash tables for progressive resizing.
 *
 * It contains two hash tables (\p ht1 and \p ht2), and a resizing position.
 */
typedef struct HMap {
    HTab ht1;            ///< The newer hash table.
    HTab ht2;            ///< The older hash table.
    size_t resizing_pos; ///< The current position in the resizing process.
} HMap;

HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));

void hm_insert(HMap *hmap, HNode *node);

HNode *hm_pop(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));

size_t hm_size(const HMap *hmap);

void hm_destroy(HMap *hmap);
