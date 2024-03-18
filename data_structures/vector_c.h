#pragma once
#if __cplusplus
#define restrict
#endif


// A mimic of C++ vector (of pointers) in C
typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
} vector_c;

vector_c *vector_new(size_t elem_size);

void vector_free(vector_c *restrict v);

bool vector_resize(vector_c *restrict v, size_t size);

bool vector_append(vector_c *restrict v, const void *restrict elems, size_t count);

bool vector_resize_expand(vector_c *restrict v, size_t new_size);

bool vector_push_back(vector_c *restrict v, const void *restrict elem);

void *vector_at(const vector_c *restrict v, size_t index);

void vector_pop_back(vector_c *restrict v);

bool vector_empty(const vector_c *restrict v);

bool vector_erase(vector_c *restrict v, size_t index);

bool vector_insert(vector_c *restrict v, size_t index, const void *restrict elem);

bool vector_set(vector_c *restrict v, size_t index, const void *restrict elem);

bool vector_set_range(vector_c *restrict v, const void *restrict elem, size_t index, size_t count);

void *vector_data(const vector_c *restrict v);

size_t vector_size(const vector_c *restrict v);

size_t vector_capacity(const vector_c *restrict v);

void vector_clear(vector_c *restrict v);

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
