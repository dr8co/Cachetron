#pragma once

#include <stdint.h>

// Hash table node
typedef struct HNode {
    struct HNode *next;
    uint64_t hcode; // hash code
} HNode;

// a simple fixed-sized hash table
typedef struct HTab {
    HNode **tab; // array of pointers to nodes
    size_t mask; // mask for fast modulo
    size_t size; // number of nodes
} HTab;

// The real hash table, which uses 2 hash tables for progressive resizing
typedef struct HMap {
    HTab ht1;            // newer
    HTab ht2;            // older
    size_t resizing_pos; // resizing position
} HMap;

HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));

void hm_insert(HMap *hmap, HNode *node);

HNode *hm_pop(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));

size_t hm_size(const HMap *hmap);

void hm_destroy(HMap *hmap);
