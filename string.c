#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "string.h"

// Create a new string, with initial capacity of 16
string *string_new() {
    string *s = malloc(sizeof(string));
    if (s) {
        if ((s->data = malloc(16 * sizeof(char)))) {
            s->size = 0;
            s->capacity = 16;
            return s;
        }
        free(s);
    }
    return nullptr;
}

// Free the memory used by the string
void string_free(string *const restrict s) {
    if (s) {
        if (s->data) {
            free(s->data);
            s->data = nullptr;
        }
        s->size = 0;
        s->capacity = 0;

        free(s);
    }
}

// Round up x to the next power of 2
static size_t clp2(unsigned x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return ++x;
}

bool string_resize(string *const restrict s, size_t size) {
    if (s) {
        if (size <= 16) return true;

        errno = 0;
        size = clp2(size);

        if (size >= s->size) {
            void *temp = realloc(s->data, size * sizeof(char));
            if (temp == nullptr || errno) {
                return false;
            }
            s->data = temp;
            s->capacity = size;
            return true;
        }
    }

    return false;
}

bool string_insert_cstr(string *const restrict s, const size_t index, const char *const restrict cstr) {
    if (s) {
        if (cstr) {
            const size_t len = strlen(cstr);
            if (index < s->size) {
                if (s->size + len > s->capacity) {
                    s->capacity = clp2(s->size + len);
                    if (!string_resize(s, s->capacity)) return false;
                }
                memmove(s->data + (index + len) * sizeof(char), s->data + index * sizeof(char),
                        (s->size - index) * sizeof(char));

                memcpy(s->data + index * sizeof(char), cstr, len * sizeof(char));
                s->size += len;

                return true;
            }
            if (index == 0 && s->size == 0) return string_append_cstr(s, cstr);
        }
    }
    return false;
}

// Append a character to the end of the string
bool string_push_back(string *const restrict s, const char c) {
    if (s) {
        errno = 0;

        if (s->size == s->capacity) {
            s->capacity *= 2;

            if (!string_resize(s, s->capacity)) return false;
        }
        s->data[s->size++] = c;
        return true;
    }
    return false;
}

// Get the character at the given index
char string_at(const string *const restrict s, const size_t index) {
    if (s && index < s->size)
        return s->data[index];

    return '\0';
}

// Remove the last character from the string
void string_pop_back(string *const restrict s) {
    if (s && s->size)
        --s->size;
}

// Check if the string is empty
bool string_empty(const string *const restrict s) {
    return s ? s->size == 0 : true;
}

// Remove the character at the given index
bool string_erase(string *const restrict s, const size_t index) {
    if (s && index < s->size) {
        memmove(s->data + index * sizeof(char), s->data + (index + 1) * sizeof(char), (s->size - index) * sizeof(char));
        --s->size;
        return true;
    }
    return false;
}

// Compare two strings
bool string_compare(const string *const restrict s1, const string *const restrict s2) {
    if (s1 == nullptr || s2 == nullptr || s1->size != s2->size)
        return false;

    return memcmp(s1->data, s2->data, s1->size) == 0;
}

// Get the length of the string
size_t string_length(const string *const restrict s) {
    return s ? s->size : 0;
}

// Get the capacity of the string
size_t string_capacity(const string *const restrict s) {
    return s ? s->capacity : 0;
}

// Clear the string
void string_clear(string *const restrict s) {
    if (s) s->size = 0;
}

// Insert a new character at the given index
bool string_insert(string *const restrict s, const size_t index, const char c) {
    if (s) {
        if (index < s->size) {
            if (s->size == s->capacity) {
                s->capacity *= 2;
                if (!string_resize(s, s->capacity)) return false;
            }
            memmove(s->data + (index + 1) * sizeof(char), s->data + index * sizeof(char),
                    (s->size - index) * sizeof(char));

            s->data[index] = c;
            ++s->size;

            return true;
        }
        if (index == 0 && s->size == 0) return string_push_back(s, c);
    }
    return false;
}

// Modify the character at the given index
void string_set(const string *const restrict s, const size_t index, const char c) {
    if (s && index < s->size)
        s->data[index] = c;
}

// Get the substring of the string
string *string_substr(const string *const restrict s, const size_t start, const size_t len) {
    if (s) {
        if (start >= s->size || start + len - 1 > s->size) return nullptr;

        string *sub = string_new();
        if (sub) {
            sub->size = len;
            if (!string_resize(sub, clp2(len))) return nullptr;

            memcpy(sub->data, s->data + start, len);
            return sub;
        }
    }
    return nullptr;
}

// Concatenate two strings
string *string_concat(const string *const restrict s1, const string *const restrict s2) {
    if (s1 && s2) {
        string *s = string_new();
        if (s) {
            if (!string_resize(s, s1->size + s2->size)) return nullptr;

            memcpy(s->data, s1->data, s1->size);
            memcpy(s->data + s1->size, s2->data, s2->size);

            s->size = s1->size + s2->size;

            return s;
        }
    }
    return nullptr;
}

// Append a string to the end of another string
bool string_append(string *const restrict s1, const string *const restrict s2) {
    if (s1 && s2) {
        if (s2->size == 0) return true;

        if (!string_resize(s1, s1->size + s2->size)) return false;

        memcpy(s1->data + s1->size, s2->data, s2->size);

        s1->size += s2->size;
        return true;
    }
    return false;
}

// Append a char array to the end of the string
bool string_append_cstr(string *const restrict s, const char *const restrict cstr) {
    if (s) {
        if (cstr) {
            const size_t len = strlen(cstr);
            if (!string_resize(s, s->size + len)) return false;

            memcpy(s->data + s->size * sizeof(char), cstr, len * sizeof(char));
            s->size += len;
        }
        return true;
    }
    return false;
}

// Convert the string to a char array
char *string_cstr(const string *const restrict s) {
    if (s) {
        char *cstr = malloc((s->size + 1) * sizeof(char));
        if (cstr) {
            memcpy(cstr, s->data, s->size);

            cstr[s->size] = '\0';
            return cstr;
        }
    }
    return nullptr;
}

// Compare the string with a char array
bool string_compare_cstr(const string *const restrict s, const char *const restrict cstr) {
    if (s && cstr) {
        if (s->size == strlen(cstr))
            return memcmp(s->data, cstr, s->size) == 0;
    }
    return false;
}
