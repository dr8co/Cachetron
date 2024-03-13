#include <assert.h>
#include "../string.h"

void string_new_creates_empty_string() {
    string *s = string_new();
    assert(s != nullptr);
    assert(string_length(s) == 0);
    assert(string_capacity(s) == 16);
    string_free(s);
}

void string_push_back_increases_size() {
    string *s = string_new();
    string_push_back(s, 'a');
    assert(string_length(s) == 1);
    string_free(s);
}

void string_push_back_stores_correct_value() {
    string *s = string_new();
    string_push_back(s, 'a');
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_pop_back_decreases_size() {
    string *s = string_new();
    string_push_back(s, 'a');
    string_pop_back(s);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_insert_increases_size() {
    string *s = string_new();
    string_insert(s, 0, 'a');
    assert(string_length(s) == 1);
    string_free(s);
}

void string_insert_stores_correct_value() {
    string *s = string_new();
    string_insert(s, 0, 'a');
    assert(string_at(s, 0) == 'a');
    string_free(s);
}

void string_erase_decreases_size() {
    string *s = string_new();
    string_push_back(s, 'a');
    string_erase(s, 0);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_clear_resets_size() {
    string *s = string_new();
    string_push_back(s, 'a');
    string_clear(s);
    assert(string_length(s) == 0);
    string_free(s);
}

void string_append_increases_size() {
    string *s1 = string_new();
    string *s2 = string_new();
    string_push_back(s1, 'a');
    string_push_back(s2, 'b');
    string_append(s1, s2);
    assert(string_length(s1) == 2);
    string_free(s1);
    string_free(s2);
}

void string_append_stores_correct_values() {
    string *s1 = string_new();
    string *s2 = string_new();
    string_push_back(s1, 'a');
    string_push_back(s2, 'b');
    string_append(s1, s2);
    assert(string_at(s1, 0) == 'a');
    assert(string_at(s1, 1) == 'b');
    string_free(s1);
    string_free(s2);
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
    return 0;
}
