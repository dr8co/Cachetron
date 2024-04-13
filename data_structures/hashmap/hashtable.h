#pragma once

#if __cplusplus
extern "C" {
#endif
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
    HNode **tab; ///< An array of pointers to nodes in the hash table.
    size_t mask; ///< A mask for fast modulo operations.
    size_t size; ///< The number of nodes in the hash table.
};

typedef struct HTab HTab;

/**
 * @brief A structure representing a real hash table that uses two hash tables for progressive resizing.
 *
 * It contains two hash tables (\p ht1 and \p ht2), and a resizing position.
 */
struct HMap {
    HTab ht1;            ///< The newer hash table.
    HTab ht2;            ///< The older hash table.
    size_t resizing_pos; ///< The current position in the resizing process.
};

typedef struct HMap HMap;

HNode *hm_lookup(HMap *hmap, const HNode *key, bool (*eq)(const HNode *, const HNode *));

void hm_insert(HMap *hmap, HNode *node);

HNode *hm_pop(HMap *hmap, const HNode *key, bool (*eq)(const HNode *, const HNode *));

size_t hm_size(const HMap *hmap);

void hm_destroy(HMap *hmap);

/**
 * @brief Initialize a hash node.
 *
 * @param node A pointer to the hash node to be initialized.
 */
static inline void init_hnode(HNode *node) {
    node->next = nullptr;
    node->hcode = 0;
}

/**
 * @brief Initialize a hash table.
 *
 * @param htab A pointer to the hash table to be initialized.
 */
static inline void init_htab(HTab *htab) {
    htab->tab = nullptr;
    htab->mask = 0;
    htab->size = 0;
}

/**
 * @brief Initialize a hash map.
 *
 * @param hmap A pointer to the hash map to be initialized.
 */
static inline void init_hmap(HMap *hmap) {
    init_htab(&hmap->ht1);
    init_htab(&hmap->ht2);
    hmap->resizing_pos = 0;
}
#if __cplusplus
    }
#endif
