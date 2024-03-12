#include <stdlib.h>
#include <errno.h>
#include "string.h"

// Create a new string, with initial capacity of 16
string string_new() {
    const string s = {
        .data = malloc(16 * sizeof(char)),
        .size = 0,
        .capacity = 16
    };
    return s;
}

// Free the memory used by the string
void string_free(string *const restrict s) {
    if (s->data) {
        free(s->data);
        s->data = nullptr;
    }
    s->size = 0;
    s->capacity = 0;
}

// Append a character to the end of the string
bool string_push_back(string *const restrict s, const char c) {
    errno = 0;
    if (s->size == s->capacity) {
        s->capacity *= 2;
        char *temp = realloc(s->data, s->capacity * sizeof(char));
        if (temp == nullptr || errno) {
            return false;
        }
        s->data = temp;
    }
    s->data[s->size++] = c;
    return true;
}

// Get the character at the given index
char string_at(const string *const restrict s, const size_t index) {
    if (index < s->size) {
        return s->data[index];
    }
    return '\0';
}

// Remove the last character from the string
void string_pop_back(string *const restrict s) {
    if (s->size) {
        --s->size;
    }
}

// Check if the string is empty
inline bool string_empty(const string *const restrict s) {
    return s->size;
}

// Remove the character at the given index
void string_erase(string *const restrict s, const size_t index) {
    if (index < s->size) {
        for (size_t i = index; i < s->size - 1; ++i) {
            s->data[i] = s->data[i + 1];
        }
        --s->size; // Reduce the size
    }
}

// Compare two strings
bool string_compare(const string *const restrict s1, const string *const restrict s2) {
    if (s1->size != s2->size) {
        return false;
    }
    for (size_t i = 0; i < s1->size; ++i) {
        if (s1->data[i] != s2->data[i]) {
            return false;
        }
    }
    return true;
}

// Get the length of the string
inline size_t string_length(const string *const restrict s) {
    return s->size;
}

// Get the capacity of the string
inline size_t string_capacity(const string *const restrict s) {
    return s->capacity;
}

// Clear the string
inline void string_clear(string *const restrict s) {
    s->size = 0;
}

// Insert a new character at the given index
bool string_insert(string *const restrict s, const size_t index, const char c) {
    errno = 0;
    if (index < s->size) {
        if (s->size == s->capacity) {
            s->capacity *= 2;
            char *temp = realloc(s->data, s->capacity * sizeof(char));
            if (temp == nullptr || errno) {
                return false;
            }
            s->data = temp;
        }
        for (size_t i = s->size; i > index; --i) {
            s->data[i] = s->data[i - 1];
        }
        s->data[index] = c;
        ++s->size; // increase the size
    }
    return true;
}

// Modify the character at the given index
void string_set(const string *const restrict s, const size_t index, const char c) {
    if (index < s->size) {
        s->data[index] = c;
    }
}

// Get the substring of the string
string string_substr(const string *const restrict s, const size_t start, const size_t len) {
    string sub = string_new();
    for (size_t i = start; i < start + len; ++i) {
        string_push_back(&sub, string_at(s, i));
    }
    return sub;
}

// Concatenate two strings
string string_concat(const string *const restrict s1, const string *const restrict s2) {
    string s = string_new();
    for (size_t i = 0; i < s1->size; ++i) {
        string_push_back(&s, string_at(s1, i));
    }
    for (size_t i = 0; i < s2->size; ++i) {
        string_push_back(&s, string_at(s2, i));
    }
    return s;
}

// Append a string to the end of another string
void string_append(string *const restrict s1, const string *const restrict s2) {
    for (size_t i = 0; i < s2->size; ++i) {
        string_push_back(s1, string_at(s2, i));
    }
}

// Append a char array to the end of the string
void string_append_cstr(string *const restrict s, const char *const restrict cstr) {
    for (size_t i = 0; cstr[i] != '\0'; ++i) {
        string_push_back(s, cstr[i]);
    }
}

// Convert the string to a char array
char *string_cstr(const string *const restrict s) {
    char *cstr = malloc((s->size + 1) * sizeof(char));
    for (size_t i = 0; i < s->size; ++i) {
        cstr[i] = s->data[i];
    }
    cstr[s->size] = '\0';
    return cstr;
}

// Compare the string with a char array
bool string_compare_cstr(const string *const restrict s, const char *const restrict cstr) {
    size_t len = 0;

    while (cstr[++len] != '\0') {
    }

    if (s->size != len) {
        return false;
    }
    for (size_t i = 0; i < s->size; ++i) {
        if (s->data[i] != cstr[i]) {
            return false;
        }
    }
    return true;
}
