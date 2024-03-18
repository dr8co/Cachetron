#pragma once
#if __cplusplus
#define restrict
#endif
#include <stddef.h>

// A simple emulation of a C++ string
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} string_c;

string_c *string_new();

void string_free(string_c *restrict s);

bool string_push_back(string_c *restrict s, char c);

bool string_resize(string_c *restrict s, size_t size);

char string_at(const string_c *restrict s, size_t index);

void string_pop_back(string_c *restrict s);

bool string_empty(const string_c *restrict s);

bool string_erase(string_c *restrict s, size_t index);

bool string_compare(const string_c *restrict s1, const string_c *restrict s2);

size_t string_length(const string_c *restrict s);

size_t string_capacity(const string_c *restrict s);

void string_clear(string_c *restrict s);

bool string_insert(string_c *restrict s, size_t index, char c);

void string_set(const string_c *restrict s, size_t index, char c);

string_c *string_substr(const string_c *restrict s, size_t start, size_t len);

string_c *string_concat(const string_c *restrict s1, const string_c *restrict s2);

bool string_append(string_c *restrict s1, const string_c *restrict s2);

bool string_append_cstr(string_c *restrict s, const char *restrict cstr);

char *string_cstr(const string_c *restrict s);

bool string_compare_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_insert_cstr(string_c *restrict s, size_t index, const char *restrict cstr);

bool string_append_cstr_range(string_c *restrict s, const char *restrict cstr, size_t count);

bool string_copy_buffer(const string_c *restrict s, char *buf);

bool string_copy(const string_c *restrict src, string_c *restrict dest);

bool string_case_compare(const string_c *restrict s1, const string_c *restrict s2);

bool string_case_compare_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_swap(string_c *restrict s1, string_c *restrict s2);
