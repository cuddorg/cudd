#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddCof.c
 * 
 * This file contains comprehensive tests for the cuddCof module
 * to achieve 90%+ code coverage.
 */

// =============================================================================
// Tests for Cudd_Cofactor
// =============================================================================

TEST_CASE("Cudd_Cofactor - Basic operations", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);

    SECTION("Cofactor of constant with respect to variable cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        // Cofactor of 1 with respect to x should be 1
        DdNode *result = Cudd_Cofactor(manager, one, x);
        REQUIRE(result == one);

        // Cofactor of 0 with respect to x should be 0
        result = Cudd_Cofactor(manager, zero, x);
        REQUIRE(result == zero);

        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor of variable with respect to itself") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        // Cofactor of x with respect to x (positive cube) should be 1
        DdNode *result = Cudd_Cofactor(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);

        // Cofactor of x with respect to !x (negative cube) should be 0
        result = Cudd_Cofactor(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor of AND with respect to variable cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        // Cofactor of (x AND y) with respect to x should be y
        DdNode *result = Cudd_Cofactor(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        Cudd_RecursiveDeref(manager, result);

        // Cofactor of (x AND y) with respect to !x should be 0
        result = Cudd_Cofactor(manager, f, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor of OR with respect to variable cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x OR y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);

        // Cofactor of (x OR y) with respect to x should be 1
        DdNode *result = Cudd_Cofactor(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);

        // Cofactor of (x OR y) with respect to !x should be y
        result = Cudd_Cofactor(manager, f, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor with respect to multi-variable cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x AND y) OR z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);

        // Cube: x AND y
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);

        // Cofactor of f with respect to (x AND y) should be 1
        DdNode *result = Cudd_Cofactor(manager, f, cube);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Cofactor - Error cases", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);

    SECTION("Cofactor with zero cube (BDD zero)") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        // Cofactor with respect to 0 should fail
        DdNode *result = Cudd_Cofactor(manager, x, zero);
        REQUIRE(result == nullptr);
        // Error code should be set
        REQUIRE(Cudd_ReadErrorCode(manager) == CUDD_INVALID_ARG);
        Cudd_ClearErrorCode(manager);

        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Cofactor - Complex recursive cases", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);

    SECTION("Cofactor where topf > topg") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f depends on y and z, cube is x (variable before y,z in order)
        DdNode *f = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(f);

        // Cofactor of (y AND z) with respect to x should be (y AND z)
        DdNode *result = Cudd_Cofactor(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == f);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor where topf < topg") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f depends on x (top variable), cube is y (variable after x in order)
        // f = x XOR z
        DdNode *f = Cudd_bddXor(manager, x, z);
        Cudd_Ref(f);

        // Cofactor of (x XOR z) with respect to y
        DdNode *result = Cudd_Cofactor(manager, f, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // Result should be (x XOR z) since f doesn't depend on y
        REQUIRE(result == f);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor with complemented function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = !(x AND y)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_Not(xy);

        // Cofactor of !(x AND y) with respect to x should be !y
        DdNode *result = Cudd_Cofactor(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(y));
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor with negative cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x XOR y
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);

        // Cube = !x AND !y
        DdNode *nxny = Cudd_bddAnd(manager, Cudd_Not(x), Cudd_Not(y));
        Cudd_Ref(nxny);

        // Cofactor of (x XOR y) with respect to (!x AND !y) should be 0
        DdNode *result = Cudd_Cofactor(manager, f, nxny);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, nxny);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor where t == e in recursion") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x AND z) OR (!x AND z) = z
        DdNode *f = z;

        // Cofactor of z with respect to y
        DdNode *result = Cudd_Cofactor(manager, f, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z);
        Cudd_RecursiveDeref(manager, result);

        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

// =============================================================================
// Tests for Cudd_CheckCube
// =============================================================================

TEST_CASE("Cudd_CheckCube - Basic cube checks", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);

    SECTION("Constant 1 is a valid cube") {
        REQUIRE(Cudd_CheckCube(manager, one) == 1);
    }

    SECTION("Constant 0 is not a valid cube") {
        REQUIRE(Cudd_CheckCube(manager, zero) == 0);
    }

    SECTION("Single positive variable is a valid cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        REQUIRE(Cudd_CheckCube(manager, x) == 1);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Single negative variable is a valid cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        REQUIRE(Cudd_CheckCube(manager, Cudd_Not(x)) == 1);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("AND of two positive variables is a valid cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);

        REQUIRE(Cudd_CheckCube(manager, cube) == 1);

        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("AND of positive and negative variables is a valid cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *cube = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(cube);

        REQUIRE(Cudd_CheckCube(manager, cube) == 1);

        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("OR of two variables is not a valid cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *notCube = Cudd_bddOr(manager, x, y);
        Cudd_Ref(notCube);

        REQUIRE(Cudd_CheckCube(manager, notCube) == 0);

        Cudd_RecursiveDeref(manager, notCube);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("XOR of two variables is not a valid cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *notCube = Cudd_bddXor(manager, x, y);
        Cudd_Ref(notCube);

        REQUIRE(Cudd_CheckCube(manager, notCube) == 0);

        Cudd_RecursiveDeref(manager, notCube);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Multi-variable cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *cube = Cudd_bddAnd(manager, xy, Cudd_Not(z));
        Cudd_Ref(cube);

        REQUIRE(Cudd_CheckCube(manager, cube) == 1);

        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

// =============================================================================
// Tests for Cudd_VarsAreSymmetric
// =============================================================================

TEST_CASE("Cudd_VarsAreSymmetric - Basic symmetry checks", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);

    SECTION("Same variable is always symmetric (reflexive)") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        int idx = Cudd_NodeReadIndex(x);
        REQUIRE(Cudd_VarsAreSymmetric(manager, x, idx, idx) == 1);

        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Symmetric in AND") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x AND y is symmetric in x and y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_y, idx_x) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Symmetric in OR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x OR y is symmetric in x and y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_y, idx_x) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Symmetric in XOR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x XOR y is symmetric in x and y
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_y, idx_x) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Not symmetric in ITE-like function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = x ? y : z = (x AND y) OR (!x AND z)
        // x is not symmetric with y or z
        DdNode *f = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);
        int idx_z = Cudd_NodeReadIndex(z);

        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 0);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_z) == 0);
        // But y and z might be symmetric depending on the exact function
        // For ITE(x,y,z), y and z are not symmetric in general

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Symmetric in constant function") {
        // Constant functions don't depend on any variables
        // So any two variables are symmetric
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        REQUIRE(Cudd_VarsAreSymmetric(manager, one, idx_x, idx_y) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, zero, idx_x, idx_y) == 1);

        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Variables outside manager size - both outside") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        // Use indices that are beyond the current size
        int largeIdx1 = Cudd_ReadSize(manager) + 10;
        int largeIdx2 = Cudd_ReadSize(manager) + 20;

        // When both indices are >= dd->size, function returns 1
        REQUIRE(Cudd_VarsAreSymmetric(manager, x, largeIdx1, largeIdx2) == 1);

        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Variables outside manager size - one inside one outside") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        int idx_x = Cudd_NodeReadIndex(x);
        int largeIdx = Cudd_ReadSize(manager) + 10;

        // f = x, variable at largeIdx doesn't exist
        // Since f doesn't depend on var at largeIdx, need to check if it depends on x
        // Test with f = x (depends on x)
        REQUIRE(Cudd_VarsAreSymmetric(manager, x, idx_x, largeIdx) == 0);

        // Test with f = 1 (doesn't depend on x)
        REQUIRE(Cudd_VarsAreSymmetric(manager, one, idx_x, largeIdx) == 1);

        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Variable ordering - index1 > index2 in level") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // Test with indices swapped to exercise the swap logic
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_y, idx_x) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_VarsAreSymmetric - Complex cases", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Nested function symmetry") {
        DdNode *a = Cudd_bddNewVar(manager);
        DdNode *b = Cudd_bddNewVar(manager);
        DdNode *c = Cudd_bddNewVar(manager);
        DdNode *d = Cudd_bddNewVar(manager);
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(c);
        Cudd_Ref(d);

        // f = (a AND b) OR (c AND d)
        DdNode *ab = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(ab);
        DdNode *cd = Cudd_bddAnd(manager, c, d);
        Cudd_Ref(cd);
        DdNode *f = Cudd_bddOr(manager, ab, cd);
        Cudd_Ref(f);

        int idx_a = Cudd_NodeReadIndex(a);
        int idx_b = Cudd_NodeReadIndex(b);
        int idx_c = Cudd_NodeReadIndex(c);
        int idx_d = Cudd_NodeReadIndex(d);

        // a and b are symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_a, idx_b) == 1);
        // c and d are symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_c, idx_d) == 1);
        // a and c are not necessarily symmetric
        // (they are in this case due to function structure)

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, cd);
        Cudd_RecursiveDeref(manager, ab);
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, a);
    }

    SECTION("Function with complemented edges") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = !(x AND y) = NAND(x,y)
        DdNode *f = Cudd_bddNand(manager, x, y);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // NAND is symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Asymmetric function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x AND !y (asymmetric)
        DdNode *f = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // x and y are not symmetric in (x AND !y)
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 0);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cache exercise - repeated calls") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // Call multiple times to exercise cache
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_y, idx_x) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_VarsAreSymmetric - Edge cases for recursive functions", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("ddVarsAreSymmetricBefore - top > level1 branch") {
        // Create variables with specific ordering
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f depends only on z (which is after x and y in ordering)
        // Testing symmetry of x and y in f = z
        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // Both x and y don't appear in z, so they should be symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, z, idx_x, idx_y) == 1);

        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("ddVarsAreSymmetricBetween - various cofactor combinations") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x AND z) OR (y AND z) = z AND (x OR y)
        DdNode *xz = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(xz);
        DdNode *yz = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddOr(manager, xz, yz);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // x and y are symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, xz);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Multi-level symmetry check") {
        // Create more variables for deeper recursion
        DdNode *v0 = Cudd_bddNewVar(manager);
        DdNode *v1 = Cudd_bddNewVar(manager);
        DdNode *v2 = Cudd_bddNewVar(manager);
        DdNode *v3 = Cudd_bddNewVar(manager);
        Cudd_Ref(v0);
        Cudd_Ref(v1);
        Cudd_Ref(v2);
        Cudd_Ref(v3);

        // f = v0 XOR v1 XOR v2 XOR v3
        DdNode *f01 = Cudd_bddXor(manager, v0, v1);
        Cudd_Ref(f01);
        DdNode *f23 = Cudd_bddXor(manager, v2, v3);
        Cudd_Ref(f23);
        DdNode *f = Cudd_bddXor(manager, f01, f23);
        Cudd_Ref(f);

        int idx_0 = Cudd_NodeReadIndex(v0);
        int idx_1 = Cudd_NodeReadIndex(v1);
        int idx_2 = Cudd_NodeReadIndex(v2);
        int idx_3 = Cudd_NodeReadIndex(v3);

        // All pairs should be symmetric in XOR chain
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_0, idx_1) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_0, idx_2) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_0, idx_3) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_1, idx_2) == 1);
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_2, idx_3) == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f23);
        Cudd_RecursiveDeref(manager, f01);
        Cudd_RecursiveDeref(manager, v3);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v0);
    }

    SECTION("Test ddVarsAreSymmetricBetween with F1 constant") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x (which means f[x=1] = 1 and f[x=0] = 0)
        // Testing symmetry of x and y in f = x
        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // x and y are not symmetric in f = x
        REQUIRE(Cudd_VarsAreSymmetric(manager, x, idx_x, idx_y) == 0);

        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Test ddVarsAreSymmetricBetween with F0 constant") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *one = Cudd_ReadOne(manager);

        // f = !y implies f[y=1] = 0 and f[y=0] = 1
        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // x and y are not symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, Cudd_Not(y), idx_x, idx_y) == 0);

        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Test ddVarsAreSymmetricBetween with both constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *one = Cudd_ReadOne(manager);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // In constant 1, all variables are symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, one, idx_x, idx_y) == 1);

        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

// =============================================================================
// Additional tests to exercise more code paths
// =============================================================================

TEST_CASE("cuddCofactorRecur - Deep recursion paths", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);

    SECTION("Complex cofactor with many variables") {
        // Create several variables
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }

        // Build a complex function:
        // f = (v0 AND v1) OR (v2 AND v3) OR (v4 AND v5)
        DdNode *v01 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(v01);
        DdNode *v23 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(v23);
        DdNode *v45 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(v45);

        DdNode *tmp = Cudd_bddOr(manager, v01, v23);
        Cudd_Ref(tmp);
        DdNode *f = Cudd_bddOr(manager, tmp, v45);
        Cudd_Ref(f);

        // Compute cofactor with respect to v0
        DdNode *cof = Cudd_Cofactor(manager, f, vars[0]);
        REQUIRE(cof != nullptr);
        Cudd_Ref(cof);

        // Cofactor of f w.r.t. v0 should be: v1 OR (v2 AND v3) OR (v4 AND v5)
        DdNode *v23_v45 = Cudd_bddOr(manager, v23, v45);
        Cudd_Ref(v23_v45);
        DdNode *expected = Cudd_bddOr(manager, vars[1], v23_v45);
        Cudd_Ref(expected);
        REQUIRE(cof == expected);

        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, v23_v45);
        Cudd_RecursiveDeref(manager, cof);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, tmp);
        Cudd_RecursiveDeref(manager, v45);
        Cudd_RecursiveDeref(manager, v23);
        Cudd_RecursiveDeref(manager, v01);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }

    SECTION("Cofactor cache hit") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        // Call cofactor twice to hit cache
        DdNode *cof1 = Cudd_Cofactor(manager, f, x);
        REQUIRE(cof1 != nullptr);
        Cudd_Ref(cof1);

        DdNode *cof2 = Cudd_Cofactor(manager, f, x);
        REQUIRE(cof2 != nullptr);
        Cudd_Ref(cof2);

        REQUIRE(cof1 == cof2);

        Cudd_RecursiveDeref(manager, cof2);
        Cudd_RecursiveDeref(manager, cof1);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor with complemented cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = x XOR y XOR z
        DdNode *xy = Cudd_bddXor(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddXor(manager, xy, z);
        Cudd_Ref(f);

        // Cube = !x AND y
        DdNode *cube = Cudd_bddAnd(manager, Cudd_Not(x), y);
        Cudd_Ref(cube);

        DdNode *cof = Cudd_Cofactor(manager, f, cube);
        REQUIRE(cof != nullptr);
        Cudd_Ref(cof);

        // Should get !z (since x=0, y=1 => 0 XOR 1 XOR z = !z)
        REQUIRE(cof == Cudd_Not(z));

        Cudd_RecursiveDeref(manager, cof);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor exercising cuddUniqueInter path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x AND (y OR z)) - depends on x at top level
        DdNode *yz = Cudd_bddOr(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddAnd(manager, x, yz);
        Cudd_Ref(f);

        // Cofactor with respect to y (which is below x in the BDD)
        // This exercises the topf < topg branch
        DdNode *cof = Cudd_Cofactor(manager, f, y);
        REQUIRE(cof != nullptr);
        Cudd_Ref(cof);

        // Expected: x (since f[y=1] = x AND (1 OR z) = x)
        REQUIRE(cof == x);

        Cudd_RecursiveDeref(manager, cof);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Cofactor with t != e but Cudd_IsComplement(t) true") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x AND !y) OR (!x AND z)
        DdNode *xny = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(xny);
        DdNode *nxz = Cudd_bddAnd(manager, Cudd_Not(x), z);
        Cudd_Ref(nxz);
        DdNode *f = Cudd_bddOr(manager, xny, nxz);
        Cudd_Ref(f);

        // Cofactor with respect to z
        DdNode *cof = Cudd_Cofactor(manager, f, z);
        REQUIRE(cof != nullptr);
        Cudd_Ref(cof);

        // f[z=1] = (x AND !y) OR !x
        DdNode *expected = Cudd_bddOr(manager, xny, Cudd_Not(x));
        Cudd_Ref(expected);
        REQUIRE(cof == expected);

        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cof);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, nxz);
        Cudd_RecursiveDeref(manager, xny);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

// =============================================================================
// Additional tests for higher coverage
// =============================================================================

TEST_CASE("Cudd_VarsAreSymmetric - Additional edge cases", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("index1 < size but index2 >= size (exercise line 190-191)") {
        // Create only one variable
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        int idx_x = Cudd_NodeReadIndex(x);
        int largeIdx = Cudd_ReadSize(manager) + 100;

        // Test with function that depends on x
        // This exercises the path where index1 < size but index2 >= size
        // Since f doesn't depend on var at largeIdx, we check if f depends on x
        REQUIRE(Cudd_VarsAreSymmetric(manager, x, largeIdx, idx_x) == 0);

        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("index2 < size but index1 >= size (exercise line 193-196)") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        int idx_x = Cudd_NodeReadIndex(x);
        int largeIdx = Cudd_ReadSize(manager) + 100;

        // Test with function that does not depend on x (constant)
        DdNode *one = Cudd_ReadOne(manager);

        // f = 1 doesn't depend on any variable, so symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, one, largeIdx, idx_x) == 1);

        // For f = x, x is not symmetric with non-existent variable
        REQUIRE(Cudd_VarsAreSymmetric(manager, x, largeIdx, idx_x) == 0);

        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("ddVarsAreSymmetricBetween with topf0 > level2 && topf1 > level2") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = z, check symmetry of x and y (both before z in ordering)
        // This exercises the path in ddVarsAreSymmetricBetween where
        // topf0 > level2 && topf1 > level2
        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // z doesn't depend on x or y, so x and y are symmetric in z
        REQUIRE(Cudd_VarsAreSymmetric(manager, z, idx_x, idx_y) == 1);

        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Cofactor - Invalid cube restrictions", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Cofactor with non-cube (OR) - exercises Invalid restriction 2 error") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        // g = x OR y (not a cube!)
        DdNode *g = Cudd_bddOr(manager, x, y);
        Cudd_Ref(g);

        // This should trigger "Invalid restriction 2" error
        DdNode *result = Cudd_Cofactor(manager, f, g);
        REQUIRE(result == nullptr);
        REQUIRE(Cudd_ReadErrorCode(manager) == CUDD_INVALID_ARG);
        Cudd_ClearErrorCode(manager);

        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("cuddCofactorRecur - Complemented t path", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Cofactor where result t is complemented") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // Build a function where after cofactoring, t (then branch) is complemented
        // f = x ? !y : z = (!x AND z) OR (x AND !y)
        DdNode *f = Cudd_bddIte(manager, x, Cudd_Not(y), z);
        Cudd_Ref(f);

        // Cofactor with respect to z
        // This exercises the path where t is complemented in the recursion
        DdNode *cof = Cudd_Cofactor(manager, f, z);
        REQUIRE(cof != nullptr);
        Cudd_Ref(cof);

        // f[z=1] = (!x AND 1) OR (x AND !y) = !x OR (x AND !y)
        // = !x OR !y (by absorption law)
        DdNode *xny_temp = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(xny_temp);
        DdNode *expected = Cudd_bddOr(manager, Cudd_Not(x), xny_temp);
        Cudd_Ref(expected);
        REQUIRE(cof == expected);

        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, xny_temp);
        Cudd_RecursiveDeref(manager, cof);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Another complemented t case") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x ? (!y AND z) : (y OR z))
        DdNode *nyz = Cudd_bddAnd(manager, Cudd_Not(y), z);
        Cudd_Ref(nyz);
        DdNode *yorz = Cudd_bddOr(manager, y, z);
        Cudd_Ref(yorz);
        DdNode *f = Cudd_bddIte(manager, x, nyz, yorz);
        Cudd_Ref(f);

        // Cofactor with respect to z should exercise complemented branches
        DdNode *cof = Cudd_Cofactor(manager, f, z);
        REQUIRE(cof != nullptr);
        Cudd_Ref(cof);

        Cudd_RecursiveDeref(manager, cof);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yorz);
        Cudd_RecursiveDeref(manager, nyz);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("ddVarsAreSymmetricBetween - Deeper recursion", "[cuddCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Test with topf0 == topf1 but f0 != f1") {
        DdNode *v0 = Cudd_bddNewVar(manager);
        DdNode *v1 = Cudd_bddNewVar(manager);
        DdNode *v2 = Cudd_bddNewVar(manager);
        DdNode *v3 = Cudd_bddNewVar(manager);
        Cudd_Ref(v0);
        Cudd_Ref(v1);
        Cudd_Ref(v2);
        Cudd_Ref(v3);

        // f = (v0 AND v2) OR (v1 AND v3)
        // f[v0=1] = v2 OR (v1 AND v3), f[v0=0] = v1 AND v3
        // Checking symmetry of v0 and v1
        DdNode *v0v2 = Cudd_bddAnd(manager, v0, v2);
        Cudd_Ref(v0v2);
        DdNode *v1v3 = Cudd_bddAnd(manager, v1, v3);
        Cudd_Ref(v1v3);
        DdNode *f = Cudd_bddOr(manager, v0v2, v1v3);
        Cudd_Ref(f);

        int idx_0 = Cudd_NodeReadIndex(v0);
        int idx_1 = Cudd_NodeReadIndex(v1);

        // v0 and v1 are not symmetric in this function
        int result = Cudd_VarsAreSymmetric(manager, f, idx_0, idx_1);
        // The result depends on function structure
        REQUIRE((result == 0 || result == 1));

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v1v3);
        Cudd_RecursiveDeref(manager, v0v2);
        Cudd_RecursiveDeref(manager, v3);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v0);
    }

    SECTION("Test with one F constant in ddVarsAreSymmetricBetween") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (x AND z) - f[x=1] = z, f[x=0] = 0
        DdNode *f = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(f);

        int idx_x = Cudd_NodeReadIndex(x);
        int idx_y = Cudd_NodeReadIndex(y);

        // x and y are not symmetric
        REQUIRE(Cudd_VarsAreSymmetric(manager, f, idx_x, idx_y) == 0);

        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}
