#include <catch2/catch_test_macros.hpp>

#include <cstring>  // For strcmp
#include <stdint.h> // For uintptr_t, intptr_t
#include <stddef.h> // For size_t

// Forward declarations for assumed library headers
#include "util.h"
#include "st.h"

// --- Type Definition from original code ---

/**
 * @brief Just some struct type.
 */
typedef struct mys {
    double a;
    int b;
    int c;
} mys_t;

// --- Helper Functions required by the tests (MUST have C linkage) ---
extern "C" {

/**
 * @brief Compares two items of type mys_t.
 * @return 0 if they compare equal and 1 otherwise.
 */
int
mys_cmp(void const * key1, void const * key2)
{
    mys_t const *m1 = (mys_t const *) key1;
    mys_t const *m2 = (mys_t const *) key2;

    // The original logic returned 0 for equal, 1 for not equal.
    return m1->b != m2->b || m1->c != m2->c;
}


/**
 * @brief Hashes one item of type mys_t.
 * @return the hash value.
 */
int
mys_hash(void const * key, int size)
{
    mys_t const *m = (mys_t const *) key;
    // Simple hash function preserved from original C code
    return (int)((((unsigned) m->b >> 4) ^ ((unsigned) m->c >> 5)) % size);
}


/**
 * @brief Accumulates the values associated to items of type mys_t.
 * @return ST_CONTINUE
 */
enum st_retval
mys_accm(void * key, void * value, void * arg)
{
    (void) key; // Avoid unused parameter warning
    uintptr_t v = (uintptr_t) value;
    uintptr_t * accum = (uintptr_t *) arg;
    *accum += v;
    return ST_CONTINUE;
}


/**
 * @brief Compares two arrays of ints, using 'arg' for array length.
 * @return 0 if they compare equal and 1 otherwise.
 */
int
array_cmp(void const * key1, void const * key2, void const *arg)
{
    int const *a1 = (int const *) key1;
    int const *a2 = (int const *) key2;
    // The 'arg' is expected to hold the size_t length of the arrays.
    size_t const size = (size_t) arg;
    for (size_t i = 0; i < size; i++) {
        if (a1[i] != a2[i])
            return 1;
    }
    return 0;
}


/**
 * @brief Hashes one array of ints, using 'arg' for array length.
 * @return the hash value.
 */
int
array_hash(void const * key, int modulus, void const * arg)
{
    unsigned int const *a = (unsigned int const *) key;
    size_t const size = (size_t) arg;
    int val = 0;
    for (size_t i = 0; i < size; i++) {
        val = val * 997u + a[i];
    }
    return (int)(val % (unsigned int)modulus);
}

} // end extern "C"

// -----------------------------------------------------------------------------
//                               CATCH2 UNIT TESTS
// -----------------------------------------------------------------------------

TEST_CASE("String Table (st_strhash)", "[string][st]") {
    // Test: st_init_table with standard string functions
    char foo[] = "foo";
    char bar[] = "bar";
    char foobar[] = "foobar";
    // st_compare_t and st_strhash are expected to have C linkage from st.h
    st_table * tbl = st_init_table((st_compare_t) strcmp, st_strhash);
    REQUIRE(tbl != NULL);

    SECTION("Insert and Membership") {
        REQUIRE(st_insert(tbl, foo, NULL) == 0);
        REQUIRE(st_insert(tbl, bar, NULL) == 0);
        REQUIRE(st_insert(tbl, foobar, NULL) == 0);
        REQUIRE(st_count(tbl) == 3);
        REQUIRE(st_is_member(tbl, "foo") == 1);
        REQUIRE(st_is_member(tbl, "baz") == 0);
    }
    SECTION("Delete and Count") {
      // Insert foo, bar, foobar into the table
      REQUIRE(st_insert(tbl, foo, nullptr) == 0);
      REQUIRE(st_insert(tbl, bar, nullptr) == 0);
      REQUIRE(st_insert(tbl, foobar, nullptr) == 0);

      // Delete foo
      char *cp = foo;
      REQUIRE(st_delete(tbl, (void **) &cp, nullptr) == 1); // returns 1 if key found
      REQUIRE(st_is_member(tbl, "foo") == 0);               // foo should be gone
      REQUIRE(st_count(tbl) == 2);                          // two keys remain: bar, foobar
    }

    SECTION("Insert Existing Key") {
      REQUIRE(st_insert(tbl, bar, NULL) == 0); // first insertion
      REQUIRE(st_insert(tbl, bar, NULL) == 1); // duplicate key
    }

    st_free_table(tbl);
}


TEST_CASE("Struct Key Table (Custom Hash/Cmp)", "[struct][custom][st]") {
    // Test: Table mapping custom struct (mys_t) to uintptr_t
    mys_t m1 = {3.5, 4, 11};
    mys_t m2 = {6.7, 5, -2};
    uintptr_t u = 0;
    // mys_cmp and mys_hash now have C linkage
    st_table * tbl = st_init_table(mys_cmp, mys_hash);
    REQUIRE(tbl != NULL);

    SECTION("Insert and Lookup") {
        REQUIRE(st_insert(tbl, &m1, (void *)(uintptr_t) 2) == 0);
        REQUIRE(st_insert(tbl, &m2, (void *)(uintptr_t) 5) == 0); 

        uintptr_t looked_up_val;
        REQUIRE(st_lookup(tbl, &m1, (void **) &looked_up_val) == 1);
        REQUIRE(looked_up_val == 2);
    }

    SECTION("Foreach and Accumulate") {
        // Ensure values are inserted before accumulation test
        REQUIRE(st_insert(tbl, &m1, (void *)(uintptr_t) 2) == 0);
        REQUIRE(st_insert(tbl, &m2, (void *)(uintptr_t) 5) == 0);

        // mys_accm now has C linkage
        REQUIRE(st_foreach(tbl, mys_accm, &u) == 1);
        REQUIRE(u == 7); // 2 + 5
    }

    st_free_table(tbl);
}


TEST_CASE("uintptr_t Key Table (st_numhash/st_numcmp)", "[uintptr][numeric][st]") {
    // Test: Table mapping uintptr_t to string
    char foo[] = "foo";
    char * cp;
    st_table * tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != NULL);

    SECTION("Insert and Lookup") {
        REQUIRE(st_insert(tbl, (void *)(uintptr_t) 2, foo) == 0);

        REQUIRE(st_lookup(tbl, (void *)(uintptr_t) 2, (void **) &cp) == 1);
        REQUIRE(strcmp(cp, "foo") == 0);
    }

    SECTION("Membership Check") {
        REQUIRE(st_is_member(tbl, (void *)(uintptr_t) 76) == 0);
    }

    st_free_table(tbl);
}


TEST_CASE("Int-to-Int Table (st_numhash/st_numcmp) and Iteration", "[int][numeric][iteration][st]") {
    int n1 = -2;
    int n2 = 0;
    void * e;
    st_table * tbl = st_init_table(st_numcmp, st_numhash);
    REQUIRE(tbl != NULL);

    SECTION("Insert, Lookup, and Delete Int") {
      REQUIRE(st_insert(tbl, (void *)(intptr_t) n1, (void *)(intptr_t) 3) == 0);

      REQUIRE(st_lookup_int(tbl, (void *)(intptr_t) n1, &n2) == 1);
      REQUIRE(n2 == 3);

      e = (void *)(intptr_t) n1;
      n2 = 0;
      REQUIRE(st_delete_int(tbl, &e, &n2) == 1);
      REQUIRE((int)(intptr_t) e == n1); // Check key retrieved
      REQUIRE(n2 == 3); // Check value retrieved
      REQUIRE(st_count(tbl) == 0);
    }

    SECTION("Bulk Insert and st_foreach_item_int") {
      int const NUM_ELEMENTS = 10000; // Reduced for faster test execution
      for (int i = 0; i < NUM_ELEMENTS; i++) {
        st_insert(tbl, (void *)(intptr_t) i, (void *)(intptr_t) i);
      }
      CHECK(st_count(tbl) == NUM_ELEMENTS);

      int count = 0;
      st_generator * gen = NULL;
      // Iterate and verify key == value
      st_foreach_item_int(tbl, gen, &e, &n1) {
          REQUIRE((int)(intptr_t) e == n1);
          count++;
      }
      CHECK(count == NUM_ELEMENTS);
    }

    st_free_table(tbl);
}


TEST_CASE("Table with Arg (array_cmp/array_hash)", "[arg][array][st]") {
    // Test: st_init_table_with_arg using array keys
    size_t const n = 5;
    int a1[] = {0,1,2,3,4};
    int a2[] = {4,3,2,1,0};
    int *a3 = a1; // Pointer for delete operation
    intptr_t val = 0;

    // 'n' (5) is passed as the extra argument used by array_cmp/array_hash
    // array_cmp and array_hash now have C linkage
    st_table *tbl = st_init_table_with_arg(array_cmp, array_hash, (void *) n);
    REQUIRE(tbl != NULL);

    SECTION("Insert and Membership") {
        REQUIRE(st_insert(tbl, a1, (void *)(intptr_t) 1) == 0);
        REQUIRE(st_insert(tbl, a2, (void *)(intptr_t) 2) == 0);
        REQUIRE(st_count(tbl) == 2);
        REQUIRE(st_is_member(tbl, a1) == 1);
        REQUIRE(st_is_member(tbl, a2) == 1);
    }

    SECTION("Delete with Arg") {
        // Ensure values are inserted
        REQUIRE(st_insert(tbl, a1, (void *)(intptr_t) 1) == 0);
        REQUIRE(st_insert(tbl, a2, (void *)(intptr_t) 2) == 0);

        // Delete a1. Deleted key is stored in a3, deleted value in val.
        REQUIRE(st_delete(tbl, (void **) &a3, (void **) &val) == 1);
        REQUIRE(a3 == a1);
        REQUIRE(val == 1);
        REQUIRE(st_count(tbl) == 1);

        // Check membership after delete
        REQUIRE(st_is_member(tbl, a1) == 0);
        REQUIRE(st_is_member(tbl, a2) == 1);
    }

    st_free_table(tbl);
}
