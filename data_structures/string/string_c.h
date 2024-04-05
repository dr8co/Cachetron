#pragma once
#if __cplusplus
#define restrict
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief A simple emulation of a C++ string in C.
 *
 * The data is stored as a pointer to a dynamically allocated array of characters.\n
 * The capacity represents the total number of characters that the string can hold without needing to be resized.\n
 * When the size reaches the capacity, the string is resized to a larger capacity to accommodate more characters.
 */
struct string_c {
    char *data;      ///< A pointer to the character data.
    size_t size;     ///< The number of characters in the string, not including the null character.
    size_t capacity; ///< The total number of characters that the string can hold.
};

typedef struct string_c string_c;

string_c *string_new();

void string_free(string_c *restrict s);

bool string_reserve(string_c *restrict s, size_t size);

bool string_push_back(string_c *restrict s, char c);

char string_at(const string_c *restrict s, size_t index);

void string_pop_back(string_c *restrict s);

bool string_empty(const string_c *restrict s);

bool string_erase(string_c *restrict s, size_t index);

char string_back(const string_c *restrict s);

char string_front(const string_c *restrict s);

bool string_compare(const string_c *restrict s1, const string_c *restrict s2);

size_t string_length(const string_c *restrict s);

size_t string_capacity(const string_c *restrict s);

void string_clear(string_c *restrict s);

bool string_insert(string_c *restrict s, size_t index, char c);

void string_set(const string_c *restrict s, size_t index, char c);

string_c *string_substr(const string_c *restrict s, size_t start, size_t len);

string_c *string_concat(const string_c *restrict s1, const string_c *restrict s2);

bool string_append_range(string_c *restrict s1, const string_c *restrict s2, size_t count);

bool string_append(string_c *restrict s1, const string_c *restrict s2);

bool string_append_cstr(string_c *restrict s, const char *restrict cstr);

char *string_cstr(const string_c *restrict s);

bool string_compare_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_insert_cstr(string_c *restrict s, const char *restrict cstr, size_t index);

bool string_append_cstr_range(string_c *restrict s, const char *restrict cstr, size_t count);

bool string_copy_buffer(const string_c *restrict s, char *buf);

bool string_copy(const string_c *restrict src, string_c *restrict dest);

bool string_case_compare(const string_c *restrict s1, const string_c *restrict s2);

bool string_case_compare_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_swap(string_c *restrict s1, string_c *restrict s2);

bool string_insert_cstr_range(string_c *restrict s, const char *restrict cstr, size_t index, size_t count);

bool string_insert_range(string_c *restrict s, const string_c *restrict sub, size_t index, size_t count);

bool string_insert_string(string_c *restrict s, const string_c *restrict sub, size_t index);

size_t string_find_last_of(const string_c *restrict s, char c);

size_t string_find_last_not_of(const string_c *restrict s, char c);

size_t string_find_first_from(const string_c *restrict s, char c, size_t start);

size_t string_find_first_of(const string_c *restrict s, char c);

size_t string_find_first_not_of(const string_c *restrict s, char c);

size_t string_find_substr_from(const string_c *restrict s, const string_c *restrict sub, size_t start);

size_t string_find_substr(const string_c *restrict s, const string_c *restrict sub);

size_t string_rfind_substr(const string_c *restrict s, const string_c *restrict sub);

size_t string_find_substr_cstr_from(const string_c *restrict s, const char *restrict cstr, size_t start);

size_t string_rfind_substr_cstr(const string_c *restrict s, const char *restrict cstr);

size_t string_find_substr_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_contains_char(const string_c *restrict s, char c);

bool string_contains(const string_c *restrict s, const string_c *restrict sub);

bool string_contains_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_starts_with(const string_c *restrict s, const string_c *restrict sub);

bool string_starts_with_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_ends_with(const string_c *restrict s, const string_c *restrict sub);

bool string_ends_with_cstr(const string_c *restrict s, const char *restrict cstr);

bool string_shrink(string_c *restrict s, size_t size);

bool string_shrink_to_fit(string_c *restrict s);

#if __cplusplus
}
#endif
