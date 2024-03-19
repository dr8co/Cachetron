#include <assert.h>
#include <stdlib.h>
#include "hashtable.h"

// C23 constexpr support
#if __GNUC__ >= 13 || __clang_major__ >= 19
constexpr size_t k_resizing_work = 128;
constexpr size_t k_max_load_factor = 8;
#else
enum : size_t {
    k_resizing_work = 128, k_max_load_factor = 8
};
#endif


/**
 * @brief Computes the smallest power of 2 greater than or equal to the input number.
 *
 * @param x The input number.
 * @return The smallest power of 2 greater than or equal to the input number.
 */
static size_t clp2(size_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return ++x;
}

/**
 * @brief Initializes a hashtable.
 *
 * This function initializes a hashtable by setting its \p size, \p mask, and \p table.
 * The \p size of the hashtable is set to \p 0, indicating that it is empty.
 * The \p mask is set to \p n \p - \p 1, where \p n is the \p size of the table.
 *
 * @param htab A pointer to the hashtable to be initialized.
 * @param n The initial size of the hashtable.
 */
static void h_init(HTab *htab, size_t n) {
    // The minimum size is 2
    if (n == 0) n = 2;

    // Round up n to the next power of 2 if it's not already a power of 2
    if ((n & (n - 1)) != 0) n = clp2(n);

    htab->tab = (HNode **) calloc(sizeof(HNode *), n);
    htab->mask = n - 1;
    htab->size = 0;
}

// Insert a node into the hashtable
/**
 * @brief Inserts a node into the hashtable.
 *
 * @param htab A pointer to the hashtable where the node will be inserted.
 * @param node A pointer to the node to be inserted into the hashtable.
 */
static void h_insert(HTab *htab, HNode *node) {
    // Calculate the position in the hashtable for the node
    const size_t pos = node->hcode & htab->mask;
    // Get the node currently at the calculated position
    HNode *next = htab->tab[pos];
    // Set the next of the node to be inserted to the node currently at the calculated position
    node->next = next;
    // Insert the node at the calculated position
    htab->tab[pos] = node;
    htab->size++;
}

/**
 * @brief Looks up a node in the hashtable.
 *
 * This function looks up a node in the hashtable by calculating a position using the bitwise AND operation
 * between the hashcode of the key and the mask of the hashtable.
 *
 * @param htab A pointer to the hashtable where the node will be looked up.
 * @param key A pointer to the node to be looked up in the hashtable.
 * @param eq A function pointer to a binary predicate that compares two nodes for equality.
 * @return A pointer to the pointer to the found node in the hashtable, or nullptr if no such node is found.
 */
static HNode **h_lookup(const HTab *htab, HNode *key, bool (*eq)(HNode *, HNode *)) {
    if (htab->tab) {
        const size_t pos = key->hcode & htab->mask;
        HNode **from = &htab->tab[pos]; // incoming pointer to the result
        for (HNode *cur; (cur = *from) != nullptr; from = &cur->next) {
            if (cur->hcode == key->hcode && eq(cur, key)) {
                return from;
            }
        }
    }
    return nullptr;
}

/**
 * @brief Detaches a node from the hashtable.
 *
 * @param htab A pointer to the hashtable from which the node will be detached.
 * @param from A pointer to the pointer to the node to be detached from the hashtable.
 * @return The detached node.
 */
static HNode *h_detach(HTab *htab, HNode **from) {
    HNode *node = *from;
    *from = node->next;
    --htab->size;
    return node;
}

/**
 * @brief Helps the resizing process of the hashtable.
 *
 * This function helps the resizing process of the hashtable by moving nodes from the second hashtable \p (ht2)
 * to the first hashtable \p (ht1).\n
 * It scans for nodes from \p ht2 and moves them to \p ht1 until a certain amount of work
 * (defined by \p k_resizing_work constant) has been done or \p ht2 is empty.\n
 *
 * @param hmap A pointer to the hashmap that contains the two hash tables.
 */
static void hm_help_resizing(HMap *hmap) {
    size_t nwork = 0;
    while (nwork < k_resizing_work && hmap->ht2.size > 0) {
        // Scan for nodes from ht2 and move them to ht1
        HNode **from = &hmap->ht2.tab[hmap->resizing_pos];
        if (*from == nullptr) {
            hmap->resizing_pos++;
            continue;
        }
        h_insert(&hmap->ht1, h_detach(&hmap->ht2, from));
        nwork++;
    }

    if (hmap->ht2.size == 0 && hmap->ht2.tab) {
        // Done
        free(hmap->ht2.tab);
        hmap->ht2 = (HTab) {};
    }
}

/**
 * @brief Starts the resizing process of the hashtable.
 *
 * This function starts the resizing process of the hashtable by creating
 * a bigger hashtable and swapping it with the existing one.
 *
 * @param hmap A pointer to the hashmap that contains the two hash tables.
 */
static void hm_start_resizing(HMap *hmap) {
    assert(hmap->ht2.tab == nullptr);
    // Create a bigger hashtable and swap them
    hmap->ht2 = hmap->ht1;
    h_init(&hmap->ht1, (hmap->ht1.mask + 1) * 2);
    hmap->resizing_pos = 0;
}

/**
 * @brief Looks up a node in the hashmap.
 *
 * @param hmap A pointer to the hashmap where the node will be looked up.
 * @param key A pointer to the node to be looked up in the hashmap.
 * @param eq A function pointer to a binary predicate that compares two nodes for equality.
 * @return A pointer to the found node in the hashmap, or nullptr if no such node is found.
 */
HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *)) {
    hm_help_resizing(hmap);
    // Look up in the first hashtable
    HNode **from = h_lookup(&hmap->ht1, key, eq);
    // If not found, look up in the second hashtable
    from = from ? from : h_lookup(&hmap->ht2, key, eq);
    return from ? *from : nullptr;
}

/**
 * @brief Inserts a node into the hashmap.
 *
 * @param hmap A pointer to the hashmap where the node will be inserted.
 * @param node A pointer to the node to be inserted into the hashmap.
 */
void hm_insert(HMap *hmap, HNode *node) {
    // Initialize the first hashtable if it's not initialized already
    if (hmap->ht1.tab == nullptr) {
        h_init(&hmap->ht1, 4);
    }
    // Insert the node into the first hashtable
    h_insert(&hmap->ht1, node);

    if (hmap->ht2.tab == nullptr) {
        // Check whether we need to resize
        const size_t load_factor = hmap->ht1.size / (hmap->ht1.mask + 1);
        if (load_factor >= k_max_load_factor) {
            hm_start_resizing(hmap);
        }
    }
    hm_help_resizing(hmap);
}

/**
 * @brief Removes a node from the hashmap.
 *
 * @param hmap A pointer to the hashmap from which the node will be removed.
 * @param key A pointer to the node to be removed from the hashmap.
 * @param eq A function pointer to a binary predicate that compares two nodes for equality.
 * @return The removed node if found, or nullptr if no such node is found.
 */
HNode *hm_pop(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *)) {
    hm_help_resizing(hmap);
    HNode **from = h_lookup(&hmap->ht1, key, eq);
    if (from)
        return h_detach(&hmap->ht1, from);

    HNode **from2 = h_lookup(&hmap->ht2, key, eq);
    if (from2)
        return h_detach(&hmap->ht2, from);

    return nullptr;
}

/**
 * @brief Gets the size of the hashmap.
 *
 * @param hmap A pointer to the hashmap whose size will be returned.
 * @return The total size of the hashmap.
 */
size_t hm_size(const HMap *hmap) {
    return hmap->ht1.size + hmap->ht2.size;
}

/**
 * @brief Destroys the hashmap.
 *
 * This function destroys the hashmap by freeing the memory allocated for the first and second hash tables.
 * It then resets the hashmap to an empty state.
 *
 * @param hmap A pointer to the hashmap to be destroyed.
 */
void hm_destroy(HMap *hmap) {
    free(hmap->ht1.tab);
    free(hmap->ht2.tab);
    *hmap = (HMap) {};
}
