#pragma once
#include "avl.h"
#include "hashtable.h"

#if __cplusplus
extern "C" {
#endif

/**
 * @brief A structure representing a sorted set,
 * implemented with an AVL tree and a hashmap.
 */
struct ZSet {
    AVLNode *tree; ///< The root of the AVL tree.
    HMap hmap;     ///< The hashmap for fast lookups.
};

typedef struct ZSet ZSet;

// Ignore the warning about zero-length arrays (the 'name' field in ZNode)
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/**
 * @brief A structure representing a node in a sorted set.
 */
struct ZNode {
    AVLNode tree; ///< The AVL tree node.
    HNode hmap;   ///< The hashmap node.
    double score; ///< The score of the node. Nodes in the set are sorted by this score.
    size_t len ;  ///< The length of the name.
    char name[0]; ///< The name of the node. This is a flexible array member, it can hold an array of any size.
};

// Restore the warning settings
#if __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

typedef struct ZNode ZNode;

bool zset_add(ZSet *zset, const char *name, size_t len, double score);

ZNode *zset_lookup(ZSet *zset, const char *name, size_t len);

ZNode *zset_pop(ZSet *zset, const char *name, size_t len);

ZNode *zset_query(const ZSet *zset, double score, const char *name, size_t len);

void zset_dispose(ZSet *zset);

ZNode *znode_offset(ZNode *node, int64_t offset);

void znode_del(ZNode *node);

static inline void zset_init(ZSet *zset) {
    zset->tree = nullptr;
    zset->hmap.resizing_pos = 0;
    zset->hmap.ht1.size = 0;
    zset->hmap.ht1.mask = 0;
    zset->hmap.ht1.tab = nullptr;
    zset->hmap.ht2.size = 0;
    zset->hmap.ht2.mask = 0;
    zset->hmap.ht2.tab = nullptr;
}

static inline void znode_init(ZNode *node) {
    avl_init(&node->tree);
    node->hmap.next = nullptr;
    node->hmap.hcode = 0;
    node->score = 0;
    node->len = 0;
}
#if __cplusplus
    }
#endif
