#pragma once
#include "avl.h"
#include "hashtable.h"

/**
 * @brief A structure representing a sorted set,
 * implemented with an AVL tree and a hashmap.
 */
struct ZSet {
    AVLNode *tree; ///< The root of the AVL tree.
    HMap hmap;     ///< The hashmap for fast lookups.
};

typedef struct ZSet ZSet;

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

typedef struct ZNode ZNode;

bool zset_add(ZSet *zset, const char *name, size_t len, double score);

ZNode *zset_lookup(ZSet *zset, const char *name, size_t len);

ZNode *zset_pop(ZSet *zset, const char *name, size_t len);

ZNode *zset_query(const ZSet *zset, double score, const char *name, size_t len);

void zset_dispose(ZSet *zset);

ZNode *znode_offset(ZNode *node, int64_t offset);

void znode_del(ZNode *node);
