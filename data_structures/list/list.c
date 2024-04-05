#include "list.h"

/**
 * @brief Detaches a node from a doubly linked list.
 *
 * @param node A pointer to the node to be detached. This must be a valid node already part of a doubly linked list.
 */
void dlist_detach(const DList *node) {
    if (node) {
        DList *prev = node->prev;
        DList *next = node->next;
        prev->next = next;
        next->prev = prev;
    }
}

/**
 * @brief Inserts a new node before a given node in a doubly linked list.
 *
 * @param node A pointer to the node before which the new node will be inserted.
 * This must be a valid node already part of a doubly linked list.
 * @param next A pointer to the new node to be inserted. This node does not need to be part of a list.
 */
void dlist_insert_before(DList *node, DList *next) {
    if (node && next) {
        if (node == next) return; // no-op (inserting before itself)

        DList *prev = node->prev;
        prev->next = next;
        next->prev = prev;
        next->next = node;
        node->prev = next;
    }
}

/**
 * @brief Inserts a new node after a given node in a doubly linked list.
 *
 * @param node A pointer to the new node to be inserted. This node does not need to be part of a list.
 * @param prev A pointer to the node after which the new node will be inserted.
 * This must be a valid node already part of a doubly linked list.
 */
void dlist_insert_after(DList *node, DList *prev) {
    if (node && prev) {
        if (node == prev) return; // no-op (inserting after itself)

        DList *next = prev->next;
        prev->next = node;
        node->prev = prev;
        node->next = next;
        next->prev = node;
    }
}

/**
 * @brief Inserts a new node at the front of a doubly linked list.
 *
 * @param node A pointer to the new node to be inserted at the front of the list.
 * @param head A pointer to the head node of the list.
 */
void dlist_push_front(DList *node, DList *head) {
    dlist_insert_after(node, head);
}

/**
 * @brief Inserts a new node at the back of a doubly linked list.
 *
 * @param node A pointer to the new node to be inserted at the back of the list.
 * @param head A pointer to the head node of the list.
 */
void dlist_push_back(DList *node, DList *head) {
    dlist_insert_before(node, head);
}

/**
 * @brief Removes and returns the first node from a doubly linked list.
 *
 * @param head A pointer to the head node of the list. This must be a valid node already part of a doubly linked list.
 * @return A pointer to the removed node, or nullptr if the list was empty.
 */
DList *dlist_pop_front(const DList *head) {
    if (head) {
        DList *node = head->next;
        if (node != head) {
            dlist_detach(node);
            return node;
        }
    }
    return nullptr;
}

/**
 * @brief Removes and returns the last node from a doubly linked list.
 *
 * @param head A pointer to the head node of the list. This must be a valid node already part of a doubly linked list.
 * @return A pointer to the removed node, or nullptr if the list was empty.
 */
DList *dlist_pop_back(const DList *head) {
    if (head) {
        DList *node = head->prev;
        if (node != head) {
            dlist_detach(node);
            return node;
        }
    }
    return nullptr;
}
