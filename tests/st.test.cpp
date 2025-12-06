/**
 * @file st.test.cpp
 * @brief Comprehensive test cases for src/st.c to achieve 90% coverage.
 *
 * This file contains unit tests for the symbol table (st) library.
 */

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <cstdint>
#include <cstdlib>

// Include st library headers
#include "util.h"
#include "st.h"

// ============================================================================
// Helper functions and callbacks
// ============================================================================

extern "C" {

/**
 * @brief A comparison function that returns 0 for equal strings.
 */
static int str_cmp(void const *x, void const *y) {
    return strcmp((const char *)x, (const char *)y);
}

/**
 * @brief A callback that returns ST_CONTINUE.
 */
static enum st_retval foreach_continue(void *key, void *value, void *arg) {
    (void)key;
    (void)value;
    int *count = (int *)arg;
    (*count)++;
    return ST_CONTINUE;
}

/**
 * @brief A callback that returns ST_STOP after the first entry.
 */
static enum st_retval foreach_stop(void *key, void *value, void *arg) {
    (void)key;
    (void)value;
    int *count = (int *)arg;
    (*count)++;
    return ST_STOP;
}

/**
 * @brief A callback that returns ST_DELETE.
 */
static enum st_retval foreach_delete(void *key, void *value, void *arg) {
    (void)key;
    (void)value;
    int *count = (int *)arg;
    (*count)++;
    return ST_DELETE;
}

/**
 * @brief A callback that deletes only even keys.
 */
static enum st_retval foreach_delete_even(void *key, void *value, void *arg) {
    (void)value;
    int *count = (int *)arg;
    intptr_t k = (intptr_t)key;
    (*count)++;
    if (k % 2 == 0) {
        return ST_DELETE;
    }
    return ST_CONTINUE;
}

/**
 * @brief Custom comparison function with arg for testing.
 */
static int compare_with_arg(void const *x, void const *y, void const *arg) {
    (void)arg;
    return (intptr_t)x - (intptr_t)y;
}

/**
 * @brief Custom hash function with arg for testing.
 */
static int hash_with_arg(void const *key, int modulus, void const *arg) {
    (void)arg;
    return (int)((uintptr_t)key % (uintptr_t)modulus);
}

} // extern "C"

// ============================================================================
// Test cases for st_init_table and st_init_table_with_params
// ============================================================================

TEST_CASE("st - st_init_table creates a table with default params", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    REQUIRE(st_count(tbl) == 0);
    st_free_table(tbl);
}

TEST_CASE("st - st_init_table_with_params with custom params", "[st]") {
    SECTION("Normal initialization") {
        st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash, 
                                                    16, 10, 1.5, 1);
        REQUIRE(tbl != nullptr);
        REQUIRE(st_count(tbl) == 0);
        st_free_table(tbl);
    }

    SECTION("Size <= 0 becomes 1") {
        st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash, 
                                                    0, 5, 2.0, 0);
        REQUIRE(tbl != nullptr);
        st_free_table(tbl);
        
        tbl = st_init_table_with_params(st_numcmp, st_numhash, 
                                         -5, 5, 2.0, 0);
        REQUIRE(tbl != nullptr);
        st_free_table(tbl);
    }
}

TEST_CASE("st - st_init_table_with_arg creates table with extra arg", "[st]") {
    size_t arg = 42;
    st_table *tbl = st_init_table_with_arg(compare_with_arg, hash_with_arg, 
                                             (void *)arg);
    REQUIRE(tbl != nullptr);
    REQUIRE(st_count(tbl) == 0);
    st_free_table(tbl);
}

TEST_CASE("st - st_init_table_with_params_and_arg", "[st]") {
    size_t arg = 42;
    st_table *tbl = st_init_table_with_params_and_arg(
        compare_with_arg, hash_with_arg, (void *)arg,
        16, 10, 2.0, 1);
    REQUIRE(tbl != nullptr);
    REQUIRE(st_count(tbl) == 0);
    
    // Insert using the arg-based hash/compare
    REQUIRE(st_insert(tbl, (void *)(intptr_t)1, (void *)(intptr_t)100) == 0);
    REQUIRE(st_insert(tbl, (void *)(intptr_t)2, (void *)(intptr_t)200) == 0);
    REQUIRE(st_count(tbl) == 2);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_numhash, st_ptrhash, st_numcmp, st_ptrcmp
// ============================================================================

TEST_CASE("st - st_numhash computes hash for numbers", "[st]") {
    SECTION("Various values") {
        int h1 = st_numhash((void *)(uintptr_t)0, 10);
        REQUIRE(h1 >= 0);
        REQUIRE(h1 < 10);
        
        int h2 = st_numhash((void *)(uintptr_t)42, 10);
        REQUIRE(h2 >= 0);
        REQUIRE(h2 < 10);
        
        int h3 = st_numhash((void *)(uintptr_t)100, 17);
        REQUIRE(h3 >= 0);
        REQUIRE(h3 < 17);
    }
}

TEST_CASE("st - st_ptrhash computes hash for pointers", "[st]") {
    SECTION("Various pointers") {
        int arr[3] = {1, 2, 3};
        
        int h1 = st_ptrhash(&arr[0], 10);
        REQUIRE(h1 >= 0);
        REQUIRE(h1 < 10);
        
        int h2 = st_ptrhash(&arr[1], 10);
        REQUIRE(h2 >= 0);
        REQUIRE(h2 < 10);
        
        int h3 = st_ptrhash(nullptr, 10);
        REQUIRE(h3 >= 0);
        REQUIRE(h3 < 10);
    }
}

TEST_CASE("st - st_numcmp compares numbers", "[st]") {
    SECTION("Equal values") {
        REQUIRE(st_numcmp((void *)(uintptr_t)5, (void *)(uintptr_t)5) == 0);
    }
    
    SECTION("Different values") {
        REQUIRE(st_numcmp((void *)(uintptr_t)5, (void *)(uintptr_t)10) != 0);
        REQUIRE(st_numcmp((void *)(uintptr_t)10, (void *)(uintptr_t)5) != 0);
    }
}

TEST_CASE("st - st_ptrcmp compares pointers", "[st]") {
    int arr[3] = {1, 2, 3};
    
    SECTION("Equal pointers") {
        REQUIRE(st_ptrcmp(&arr[0], &arr[0]) == 0);
    }
    
    SECTION("Different pointers") {
        REQUIRE(st_ptrcmp(&arr[0], &arr[1]) != 0);
    }
}

// ============================================================================
// Test cases for st_strhash
// ============================================================================

TEST_CASE("st - st_strhash computes hash for strings", "[st]") {
    SECTION("Empty string") {
        int h = st_strhash("", 10);
        REQUIRE(h >= 0);
        REQUIRE(h < 10);
    }
    
    SECTION("Normal strings") {
        int h1 = st_strhash("hello", 100);
        REQUIRE(h1 >= 0);
        REQUIRE(h1 < 100);
        
        int h2 = st_strhash("world", 100);
        REQUIRE(h2 >= 0);
        REQUIRE(h2 < 100);
    }
    
    SECTION("Long string") {
        int h = st_strhash("this is a very long string for testing", 1000);
        REQUIRE(h >= 0);
        REQUIRE(h < 1000);
    }
}

// ============================================================================
// Test cases for st_find
// ============================================================================

TEST_CASE("st - st_find looks up without creating", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    SECTION("Find non-existent key returns 0") {
        void **slot = nullptr;
        int result = st_find(tbl, (void *)(intptr_t)42, &slot);
        REQUIRE(result == 0);
    }
    
    SECTION("Find existing key returns 1 and slot") {
        REQUIRE(st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100) == 0);
        
        void **slot = nullptr;
        int result = st_find(tbl, (void *)(intptr_t)42, &slot);
        REQUIRE(result == 1);
        REQUIRE(slot != nullptr);
        REQUIRE(*slot == (void *)(intptr_t)100);
    }
    
    SECTION("Find with null slot parameter") {
        REQUIRE(st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100) == 0);
        
        int result = st_find(tbl, (void *)(intptr_t)42, nullptr);
        REQUIRE(result == 1);
    }
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_copy
// ============================================================================

TEST_CASE("st - st_copy creates a copy of a table", "[st]") {
    SECTION("Copy empty table") {
        st_table *tbl = st_init_table(st_numcmp, st_numhash);
        REQUIRE(tbl != nullptr);
        
        st_table *copy = st_copy(tbl);
        REQUIRE(copy != nullptr);
        REQUIRE(st_count(copy) == 0);
        
        st_free_table(copy);
        st_free_table(tbl);
    }
    
    SECTION("Copy table with entries") {
        st_table *tbl = st_init_table(st_numcmp, st_numhash);
        REQUIRE(tbl != nullptr);
        
        // Insert entries
        for (int i = 0; i < 10; i++) {
            st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
        }
        REQUIRE(st_count(tbl) == 10);
        
        st_table *copy = st_copy(tbl);
        REQUIRE(copy != nullptr);
        REQUIRE(st_count(copy) == 10);
        
        // Verify all entries are copied
        for (int i = 0; i < 10; i++) {
            void *value = nullptr;
            REQUIRE(st_lookup(copy, (void *)(intptr_t)i, &value) == 1);
            REQUIRE(value == (void *)(intptr_t)(i * 10));
        }
        
        st_free_table(copy);
        st_free_table(tbl);
    }
    
    SECTION("Copy table with string hash") {
        st_table *tbl = st_init_table(str_cmp, st_strhash);
        REQUIRE(tbl != nullptr);
        
        char key1[] = "hello";
        char key2[] = "world";
        st_insert(tbl, key1, (void *)(intptr_t)1);
        st_insert(tbl, key2, (void *)(intptr_t)2);
        
        st_table *copy = st_copy(tbl);
        REQUIRE(copy != nullptr);
        REQUIRE(st_count(copy) == 2);
        
        st_free_table(copy);
        st_free_table(tbl);
    }
    
    SECTION("Copy table with multiple bins") {
        // Create table with small initial size to ensure multiple bins
        st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                    5, 5, 2.0, 0);
        REQUIRE(tbl != nullptr);
        
        // Insert entries that will spread across bins
        for (int i = 0; i < 50; i++) {
            st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
        }
        
        st_table *copy = st_copy(tbl);
        REQUIRE(copy != nullptr);
        REQUIRE(st_count(copy) == 50);
        
        st_free_table(copy);
        st_free_table(tbl);
    }
}

// ============================================================================
// Test cases for st_find_or_add
// ============================================================================

TEST_CASE("st - st_find_or_add finds or creates entry", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    SECTION("Add new entry") {
        void **slot = nullptr;
        int result = st_find_or_add(tbl, (void *)(intptr_t)42, &slot);
        REQUIRE(result == 0);  // 0 means it was added
        REQUIRE(slot != nullptr);
        REQUIRE(st_count(tbl) == 1);
        
        // Set value via slot
        *slot = (void *)(intptr_t)100;
        
        // Verify
        void *value = nullptr;
        REQUIRE(st_lookup(tbl, (void *)(intptr_t)42, &value) == 1);
        REQUIRE(value == (void *)(intptr_t)100);
    }
    
    SECTION("Find existing entry") {
        st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100);
        
        void **slot = nullptr;
        int result = st_find_or_add(tbl, (void *)(intptr_t)42, &slot);
        REQUIRE(result == 1);  // 1 means it already existed
        REQUIRE(slot != nullptr);
        REQUIRE(*slot == (void *)(intptr_t)100);
        REQUIRE(st_count(tbl) == 1);
    }
    
    SECTION("Find or add with null slot") {
        int result = st_find_or_add(tbl, (void *)(intptr_t)42, nullptr);
        REQUIRE(result == 0);
        REQUIRE(st_count(tbl) == 1);
    }
    
    st_free_table(tbl);
}

TEST_CASE("st - st_find_or_add triggers rehash", "[st]") {
    // Create small table that will need to rehash
    st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                1, 1, 2.0, 0);
    REQUIRE(tbl != nullptr);
    
    // Insert entries to trigger rehash during find_or_add
    void **slot = nullptr;
    for (int i = 0; i < 10; i++) {
        st_find_or_add(tbl, (void *)(intptr_t)i, &slot);
        *slot = (void *)(intptr_t)(i * 10);
    }
    REQUIRE(st_count(tbl) == 10);
    
    // Verify all entries
    for (int i = 0; i < 10; i++) {
        void *value = nullptr;
        REQUIRE(st_lookup(tbl, (void *)(intptr_t)i, &value) == 1);
        REQUIRE(value == (void *)(intptr_t)(i * 10));
    }
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_add_direct
// ============================================================================

TEST_CASE("st - st_add_direct adds without checking", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    SECTION("Add entries") {
        REQUIRE(st_add_direct(tbl, (void *)(intptr_t)1, (void *)(intptr_t)10) == 1);
        REQUIRE(st_add_direct(tbl, (void *)(intptr_t)2, (void *)(intptr_t)20) == 1);
        REQUIRE(st_count(tbl) == 2);
    }
    
    st_free_table(tbl);
}

TEST_CASE("st - st_add_direct triggers rehash", "[st]") {
    // Create small table that will rehash
    st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                1, 1, 2.0, 0);
    REQUIRE(tbl != nullptr);
    
    for (int i = 0; i < 20; i++) {
        REQUIRE(st_add_direct(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10)) == 1);
    }
    REQUIRE(st_count(tbl) == 20);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_foreach with different return values
// ============================================================================

TEST_CASE("st - st_foreach with ST_CONTINUE", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    // Insert entries
    for (int i = 0; i < 10; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    int count = 0;
    int result = st_foreach(tbl, foreach_continue, &count);
    REQUIRE(result == 1);  // 1 means completed all items
    REQUIRE(count == 10);
    REQUIRE(st_count(tbl) == 10);  // No entries deleted
    
    st_free_table(tbl);
}

TEST_CASE("st - st_foreach with ST_STOP", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    // Insert entries
    for (int i = 0; i < 10; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    int count = 0;
    int result = st_foreach(tbl, foreach_stop, &count);
    REQUIRE(result == 0);  // 0 means stopped early
    REQUIRE(count == 1);   // Only processed one entry
    REQUIRE(st_count(tbl) == 10);  // No entries deleted
    
    st_free_table(tbl);
}

TEST_CASE("st - st_foreach with ST_DELETE", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    // Insert entries
    for (int i = 0; i < 10; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    int count = 0;
    int result = st_foreach(tbl, foreach_delete, &count);
    REQUIRE(result == 1);  // Completed all items
    REQUIRE(count == 10);  // Processed all entries
    REQUIRE(st_count(tbl) == 0);  // All entries deleted
    
    st_free_table(tbl);
}

TEST_CASE("st - st_foreach with selective ST_DELETE", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    // Insert entries
    for (int i = 0; i < 10; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    int count = 0;
    int result = st_foreach(tbl, foreach_delete_even, &count);
    REQUIRE(result == 1);
    REQUIRE(count == 10);
    REQUIRE(st_count(tbl) == 5);  // Only odd entries remain
    
    // Verify only odd entries remain
    for (int i = 0; i < 10; i++) {
        int expected = (i % 2 == 0) ? 0 : 1;
        REQUIRE(st_is_member(tbl, (void *)(intptr_t)i) == expected);
    }
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_gen and st_gen_int with null value_p
// ============================================================================

TEST_CASE("st - st_gen with null value_p", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    for (int i = 0; i < 5; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    st_generator *gen = st_init_gen(tbl);
    REQUIRE(gen != nullptr);
    
    int count = 0;
    void *key;
    while (st_gen(gen, &key, nullptr)) {  // null value_p
        count++;
    }
    REQUIRE(count == 5);
    
    st_free_gen(gen);
    st_free_table(tbl);
}

TEST_CASE("st - st_gen_int with null value_p", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    for (int i = 0; i < 5; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    st_generator *gen = st_init_gen(tbl);
    REQUIRE(gen != nullptr);
    
    int count = 0;
    void *key;
    while (st_gen_int(gen, &key, nullptr)) {  // null value_p
        count++;
    }
    REQUIRE(count == 5);
    
    st_free_gen(gen);
    st_free_table(tbl);
}

TEST_CASE("st - st_gen on empty table", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_generator *gen = st_init_gen(tbl);
    REQUIRE(gen != nullptr);
    
    void *key;
    void *value;
    REQUIRE(st_gen(gen, &key, &value) == 0);
    
    st_free_gen(gen);
    st_free_table(tbl);
}

TEST_CASE("st - st_gen_int on empty table", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_generator *gen = st_init_gen(tbl);
    REQUIRE(gen != nullptr);
    
    void *key;
    int value;
    REQUIRE(st_gen_int(gen, &key, &value) == 0);
    
    st_free_gen(gen);
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_lookup with null value
// ============================================================================

TEST_CASE("st - st_lookup with null value pointer", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100);
    
    // Lookup with null value pointer
    REQUIRE(st_lookup(tbl, (void *)(intptr_t)42, nullptr) == 1);
    REQUIRE(st_lookup(tbl, (void *)(intptr_t)99, nullptr) == 0);
    
    st_free_table(tbl);
}

TEST_CASE("st - st_lookup_int with null value pointer", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100);
    
    // Lookup with null value pointer
    REQUIRE(st_lookup_int(tbl, (void *)(intptr_t)42, nullptr) == 1);
    REQUIRE(st_lookup_int(tbl, (void *)(intptr_t)99, nullptr) == 0);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_delete and st_delete_int with null value
// ============================================================================

TEST_CASE("st - st_delete with null value pointer", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100);
    
    void *key = (void *)(intptr_t)42;
    REQUIRE(st_delete(tbl, &key, nullptr) == 1);  // null value pointer
    REQUIRE(st_count(tbl) == 0);
    
    st_free_table(tbl);
}

TEST_CASE("st - st_delete_int with null value pointer", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100);
    
    void *key = (void *)(intptr_t)42;
    REQUIRE(st_delete_int(tbl, &key, nullptr) == 1);  // null value pointer
    REQUIRE(st_count(tbl) == 0);
    
    st_free_table(tbl);
}

TEST_CASE("st - st_delete non-existent key", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    void *key = (void *)(intptr_t)42;
    void *value;
    REQUIRE(st_delete(tbl, &key, &value) == 0);
    
    st_free_table(tbl);
}

TEST_CASE("st - st_delete_int non-existent key", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    void *key = (void *)(intptr_t)42;
    int value;
    REQUIRE(st_delete_int(tbl, &key, &value) == 0);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for reorder_flag (move-to-front on lookup)
// ============================================================================

TEST_CASE("st - reorder_flag moves found entry to front", "[st]") {
    // Create table with reorder_flag enabled
    st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                11, 5, 2.0, 1);
    REQUIRE(tbl != nullptr);
    
    // Insert entries (all will hash to same bucket ideally)
    st_insert(tbl, (void *)(intptr_t)1, (void *)(intptr_t)10);
    st_insert(tbl, (void *)(intptr_t)2, (void *)(intptr_t)20);
    st_insert(tbl, (void *)(intptr_t)3, (void *)(intptr_t)30);
    
    // Lookup last inserted entry (first in chain) - no reorder needed
    void *value;
    st_lookup(tbl, (void *)(intptr_t)3, &value);
    
    // Lookup first inserted entry (last in chain) - should be moved to front
    st_lookup(tbl, (void *)(intptr_t)1, &value);
    REQUIRE(value == (void *)(intptr_t)10);
    
    // Lookup again - should still work
    st_lookup(tbl, (void *)(intptr_t)2, &value);
    REQUIRE(value == (void *)(intptr_t)20);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for rehash
// ============================================================================

TEST_CASE("st - rehash during insert", "[st]") {
    // Create table that will need to rehash
    st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                2, 1, 2.0, 0);
    REQUIRE(tbl != nullptr);
    
    // Insert enough entries to trigger multiple rehashes
    for (int i = 0; i < 100; i++) {
        REQUIRE(st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10)) == 0);
    }
    REQUIRE(st_count(tbl) == 100);
    
    // Verify all entries are still accessible
    for (int i = 0; i < 100; i++) {
        void *value;
        REQUIRE(st_lookup(tbl, (void *)(intptr_t)i, &value) == 1);
        REQUIRE(value == (void *)(intptr_t)(i * 10));
    }
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for collision handling
// ============================================================================

TEST_CASE("st - collision handling in hash buckets", "[st]") {
    // Create a small table to ensure collisions
    st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                3, 100, 2.0, 0);
    REQUIRE(tbl != nullptr);
    
    // Insert entries (many will collide in same bucket)
    for (int i = 0; i < 20; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    REQUIRE(st_count(tbl) == 20);
    
    // Verify all entries
    for (int i = 0; i < 20; i++) {
        void *value;
        REQUIRE(st_lookup(tbl, (void *)(intptr_t)i, &value) == 1);
        REQUIRE(value == (void *)(intptr_t)(i * 10));
    }
    
    // Delete some entries
    for (int i = 0; i < 10; i++) {
        void *key = (void *)(intptr_t)i;
        void *value;
        REQUIRE(st_delete(tbl, &key, &value) == 1);
    }
    REQUIRE(st_count(tbl) == 10);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for st_foreach_item and st_foreach_item_int macros
// ============================================================================

TEST_CASE("st - st_foreach_item macro iteration", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    for (int i = 0; i < 5; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    int count = 0;
    void *key;
    void *value;
    st_generator *gen;
    st_foreach_item(tbl, gen, &key, &value) {
        REQUIRE(value == (void *)(intptr_t)((intptr_t)key * 10));
        count++;
    }
    REQUIRE(count == 5);
    
    st_free_table(tbl);
}

TEST_CASE("st - st_foreach_item_int macro iteration", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    for (int i = 0; i < 5; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    int count = 0;
    void *key;
    int value;
    st_generator *gen;
    st_foreach_item_int(tbl, gen, &key, &value) {
        REQUIRE(value == (int)((intptr_t)key * 10));
        count++;
    }
    REQUIRE(count == 5);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for pointer table using st_ptrhash and st_ptrcmp
// ============================================================================

TEST_CASE("st - table with st_ptrhash and st_ptrcmp", "[st]") {
    st_table *tbl = st_init_table(st_ptrcmp, st_ptrhash);
    REQUIRE(tbl != nullptr);
    
    int arr[5] = {10, 20, 30, 40, 50};
    
    // Insert with pointers as keys
    for (int i = 0; i < 5; i++) {
        st_insert(tbl, &arr[i], (void *)(intptr_t)i);
    }
    REQUIRE(st_count(tbl) == 5);
    
    // Lookup
    for (int i = 0; i < 5; i++) {
        void *value;
        REQUIRE(st_lookup(tbl, &arr[i], &value) == 1);
        REQUIRE(value == (void *)(intptr_t)i);
    }
    
    // Membership
    REQUIRE(st_is_member(tbl, &arr[0]) == 1);
    
    int other = 100;
    REQUIRE(st_is_member(tbl, &other) == 0);
    
    st_free_table(tbl);
}

// ============================================================================
// Test cases for inserting/updating existing keys
// ============================================================================

TEST_CASE("st - update existing key with st_insert", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    REQUIRE(st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100) == 0);  // new
    REQUIRE(st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)200) == 1);  // existing
    
    void *value;
    st_lookup(tbl, (void *)(intptr_t)42, &value);
    REQUIRE(value == (void *)(intptr_t)200);  // Updated value
    REQUIRE(st_count(tbl) == 1);
    
    st_free_table(tbl);
}

// ============================================================================
// Test for st_is_member macro
// ============================================================================

TEST_CASE("st - st_is_member checks membership", "[st]") {
    st_table *tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != nullptr);
    
    st_insert(tbl, (void *)(intptr_t)42, (void *)(intptr_t)100);
    
    REQUIRE(st_is_member(tbl, (void *)(intptr_t)42) == 1);
    REQUIRE(st_is_member(tbl, (void *)(intptr_t)99) == 0);
    
    st_free_table(tbl);
}

// ============================================================================
// Test comprehensive iteration through multiple bins
// ============================================================================

TEST_CASE("st - iteration through multiple bins", "[st]") {
    st_table *tbl = st_init_table_with_params(st_numcmp, st_numhash,
                                                3, 100, 2.0, 0);
    REQUIRE(tbl != nullptr);
    
    // Insert entries that will be distributed across bins
    for (int i = 0; i < 30; i++) {
        st_insert(tbl, (void *)(intptr_t)i, (void *)(intptr_t)(i * 10));
    }
    
    st_generator *gen = st_init_gen(tbl);
    REQUIRE(gen != nullptr);
    
    int count = 0;
    void *key;
    void *value;
    while (st_gen(gen, &key, &value)) {
        REQUIRE(value == (void *)(intptr_t)((intptr_t)key * 10));
        count++;
    }
    REQUIRE(count == 30);
    
    st_free_gen(gen);
    st_free_table(tbl);
}
