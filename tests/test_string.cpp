#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "../data_structures/string/lite_string.h"

void string_new_creates_empty_string() {
    lite_string *s = string_new();
    assert(s != nullptr);
    assert(string_length(s) == 0);
    assert(string_capacity(s) == 16);
    string_free(s);
}

void string_push_back_increases_size() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_length(s) == 1);
    string_free(s);
}

void string_push_back_stores_correct_value() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_pop_back_decreases_size() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    string_pop_back(s);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_insert_increases_size() {
    lite_string *s = string_new();
    assert(string_insert(s, 0, 'a'));
    assert(string_length(s) == 1);
    string_free(s);
}

void string_insert_stores_correct_value() {
    lite_string *s = string_new();
    assert(string_insert(s, 0, 'a'));
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_erase_decreases_size() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_erase(s, 0));
    assert(string_length(s) == 0);
    string_free(s);
}

void string_clear_resets_size() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    string_clear(s);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_append_increases_size() {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    assert(string_push_back(s1, 'a'));
    assert(string_push_back(s2, 'b'));
    string_append(s1, s2);
    assert(string_length(s1) == 2);
    string_free(s1);
    string_free(s2);
}

void string_append_stores_correct_values() {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    assert(string_push_back(s1, 'a'));
    assert(string_push_back(s2, 'b'));
    string_append(s1, s2);
    assert(string_at(s1, 0) == 'a');
    assert(string_at(s1, 1) == 'b');
    string_free(s1);
    string_free(s2);
}

void string_push_back_increases_capacity_when_needed() {
    lite_string *s = string_new();
    for (int i = 0; i < 17; ++i) {
        string_push_back(s, 'a');
    }
    assert(string_capacity(s) > (size_t) 16);
    string_free(s);
}

void string_pop_back_does_not_decrease_capacity() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    const size_t old_capacity = string_capacity(s);
    string_pop_back(s);
    assert(string_capacity(s) == old_capacity);
    string_free(s);
}

void string_insert_increases_capacity_when_needed() {
    lite_string *s = string_new();
    for (int i = 0; i < 17; ++i) {
        string_insert(s, 0, 'a');
    }
    assert(string_capacity(s) > (size_t) 16);
    string_free(s);
}

void string_erase_does_not_decrease_capacity() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    const size_t old_capacity = string_capacity(s);
    string_erase(s, 0);
    assert(string_capacity(s) == old_capacity);
    string_free(s);
}

void string_clear_does_not_decrease_capacity() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    const size_t old_capacity = string_capacity(s);
    string_clear(s);
    assert(string_capacity(s) == old_capacity);
    string_free(s);
}

void string_append_increases_capacity_when_needed() {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
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
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    string_set(s, 0, 'b');
    assert(string_at(s, 0) == 'b');
    string_free(s);
}

void string_substr_returns_correct_string() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_push_back(s, 'b'));
    assert(string_push_back(s, 'c'));
    lite_string *s2 = string_substr(s, 1, 2);
    assert(string_length(s2) == 2);
    assert(string_at(s2, 0) == 'b');
    assert(string_at(s2, 1) == 'c');
    string_free(s);
    string_free(s2);
}

void string_concat_returns_correct_string() {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    assert(string_push_back(s1, 'a'));
    assert(string_push_back(s2, 'b'));
    lite_string *s3 = string_concat(s1, s2);
    assert(string_length(s3) == 2);
    assert(string_at(s3, 0) == 'a');
    assert(string_at(s3, 1) == 'b');
    string_free(s1);
    string_free(s2);
    string_free(s3);
}

void string_append_cstr_stores_correct_values() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    string_append_cstr(s, "bc");
    assert(string_length(s) == 3);
    assert(string_at(s, 1) == 'b');
    assert(string_at(s, 2) == 'c');
    string_free(s);
}

void string_insert_cstr_stores_correct_values() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_push_back(s, 'b'));

    assert(string_insert_cstr(s, "cd", 1));
    assert(string_length(s) == 4);
    assert(string_at(s, 1) == 'c');
    assert(string_at(s, 2) == 'd');
    string_free(s);
}

void string_cstr_returns_correct_cstr() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_push_back(s, 'b'));
    assert(string_push_back(s, 'c'));

    char *cstr = string_cstr(s);
    assert(cstr[3] == '\0');

    assert(strcmp(cstr, "abc") == 0);
    free(cstr);
    string_free(s);
}

void string_compare_cstr_returns_true_for_equal_strings() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_push_back(s, 'b'));
    assert(string_compare_cstr(s, "ab"));
    string_free(s);
}

void string_empty_returns_false_for_non_empty_string() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(!string_empty(s));
    string_free(s);
}

void string_empty_returns_true_for_empty_string() {
    lite_string *s = string_new();
    assert(string_empty(s));
    string_free(s);
}

void string_at_returns_correct_value() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_erase_removes_correct_value() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));
    assert(string_push_back(s, 'b'));
    string_erase(s, 0);
    assert(string_at(s, 0) == 'b');
    string_free(s);
}

void string_compare_works_correctly() {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    assert(string_push_back(s1, 'a'));
    assert(string_push_back(s2, 'a'));
    assert(string_compare(s1, s2));
    string_free(s1);
    string_free(s2);
}

void string_swap_swaps_contents_correctly() {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();

    assert(string_push_back(s1, 'a'));
    assert(string_push_back(s1, 'b'));
    assert(string_push_back(s1, 'c'));
    assert(string_push_back(s1, 'z'));
    const size_t size1 = string_length(s1);

    assert(string_push_back(s2, 'd'));
    assert(string_push_back(s2, 'e'));
    assert(string_push_back(s2, 'f'));
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
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    assert(string_push_back(s1, 'a'));

    assert(string_swap(s1, s2));
    assert(string_empty(s1));
    assert(string_at(s2, 0) == 'a');

    string_free(s1);
    string_free(s2);
}

void string_swap_returns_false_for_nullptr() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'a'));

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
    assert(!string_insert_cstr(nullptr, nullptr, 5));

    assert(!string_insert_range(nullptr, nullptr, 99, 1102));
    assert(!string_insert_string(nullptr, nullptr, 601));

    assert(string_find_last_of(nullptr, 'a') == SIZE_MAX);
    assert(string_find_last_not_of(nullptr, 'a') == SIZE_MAX);

    assert(string_find_first_from(nullptr, 'a', 10) == SIZE_MAX);
    assert(string_find_first_of(nullptr, 'a') == SIZE_MAX);
    assert(string_find_first_not_of(nullptr, 'a') == SIZE_MAX);

    assert(string_find_substr_from(nullptr, nullptr, 17) == SIZE_MAX);
    assert(string_find_substr(nullptr, nullptr) == SIZE_MAX);

    assert(string_rfind_substr(nullptr, nullptr) == SIZE_MAX);
    assert(string_find_substr_cstr_from(nullptr, nullptr, 1900) == SIZE_MAX);
    assert(string_rfind_substr_cstr(nullptr, nullptr) == SIZE_MAX);
    assert(string_find_substr_cstr(nullptr, nullptr) == SIZE_MAX);

    assert(!string_contains_char(nullptr, 'a'));
    assert(!string_contains(nullptr, nullptr));

    assert(!string_starts_with(nullptr, nullptr));
    assert(!string_ends_with(nullptr, nullptr));
    assert(!string_ends_with_cstr(nullptr, nullptr));

    assert(!string_shrink(nullptr, 54));
    assert(!string_shrink_to_fit(nullptr));
}

void string_insert_cstr_inserts_at_valid_index() {
    lite_string *s = string_new();
    assert(string_insert_cstr(s, "Hello", 0));
    assert(strcmp(s->data, "Hello") == 0);
    string_free(s);
}

void string_insert_cstr_does_not_insert_at_invalid_index() {
    lite_string *s = string_new();
    assert(!string_insert_cstr(s, "Hello", 5));
    string_free(s);
}

void string_insert_cstr_inserts_in_middle_of_string() {
    lite_string *s = string_new();
    assert(string_insert_cstr(s, "Hello", 0));
    assert(string_insert_cstr(s, " world", 5));
    assert(strcmp(s->data, "Hello world") == 0);
    string_free(s);
}

void string_insert_cstr_does_not_insert_null_cstr() {
    lite_string *s = string_new();
    assert(!string_insert_cstr(s, NULL, 0));
    string_free(s);
}

void string_insert_cstr_resizes_string_if_needed() {
    lite_string *s = string_new();
    assert(string_insert_cstr(s, "Hello, this is a long string that will require resizing", 0));
    assert(strcmp(s->data, "Hello, this is a long string that will require resizing") == 0);
    string_free(s);
}

void string_back_returns_last_character_for_non_empty_string() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'H'));
    assert(string_push_back(s, 'i'));
    assert(string_back(s) == 'i');
    string_free(s);
}

void string_back_returns_null_character_for_empty_string() {
    lite_string *s = string_new();
    assert(string_back(s) == '\0');
    string_free(s);
}

void string_front_returns_first_character_for_non_empty_string() {
    lite_string *s = string_new();
    assert(string_push_back(s, 'H'));
    assert(string_push_back(s, 'i'));
    assert(string_front(s) == 'H');
    string_free(s);
}

void string_front_returns_null_character_for_empty_string() {
    lite_string *s = string_new();
    assert(string_front(s) == '\0');
    string_free(s);
}

void string_insert_range_inserts_at_valid_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(sub, "Hello"));
    assert(string_insert_range(s, sub, 0, 5));
    assert(string_compare_cstr(s, "Hello"));
    string_free(s);
    string_free(sub);
}

void string_insert_range_does_not_insert_at_invalid_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(sub, "Hello"));
    assert(!string_insert_range(s, sub, 5, 5));
    string_free(s);
    string_free(sub);
}

void string_insert_range_inserts_in_middle_of_string() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Helo"));
    assert(string_append_cstr(sub, "l"));
    assert(string_insert_range(s, sub, 2, 1));
    assert(string_compare_cstr(s, "Hello"));
    string_free(s);
    string_free(sub);
}

void string_insert_range_does_not_insert_null_substring() {
    lite_string *s = string_new();
    assert(!string_insert_range(s, NULL, 0, 0));
    string_free(s);
}

void string_insert_range_resizes_string_if_needed() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(sub, "Hello, this is a long string that will require resizing"));
    assert(string_insert_range(s, sub, 0, sub->size));
    assert(string_compare(s, sub));
    string_free(s);
    string_free(sub);
}

void string_insert_string_inserts_at_valid_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(sub, "Hello"));
    assert(string_insert_string(s, sub, 0));
    assert(string_compare_cstr(s, "Hello"));
    string_free(s);
    string_free(sub);
}

void string_insert_string_does_not_insert_at_invalid_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(sub, "Hello"));
    assert(!string_insert_string(s, sub, 5));
    string_free(s);
    string_free(sub);
}

void string_insert_string_inserts_in_middle_of_string() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Helo"));
    assert(string_append_cstr(sub, "l"));
    assert(string_insert_string(s, sub, 2));
    assert(string_compare_cstr(s, "Hello"));
    string_free(s);
    string_free(sub);
}

void string_insert_string_does_not_insert_null_substring() {
    lite_string *s = string_new();
    assert(!string_insert_string(s, NULL, 0));
    string_free(s);
}

void string_insert_string_resizes_string_if_needed() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(sub, "Hello, this is a long string that will require resizing"));
    assert(string_insert_string(s, sub, 0));
    assert(string_compare(s, sub));
    string_free(s);
    string_free(sub);
}

void string_find_last_of_returns_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_last_of(s, 'o') == 8);
    string_free(s);
}

void string_find_last_of_returns_max_size() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_last_of(s, 'z') == SIZE_MAX);
    string_free(s);
}

void string_find_last_not_of_returns_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_last_not_of(s, '!') == 11);
    string_free(s);
}

void string_find_first_from_returns_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_first_from(s, 'o', 5) == 8);
    string_free(s);
}

void string_find_first_of_returns_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_first_of(s, 'o') == 4);
    string_free(s);
}

void string_find_first_not_of_returns_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_first_not_of(s, 'H') == 1);
    string_free(s);
}


void string_find_substr_from_finds_correct_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "World"));
    assert(string_find_substr_from(s, sub, 0) == 7);
    string_free(s);
    string_free(sub);
}

void string_find_substr_from_returns_max_size() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "Planet"));
    assert(string_find_substr_from(s, sub, 0) == SIZE_MAX);
    string_free(s);
    string_free(sub);
}

void string_find_substr_finds_correct_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "World"));
    assert(string_find_substr(s, sub) == 7);
    string_free(s);
    string_free(sub);
}

void string_rfind_substr_finds_correct_index() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World! World!"));
    assert(string_append_cstr(sub, "World"));
    assert(string_rfind_substr(s, sub) == 14);
    string_free(s);
    string_free(sub);
}

void string_find_substr_cstr_from_finds_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_substr_cstr_from(s, "World", 0) == 7);
    string_free(s);
}

void string_rfind_substr_cstr_finds_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World! World!"));
    assert(string_rfind_substr_cstr(s, "World") == 14);
    string_free(s);
}

void string_contains_char_returns_true_when_char_exists() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello"));
    assert(string_contains_char(s, 'e'));
    string_free(s);
}

void string_contains_char_returns_false_when_char_does_not_exist() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello"));
    assert(!string_contains_char(s, 'z'));
    string_free(s);
}

void string_contains_char_returns_false_for_empty_string() {
    lite_string *s = string_new();
    assert(!string_contains_char(s, 'e'));
    string_free(s);
}

void string_find_substr_cstr_returns_correct_index() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_substr_cstr(s, "World") == 7);
    string_free(s);
}

void string_find_substr_cstr_returns_max_size() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_find_substr_cstr(s, "Planet") == SIZE_MAX);
    string_free(s);
}

void string_contains_returns_true_when_substring_exists() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "World"));
    assert(string_contains(s, sub));
    string_free(s);
    string_free(sub);
}

void string_contains_returns_false_when_substring_does_not_exist() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "Planet"));
    assert(!string_contains(s, sub));
    string_free(s);
    string_free(sub);
}

void string_starts_with_returns_true_when_substring_is_prefix() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "Hello"));
    assert(string_starts_with(s, sub));
    string_free(s);
    string_free(sub);
}

void string_starts_with_returns_false_when_substring_is_not_prefix() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "World"));
    assert(!string_starts_with(s, sub));
    string_free(s);
    string_free(sub);
}

void string_ends_with_returns_true_when_substring_is_suffix() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "World!"));
    assert(string_ends_with(s, sub));
    string_free(s);
    string_free(sub);
}

void string_ends_with_returns_false_when_substring_is_not_suffix() {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_append_cstr(sub, "Hello"));
    assert(!string_ends_with(s, sub));
    string_free(s);
    string_free(sub);
}

void string_ends_with_cstr_returns_true_when_cstr_is_suffix() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_ends_with_cstr(s, "World!"));
    string_free(s);
}

void string_ends_with_cstr_returns_false_when_cstr_is_not_suffix() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(!string_ends_with_cstr(s, "Hello"));
    string_free(s);
}

void string_shrink_reduces_size_correctly() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_shrink(s, 5));
    assert(s->size == 5);
    string_free(s);
}

void string_shrink_does_nothing_when_new_size_is_greater() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(!string_shrink(s, 20));
    assert(s->size == 13);
    string_free(s);
}

void string_shrink_to_fit_reduces_capacity_to_size() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello, World!"));
    assert(string_shrink_to_fit(s));
    assert(s->capacity == s->size);
    string_free(s);
}

void string_shrink_to_fit_does_nothing_when_size_is_capacity() {
    lite_string *s = string_new();
    assert(string_append_cstr(s, "Hello"));
    assert(string_shrink_to_fit(s));
    assert(s->capacity == s->size);
    string_free(s);
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

    string_insert_cstr_inserts_at_valid_index();
    string_insert_cstr_does_not_insert_at_invalid_index();
    string_insert_cstr_inserts_in_middle_of_string();
    string_insert_cstr_does_not_insert_null_cstr();
    string_insert_cstr_resizes_string_if_needed();

    string_back_returns_last_character_for_non_empty_string();
    string_back_returns_null_character_for_empty_string();
    string_front_returns_first_character_for_non_empty_string();
    string_front_returns_null_character_for_empty_string();

    string_insert_range_inserts_at_valid_index();
    string_insert_range_does_not_insert_at_invalid_index();
    string_insert_range_inserts_in_middle_of_string();
    string_insert_range_does_not_insert_null_substring();
    string_insert_range_resizes_string_if_needed();

    string_insert_string_inserts_at_valid_index();
    string_insert_string_does_not_insert_at_invalid_index();
    string_insert_string_inserts_in_middle_of_string();
    string_insert_string_does_not_insert_null_substring();
    string_insert_string_resizes_string_if_needed();

    string_find_last_of_returns_correct_index();
    string_find_last_of_returns_max_size();
    string_find_last_not_of_returns_correct_index();
    string_find_first_from_returns_correct_index();
    string_find_first_of_returns_correct_index();
    string_find_first_not_of_returns_correct_index();

    string_find_substr_from_finds_correct_index();
    string_find_substr_from_returns_max_size();
    string_find_substr_finds_correct_index();
    string_rfind_substr_finds_correct_index();
    string_find_substr_cstr_from_finds_correct_index();
    string_rfind_substr_cstr_finds_correct_index();

    string_contains_char_returns_true_when_char_exists();
    string_contains_char_returns_false_when_char_does_not_exist();
    string_contains_char_returns_false_for_empty_string();

    string_find_substr_cstr_returns_correct_index();
    string_find_substr_cstr_returns_max_size();
    string_contains_returns_true_when_substring_exists();
    string_contains_returns_false_when_substring_does_not_exist();
    string_starts_with_returns_true_when_substring_is_prefix();
    string_starts_with_returns_false_when_substring_is_not_prefix();
    string_ends_with_returns_true_when_substring_is_suffix();
    string_ends_with_returns_false_when_substring_is_not_suffix();

    string_ends_with_cstr_returns_true_when_cstr_is_suffix();
    string_ends_with_cstr_returns_false_when_cstr_is_not_suffix();
    string_shrink_reduces_size_correctly();
    string_shrink_does_nothing_when_new_size_is_greater();
    string_shrink_to_fit_reduces_capacity_to_size();
    string_shrink_to_fit_does_nothing_when_size_is_capacity();

    return 0;
}
