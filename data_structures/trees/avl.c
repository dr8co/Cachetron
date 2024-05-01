#include <stddef.h>
#include "avl.h"

/**
 * @brief Calculate the height of an AVL tree node.
 *
 * @param node A pointer to the AVLNode for which the height is to be calculated.
 * @return The height of the node. If the node is NULL, returns 0.
 */
uint32_t avl_height(const AVLNode *node) {
    return node ? node->height : 0;
}

/**
 * @brief Calculate the count of an AVL tree node.
 *
 * @param node A pointer to the AVLNode for which the count is to be calculated.
 * @return The count of the node. If the node is NULL, returns 0.
 */
uint32_t avl_count(const AVLNode *node) {
    return node ? node->count : 0;
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
 * @brief Update the height and count of an AVL tree node.
 *
 * @param node A pointer to the AVLNode which needs to be updated.
 */
void avl_update(AVLNode *node) {
    node->height = 1 + max(avl_height(node->left), avl_height(node->right));
    node->count = 1 + avl_count(node->left) + avl_count(node->right);
}

/**
 * @brief Perform a left rotation on an AVL tree node.
 *
 * @param node A pointer to the AVLNode on which the rotation is to be performed.
 * @return A pointer to the new root after the rotation.
 */
AVLNode *rot_left(AVLNode *node) {
    AVLNode *new_node = node->right;
    if (new_node->left) {
        new_node->left->parent = node;
    }
    node->right = new_node->left;
    new_node->left = node;
    new_node->parent = node->parent;
    node->parent = new_node;

    avl_update(node);
    avl_update(new_node);
    return new_node;
}

/**
 * @brief Perform a right rotation on an AVL tree node.
 *
 * @param node A pointer to the AVLNode on which the rotation is to be performed.
 * @return A pointer to the new root after the rotation.
 */
AVLNode *rot_right(AVLNode *node) {
    AVLNode *new_node = node->left;
    if (new_node->right) {
        new_node->right->parent = node;
    }
    node->left = new_node->right;
    new_node->right = node;
    new_node->parent = node->parent;
    node->parent = new_node;

    avl_update(node);
    avl_update(new_node);
    return new_node;
}

/**
 * @brief Fix an AVL tree node when the left subtree is too deep.
 *
 * @param root A pointer to the AVLNode that needs to be fixed.
 * @return A pointer to the new root after the rotations.
 */
AVLNode *avl_fix_left(AVLNode *root) {
    if (avl_height(root->left->left) < avl_height(root->left->right)) {
        root->left = rot_left(root->left);
    }
    return rot_right(root);
}

/**
 * @brief Fix an AVL tree node when the right subtree is too deep.
 *
 * @param root A pointer to the AVLNode that needs to be fixed.
 * @return A pointer to the new root after the rotations.
 */
AVLNode *avl_fix_right(AVLNode *root) {
    if (avl_height(root->right->right) < avl_height(root->right->left)) {
        root->right = rot_right(root->right);
    }
    return rot_left(root);
}

/**
 * @brief Fix imbalanced AVL tree nodes and maintain invariants until the root is reached.
 *
 * @param node A pointer to the AVLNode from where the fixing process starts.
 * @return A pointer to the root of the tree after all the necessary rotations have been performed.
 */
AVLNode *avl_fix(AVLNode *node) {
    while (true) {
        avl_update(node);
        const uint32_t l = avl_height(node->left);
        const uint32_t r = avl_height(node->right);
        AVLNode **from = nullptr;
        if (node->parent) {
            from = node->parent->left == node ? &node->parent->left : &node->parent->right;
        }
        if (l == r + 2) {
            node = avl_fix_left(node);
        } else if (l + 2 == r) {
            node = avl_fix_right(node);
        }
        if (from == nullptr) return node;
        *from = node;
        node = node->parent;
    }
}

/**
 * @brief Detach a node and return the new root of the tree.
 *
 * @param node A pointer to the AVLNode to be deleted.
 * @return A pointer to the new root of the tree after the deletion and necessary rotations.
 */
AVLNode *avl_del(const AVLNode *node) {
    if (node->right == nullptr) {
        // No right subtree, replace the node with the left subtree.
        // Link the left subtree to the parent
        AVLNode *parent = node->parent;
        if (node->left)
            node->left->parent = parent;

        if (parent) {
            // Attach the left subtree to the parent
            if (parent->left == node) parent->left = node->left;
            else parent->right = node->left;
            return avl_fix(parent);
        }
        // Removing the root?
        return node->left;
    }
    // Swap the node with its next sibling
    AVLNode *victim = node->right;
    while (victim->left) {
        victim = victim->left;
    }
    AVLNode *root = avl_del(victim);

    *victim = *node;
    if (victim->left)
        victim->left->parent = victim;

    if (victim->right)
        victim->right->parent = victim;

    AVLNode *parent = node->parent;
    if (parent) {
        if (parent->left == node) parent->left = victim;
        else parent->right = victim;
        return root;
    }
    // Removing the root?
    return victim;
}

/**
 * @brief Offset into the succeeding or preceding node.
 *
 * @param node A pointer to the AVLNode from where the offset is calculated.
 * @param offset The offset to the target node. Can be positive or negative.
 * @return A pointer to the target node if it exists, or nullptr if the offset is out of range.
 */
AVLNode *avl_offset(AVLNode *node, const int64_t offset) {
    // Position relative to the starting node
    int64_t pos = 0;
    while (offset != pos) {
        if (pos < offset && pos + avl_count(node->right) >= offset) {
            // The target is inside the right subtree
            node = node->right;
            pos += avl_count(node->left) + 1;
        } else if (pos > offset && pos - avl_count(node->left) <= offset) {
            // The target is inside the left subtree
            node = node->left;
            pos -= avl_count(node->right) + 1;
        } else {
            // Go to the parent
            AVLNode *parent = node->parent;
            if (parent == nullptr) {
                return nullptr;
            }
            if (parent->right == node) {
                pos -= avl_count(node->left) + 1;
            } else {
                pos += avl_count(node->right) + 1;
            }
            node = parent;
        }
    }
    return node;
}
