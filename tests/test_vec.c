#include <assert.h>
#include "../vector.h"

#if !(__GNUC__ >= 13 || __clang_major__ >= 19)
#define constexpr const
#endif

void vector_new_creates_empty_vector() {
    vector *v = vector_new(sizeof(int));
    assert(v != nullptr);
    assert(vector_size(v) == 0);
    assert(vector_capacity(v) == 16);
    vector_free(v);
}

void vector_push_back_increases_size() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    assert(vector_size(v) == 1);
    vector_free(v);
}

void vector_push_back_stores_correct_value() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    assert(*(int *)vector_at(v, 0) == value);
    vector_free(v);
}

void vector_pop_back_decreases_size() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    vector_pop_back(v);
    assert(vector_size(v) == 0);
    vector_free(v);
}

void vector_append_increases_size() {
    vector *v = vector_new(sizeof(int));
    constexpr int values[] = {1, 2, 3, 4, 5};
    vector_append(v, values, 5);
    assert(vector_size(v) == 5);
    vector_free(v);
}

void vector_append_stores_correct_values() {
    vector *v = vector_new(sizeof(int));
    constexpr int values[] = {1, 2, 3, 4, 5};
    vector_append(v, values, 5);
    for (size_t i = 0; i < 5; ++i) {
        assert(*(int *)vector_at(v, i) == values[i]);
    }
    vector_free(v);
}

void vector_insert_increases_size() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_insert(v, 0, &value);
    assert(vector_size(v) == 1);
    vector_free(v);
}

void vector_insert_stores_correct_value() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_insert(v, 0, &value);
    assert(*(int *)vector_at(v, 0) == value);
    vector_free(v);
}

void vector_erase_decreases_size() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    vector_erase(v, 0);
    assert(vector_size(v) == 0);
    vector_free(v);
}

void vector_resize_expand_increases_size() {
    vector *v = vector_new(sizeof(int));
    vector_resize_expand(v, 20);
    assert(vector_size(v) == 20);
    vector_free(v);
}

void vector_clear_resets_size() {
    vector *v = vector_new(sizeof(int));
    constexpr int value = 5;
    vector_push_back(v, &value);
    vector_clear(v);
    assert(vector_size(v) == 0);
    vector_free(v);
}

int main() {
    vector_new_creates_empty_vector();
    vector_push_back_increases_size();
    vector_push_back_stores_correct_value();
    vector_pop_back_decreases_size();
    vector_append_increases_size();
    vector_append_stores_correct_values();
    vector_insert_increases_size();
    vector_insert_stores_correct_value();
    vector_erase_decreases_size();
    vector_resize_expand_increases_size();
    vector_clear_resets_size();
    return 0;
}
