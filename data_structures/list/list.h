#pragma once

#if __cplusplus
extern "C" {
#endif

/**
 * @brief A structure representing a node in a doubly linked list.
 *
 * This structure represents a node in a doubly linked list.
 * Each node has a pointer to the previous node and the next node in the list.
 * This allows for efficient insertion and deletion of nodes at both ends of the list, as well as in the middle.
 */
struct DList {
    struct DList *prev; ///< Pointer to the previous node in the list, or nullptr if this is the first node.
    struct DList *next; ///< Pointer to the next node in the list, or nullptr if this is the last node.
};

typedef struct DList DList;

/**
 * @brief Initializes a node to be a standalone node in a doubly linked list.
 *
 * @param node A pointer to the node to be initialized.
 * This does not need to be a valid node already part of a doubly linked list.
 */
static inline void dlist_init(DList *node) {
    if (node) node->prev = node->next = node;
}

/**
 * @brief Checks if a node is a standalone node in a doubly linked list.
 *
 * @param node A pointer to the node to be checked.
 * This must be a valid node, but it does not need to be part of a doubly linked list.
 * @return true if the node is a standalone node, false otherwise.
 */
static inline bool dlist_empty(const DList *node) {
    return node ? node->next == node : true;
}

void dlist_detach(const DList *node);

void dlist_insert_before(DList *node, DList *next);

void dlist_insert_after(DList *node, DList *prev);

void dlist_push_front(DList *node, DList *head);

void dlist_push_back(DList *node, DList *head);

DList *dlist_pop_front(const DList *head);

DList *dlist_pop_back(const DList *head);

#if __cplusplus
}
#endif
