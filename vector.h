#pragma once

// A mimic of C++ vector (of pointers) in C
typedef struct {
    void **data;
    size_t size;
    size_t capacity;
} vector;

vector vector_new();

void vector_free(vector *restrict v);

bool vector_resize(vector *restrict v, size_t size);

bool vector_push_back(vector *restrict v, void *restrict elem);

void *vector_at(const vector *restrict v, size_t index);

void vector_pop_back(vector *restrict v);

bool vector_empty(const vector *restrict v);

void vector_erase(vector *restrict v, size_t index);

bool vector_insert(vector *restrict v, size_t index, void *restrict elem);

void vector_set(const vector *restrict v, size_t index, void *restrict elem);

size_t vector_size(const vector *restrict v);

size_t vector_capacity(const vector *restrict v);

void vector_clear(vector *restrict v);
