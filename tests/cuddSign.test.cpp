#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include <cstdlib>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddSign.c
 * 
 * This file contains comprehensive tests for the cuddSign module
 * to achieve 90%+ code coverage for the Cudd_CofMinterm function
 * and its helper ddCofMintermAux.
 */

TEST_CASE("Cudd_CofMinterm - Constant functions", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Constant one with single variable (DD_ONE)") {
        // Create one variable
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        DdNode *one = Cudd_ReadOne(manager);
        REQUIRE(one != nullptr);

        double *result = Cudd_CofMinterm(manager, one);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 1);
        // For constant one with 1 variable, all positions should be 1.0
        for (int i = 0; i <= size; i++) {
            REQUIRE(result[i] == Catch::Approx(1.0));
        }

        FREE(result);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Constant zero with single variable (DD_ZERO)") {
        // Create one variable
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);

        double *result = Cudd_CofMinterm(manager, zero);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 1);
        // For constant zero, all positions should be 0.0
        for (int i = 0; i <= size; i++) {
            REQUIRE(result[i] == Catch::Approx(0.0));
        }

        FREE(result);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Single variable", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Single positive variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        double *result = Cudd_CofMinterm(manager, x);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 1);

        // For single variable x:
        // - result[0] is the signature for variable at index 0
        // - result[size] = result[1] is the overall fraction (should be 0.5)
        REQUIRE(result[size] == Catch::Approx(0.5));
        // The signature for x's positive cofactor should be 1.0
        REQUIRE(result[0] == Catch::Approx(1.0));

        FREE(result);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("Single complemented variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        DdNode *notX = Cudd_Not(x);

        double *result = Cudd_CofMinterm(manager, notX);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 1);

        // For NOT x:
        // - result[size] should be 0.5 (half minterms)
        // - result[0] is signature for x's positive cofactor (should be 0.0)
        REQUIRE(result[size] == Catch::Approx(0.5));
        REQUIRE(result[0] == Catch::Approx(0.0));

        FREE(result);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Two variables", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("AND of two variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 2);

        // For f = x AND y:
        // - result[size] = 0.25 (1 minterm out of 4)
        REQUIRE(result[size] == Catch::Approx(0.25));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    SECTION("OR of two variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 2);

        // For f = x OR y:
        // - result[size] = 0.75 (3 minterms out of 4)
        REQUIRE(result[size] == Catch::Approx(0.75));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    SECTION("XOR of two variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 2);

        // For f = x XOR y:
        // - result[size] = 0.5 (2 minterms out of 4)
        REQUIRE(result[size] == Catch::Approx(0.5));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Multiple variables", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Three variable function") {
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

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 3);

        // For f = (x AND y) OR z:
        // Minterms: 001, 011, 101, 110, 111 = 5 out of 8 = 0.625
        REQUIRE(result[size] == Catch::Approx(0.625));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }

    SECTION("Five variable function") {
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }

        // f = v0 AND v1 AND v2 AND v3 AND v4
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 5; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 5);

        // For f = v0 AND v1 AND v2 AND v3 AND v4:
        // Only 1 minterm out of 32 = 1/32 = 0.03125
        REQUIRE(result[size] == Catch::Approx(0.03125));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Shared nodes (ref > 1)", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Shared intermediate nodes") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // Create shared structure
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        Cudd_Ref(xy);  // Extra ref to make ref > 1

        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] == Catch::Approx(0.625));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, xy);  // Deref the extra ref
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }

    SECTION("Multiple calls for caching behavior") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        Cudd_Ref(f);  // Extra ref to enable caching

        // First call
        double *result1 = Cudd_CofMinterm(manager, f);
        REQUIRE(result1 != nullptr);

        // Second call on same node should use cached values
        double *result2 = Cudd_CofMinterm(manager, f);
        REQUIRE(result2 != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result1[size] == result2[size]);

        FREE(result1);
        FREE(result2);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Complemented edges", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Complemented function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = NOT(x AND y)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_Not(xy);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // NOT(x AND y) has 3 minterms: 00, 01, 10 = 0.75
        REQUIRE(result[size] == Catch::Approx(0.75));

        FREE(result);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    SECTION("Mixed complemented and non-complemented edges") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = (NOT x AND y) OR z
        DdNode *notX = Cudd_Not(x);
        DdNode *notXy = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(notXy);
        DdNode *f = Cudd_bddOr(manager, notXy, z);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // (NOT x AND y) OR z = 010, 001, 011, 101, 111 = 5/8 = 0.625
        REQUIRE(result[size] == Catch::Approx(0.625));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, notXy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Variable signatures", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Check individual variable signatures") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        // f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);

        // For f = x AND y:
        // - Positive cofactor of x (when x=1) gives y, which has 0.5 fraction
        // - Positive cofactor of y (when y=1) gives x, which has 0.5 fraction
        int xIndex = Cudd_NodeReadIndex(x);
        int yIndex = Cudd_NodeReadIndex(y);

        REQUIRE(result[xIndex] == Catch::Approx(0.5));
        REQUIRE(result[yIndex] == Catch::Approx(0.5));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Edge cases", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Function with skipped levels") {
        // Create variables at specific levels
        DdNode *v0 = Cudd_bddNewVar(manager);
        DdNode *v1 = Cudd_bddNewVar(manager);
        DdNode *v2 = Cudd_bddNewVar(manager);
        Cudd_Ref(v0);
        Cudd_Ref(v1);
        Cudd_Ref(v2);

        // f = v0 AND v2 (skips v1)
        DdNode *f = Cudd_bddAnd(manager, v0, v2);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 3);

        // v0 AND v2 has 2 minterms: 101, 111 = 2/8 = 0.25
        REQUIRE(result[size] == Catch::Approx(0.25));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v0);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v2);
    }

    SECTION("ITE function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // f = ITE(x, y, z) = (x AND y) OR (NOT x AND z)
        DdNode *f = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // ITE(x, y, z): 011, 001, 110, 111 = 4/8 = 0.5
        REQUIRE(result[size] == Catch::Approx(0.5));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Nodes with ref count 1 (free values path)", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Single ref node cleanup") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        // Don't ref x and y, keep their refs at 1

        // Create function with single ref
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] == Catch::Approx(0.25));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
    }

    SECTION("Nested function with ref=1 children") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        // Keep ref counts at default

        // Create nested structure
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        // Don't ref xy to keep its ref at 1
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] == Catch::Approx(0.625));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - ADD constant nodes", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("ADD constant one") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        // Get ADD constant 1
        DdNode *addOne = Cudd_addConst(manager, 1.0);
        Cudd_Ref(addOne);

        double *result = Cudd_CofMinterm(manager, addOne);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] == Catch::Approx(1.0));

        FREE(result);
        Cudd_RecursiveDeref(manager, addOne);
        Cudd_RecursiveDeref(manager, x);
    }

    SECTION("ADD constant zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);

        // Get ADD constant 0
        DdNode *addZero = Cudd_ReadZero(manager);

        double *result = Cudd_CofMinterm(manager, addZero);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] == Catch::Approx(0.0));

        FREE(result);
        Cudd_RecursiveDeref(manager, x);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Complex functions for coverage", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Deep nested structure") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }

        // Build complex function: v0 XOR v1 XOR v2 XOR v3 XOR v4 XOR v5
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 6; i++) {
            DdNode *tmp = Cudd_bddXor(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // XOR of 6 variables has exactly half minterms
        REQUIRE(result[size] == Catch::Approx(0.5));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }

    SECTION("Mix of AND, OR, NOT") {
        DdNode *a = Cudd_bddNewVar(manager);
        DdNode *b = Cudd_bddNewVar(manager);
        DdNode *c = Cudd_bddNewVar(manager);
        DdNode *d = Cudd_bddNewVar(manager);
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(c);
        Cudd_Ref(d);

        // f = (a AND b) OR (NOT c AND d)
        DdNode *ab = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(ab);
        DdNode *notC = Cudd_Not(c);
        DdNode *notCd = Cudd_bddAnd(manager, notC, d);
        Cudd_Ref(notCd);
        DdNode *f = Cudd_bddOr(manager, ab, notCd);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // Result should be a valid fraction
        REQUIRE(result[size] >= 0.0);
        REQUIRE(result[size] <= 1.0);

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, notCd);
        Cudd_RecursiveDeref(manager, ab);
        Cudd_RecursiveDeref(manager, a);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, d);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Large function test", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Eight variables") {
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }

        // f = v0 OR v1 OR v2 OR v3 OR v4 OR v5 OR v6 OR v7
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 8; i++) {
            DdNode *tmp = Cudd_bddOr(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 8);

        // OR of 8 variables: 2^8 - 1 = 255 minterms out of 256
        REQUIRE(result[size] == Catch::Approx(255.0 / 256.0));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Test firstLevel handling for non-constant nodes", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Test i < firstLevel case") {
        // Create variables out of order to test various levels
        DdNode *v0 = Cudd_bddNewVar(manager);
        DdNode *v1 = Cudd_bddNewVar(manager);
        DdNode *v2 = Cudd_bddNewVar(manager);
        Cudd_Ref(v0);
        Cudd_Ref(v1);
        Cudd_Ref(v2);

        // Function that only depends on v2 (higher level)
        DdNode *f = v2;
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // v2 alone has 0.5 fraction
        REQUIRE(result[size] == Catch::Approx(0.5));

        // Check that signatures for variables above the function's top
        // are computed correctly (they should equal the overall minterm fraction)
        int v0Index = Cudd_NodeReadIndex(v0);
        int v1Index = Cudd_NodeReadIndex(v1);
        // For variables above the function's top variable,
        // their positive cofactor is the same as the function itself
        REQUIRE(result[v0Index] == Catch::Approx(0.5));
        REQUIRE(result[v1Index] == Catch::Approx(0.5));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v0);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v2);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Test ddCofMintermAux recursion paths", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Test vT and vE computation paths") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);

        // Create function with different depths in then/else branches
        // f = ITE(x, ITE(y, z, w), w)
        DdNode *inner = Cudd_bddIte(manager, y, z, w);
        Cudd_Ref(inner);
        DdNode *f = Cudd_bddIte(manager, x, inner, w);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 4);
        REQUIRE(result[size] >= 0.0);
        REQUIRE(result[size] <= 1.0);

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, inner);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }

    SECTION("Test constant then and else branches") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);

        // f = ITE(x, 1, y) = x OR y
        DdNode *f = Cudd_bddIte(manager, x, one, y);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // x OR y = 0.75
        REQUIRE(result[size] == Catch::Approx(0.75));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    SECTION("Test function with constant zero in else branch") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);

        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);

        // f = ITE(x, y, 0) = x AND y
        DdNode *f = Cudd_bddIte(manager, x, y, zero);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        // x AND y = 0.25
        REQUIRE(result[size] == Catch::Approx(0.25));

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Test loop coverage in ddCofMintermAux", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Test i < cutoff condition in vT loop") {
        // Create a function where the then branch is at a higher level
        DdNode *v0 = Cudd_bddNewVar(manager);
        DdNode *v1 = Cudd_bddNewVar(manager);
        DdNode *v2 = Cudd_bddNewVar(manager);
        DdNode *v3 = Cudd_bddNewVar(manager);
        Cudd_Ref(v0);
        Cudd_Ref(v1);
        Cudd_Ref(v2);
        Cudd_Ref(v3);

        // f = ITE(v0, v3, v1 AND v2)
        // Then branch (v3) has higher level than v0
        // Else branch (v1 AND v2) has lower level
        DdNode *v1v2 = Cudd_bddAnd(manager, v1, v2);
        Cudd_Ref(v1v2);
        DdNode *f = Cudd_bddIte(manager, v0, v3, v1v2);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] >= 0.0);
        REQUIRE(result[size] <= 1.0);

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v1v2);
        Cudd_RecursiveDeref(manager, v0);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v3);
    }

    SECTION("Test i < cutoff condition in vE loop") {
        // Create a function where the else branch is at a higher level
        DdNode *v0 = Cudd_bddNewVar(manager);
        DdNode *v1 = Cudd_bddNewVar(manager);
        DdNode *v2 = Cudd_bddNewVar(manager);
        DdNode *v3 = Cudd_bddNewVar(manager);
        Cudd_Ref(v0);
        Cudd_Ref(v1);
        Cudd_Ref(v2);
        Cudd_Ref(v3);

        // f = ITE(v0, v1 AND v2, v3)
        // Then branch (v1 AND v2) has lower level
        // Else branch (v3) has higher level than v0
        DdNode *v1v2 = Cudd_bddAnd(manager, v1, v2);
        Cudd_Ref(v1v2);
        DdNode *f = Cudd_bddIte(manager, v0, v1v2, v3);
        Cudd_Ref(f);

        double *result = Cudd_CofMinterm(manager, f);
        REQUIRE(result != nullptr);

        int size = Cudd_ReadSize(manager);
        REQUIRE(result[size] >= 0.0);
        REQUIRE(result[size] <= 1.0);

        FREE(result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v1v2);
        Cudd_RecursiveDeref(manager, v0);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v3);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_CofMinterm - Test st_add_direct cache path", "[cuddSign]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Force cache insertion with ref > 1") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);

        // Create shared node
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        Cudd_Ref(xy);  // Make ref > 1

        // Use xy in multiple places to ensure it gets cached
        DdNode *f1 = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddXor(manager, xy, z);
        Cudd_Ref(f2);

        // Both f1 and f2 share xy, so when we call Cudd_CofMinterm on f1,
        // xy should be cached for potential reuse
        double *result1 = Cudd_CofMinterm(manager, f1);
        REQUIRE(result1 != nullptr);

        double *result2 = Cudd_CofMinterm(manager, f2);
        REQUIRE(result2 != nullptr);

        FREE(result1);
        FREE(result2);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }

    Cudd_Quit(manager);
}
