#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "string_c.h"

/**
 * @brief Creates a new string with an initial capacity of 16.
 *
 * @return A pointer to the newly created string, or NULL if memory allocation failed.
 * @note The returned pointer must be freed by the caller, using \p string_free
 */
string_c *string_new() {
    string_c *s = malloc(sizeof(string_c));
    if (s) {
        if ((s->data = (char *) calloc(16, sizeof(char)))) {
            s->size = 0;
            s->capacity = 16;
            return s;
        }
        // If memory allocation failed, free the string
        free(s);
    }
    return nullptr;
}

/**
 * @brief Frees the memory used by a string.
 *
 * If the input pointer is NULL, the function does nothing.
 *
 * @param s A pointer to the string to be freed.
 */
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

/**
 * @brief Resizes the string to the given size.
 *
 * @param s A pointer to the string to be resized.
 * @param size The new size for the string.
 * @return False if resizing was necessary but failed, or if the string is invalid. True otherwise.
 *
 * @note This function only resizes the string if it needs to be resized.\n
 * Resizing is needed if the new size is greater than the current capacity
 * (which should be greater than 16).\n
 * The new size is rounded up to the next power of 2 to improve performance.
 * @warning This function is for internal use only, and should not be called directly by the user.
 */
static bool string_resize(string_c *const restrict s, size_t size) {
    if (s) {
        if (size <= 16) return true;

        // Round up the new size to the next power of 2, and reallocate the memory if necessary
        size = clp2(size);
        if (size >= s->capacity) {
            void *temp = realloc(s->data, size * sizeof(char));
            if (temp == nullptr) return false;

            // Update the string's data pointer
            s->data = (char *) temp;
            // Set the new capacity to the new size, and fill the rest of the string with null characters
            memset(s->data + s->size, '\0', s->capacity - s->size);
            s->capacity = size;
        }
        return true;
    }

    return false;
}

/**
 * @brief Inserts a C-string into a string at a specified index.
 *
 * @param s A pointer to the string where the C-string will be inserted.
 * @param index The index at which the C-string will be inserted.
 * @param cstr The C-string to be inserted.
 * @return true if the C-string was successfully inserted, false otherwise.
 */
bool string_insert_cstr(string_c *const restrict s, const size_t index, const char *const restrict cstr) {
    if (s) {
        if (cstr) {
            const size_t len = strlen(cstr);
            if (len) {
                // The index must be within the bounds of the string
                if (index < s->size) {
                    // Resize the string if necessary
                    if (s->size + len > s->capacity) {
                        if (!string_resize(s, s->size + len)) return false;
                    }
                    // Move the characters after the index to make room for the C-string
                    memmove(s->data + (index + len) * sizeof(char), s->data + index * sizeof(char),
                            (s->size - index) * sizeof(char));

                    // Copy the C-string into the string
                    memcpy(s->data + index * sizeof(char), cstr, len * sizeof(char));
                    s->size += len;

                    return true;
                }
                if (index == 0 && s->size == 0) return string_append_cstr(s, cstr);
            }
        }
    }
    return false;
}

/**
 * @brief Appends a character to the end of a string.
 *
 * @param s A pointer to the string where the character will be appended.
 * @param c The character to be appended.
 * @return true if the character was successfully appended, false otherwise.
 *
 * @note If the string is full, it is resized to twice its current capacity.\n
 * The character is not appended if it is the null character.
 */
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

/**
 * @brief Retrieves the character at a given index in the string.
 *
 * @param s A pointer to the string.
 * @param index The index of the character to be retrieved.
 * @return The character at the given index, or the null character if the index is out of bounds or the string is invalid.
 */
char string_at(const string_c *const restrict s, const size_t index) {
    if (s && index < s->size)
        return s->data[index];

    return '\0';
}

/**
 * @brief Removes the last character from the string.
 *
 * If the string is valid and not empty, it replaces replaces the last character with the null character.
 *
 * @param s A pointer to the string.
 */
void string_pop_back(string_c *const restrict s) {
    if (s && s->size)
        s->data[--s->size] = '\0';
}

/**
 * @brief Checks if the string is empty.
 *
 * @param s A pointer to the string.
 * @return true if the string is empty or invalid, false otherwise.
 */
bool string_empty(const string_c *const restrict s) {
    return s == nullptr || s->size == 0;
}

/**
 * @brief Removes the character at a given index in the string.
 *
 * @param s A pointer to the string.
 * @param index The index of the character to be removed.
 * @return true if the character was successfully removed, false otherwise.
 */
bool string_erase(string_c *const restrict s, const size_t index) {
    if (s && index < s->size) {
        // Move the characters after the index to overwrite the character to be removed
        memmove(s->data + index * sizeof(char), s->data + (index + 1) * sizeof(char), (s->size - index) * sizeof(char));
        // Replace the last character with the null character
        s->data[--s->size] = '\0';
        return true;
    }
    return false;
}

/**
 * @brief Compares two strings for equality.
 *
 * @param s1 A pointer to the first string.
 * @param s2 A pointer to the second string.
 * @return true if the strings are equal, false otherwise.
 */
bool string_compare(const string_c *const restrict s1, const string_c *const restrict s2) {
    if (s1 == nullptr || s2 == nullptr || s1->size != s2->size)
        return false;

    return memcmp(s1->data, s2->data, s1->size) == 0;
}

/**
 * @brief Compares two strings for equality, ignoring case.
 *
 * @param s1 A pointer to the first string.
 * @param s2 A pointer to the second string.
 * @return true if the strings are equal (ignoring case), false otherwise.
 */
bool string_case_compare(const string_c *const restrict s1, const string_c *const restrict s2) {
    if (s1 == nullptr || s2 == nullptr || s1->size != s2->size)
        return false;

    return strncasecmp(s1->data, s2->data, s1->size) == 0;
}

/**
 * @brief Returns the length of the string.
 *
 * @param s A pointer to the string.
 * @return The length of the string, or 0 if the string is invalid.
 */
size_t string_length(const string_c *const restrict s) {
    return s ? s->size : 0;
}

/**
 * @brief Returns the capacity of the string.
 *
 * @param s A pointer to the string.
 * @return The capacity of the string, or 0 if the string is invalid.
 */
size_t string_capacity(const string_c *const restrict s) {
    return s ? s->capacity : 0;
}

/**
 * @brief Clears the string.
 *
 * @param s A pointer to the string.
 */
void string_clear(string_c *const restrict s) {
    if (s && s->size) {
        memset(s->data, '\0', s->size);
        s->size = 0;
    }
}

/**
 * @brief Inserts a new character at a given index in the string.
 *
 * @param s A pointer to the string where the character will be inserted.
 * @param index The index at which the character will be inserted.
 * @param c The character to be inserted.
 * @return true if the character was successfully inserted, false otherwise.
 */
bool string_insert(string_c *const restrict s, const size_t index, const char c) {
    if (s && c != '\0') {
        if (index < s->size) {
            // Resize the string if necessary
            if (s->size == s->capacity) {
                if (!string_resize(s, s->capacity * 2)) return false;
            }
            // Move the characters after the index to make room for the new character
            memmove(s->data + (index + 1) * sizeof(char), s->data + index * sizeof(char),
                    (s->size - index) * sizeof(char));

            // Insert the new character into the string
            s->data[index] = c;
            ++s->size;

            return true;
        }
        if (index == 0 && s->size == 0) return string_push_back(s, c);
    }
    return false;
}

/**
 * @brief Modifies the character at a given index in the string.
 *
 * @param s A pointer to the string where the character will be modified.
 * @param index The index of the character to be modified.
 * @param c The new character.
 */
void string_set(const string_c *const restrict s, const size_t index, const char c) {
    if (s && c != '\0' && index < s->size)
        s->data[index] = c;
}

/**
 * @brief Retrieves a substring from the string.
 *
 * @param s A pointer to the string from which the substring will be retrieved.
 * @param start The start index of the substring.
 * @param len The length of the substring.
 * @return A pointer to the new string containing the substring, or NULL if the substring could not be retrieved.
 *
 * @note The returned pointer must be freed by the caller, using \p string_free
 */
string_c *string_substr(const string_c *const restrict s, const size_t start, const size_t len) {
    if (s) {
        // The requested substring must be within the bounds of the string
        if (len == 0 || start + len - 1 > s->size) return nullptr;

        // Create a new string to store the substring
        string_c *sub = string_new();
        if (sub) {
            // Resize the new string to the length of the substring
            if (string_resize(sub, len)) {
                // Copy the substring into the new string
                memcpy(sub->data, s->data + start, len);
                sub->size = len;
                return sub;
            }
            // If resizing failed, free the new string
            string_free(sub);
        }
    }
    return nullptr;
}

/**
 * @brief Concatenates two strings.
 *
 * @param s1 A pointer to the first string.
 * @param s2 A pointer to the second string.
 * @return A pointer to the new string containing the concatenated strings,
 * or NULL if the strings could not be concatenated.
 * @note The returned pointer must be freed by the caller, using \p string_free
 */
string_c *string_concat(const string_c *const restrict s1, const string_c *const restrict s2) {
    if (s1 && s2) {
        string_c *s = string_new();
        if (s) {
            // Resize the new string appropriately
            if (string_resize(s, s1->size + s2->size)) {
                // Copy the input strings into the new string
                memcpy(s->data, s1->data, s1->size * sizeof(char));
                memcpy(s->data + s1->size * sizeof(char), s2->data, s2->size * sizeof(char));

                s->size = s1->size + s2->size;

                return s;
            }
            // If resizing failed, free the new string
            string_free(s);
        }
    }
    return nullptr;
}

/**
 * @brief Appends a specified number of characters from one string to another.
 *
 * @param s1 A pointer to the string where the characters will be appended.
 * @param s2 A pointer to the string from which the characters will be copied.
 * @param count The number of characters to be copied from the second string to the first string.
 * @return true if the characters were successfully appended, false otherwise.
 */
bool string_append_range(string_c *const restrict s1, const string_c *const restrict s2, const size_t count) {
    if (s1) {
        if (count == 0) return true;

        if (s2) {
            if (count <= s2->size) {
                if (string_resize(s1, s1->size + count)) {
                    // Copy the characters from the second string into the first string
                    memcpy(s1->data + s1->size * sizeof(char), s2->data, count * sizeof(char));

                    s1->size += count;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Appends a string to the end of another string.
 *
 * @param s1 A pointer to the string where the second string will be appended.
 * @param s2 A pointer to the string that will be appended to the first string.
 * @return true if the second string was successfully appended, false otherwise.
 */
bool string_append(string_c *const restrict s1, const string_c *const restrict s2) {
    return s2 && string_append_range(s1, s2, s2->size);
}

/**
 * @brief Appends a specified number of characters from a C-string to a string.
 *
 * @param s A pointer to the string where the characters will be appended.
 * @param cstr The C-string from which the characters will be copied.
 * @param count The number of characters to be copied from the C-string to the string.
 * @return true if the characters were successfully appended, false otherwise.
 */
bool string_append_cstr_range(string_c *const restrict s, const char *const restrict cstr, const size_t count) {
    if (s) {
        if (count == 0) return true;
        if (cstr) {
            // Resize the string if necessary
            if (count <= strlen(cstr)) {
                if (string_resize(s, s->size + count)) {
                    // Copy the C-string into the string
                    memcpy(s->data + s->size * sizeof(char), cstr, count * sizeof(char));
                    s->size += count;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Appends a C-string to the end of a string.
 *
 * @param s A pointer to the string where the C-string will be appended.
 * @param cstr The C-string to be appended to the string.
 * @return true if the C-string was successfully appended, false otherwise.
 */
bool string_append_cstr(string_c *const restrict s, const char *const restrict cstr) {
    return string_append_cstr_range(s, cstr, strlen(cstr));
}

/**
 * @brief Converts a string to a C-string.
 *
 * @param s A pointer to the string to be converted.
 * @return A pointer to the newly created C-string, or NULL if the string could not be converted.
 * @note The returned C-string must be freed by the caller.\n \n
 * It is better to use \p string_copy_buffer if the C-string is only needed temporarily.
 */
char *string_cstr(const string_c *const restrict s) {
    if (s) {
        // Allocate memory for the C-string
        char *cstr = (char *) malloc((s->size + 1) * sizeof(char));
        if (cstr) {
            // Copy the characters from the string to the C-string
            memcpy(cstr, s->data, s->size);

            // Append the null character to the end of the C-string
            cstr[s->size] = '\0';
            return cstr;
        }
    }
    return nullptr;
}

/**
 * @brief Compares a string with a C-string for equality.
 *
 * @param s A pointer to the string.
 * @param cstr The C-string to be compared with the string.
 * @return true if the string and the C-string are equal, false otherwise.
 */
bool string_compare_cstr(const string_c *const restrict s, const char *const restrict cstr) {
    if (s && cstr) {
        if (s->size == strlen(cstr))
            return memcmp(s->data, cstr, s->size) == 0;
    }
    return false;
}

/**
 * @brief Compares a string with a C-string for equality, ignoring case.
 *
 * @param s A pointer to the string.
 * @param cstr The C-string to be compared with the string.
 * @return true if the string and the C-string are equal (ignoring case), false otherwise.
 */
bool string_case_compare_cstr(const string_c *const restrict s, const char *const restrict cstr) {
    if (s && cstr) {
        if (s->size == strlen(cstr))
            return strncasecmp(s->data, cstr, s->size) == 0;
    }
    return false;
}

/**
 * @brief Copies the characters from a string to a buffer.
 *
 * @param s A pointer to the string.
 * @param buf The buffer where the characters will be copied.
 * @return true if the characters were successfully copied, false otherwise.
 */
bool string_copy_buffer(const string_c *const restrict s, char *buf) {
    if (s && !string_empty(s)) {
        // Copy the characters from the string to the buffer
        memcpy(buf, s->data, s->size * sizeof(char));
        // Append the null character to the end of the buffer
        buf[s->size] = '\0';
        return true;
    }
    return false;
}

/**
 * @brief Copies the contents of one string to another.
 *
 * @param src A pointer to the source string.
 * @param dest A pointer to the destination string.
 * @return true if the contents of the source string were successfully copied to the destination string, false otherwise.
 */
bool string_copy(const string_c *const restrict src, string_c *const restrict dest) {
    if (src && dest && src->data && dest->data) {
        // Resize the destination string if necessary
        if (src->size > dest->size)
            if (!string_resize(dest, src->size)) return false;

        // Copy the characters from the source string to the destination string
        memcpy(dest->data, src->data, src->size * sizeof(char));
        dest->size = src->size;
        return true;
    }
    return false;
}

/**
 * @brief Swaps the contents of two strings.
 *
 * @param s1 A pointer to the first string.
 * @param s2 A pointer to the second string.
 * @return true if the contents of the strings were successfully swapped, false otherwise.
 */
bool string_swap(string_c *const restrict s1, string_c *const restrict s2) {
    if (s1 && s2) {
        const string_c temp = *s1;
        *s1 = *s2;
        *s2 = temp;

        return true;
    }
    return false;
}
