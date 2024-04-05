#pragma once
#if __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief A node in an AVL tree.
 *
 * This structure represents a node in an AVL (Adelson-Velsky and Landis) tree.
 * An AVL tree is a self-balancing binary search tree.
 *
 */
struct AVLNode {
    uint32_t height;        ///< The height of the AVL node.
    uint32_t count;         ///< The number of nodes in the subtree rooted at this node, including the node itself.
    struct AVLNode *left;   ///< A pointer to the left child of the AVL node.
    struct AVLNode *right;  ///< A pointer to the right child of AVL the node.
    struct AVLNode *parent; ///< A pointer to the parent of the AVL node.
};

typedef struct AVLNode AVLNode;

/**
 * @brief Initialize an AVL tree node.
 *
 * @param node A pointer to the AVLNode to be initialized.
 */
static inline void avl_init(AVLNode *node) {
    node->height = 1;
    node->count = 1;
    node->left = node->right = node->parent = nullptr;
}

uint32_t avl_height(const AVLNode *node);

uint32_t avl_count(const AVLNode *node);

void avl_update(AVLNode *node);

AVLNode *rot_left(AVLNode *node);

AVLNode *rot_right(AVLNode *node);

AVLNode *avl_fix_left(AVLNode *root);

AVLNode *avl_fix_right(AVLNode *root);

AVLNode *avl_fix(AVLNode *node);

AVLNode *avl_del(const AVLNode *node);

AVLNode *avl_offset(AVLNode *node, int64_t offset);

#if __cplusplus
    }
#endif
