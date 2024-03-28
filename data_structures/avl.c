#include "avl.h"

void avl_init(AVLNode *node) {
    node->height = 1;
    node->count = 1;
    node->left = node->right = node->parent = nullptr;
}

uint32_t avl_height(const AVLNode *node) {
    return node ? node->height : 0;
}

uint32_t avl_count(const AVLNode *node) {
    return node ? node->count : 0;
}

static uint32_t max(const uint32_t lhs, const uint32_t rhs) {
    return lhs < rhs ? rhs : lhs;
}

// maintaining the height and cnt field
void avl_update(AVLNode *node) {
    node->height = 1 + max(avl_height(node->left), avl_height(node->right));
    node->count = 1 + avl_count(node->left) + avl_count(node->right);
}

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

// the left subtree is too deep
AVLNode *avl_fix_left(AVLNode *root) {
    if (avl_height(root->left->left) < avl_height(root->left->right)) {
        root->left = rot_left(root->left);
    }
    return rot_right(root);
}

// the right subtree is too deep
AVLNode *avl_fix_right(AVLNode *root) {
    if (avl_height(root->right->right) < avl_height(root->right->left)) {
        root->right = rot_right(root->right);
    }
    return rot_left(root);
}

// fix imbalanced nodes and maintain invariants until the root is reached
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

// detach a node and returns the new root of the tree
AVLNode *avl_del(AVLNode *node) {
    if (node->right == nullptr) {
        // no right subtree, replace the node with the left subtree
        // link the left subtree to the parent
        AVLNode *parent = node->parent;
        if (node->left) {
            node->left->parent = parent;
        }
        if (parent) {
            // attach the left subtree to the parent
            node->left = parent->left == node ? parent->left : parent->right;
            return avl_fix(parent);
        } else {
            // removing root?
            return node->left;
        }
    } else {
        // swap the node with its next sibling
        AVLNode *victim = node->right;
        while (victim->left) {
            victim = victim->left;
        }
        AVLNode *root = avl_del(victim);

        *victim = *node;
        if (victim->left) {
            victim->left->parent = victim;
        }
        if (victim->right) {
            victim->right->parent = victim;
        }
        AVLNode *parent = node->parent;
        if (parent) {
            victim = parent->left == node ? parent->left : parent->right;
            return root;
        } else {
            // removing root?
            return victim;
        }
    }
}

// offset into the succeeding or preceding node.
AVLNode *avl_offset(AVLNode *node, const int64_t offset) {
    int64_t pos = 0; // relative to the starting node
    while (offset != pos) {
        if (pos < offset && pos + avl_count(node->right) >= offset) {
            // the target is inside the right subtree
            node = node->right;
            pos += avl_count(node->left) + 1;
        } else if (pos > offset && pos - avl_count(node->left) <= offset) {
            // the target is inside the left subtree
            node = node->left;
            pos -= avl_count(node->right) + 1;
        } else {
            // go to the parent
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
