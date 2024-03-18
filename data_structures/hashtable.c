#include <assert.h>
#include <stdlib.h>
#include "hashtable.h"

// C23 constexpr support
#if __GNUC__ >= 13 || __clang_major__ >= 19
constexpr size_t k_resizing_work = 128;
constexpr size_t k_max_load_factor = 8;
#else
enum : size_t { k_resizing_work = 128, k_max_load_factor = 8 };
#endif


// Round up x to the next power of 2
static size_t clp2(size_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return ++x;
}

// Initialize the hashtable
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
static void h_insert(HTab *htab, HNode *node) {
    const size_t pos = node->hcode & htab->mask;
    HNode *next = htab->tab[pos];
    node->next = next;
    htab->tab[pos] = node;
    htab->size++;
}

// Hash table look up subroutine.
// The return value is the address of the parent pointer that owns the target node,
// which can be used to delete the target node.
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

// Remove a node from the hashtable
static HNode *h_detach(HTab *htab, HNode **from) {
    HNode *node = *from;
    *from = node->next;
    --htab->size;
    return node;
}

// Help the resizing process
static void hm_help_resizing(HMap *hmap) {
    size_t nwork = 0;
    while (nwork < k_resizing_work && hmap->ht2.size > 0) {
        // scan for nodes from ht2 and move them to ht1
        HNode **from = &hmap->ht2.tab[hmap->resizing_pos];
        if (*from == nullptr) {
            hmap->resizing_pos++;
            continue;
        }
        h_insert(&hmap->ht1, h_detach(&hmap->ht2, from));
        nwork++;
    }

    if (hmap->ht2.size == 0 && hmap->ht2.tab) {
        // done
        free(hmap->ht2.tab);
        hmap->ht2 = (HTab) {};
    }
}

// Start the resizing process
static void hm_start_resizing(HMap *hmap) {
    assert(hmap->ht2.tab == nullptr);
    // create a bigger hashtable and swap them
    hmap->ht2 = hmap->ht1;
    h_init(&hmap->ht1, (hmap->ht1.mask + 1) * 2);
    hmap->resizing_pos = 0;
}

// Look up a node in the hashtable
HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *)) {
    hm_help_resizing(hmap);
    HNode **from = h_lookup(&hmap->ht1, key, eq);
    from = from ? from : h_lookup(&hmap->ht2, key, eq);
    return from ? *from : nullptr;
}

// Insert a node into the hashtable
void hm_insert(HMap *hmap, HNode *node) {
    if (hmap->ht1.tab == nullptr) {
        h_init(&hmap->ht1, 4);
    }
    h_insert(&hmap->ht1, node);

    if (hmap->ht2.tab == nullptr) {
        // check whether we need to resize
        const size_t load_factor = hmap->ht1.size / (hmap->ht1.mask + 1);
        if (load_factor >= k_max_load_factor) {
            hm_start_resizing(hmap);
        }
    }
    hm_help_resizing(hmap);
}

// Remove a node from the hashtable
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

// Get the size of the hashtable
size_t hm_size(const HMap *hmap) {
    return hmap->ht1.size + hmap->ht2.size;
}

// Destroy the hashtable
void hm_destroy(HMap *hmap) {
    free(hmap->ht1.tab);
    free(hmap->ht2.tab);
    *hmap = (HMap) {};
}
