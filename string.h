#pragma once

// A simple emulation of a C++ string
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} string;

string string_new();

void string_free(string *restrict s);

void string_push_back(string *restrict s, char c);

char string_at(const string *restrict s, size_t index);

void string_pop_back(string *restrict s);

inline bool string_empty(const string *restrict s);

void string_erase(string *restrict s, size_t index);

bool string_compare(const string *restrict s1, const string *restrict s2);

inline size_t string_length(const string *restrict s);

inline size_t string_capacity(const string *restrict s);

inline void string_clear(string *restrict s);

void string_insert(string *restrict s, size_t index, char c);

void string_set(const string *restrict s, size_t index, char c);

string string_substr(const string *restrict s, size_t start, size_t len);

string string_concat(const string *restrict s1, const string *restrict s2);

void string_append(string *restrict s1, const string *restrict s2);

void string_append_cstr(string *restrict s, const char *restrict cstr);

char *string_cstr(const string *restrict s);

bool string_compare_cstr(const string *restrict s, const char *restrict cstr);
