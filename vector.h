#pragma once
#include <stddef.h>

// A mimic of C++ vector (of pointers) in C
typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} vector;

vector *vector_new(size_t elem_size);

void vector_free(vector *restrict v);

bool vector_resize(vector *restrict v, size_t size);

bool vector_append(vector *restrict v, const void *restrict elems, size_t count);

bool vector_resize_expand(vector *restrict v, size_t new_size);

bool vector_push_back(vector *restrict v, const void *restrict elem);

void *vector_at(const vector *restrict v, size_t index);

void vector_pop_back(vector *restrict v);

bool vector_empty(const vector *restrict v);

bool vector_erase(vector *restrict v, size_t index);

bool vector_insert(vector *restrict v, size_t index, const void *restrict elem);

bool vector_set(vector *restrict v, size_t index, const void *restrict elem);

bool vector_set_range(vector *restrict v, const void *restrict elem, size_t index, size_t count);

void *vector_data(const vector *restrict v);

size_t vector_size(const vector *restrict v);

size_t vector_capacity(const vector *restrict v);

void vector_clear(vector *restrict v);

////////////// ptr_vector //////////////

// A specializations of vector for pointers
typedef struct {
    void **data;
    size_t size;
    size_t capacity;
} ptr_vector;

ptr_vector *ptr_vector_new();

void ptr_vector_free(ptr_vector *restrict v);

bool ptr_vector_resize(ptr_vector *restrict v, size_t size);

bool ptr_vector_push_back(ptr_vector *restrict v, void *restrict elem);

void *ptr_vector_at(const ptr_vector *restrict v, size_t index);

void ptr_vector_pop_back(ptr_vector *restrict v);

bool ptr_vector_empty(const ptr_vector *restrict v);

bool ptr_vector_erase(ptr_vector *restrict v, size_t index);

bool ptr_vector_set(const ptr_vector *restrict v, size_t index, void *restrict elem);

bool ptr_vector_resize_expand(ptr_vector *restrict v, size_t new_size);

void **ptr_vector_data(const ptr_vector *restrict v);

size_t ptr_vector_size(const ptr_vector *restrict v);

size_t ptr_vector_capacity(const ptr_vector *restrict v);

void ptr_vector_clear(ptr_vector *restrict v);
