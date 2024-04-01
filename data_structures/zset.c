#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "zset.h"
#include "../common.h"

/**
 * @brief Creates a new ZNode.
 *
 * @param name The name of the ZNode.
 * @param len The length of the name.
 * @param score The score of the ZNode.
 * @return A pointer to the newly created ZNode, or nullptr if memory allocation failed.
 */
static ZNode *znode_new(const char *name, const size_t len, const double score) {
    ZNode *node = malloc(sizeof(ZNode) + len);
    if (node) {
        avl_init(&node->tree);
        node->hmap.next = nullptr;
        node->hmap.hcode = fnv1a_hash((uint8_t *) name, len);
        node->score = score;
        node->len = len;
        memcpy(&node->name[0], name, len);
        return node;
    }
    return nullptr;
}

static size_t min(const size_t lhs, const size_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

/**
 * @brief A helper structure for hashtable lookup in ZSet.
 */
struct HKey {
    HNode node;       ///< The HNode used for the hashtable lookup.
    const char *name; ///< The name of the tuple to look up.
    size_t len;       ///< The length of the name.
};

typedef struct HKey HKey;

static void hkey_init(HKey *key) {
    init_hnode(&key->node);
    key->name = nullptr;
    key->len = 0;
}

// Ignore the warning about statement expressions in macros (the 'container_of' macro)
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/**
 * @brief Compares a ZNode with a given score and name.
 *
 * @param lhs The AVLNode representing the ZNode.
 * @param score The score to compare with.
 * @param name The name to compare with.
 * @param len The length of the name.
 * @return True if the ZNode is less than the given score and name, false otherwise.
 */
static bool zless(const AVLNode *lhs, const double score, const char *name, const size_t len) {
    const ZNode *zl = container_of(lhs, ZNode, tree);
    if (zl) {
        if (zl->score != score)
            return zl->score < score;

        const int rv = memcmp(zl->name, name, min(zl->len, len));
        if (rv != 0) return rv < 0;

        return zl->len < len;
    }
    return false;
}

/**
 * @brief Compares two ZNodes.
 *
 * @param lhs The AVLNode representing the left-hand side ZNode.
 * @param rhs The AVLNode representing the right-hand side ZNode.
 * @return True if the left-hand side ZNode is less than the right-hand side ZNode, false otherwise.
 */
static bool zless2(const AVLNode *lhs, const AVLNode *rhs) {
    const ZNode *zr = container_of(rhs, ZNode, tree);
    return zr ? zless(lhs, zr->score, zr->name, zr->len) : false;
}

/**
 * @brief Compares two HNodes by their associated ZNodes.
 *
 * @param node The first HNode to compare.
 * @param key The second HNode to compare.
 * @return True if the lengths and names of the ZNodes associated with the HNodes are equal, false otherwise.
 */
static bool hcmp(const HNode *node, const HNode *key) {
    const ZNode *znode = container_of(node, ZNode, hmap);
    const HKey *hkey = container_of(key, HKey, node);
    if (znode->len != hkey->len) return false;
    return memcmp(znode->name, hkey->name, znode->len) == 0;
}

/**
 * @brief Looks up a ZNode in a ZSet by name.
 *
 * @param zset The ZSet to look up the ZNode in.
 * @param name The name of the ZNode to look up.
 * @param len The length of the name.
 * @return A pointer to the ZNode if found, nullptr otherwise.
 */
ZNode *zset_lookup(ZSet *zset, const char *name, const size_t len) {
    if (zset->tree) {
        HKey key;
        hkey_init(&key);
        key.node.hcode = fnv1a_hash((uint8_t *) name, len);
        key.name = name;
        key.len = len;
        const HNode *found = hm_lookup(&zset->hmap, &key.node, (bool(*)(HNode *, HNode *)) &hcmp);
        if (found) return container_of(found, ZNode, hmap);
    }
    return nullptr;
}

/**
 * @brief Removes a ZNode from a ZSet by name.
 *
 * @param zset The ZSet to remove the ZNode from.
 * @param name The name of the ZNode to remove.
 * @param len The length of the name.
 * @return A pointer to the ZNode if found and removed, nullptr otherwise.
 */
ZNode *zset_pop(ZSet *zset, const char *name, const size_t len) {
    if (zset->tree) {
        HKey key;
        hkey_init(&key);
        key.node.hcode = fnv1a_hash((uint8_t *) name, len);
        key.name = name;
        key.len = len;

        const HNode *found = hm_pop(&zset->hmap, &key.node, (bool(*)(HNode *, HNode *)) &hcmp);
        if (found) {
            ZNode *node = container_of(found, ZNode, hmap);
            zset->tree = avl_del(&node->tree);
            return node;
        }
    }
    return nullptr;
}

/**
 * @brief Queries a ZSet for a ZNode that is greater or equal to the given score and name.
 *
 * @param zset The ZSet to query.
 * @param score The score to query for.
 * @param name The name to query for.
 * @param len The length of the name.
 * @return A pointer to the ZNode if found, nullptr otherwise.
 */
ZNode *zset_query(const ZSet *zset, const double score, const char *name, const size_t len) {
    const AVLNode *found = nullptr;
    const AVLNode *cur = zset->tree;
    while (cur) {
        if (zless(cur, score, name, len)) {
            cur = cur->right;
        } else {
            found = cur; // candidate
            cur = cur->left;
        }
    }
    return found ? container_of(found, ZNode, tree) : nullptr;
}

/**
 * @brief Offsets into the succeeding or preceding node.
 *
 * @param node The ZNode to offset from.
 * @param offset The offset to apply.
 * @return A pointer to the ZNode at the offset position if found, nullptr otherwise.
 */
ZNode *znode_offset(ZNode *node, const int64_t offset) {
    const AVLNode *tnode = node ? avl_offset(&node->tree, offset) : nullptr;
    return tnode ? container_of(tnode, ZNode, tree) : nullptr;
}

/**
 * @brief Disposes of an AVL tree.
 *
 * @param node The root of the AVL tree to dispose of.
 */
static void tree_dispose(const AVLNode *node) {
    if (node) {
        tree_dispose(node->left);
        tree_dispose(node->right);
        znode_del(container_of(node, ZNode, tree));
    }
}

// Restore the warning about statement expressions in macros
#if __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

/**
 * @brief Inserts a ZNode into the AVL tree of a ZSet.
 *
 * @param zset The ZSet to insert the ZNode into.
 * @param node The ZNode to insert.
 */
static void tree_add(ZSet *zset, ZNode *node) {
    AVLNode *cur = nullptr; // current node
    AVLNode **from = &zset->tree; // the incoming pointer to the next node
    while (*from) {
        // tree search
        cur = *from;
        from = zless2(&node->tree, cur) ? &cur->left : &cur->right;
    }
    *from = &node->tree; // attach the new node
    node->tree.parent = cur;
    zset->tree = avl_fix(&node->tree);
}

/**
 * @brief Updates the score of an existing node in a ZSet.
 *
 * @param zset The ZSet containing the node.
 * @param node The node to update.
 * @param score The new score.
 */
static void zset_update(ZSet *zset, ZNode *node, const double score) {
    if (node->score == score) return;

    zset->tree = avl_del(&node->tree);
    node->score = score;
    avl_init(&node->tree);
    tree_add(zset, node);
}

/**
 * @brief Adds a new (score, name) tuple to a ZSet, or updates the score of an existing tuple.
 *
 * @param zset The ZSet to add the tuple to.
 * @param name The name of the tuple.
 * @param len The length of the name.
 * @param score The score of the tuple.
 * @return True if a new tuple was added, false if an existing tuple was updated.
 */
bool zset_add(ZSet *zset, const char *name, const size_t len, const double score) {
    ZNode *node = zset_lookup(zset, name, len);
    if (node) {
        zset_update(zset, node, score);
        return false;
    }
    node = znode_new(name, len, score);
    hm_insert(&zset->hmap, &node->hmap);
    tree_add(zset, node);
    return true;
}

/**
 * @brief Deletes a ZNode.
 *
 * @param node The ZNode to delete.
 */
void znode_del(ZNode *node) { free(node); }

/**
 * @brief Disposes of a ZSet.
 *
 * @param zset The ZSet to dispose of.
 */
void zset_dispose(ZSet *zset) {
    tree_dispose(zset->tree);
    hm_destroy(&zset->hmap);
}
