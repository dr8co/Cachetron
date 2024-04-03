#pragma once
#if __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct AVLNode {
    uint32_t height;
    uint32_t count;
    struct AVLNode *left;
    struct AVLNode *right;
    struct AVLNode *parent;
};

typedef struct AVLNode AVLNode;

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
