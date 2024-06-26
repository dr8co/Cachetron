#include <gtest/gtest.h>
#include <cstdlib>
#include "../data_structures/string/lite_string.h"

TEST(LiteStringTest, NewCreatesEmptyString) {
    lite_string *s = string_new();
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(string_length(s), 0);
    EXPECT_EQ(string_capacity(s), 16);
    string_free(s);
}

TEST(LiteStringTest, PushBackIncreasesSize) {
    lite_string *s = string_new();
    EXPECT_TRUE(string_push_back(s, 'a'));
    EXPECT_EQ(string_length(s), 1);
    string_free(s);
}

TEST(LiteStringTest, PushBackStoresCorrectValue) {
    lite_string *s = string_new();
    EXPECT_TRUE(string_push_back(s, 'a'));
    EXPECT_EQ(string_at(s, 0), 'a');
    string_free(s);
}

TEST(LiteStringTest, PopBackDecreasesSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    string_pop_back(s);
    EXPECT_EQ(string_length(s), 0);
    string_free(s);
}

TEST(LiteStringTest, InsertIncreasesSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_insert(s, 0, 'a'));
    EXPECT_EQ(string_length(s), 1);
    string_free(s);
}

TEST(LiteStringTest, InsertStoresCorrectValue) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_insert(s, 0, 'a'));
    EXPECT_EQ(string_at(s, 0), 'a');
    string_free(s);
}

TEST(LiteStringTest, EraseDecreasesSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    ASSERT_TRUE(string_erase(s, 0));
    EXPECT_EQ(string_length(s), 0);
    string_free(s);
}

TEST(LiteStringTest, ClearResetsSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    string_clear(s);
    EXPECT_EQ(string_length(s), 0);
    string_free(s);
}

TEST(LiteStringTest, AppendIncreasesSize) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    ASSERT_TRUE(string_push_back(s1, 'a'));
    ASSERT_TRUE(string_push_back(s2, 'b'));
    string_append(s1, s2);
    EXPECT_EQ(string_length(s1), 2);
    string_free(s1);
    string_free(s2);
}

TEST(LiteStringTest, AppendStoresCorrectValues) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    ASSERT_TRUE(string_push_back(s1, 'a'));
    ASSERT_TRUE(string_push_back(s2, 'b'));
    string_append(s1, s2);
    EXPECT_EQ(string_at(s1, 0), 'a');
    EXPECT_EQ(string_at(s1, 1), 'b');
    string_free(s1);
    string_free(s2);
}

TEST(LiteStringTest, PushBackIncreasesCapacityWhenNeeded) {
    lite_string *s = string_new();
    for (int i = 0; i < 17; ++i) {
        ASSERT_TRUE(string_push_back(s, 'a'));
    }
    EXPECT_GT(string_capacity(s), (size_t) 16);
    string_free(s);
}

TEST(LiteStringTest, PopBackDoesNotDecreaseCapacity) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    const size_t old_capacity = string_capacity(s);
    string_pop_back(s);
    EXPECT_EQ(string_capacity(s), old_capacity);
    string_free(s);
}

TEST(LiteStringTest, InsertIncreasesCapacityWhenNeeded) {
    lite_string *s = string_new();
    for (int i = 0; i < 17; ++i) {
        ASSERT_TRUE(string_insert(s, 0, 'a'));
    }
    EXPECT_GT(string_capacity(s), (size_t) 16);
    string_free(s);
}

TEST(LiteStringTest, EraseDoesNotDecreaseCapacity) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    const size_t old_capacity = string_capacity(s);
    ASSERT_TRUE(string_erase(s, 0));
    EXPECT_EQ(string_capacity(s), old_capacity);
    string_free(s);
}

TEST(LiteStringTest, ClearDoesNotDecreaseCapacity) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    const size_t old_capacity = string_capacity(s);
    string_clear(s);
    EXPECT_EQ(string_capacity(s), old_capacity);
    string_free(s);
}

TEST(LiteStringTest, AppendIncreasesCapacityWhenNeeded) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    for (int i = 0; i < 9; ++i) {
        ASSERT_TRUE(string_push_back(s1, 'a'));
        ASSERT_TRUE(string_push_back(s2, 'b'));
    }
    ASSERT_TRUE(string_append(s1, s2));
    EXPECT_GT(string_capacity(s1), (size_t) 16);
    string_free(s1);
    string_free(s2);
}

TEST(LiteStringTest, SetStoresCorrectValue) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    string_set(s, 0, 'b');
    EXPECT_EQ(string_at(s, 0), 'b');
    string_free(s);
}

TEST(LiteStringTest, SubstrReturnsCorrectString) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    ASSERT_TRUE(string_push_back(s, 'b'));
    ASSERT_TRUE(string_push_back(s, 'c'));
    lite_string *s2 = string_substr(s, 1, 2);
    ASSERT_NE(s2, nullptr);
    EXPECT_EQ(string_length(s2), 2);
    EXPECT_EQ(string_at(s2, 0), 'b');
    EXPECT_EQ(string_at(s2, 1), 'c');
    string_free(s);
    string_free(s2);
}

TEST(LiteStringTest, ConcatReturnsCorrectString) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    ASSERT_TRUE(string_push_back(s1, 'a'));
    ASSERT_TRUE(string_push_back(s2, 'b'));
    lite_string *s3 = string_concat(s1, s2);
    ASSERT_EQ(string_length(s3), 2);
    EXPECT_EQ(string_at(s3, 0), 'a');
    EXPECT_EQ(string_at(s3, 1), 'b');
    string_free(s1);
    string_free(s2);
    string_free(s3);
}

TEST(LiteStringTest, AppendCStrStoresCorrectValues) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    string_append_cstr(s, "bc");
    ASSERT_EQ(string_length(s), 3);
    ASSERT_EQ(string_at(s, 1), 'b');
    ASSERT_EQ(string_at(s, 2), 'c');
    string_free(s);
}

TEST(LiteStringTest, InsertCStrStoresCorrectValues) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    ASSERT_TRUE(string_push_back(s, 'b'));

    ASSERT_TRUE(string_insert_cstr(s, "cd", 1));
    ASSERT_EQ(string_length(s), 4);
    ASSERT_EQ(string_at(s, 1), 'c');
    ASSERT_EQ(string_at(s, 2), 'd');
    string_free(s);
}

TEST(LiteStringTest, CStrReturnsCorrectCStr) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    ASSERT_TRUE(string_push_back(s, 'b'));
    ASSERT_TRUE(string_push_back(s, 'c'));

    char *cstr = string_cstr(s);
    ASSERT_EQ(cstr[3], '\0');

    ASSERT_STREQ(cstr, "abc");
    free(cstr);
    string_free(s);
}

TEST(LiteStringTest, CompareCStrReturnsTrueForEqualStrings) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    ASSERT_TRUE(string_push_back(s, 'b'));
    ASSERT_TRUE(string_compare_cstr(s, "ab"));
    string_free(s);
}

TEST(LiteStringTest, EmptyReturnsFalseForNonEmptyString) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    EXPECT_FALSE(string_empty(s));
    string_free(s);
}

TEST(LiteStringTest, EmptyReturnsTrueForEmptyString) {
    lite_string *s = string_new();
    EXPECT_TRUE(string_empty(s));
    string_free(s);
}

TEST(LiteStringTest, AtReturnsCorrectValue) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    EXPECT_EQ(string_at(s, 0), 'a');
    string_free(s);
}

TEST(LiteStringTest, EraseRemovesCorrectValue) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));
    ASSERT_TRUE(string_push_back(s, 'b'));
    string_erase(s, 0);
    EXPECT_EQ(string_at(s, 0), 'b');
    string_free(s);
}

TEST(LiteStringTest, CompareWorksCorrectly) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    ASSERT_TRUE(string_push_back(s1, 'a'));
    ASSERT_TRUE(string_push_back(s2, 'a'));
    EXPECT_TRUE(string_compare(s1, s2));
    string_free(s1);
    string_free(s2);
}

TEST(LiteStringTest, SwapSwapsContentsCorrectly) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();

    ASSERT_TRUE(string_push_back(s1, 'a'));
    ASSERT_TRUE(string_push_back(s1, 'b'));
    ASSERT_TRUE(string_push_back(s1, 'c'));
    ASSERT_TRUE(string_push_back(s1, 'z'));
    const size_t size1 = string_length(s1);

    ASSERT_TRUE(string_push_back(s2, 'd'));
    ASSERT_TRUE(string_push_back(s2, 'e'));
    ASSERT_TRUE(string_push_back(s2, 'f'));
    const size_t size2 = string_length(s2);

    ASSERT_TRUE(string_swap(s1, s2));
    EXPECT_EQ(string_at(s1, 0), 'd');
    EXPECT_EQ(string_at(s1, 2), 'f');

    EXPECT_EQ(string_at(s2, 0), 'a');
    EXPECT_EQ(string_at(s2, 3), 'z');

    EXPECT_EQ(size1, string_length(s2));
    EXPECT_EQ(size2, string_length(s1));

    string_free(s1);
    string_free(s2);
}

TEST(LiteStringTest, SwapHandlesEmptyStrings) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    ASSERT_TRUE(string_push_back(s1, 'a'));

    ASSERT_TRUE(string_swap(s1, s2));
    EXPECT_TRUE(string_empty(s1));
    EXPECT_EQ(string_at(s2, 0), 'a');

    string_free(s1);
    string_free(s2);
}

TEST(LiteStringTest, SwapReturnsFalseForNullptr) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'a'));

    EXPECT_FALSE(string_swap(s, nullptr));
    EXPECT_FALSE(string_swap(nullptr, s));
    EXPECT_FALSE(string_swap(nullptr, nullptr));

    string_free(s);
}

TEST(LiteStringTest, FunctionsDoNotCrashForNullptr) {
    string_free(nullptr);
    EXPECT_FALSE(string_push_back(nullptr, 'a'));
    EXPECT_EQ(string_at(nullptr, 10), '\0');
    string_pop_back(nullptr);

    EXPECT_TRUE(string_empty(nullptr));
    EXPECT_FALSE(string_erase(nullptr, 17));
    EXPECT_FALSE(string_compare(nullptr, nullptr));
    EXPECT_EQ(string_length(nullptr), 0);
    EXPECT_EQ(string_capacity(nullptr), 0);
    string_clear(nullptr);

    EXPECT_FALSE(string_insert(nullptr, 1000, 'm'));
    string_set(nullptr, 0, 'x');
    EXPECT_EQ(string_substr(nullptr, 12, 30), nullptr);
    EXPECT_EQ(string_concat(nullptr, nullptr), nullptr);
    EXPECT_FALSE(string_append(nullptr, nullptr));

    EXPECT_EQ(string_cstr(nullptr), nullptr);
    EXPECT_FALSE(string_compare_cstr(nullptr, nullptr));
    EXPECT_FALSE(string_insert_cstr(nullptr, nullptr, 5));

    EXPECT_FALSE(string_insert_range(nullptr, nullptr, 99, 1102));
    EXPECT_FALSE(string_insert_string(nullptr, nullptr, 601));

    EXPECT_EQ(string_find_last_of(nullptr, 'a'), SIZE_MAX);
    EXPECT_EQ(string_find_last_not_of(nullptr, 'a'), SIZE_MAX);

    EXPECT_EQ(string_find_first_from(nullptr, 'a', 10), SIZE_MAX);
    EXPECT_EQ(string_find_first_of(nullptr, 'a'), SIZE_MAX);
    EXPECT_EQ(string_find_first_not_of(nullptr, 'a'), SIZE_MAX);

    EXPECT_EQ(string_find_substr_from(nullptr, nullptr, 17), SIZE_MAX);
    EXPECT_EQ(string_find_substr(nullptr, nullptr), SIZE_MAX);

    EXPECT_EQ(string_rfind_substr(nullptr, nullptr), SIZE_MAX);
    EXPECT_EQ(string_find_substr_cstr_from(nullptr, nullptr, 1900), SIZE_MAX);
    EXPECT_EQ(string_rfind_substr_cstr(nullptr, nullptr), SIZE_MAX);
    EXPECT_EQ(string_find_substr_cstr(nullptr, nullptr), SIZE_MAX);

    EXPECT_FALSE(string_contains_char(nullptr, 'a'));
    EXPECT_FALSE(string_contains(nullptr, nullptr));

    EXPECT_FALSE(string_starts_with(nullptr, nullptr));
    EXPECT_FALSE(string_ends_with(nullptr, nullptr));
    EXPECT_FALSE(string_ends_with_cstr(nullptr, nullptr));

    EXPECT_FALSE(string_shrink(nullptr, 54));
    EXPECT_FALSE(string_shrink_to_fit(nullptr));
}

TEST(LiteStringTest, InsertCStrInsertsAtValidIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_insert_cstr(s, "Hello", 0));
    EXPECT_STREQ(s->data, "Hello");
    string_free(s);
}

TEST(LiteStringTest, InsertCStrDoesNotInsertAtInvalidIndex) {
    lite_string *s = string_new();
    EXPECT_FALSE(string_insert_cstr(s, "Hello", 5));
    string_free(s);
}

TEST(LiteStringTest, InsertCStrInsertsInMiddleOfString) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_insert_cstr(s, "Hello", 0));
    ASSERT_TRUE(string_insert_cstr(s, " world", 5));
    EXPECT_STREQ(s->data, "Hello world");
    string_free(s);
}

TEST(LiteStringTest, InsertCStrDoesNotInsertNullCStr) {
    lite_string *s = string_new();
    EXPECT_FALSE(string_insert_cstr(s, nullptr, 0));
    string_free(s);
}

TEST(LiteStringTest, InsertCStrResizesStringIfNeeded) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_insert_cstr(s, "Hello, this is a long string that will require resizing", 0));
    EXPECT_STREQ(s->data, "Hello, this is a long string that will require resizing");
    string_free(s);
}

TEST(LiteStringTest, BackReturnsLastCharacterForNonEmptyString) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'H'));
    ASSERT_TRUE(string_push_back(s, 'i'));
    EXPECT_EQ(string_back(s), 'i');
    string_free(s);
}

TEST(LiteStringTest, BackReturnsNullCharacterForEmptyString) {
    lite_string *s = string_new();
    EXPECT_EQ(string_back(s), '\0');
    string_free(s);
}

TEST(LiteStringTest, FrontReturnsFirstCharacterForNonEmptyString) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_push_back(s, 'H'));
    ASSERT_TRUE(string_push_back(s, 'i'));
    EXPECT_EQ(string_front(s), 'H');
    string_free(s);
}

TEST(LiteStringTest, FrontReturnsNullCharacterForEmptyString) {
    lite_string *s = string_new();
    EXPECT_EQ(string_front(s), '\0');
    string_free(s);
}

TEST(LiteStringTest, InsertRangeInsertsAtValidIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(sub, "Hello"));
    ASSERT_TRUE(string_insert_range(s, sub, 0, 5));
    const char *cstr = string_cstr(s);
    EXPECT_STREQ(cstr, "Hello");
    string_free(s);
    string_free(sub);
    free((void *) cstr);
}

TEST(LiteStringTest, InsertRangeDoesNotInsertAtInvalidIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(sub, "Hello"));
    EXPECT_FALSE(string_insert_range(s, sub, 5, 5));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, InsertRangeInsertsInMiddleOfString) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Helo"));
    ASSERT_TRUE(string_append_cstr(sub, "l"));
    ASSERT_TRUE(string_insert_range(s, sub, 2, 1));
    const char *cstr = string_cstr(s);
    EXPECT_STREQ(cstr, "Hello");
    string_free(s);
    string_free(sub);
    free((void *) cstr);
}

TEST(LiteStringTest, InsertRangeDoesNotInsertNullSubstring) {
    lite_string *s = string_new();
    EXPECT_FALSE(string_insert_range(s, nullptr, 0, 0));
    string_free(s);
}

TEST(LiteStringTest, InsertRangeResizesStringIfNeeded) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(sub, "Hello, this is a long string that will require resizing"));
    ASSERT_TRUE(string_insert_range(s, sub, 0, sub->size));
    EXPECT_TRUE(string_compare(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, InsertStringInsertsAtValidIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(sub, "Hello"));
    ASSERT_TRUE(string_insert_string(s, sub, 0));
    EXPECT_TRUE(string_compare_cstr(s, "Hello"));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, InsertStringDoesNotInsertAtInvalidIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(sub, "Hello"));
    EXPECT_FALSE(string_insert_string(s, sub, 5));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, InsertStringInsertsInMiddleOfString) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Helo"));
    ASSERT_TRUE(string_append_cstr(sub, "l"));
    ASSERT_TRUE(string_insert_string(s, sub, 2));
    const char *cstr = string_cstr(s);
    EXPECT_STREQ(cstr, "Hello");
    string_free(s);
    string_free(sub);
    free((void *) cstr);
}

TEST(LiteStringTest, InsertStringDoesNotInsertNullSubstring) {
    lite_string *s = string_new();
    EXPECT_FALSE(string_insert_string(s, nullptr, 0));
    string_free(s);
}

TEST(LiteStringTest, InsertStringResizesStringIfNeeded) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(sub, "Hello, this is a long string that will require resizing"));
    ASSERT_TRUE(string_insert_string(s, sub, 0));
    EXPECT_TRUE(string_compare(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, FindLastOfReturnsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_last_of(s, 'o'), 8);
    string_free(s);
}

TEST(LiteStringTest, FindLastOfReturnsMaxSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_last_of(s, 'z'), SIZE_MAX);
    string_free(s);
}

TEST(LiteStringTest, FindLastNotOfReturnsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_last_not_of(s, '!'), 11);
    string_free(s);
}

TEST(LiteStringTest, FindFirstFromReturnsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_first_from(s, 'o', 5), 8);
    string_free(s);
}

TEST(LiteStringTest, FindFirstOfReturnsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_first_of(s, 'o'), 4);
    string_free(s);
}

TEST(LiteStringTest, FindFirstNotOfReturnsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_first_not_of(s, 'H'), 1);
    string_free(s);
}

TEST(LiteStringTest, FindSubstrFromFindsCorrectIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "World"));
    EXPECT_EQ(string_find_substr_from(s, sub, 0), 7);
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, FindSubstrFromReturnsMaxSize) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "Planet"));
    EXPECT_EQ(string_find_substr_from(s, sub, 0), SIZE_MAX);
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, FindSubstrFindsCorrectIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "World"));
    EXPECT_EQ(string_find_substr(s, sub), 7);
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, RFindSubstrFindsCorrectIndex) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World! World!"));
    ASSERT_TRUE(string_append_cstr(sub, "World"));
    EXPECT_EQ(string_rfind_substr(s, sub), 14);
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, FindSubstrCStrFromFindsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_substr_cstr_from(s, "World", 0), 7);
    string_free(s);
}

TEST(LiteStringTest, RFindSubstrCStrFindsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World! World!"));
    EXPECT_EQ(string_rfind_substr_cstr(s, "World"), 14);
    string_free(s);
}

TEST(LiteStringTest, ContainsCharReturnsTrueWhenCharExists) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello"));
    EXPECT_TRUE(string_contains_char(s, 'e'));
    string_free(s);
}

TEST(LiteStringTest, ContainsCharReturnsFalseWhenCharDoesNotExist) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello"));
    EXPECT_FALSE(string_contains_char(s, 'z'));
    string_free(s);
}

TEST(LiteStringTest, ContainsCharReturnsFalseForEmptyString) {
    lite_string *s = string_new();
    EXPECT_FALSE(string_contains_char(s, 'e'));
    string_free(s);
}

TEST(LiteStringTest, FindSubstrCStrReturnsCorrectIndex) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_substr_cstr(s, "World"), 7);
    string_free(s);
}

TEST(LiteStringTest, FindSubstrCStrReturnsMaxSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_EQ(string_find_substr_cstr(s, "Planet"), SIZE_MAX);
    string_free(s);
}

TEST(LiteStringTest, ContainsReturnsTrueWhenSubstringExists) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "World"));
    EXPECT_TRUE(string_contains(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, ContainsReturnsFalseWhenSubstringDoesNotExist) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "Planet"));
    EXPECT_FALSE(string_contains(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, StartsWithReturnsTrueWhenSubstringIsPrefix) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "Hello"));
    EXPECT_TRUE(string_starts_with(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, StartsWithReturnsFalseWhenSubstringIsNotPrefix) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "World"));
    EXPECT_FALSE(string_starts_with(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, EndsWithReturnsTrueWhenSubstringIsSuffix) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "World!"));
    EXPECT_TRUE(string_ends_with(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, EndsWithReturnsFalseWhenSubstringIsNotSuffix) {
    lite_string *s = string_new();
    lite_string *sub = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_append_cstr(sub, "Hello"));
    EXPECT_FALSE(string_ends_with(s, sub));
    string_free(s);
    string_free(sub);
}

TEST(LiteStringTest, EndsWithCStrReturnsTrueWhenCStrIsSuffix) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_TRUE(string_ends_with_cstr(s, "World!"));
    string_free(s);
}

TEST(LiteStringTest, EndsWithCStrReturnsFalseWhenCStrIsNotSuffix) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    EXPECT_FALSE(string_ends_with_cstr(s, "Hello"));
    string_free(s);
}

TEST(LiteStringTest, ShrinkReducesSizeCorrectly) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_shrink(s, 5));
    EXPECT_EQ(s->size, 5);
    string_free(s);
}

TEST(LiteStringTest, ShrinkDoesNothingWhenNewSizeIsGreater) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_FALSE(string_shrink(s, 20));
    EXPECT_EQ(s->size, 13);
    string_free(s);
}

TEST(LiteStringTest, ShrinkToFitReducesCapacityToSize) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello, World!"));
    ASSERT_TRUE(string_shrink_to_fit(s));
    EXPECT_EQ(s->capacity, s->size);
    string_free(s);
}

TEST(LiteStringTest, ShrinkToFitDoesNothingWhenSizeIsCapacity) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_append_cstr(s, "Hello"));
    ASSERT_TRUE(string_shrink_to_fit(s));
    EXPECT_EQ(s->capacity, s->size);
    string_free(s);
}

// Test copying a non-empty string to a buffer
TEST(LiteStringTest, CopyingStringToBufferStoresCorrectValue) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    char buf[50];
    ASSERT_TRUE(string_copy_buffer(s, buf));
    ASSERT_STREQ(buf, "Hello, World!");
    string_free(s);
}

// Test copying an empty string to a buffer
TEST(LiteStringTest, CopyingEmptyStringToBufferStoresNullCharacter) {
    lite_string *s = string_new();
    char buf[50];
    ASSERT_FALSE(string_copy_buffer(s, buf));
    string_free(s);
}

// Test copying a string to a null buffer
TEST(LiteStringTest, CopyingStringToNullBufferFails) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_FALSE(string_copy_buffer(s, nullptr));
    string_free(s);
}

// Test copying a null string to a buffer
TEST(LiteStringTest, CopyingNullStringToBufferFails) {
    char buf[50];
    ASSERT_FALSE(string_copy_buffer(nullptr, buf));
}

// Copying a non-empty string to another string
TEST(LiteStringTest, CopyingStringStoresCorrectValue) {
    lite_string *src = string_new();
    lite_string *dest = string_new();
    string_append_cstr(src, "Hello, World!");
    ASSERT_TRUE(string_copy(src, dest));
    ASSERT_TRUE(string_compare(src, dest));
    string_free(src);
    string_free(dest);
}

// Copying an empty string to another string
TEST(LiteStringTest, CopyingEmptyStringStoresCorrectValue) {
    lite_string *src = string_new();
    lite_string *dest = string_new();
    ASSERT_TRUE(string_copy(src, dest));
    ASSERT_TRUE(string_compare(src, dest));
    string_free(src);
    string_free(dest);
}

// Copying a string to a null string
TEST(LiteStringTest, CopyingStringToNullStringFails) {
    lite_string *src = string_new();
    string_append_cstr(src, "Hello, World!");
    ASSERT_FALSE(string_copy(src, nullptr));
    string_free(src);
}

// Copying a null string to a string
TEST(LiteStringTest, CopyingNullStringFails) {
    lite_string *dest = string_new();
    ASSERT_FALSE(string_copy(nullptr, dest));
    string_free(dest);
}

// Comparing a string with a C-string for equality, ignoring case
TEST(LiteStringTest, CaseCompareCstrReturnsCorrectValue) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_TRUE(string_case_compare_cstr(s, "HELLO, WORLD!"));
    ASSERT_FALSE(string_case_compare_cstr(s, "HELLO, UNIVERSE!"));
    string_free(s);
}

// Comparing a string with a null C-string
TEST(LiteStringTest, CaseCompareWithNullCStrReturnsFalse) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_FALSE(string_case_compare_cstr(s, nullptr));
    string_free(s);
}

// Comparing a null string with a C-string
TEST(LiteStringTest, CaseCompareCstrNullStringReturnsFalse) {
    ASSERT_FALSE(string_case_compare_cstr(nullptr, "HELLO, WORLD!"));
}

// String contains the C-string
TEST(LiteStringTest, ContainsCstrReturnsCorrectValue) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_TRUE(string_contains_cstr(s, "World"));
    ASSERT_FALSE(string_contains_cstr(s, "Universe"));
    string_free(s);
}

// String is null
TEST(LiteStringTest, ContainsCstrNullReturnsFalse) {
    ASSERT_FALSE(string_contains_cstr(nullptr, "World"));
}

// C-string is null
TEST(LiteStringTest, ContainsCstrNullCStrReturnsFalse) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_FALSE(string_contains_cstr(s, nullptr));
    string_free(s);
}

// String and C-string are both null
TEST(LiteStringTest, ContainsCstrNullStringAndCStrReturnsFalse) {
    ASSERT_FALSE(string_contains_cstr(nullptr, nullptr));
}

// String is empty
TEST(LiteStringTest, ContainsCstrEmptyStringReturnsFalse) {
    lite_string *s = string_new();
    ASSERT_FALSE(string_contains_cstr(s, "World"));
    string_free(s);
}

// C-string is empty
TEST(LiteStringTest, ContainsCstrEmptyCStrReturnsTrue) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_TRUE(string_contains_cstr(s, ""));
    string_free(s);
}

// String and C-string are both empty
TEST(LiteStringTest, ContainsCstrEmptyStringAndCStrReturnsTrue) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_contains_cstr(s, ""));
    string_free(s);
}

// String starts with the C-string
TEST(LiteStringTest, StartsWithCstrReturnsCorrectValue) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_TRUE(string_starts_with_cstr(s, "Hello"));
    ASSERT_FALSE(string_starts_with_cstr(s, "World"));
    string_free(s);
}

// String is null
TEST(LiteStringTest, StartsWithCstrNullReturnsFalse) {
    ASSERT_FALSE(string_starts_with_cstr(nullptr, "Hello"));
}

// C-string is null
TEST(LiteStringTest, StartsWithCstrNullCStrReturnsFalse) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_FALSE(string_starts_with_cstr(s, nullptr));
    string_free(s);
}

// String and C-string are both null
TEST(LiteStringTest, StartsWithCstrNullStringAndCStrReturnsFalse) {
    ASSERT_FALSE(string_starts_with_cstr(nullptr, nullptr));
}

// String is empty
TEST(LiteStringTest, StartsWithCstrEmptyStringReturnsFalse) {
    lite_string *s = string_new();
    ASSERT_FALSE(string_starts_with_cstr(s, "Hello"));
    string_free(s);
}

// C-string is empty
TEST(LiteStringTest, StartsWithCstrEmptyCStrReturnsTrue) {
    lite_string *s = string_new();
    string_append_cstr(s, "Hello, World!");
    ASSERT_TRUE(string_starts_with_cstr(s, ""));
    string_free(s);
}

// String and C-string are both empty
TEST(LiteStringTest, StartsWithCstrEmptyStringAndCStrReturnsTrue) {
    lite_string *s = string_new();
    ASSERT_TRUE(string_starts_with_cstr(s, ""));
    string_free(s);
}

// Comparing two identical strings, ignoring case
TEST(LiteStringTest, CaseCompareReturnsCorrectValue) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    lite_string *s3 = string_new();

    string_append_cstr(s1, "Hello, World!");
    string_append_cstr(s2, "HELLO, WORLD!");
    string_append_cstr(s3, "HELLO, UNIVERSE!");

    ASSERT_TRUE(string_case_compare(s1, s2));
    ASSERT_FALSE(string_case_compare(s1, s3));
    string_free(s1);
    string_free(s2);
    string_free(s3);
}

// Comparing a string with a null string, ignoring case
TEST(LiteStringTest, CaseCompareNullStringReturnsFalse) {
    lite_string *s1 = string_new();
    string_append_cstr(s1, "Hello, World!");
    ASSERT_FALSE(string_case_compare(s1, nullptr));
    ASSERT_FALSE(string_case_compare(nullptr, s1));
    string_free(s1);
}

// Comparing two null strings, ignoring case
TEST(LiteStringTest, CaseCompareNullStringsReturnsFalse) {
    ASSERT_FALSE(string_case_compare(nullptr, nullptr));
}

// Comparing two empty strings, ignoring case
TEST(LiteStringTest, CaseCompareEmptyStringsReturnsTrue) {
    lite_string *s1 = string_new();
    lite_string *s2 = string_new();
    ASSERT_TRUE(string_case_compare(s1, s2));
    string_free(s1);
    string_free(s2);
}
