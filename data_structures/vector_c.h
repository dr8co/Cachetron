#pragma once
#if __cplusplus
#define restrict
extern "C" {
#endif


/**
 * @brief A structure representing a dynamic array in C.
 */
struct vector_c {
    void *data;          ///< A void pointer to the data the vector holds. This allows the vector to hold any type of data.
    size_t size;         ///< The current number of elements in the vector.
    size_t capacity;     ///< The total number of elements the vector can currently hold before it needs to resize.
    size_t element_size; ///< The size of each element in the vector.
};

typedef struct vector_c vector_c;

vector_c *vector_new(size_t elem_size);

void vector_free(vector_c *restrict v);

bool vector_append(vector_c *restrict v, const void *restrict elems, size_t count);

bool vector_push_back(vector_c *restrict v, const void *restrict elem);

void *vector_at(const vector_c *restrict v, size_t index);

void vector_pop_back(vector_c *restrict v);

bool vector_empty(const vector_c *restrict v);

bool vector_erase(vector_c *restrict v, size_t index);

bool vector_insert(vector_c *restrict v, size_t index, const void *restrict elem);

bool vector_set(vector_c *restrict v, size_t index, const void *restrict elem);

bool vector_set_range(vector_c *restrict v, const void *restrict elem, size_t index, size_t count);

void *vector_data(const vector_c *restrict v);

size_t vector_size(const vector_c *restrict v);

size_t vector_capacity(const vector_c *restrict v);

void vector_clear(vector_c *restrict v);

/*/////////////////////// ptr_vector ///////////////////////*/

/**
 * @brief A structure representing a dynamic array of pointers in C.
 */
struct ptr_vector {
    void **data;     ///< A double void pointer to the data the vector holds. This allows the vector to hold any type of pointer.
    size_t size;     ///< The current number of elements in the vector.
    size_t capacity; ///< The total number of elements the vector can currently hold before it needs to resize.
};

typedef struct ptr_vector ptr_vector;

ptr_vector *ptr_vector_new();

void ptr_vector_free(ptr_vector *restrict v);

bool ptr_vector_push_back(ptr_vector *restrict v, void *restrict elem);

void *ptr_vector_at(const ptr_vector *restrict v, size_t index);

void ptr_vector_pop_back(ptr_vector *restrict v);

bool ptr_vector_empty(const ptr_vector *restrict v);

bool ptr_vector_erase(ptr_vector *restrict v, size_t index);

bool ptr_vector_set(const ptr_vector *restrict v, size_t index, void *restrict elem);

bool ptr_vector_expand(ptr_vector *restrict v, size_t new_size);

void **ptr_vector_data(const ptr_vector *restrict v);

size_t ptr_vector_size(const ptr_vector *restrict v);

size_t ptr_vector_capacity(const ptr_vector *restrict v);

void ptr_vector_clear(ptr_vector *restrict v);

#if __cplusplus
    }
#endif
