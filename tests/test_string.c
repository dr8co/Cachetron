#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../data_structures/string_c.h"

void string_new_creates_empty_string() {
    string_c *s = string_new();
    assert(s != nullptr);
    assert(string_length(s) == 0);
    assert(string_capacity(s) == 16);
    string_free(s);
}

void string_push_back_increases_size() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    assert(string_length(s) == 1);
    string_free(s);
}

void string_push_back_stores_correct_value() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_pop_back_decreases_size() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_pop_back(s);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_insert_increases_size() {
    string_c *s = string_new();
    string_insert(s, 0, 'a');
    assert(string_length(s) == 1);
    string_free(s);
}

void string_insert_stores_correct_value() {
    string_c *s = string_new();
    string_insert(s, 0, 'a');
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_erase_decreases_size() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_erase(s, 0);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_clear_resets_size() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_clear(s);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_append_increases_size() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();
    string_push_back(s1, 'a');
    string_push_back(s2, 'b');
    string_append(s1, s2);
    assert(string_length(s1) == 2);
    string_free(s1);
    string_free(s2);
}

void string_append_stores_correct_values() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();
    string_push_back(s1, 'a');
    string_push_back(s2, 'b');
    string_append(s1, s2);
    assert(string_at(s1, 0) == 'a');
    assert(string_at(s1, 1) == 'b');
    string_free(s1);
    string_free(s2);
}

void string_push_back_increases_capacity_when_needed() {
    string_c *s = string_new();
    for (int i = 0; i < 17; ++i) {
        string_push_back(s, 'a');
    }
    assert(string_capacity(s) > (size_t) 16);
    string_free(s);
}

void string_pop_back_does_not_decrease_capacity() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    const size_t old_capacity = string_capacity(s);
    string_pop_back(s);
    assert(string_capacity(s) == old_capacity);
    string_free(s);
}

void string_insert_increases_capacity_when_needed() {
    string_c *s = string_new();
    for (int i = 0; i < 17; ++i) {
        string_insert(s, 0, 'a');
    }
    assert(string_capacity(s) > (size_t) 16);
    string_free(s);
}

void string_erase_does_not_decrease_capacity() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    const size_t old_capacity = string_capacity(s);
    string_erase(s, 0);
    assert(string_capacity(s) == old_capacity);
    string_free(s);
}

void string_clear_does_not_decrease_capacity() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    const size_t old_capacity = string_capacity(s);
    string_clear(s);
    assert(string_capacity(s) == old_capacity);
    string_free(s);
}

void string_append_increases_capacity_when_needed() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();
    for (int i = 0; i < 9; ++i) {
        string_push_back(s1, 'a');
        string_push_back(s2, 'b');
    }
    string_append(s1, s2);
    assert(string_capacity(s1) > (size_t) 16);
    string_free(s1);
    string_free(s2);
}

void string_set_stores_correct_value() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_set(s, 0, 'b');
    assert(string_at(s, 0) == 'b');
    string_free(s);
}

void string_substr_returns_correct_string() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_push_back(s, 'b');
    string_push_back(s, 'c');
    string_c *s2 = string_substr(s, 1, 2);
    assert(string_length(s2) == 2);
    assert(string_at(s2, 0) == 'b');
    assert(string_at(s2, 1) == 'c');
    string_free(s);
    string_free(s2);
}

void string_concat_returns_correct_string() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();
    string_push_back(s1, 'a');
    string_push_back(s2, 'b');
    string_c *s3 = string_concat(s1, s2);
    assert(string_length(s3) == 2);
    assert(string_at(s3, 0) == 'a');
    assert(string_at(s3, 1) == 'b');
    string_free(s1);
    string_free(s2);
    string_free(s3);
}

void string_append_cstr_stores_correct_values() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_append_cstr(s, "bc");
    assert(string_length(s) == 3);
    assert(string_at(s, 1) == 'b');
    assert(string_at(s, 2) == 'c');
    string_free(s);
}

void string_insert_cstr_stores_correct_values() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_push_back(s, 'b');

    string_insert_cstr(s, 1, "cd");
    assert(string_length(s) == 4);
    assert(string_at(s, 1) == 'c');
    assert(string_at(s, 2) == 'd');
    string_free(s);
}

void string_cstr_returns_correct_cstr() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_push_back(s, 'b');
    string_push_back(s, 'c');

    char *cstr = string_cstr(s);
    assert(cstr[3] == '\0');

    assert(strcmp(cstr, "abc") == 0);
    free(cstr);
    string_free(s);
}

void string_compare_cstr_returns_true_for_equal_strings() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_push_back(s, 'b');
    assert(string_compare_cstr(s, "ab"));
    string_free(s);
}

void string_empty_returns_false_for_non_empty_string() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    assert(!string_empty(s));
    string_free(s);
}

void string_empty_returns_true_for_empty_string() {
    string_c *s = string_new();
    assert(string_empty(s));
    string_free(s);
}

void string_at_returns_correct_value() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_erase_removes_correct_value() {
    string_c *s = string_new();
    string_push_back(s, 'a');
    string_push_back(s, 'b');
    string_erase(s, 0);
    assert(string_at(s, 0) == 'b');
    string_free(s);
}

void string_compare_works_correctly() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();
    string_push_back(s1, 'a');
    string_push_back(s2, 'a');
    assert(string_compare(s1, s2));
    string_free(s1);
    string_free(s2);
}

void string_swap_swaps_contents_correctly() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();

    string_push_back(s1, 'a');
    string_push_back(s1, 'b');
    string_push_back(s1, 'c');
    string_push_back(s1, 'z');
    const size_t size1 = string_length(s1);

    string_push_back(s2, 'd');
    string_push_back(s2, 'e');
    string_push_back(s2, 'f');
    const size_t size2 = string_length(s2);

    assert(string_swap(s1, s2));
    assert(string_at(s1, 0) == 'd');
    assert(string_at(s1, 2) == 'f');

    assert(string_at(s2, 0) == 'a');
    assert(string_at(s2, 3) == 'z');

    assert(size1 == string_length(s2));
    assert(size2 == string_length(s1));

    string_free(s1);
    string_free(s2);
}

void string_swap_handles_empty_strings() {
    string_c *s1 = string_new();
    string_c *s2 = string_new();
    string_push_back(s1, 'a');

    assert(string_swap(s1, s2));
    assert(string_empty(s1));
    assert(string_at(s2, 0) == 'a');

    string_free(s1);
    string_free(s2);
}

void string_swap_returns_false_for_nullptr() {
    string_c *s = string_new();
    string_push_back(s, 'a');

    assert(!string_swap(s, nullptr));
    assert(!string_swap(nullptr, s));
    assert(!string_swap(nullptr, nullptr));

    string_free(s);
}

void string_functions_do_not_crash_for_nullptr() {
    string_free(nullptr);
    assert(!string_push_back(nullptr, 'a'));
    assert(string_at(nullptr, 10) == '\0');
    string_pop_back(nullptr);

    assert(string_empty(nullptr));
    assert(!string_erase(nullptr, 17));
    assert(!string_compare(nullptr, nullptr));
    assert(!string_length(nullptr));
    assert(!string_capacity(nullptr));
    string_clear(nullptr);

    assert(!string_insert(nullptr, 1000, 'm'));
    string_set(nullptr, 0, 'x');
    assert(string_substr(nullptr, 12, 30) == nullptr);
    assert(string_concat(nullptr, nullptr) == nullptr);
    assert(!string_append(nullptr, nullptr));

    assert(string_cstr(nullptr) == nullptr);
    assert(!string_compare_cstr(nullptr, nullptr));
    assert(!string_insert_cstr(nullptr, 5, nullptr));
}

int main() {
    string_new_creates_empty_string();
    string_push_back_increases_size();
    string_push_back_stores_correct_value();
    string_pop_back_decreases_size();
    string_insert_increases_size();
    string_insert_stores_correct_value();
    string_erase_decreases_size();
    string_clear_resets_size();
    string_append_increases_size();
    string_append_stores_correct_values();

    string_push_back_increases_capacity_when_needed();
    string_pop_back_does_not_decrease_capacity();
    string_insert_increases_capacity_when_needed();
    string_erase_does_not_decrease_capacity();
    string_clear_does_not_decrease_capacity();
    string_append_increases_capacity_when_needed();

    string_set_stores_correct_value();
    string_substr_returns_correct_string();
    string_concat_returns_correct_string();
    string_append_cstr_stores_correct_values();
    string_insert_cstr_stores_correct_values();
    string_cstr_returns_correct_cstr();
    string_compare_cstr_returns_true_for_equal_strings();
    string_empty_returns_false_for_non_empty_string();

    string_empty_returns_true_for_empty_string();
    string_at_returns_correct_value();
    string_erase_removes_correct_value();
    string_compare_works_correctly();
    string_functions_do_not_crash_for_nullptr();

    string_swap_swaps_contents_correctly();
    string_swap_handles_empty_strings();
    string_swap_returns_false_for_nullptr();
    return 0;
}
