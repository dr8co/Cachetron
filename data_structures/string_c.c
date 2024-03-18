#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "string_c.h"

// Create a new string, with initial capacity of 16
string_c *string_new() {
    string_c *s = malloc(sizeof(string_c));
    if (s) {
        if ((s->data = (char *) calloc(16, sizeof(char)))) {
            s->size = 0;
            s->capacity = 16;
            return s;
        }
        free(s);
    }
    return nullptr;
}

// Free the memory used by the string
void string_free(string_c *const restrict s) {
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

bool string_resize(string_c *const restrict s, size_t size) {
    if (s) {
        if (size <= 16) return true;

        errno = 0;
        size = clp2(size);

        if (size >= s->capacity) {
            void *temp = realloc(s->data, size * sizeof(char));
            if (temp == nullptr || errno) {
                return false;
            }
            s->data = (char *) temp;
            memset(s->data + s->size, '\0', s->capacity - s->size);
            s->capacity = size;
        }
        return true;
    }

    return false;
}

bool string_insert_cstr(string_c *const restrict s, const size_t index, const char *const restrict cstr) {
    if (s) {
        if (cstr) {
            const size_t len = strlen(cstr);
            if (index < s->size) {
                if (s->size + len > s->capacity) {
                    if (!string_resize(s, s->size + len)) return false;
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
bool string_push_back(string_c *const restrict s, const char c) {
    if (s && c != '\0') {
        if (s->size == s->capacity) {
            if (!string_resize(s, s->capacity * 2)) return false;
        }
        s->data[s->size++] = c;
        return true;
    }
    return false;
}

// Get the character at the given index
char string_at(const string_c *const restrict s, const size_t index) {
    if (s && index < s->size)
        return s->data[index];

    return '\0';
}

// Remove the last character from the string
void string_pop_back(string_c *const restrict s) {
    if (s && s->size)
        s->data[--s->size] = '\0';
}

// Check if the string is empty
bool string_empty(const string_c *const restrict s) {
    return s == nullptr || s->size == 0;
}

// Remove the character at the given index
bool string_erase(string_c *const restrict s, const size_t index) {
    if (s && index < s->size) {
        memmove(s->data + index * sizeof(char), s->data + (index + 1) * sizeof(char), (s->size - index) * sizeof(char));
        s->data[--s->size] = '\0';
        return true;
    }
    return false;
}

// Compare two strings
bool string_compare(const string_c *const restrict s1, const string_c *const restrict s2) {
    if (s1 == nullptr || s2 == nullptr || s1->size != s2->size)
        return false;

    return memcmp(s1->data, s2->data, s1->size) == 0;
}

bool string_case_compare(const string_c *const restrict s1, const string_c *const restrict s2) {
    if (s1 == nullptr || s2 == nullptr || s1->size != s2->size)
        return false;

    return strncasecmp(s1->data, s2->data, s1->size) == 0;
}

// Get the length of the string
size_t string_length(const string_c *const restrict s) {
    return s ? s->size : 0;
}

// Get the capacity of the string
size_t string_capacity(const string_c *const restrict s) {
    return s ? s->capacity : 0;
}

// Clear the string
void string_clear(string_c *const restrict s) {
    if (s) {
        memset(s->data, '\0', s->size);
        s->size = 0;
    }
}

// Insert a new character at the given index
bool string_insert(string_c *const restrict s, const size_t index, const char c) {
    if (s && c != '\0') {
        if (index < s->size) {
            if (s->size == s->capacity) {
                if (!string_resize(s, s->capacity * 2)) return false;
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
void string_set(const string_c *const restrict s, const size_t index, const char c) {
    if (s && c != '\0' && index < s->size)
        s->data[index] = c;
}

// Get the substring of the string
string_c *string_substr(const string_c *const restrict s, const size_t start, const size_t len) {
    if (s) {
        if (len == 0 || start + len - 1 > s->size) return nullptr;

        string_c *sub = string_new();
        if (sub) {
            if (string_resize(sub, len)) {
                memcpy(sub->data, s->data + start, len);
                sub->size = len;
                return sub;
            }
            string_free(sub);
        }
    }
    return nullptr;
}

// Concatenate two strings
string_c *string_concat(const string_c *const restrict s1, const string_c *const restrict s2) {
    if (s1 && s2) {
        string_c *s = string_new();
        if (s) {
            if (string_resize(s, s1->size + s2->size)) {
                memcpy(s->data, s1->data, s1->size * sizeof(char));
                memcpy(s->data + s1->size * sizeof(char), s2->data, s2->size * sizeof(char));

                s->size = s1->size + s2->size;

                return s;
            }
            string_free(s);
        }
    }
    return nullptr;
}

bool string_append_range(string_c *const restrict s1, const string_c *const restrict s2, const size_t count) {
    if (s1) {
        if (count == 0) return true;

        if (s2) {
            if (count <= s2->size) {
                if (string_resize(s1, s1->size + count)) {
                    memcpy(s1->data + s1->size * sizeof(char), s2->data, count * sizeof(char));

                    s1->size += count;
                    return true;
                }
            }
        }
    }
    return false;
}

// Append a string to the end of another string
bool string_append(string_c *const restrict s1, const string_c *const restrict s2) {
    return s2 != nullptr && string_append_range(s1, s2, s2->size);
}

bool string_append_cstr_range(string_c *const restrict s, const char *const restrict cstr, const size_t count) {
    if (s) {
        if (count == 0) return true;
        if (cstr) {
            if (count <= strlen(cstr)) {
                if (string_resize(s, s->size + count)) {
                    memcpy(s->data + s->size * sizeof(char), cstr, count * sizeof(char));
                    s->size += count;
                    return true;
                }
            }
        }
    }
    return false;
}

// Append a char array to the end of the string
bool string_append_cstr(string_c *const restrict s, const char *const restrict cstr) {
    return string_append_cstr_range(s, cstr, strlen(cstr));
}

// Convert the string to a char array
char *string_cstr(const string_c *const restrict s) {
    if (s) {
        char *cstr = (char *) malloc((s->size + 1) * sizeof(char));
        if (cstr) {
            memcpy(cstr, s->data, s->size);

            cstr[s->size] = '\0';
            return cstr;
        }
    }
    return nullptr;
}

// Compare the string with a char array
bool string_compare_cstr(const string_c *const restrict s, const char *const restrict cstr) {
    if (s && cstr) {
        if (s->size == strlen(cstr))
            return memcmp(s->data, cstr, s->size) == 0;
    }
    return false;
}

bool string_case_compare_cstr(const string_c *const restrict s, const char *const restrict cstr) {
    if (s && cstr) {
        if (s->size == strlen(cstr))
            return strncasecmp(s->data, cstr, s->size) == 0;
    }
    return false;
}

bool string_copy_buffer(const string_c *const restrict s, char *buf) {
    if (s && !string_empty(s)) {
        memcpy(buf, s->data, s->size * sizeof(char));
        buf[s->size] = '\0';
        return true;
    }
    return false;
}

bool string_copy(const string_c *const restrict src, string_c *const restrict dest) {
    if (src && dest && src->data && dest->data) {
        if (src->size > dest->size)
            if (!string_resize(dest, src->size)) return false;

        memcpy(dest->data, src->data, src->size * sizeof(char));
        dest->size = src->size;
        return true;
    }
    return false;
}

// Swap the contents of two strings
bool string_swap(string_c *const restrict s1, string_c *const restrict s2) {
    if (s1 && s2) {
        const string_c temp = *s1;
        *s1 = *s2;
        *s2 = temp;
        return true;
    }
    return false;
}
