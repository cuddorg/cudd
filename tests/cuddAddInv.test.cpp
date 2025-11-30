#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddAddInv.c
 * 
 * This file contains comprehensive tests for Cudd_addScalarInverse function
 * and the internal cuddAddScalarInverseRecur function to achieve 90%+ coverage.
 */

TEST_CASE("Cudd_addScalarInverse - Constant ADD", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Inverse of constant 2.0 is 0.5") {
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, two, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(0.5, 1e-10));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, two);
    }

    SECTION("Inverse of constant 0.25 is 4.0") {
        DdNode *quarter = Cudd_addConst(manager, 0.25);
        Cudd_Ref(quarter);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, quarter, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(4.0, 1e-10));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, quarter);
    }

    SECTION("Inverse of constant 1.0 is 1.0") {
        DdNode *one = Cudd_addConst(manager, 1.0);
        Cudd_Ref(one);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, one, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(1.0, 1e-10));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, one);
    }

    SECTION("Inverse of negative constant -2.0 is -0.5") {
        DdNode *negTwo = Cudd_addConst(manager, -2.0);
        Cudd_Ref(negTwo);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, negTwo, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(-0.5, 1e-10));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, negTwo);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Invalid epsilon", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Non-constant epsilon returns NULL") {
        DdNode *constTwo = Cudd_addConst(manager, 2.0);
        Cudd_Ref(constTwo);
        
        // Create a non-constant ADD to use as epsilon (invalid)
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        // Redirect stderr to suppress error message during test
        FILE *oldStderr = Cudd_ReadStderr(manager);
        FILE *tmpFile = tmpfile();  // Cross-platform temporary file
        if (tmpFile) {
            Cudd_SetStderr(manager, tmpFile);
        }
        
        DdNode *result = Cudd_addScalarInverse(manager, constTwo, var0);
        
        // Restore stderr
        if (tmpFile) {
            Cudd_SetStderr(manager, oldStderr);
            fclose(tmpFile);
        }
        
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, constTwo);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Value smaller than epsilon", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Returns NULL when absolute value smaller than epsilon") {
        DdNode *small = Cudd_addConst(manager, 1e-12);
        Cudd_Ref(small);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-6);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, small, epsilon);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, small);
    }

    SECTION("Returns NULL for zero value") {
        DdNode *zero = Cudd_addConst(manager, 0.0);
        Cudd_Ref(zero);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, zero, epsilon);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, zero);
    }

    SECTION("Returns NULL when negative value abs is smaller than epsilon") {
        DdNode *negSmall = Cudd_addConst(manager, -1e-12);
        Cudd_Ref(negSmall);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-6);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, negSmall, epsilon);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, negSmall);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Multi-level ADD", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("ADD with single variable") {
        // Create ADD: if x0 then 2.0 else 4.0
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        
        DdNode *four = Cudd_addConst(manager, 4.0);
        Cudd_Ref(four);
        
        DdNode *f = Cudd_addIte(manager, var0, two, four);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Verify: if x0 then 0.5 else 0.25
        REQUIRE(!Cudd_IsConstant(result));
        
        // Check the then branch (value 0.5 for x0=1)
        DdNode *thenBranch = Cudd_T(result);
        REQUIRE(Cudd_IsConstant(thenBranch));
        REQUIRE_THAT(Cudd_V(thenBranch), Catch::Matchers::WithinAbs(0.5, 1e-10));
        
        // Check the else branch (value 0.25 for x0=0)
        DdNode *elseBranch = Cudd_E(result);
        REQUIRE(Cudd_IsConstant(elseBranch));
        REQUIRE_THAT(Cudd_V(elseBranch), Catch::Matchers::WithinAbs(0.25, 1e-10));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, four);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, var0);
    }

    SECTION("ADD with multiple variables") {
        // Create a more complex ADD: if x0 then (if x1 then 2 else 4) else (if x1 then 5 else 10)
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        DdNode *four = Cudd_addConst(manager, 4.0);
        Cudd_Ref(four);
        DdNode *five = Cudd_addConst(manager, 5.0);
        Cudd_Ref(five);
        DdNode *ten = Cudd_addConst(manager, 10.0);
        Cudd_Ref(ten);
        
        DdNode *inner1 = Cudd_addIte(manager, var1, two, four);
        Cudd_Ref(inner1);
        DdNode *inner2 = Cudd_addIte(manager, var1, five, ten);
        Cudd_Ref(inner2);
        DdNode *f = Cudd_addIte(manager, var0, inner1, inner2);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // The result should be non-constant
        REQUIRE(!Cudd_IsConstant(result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, inner2);
        Cudd_RecursiveDeref(manager, inner1);
        Cudd_RecursiveDeref(manager, ten);
        Cudd_RecursiveDeref(manager, five);
        Cudd_RecursiveDeref(manager, four);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Cache hit behavior", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Repeated calls use cache") {
        // Create ADD: if x0 then 2.0 else 4.0
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        
        DdNode *four = Cudd_addConst(manager, 4.0);
        Cudd_Ref(four);
        
        DdNode *f = Cudd_addIte(manager, var0, two, four);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        // First call
        DdNode *result1 = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        
        // Second call should return same result from cache
        DdNode *result2 = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        
        // Results should be the same node
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, four);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Then equals else case", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("t == e case - ADD with same then and else terminals") {
        // Create an ADD where after inverse, then and else are equal
        // if x0 then 2.0 else 2.0 (but structured as non-constant due to construction)
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        
        // This should reduce to just two
        DdNode *f = Cudd_addIte(manager, var0, two, two);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be constant 0.5
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(0.5, 1e-10));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Partial failure in recursion", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Then branch has value below epsilon") {
        // Create ADD: if x0 then 1e-12 else 2.0
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode *small = Cudd_addConst(manager, 1e-12);
        Cudd_Ref(small);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        
        DdNode *f = Cudd_addIte(manager, var0, small, two);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-6);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        // Should fail because then branch has value smaller than epsilon
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, small);
        Cudd_RecursiveDeref(manager, var0);
    }

    SECTION("Else branch has value below epsilon") {
        // Create ADD: if x0 then 2.0 else 1e-12
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        
        DdNode *small = Cudd_addConst(manager, 1e-12);
        Cudd_Ref(small);
        
        DdNode *f = Cudd_addIte(manager, var0, two, small);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-6);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        // Should fail because else branch has value smaller than epsilon
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, small);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Edge cases", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Large values") {
        DdNode *large = Cudd_addConst(manager, 1e10);
        Cudd_Ref(large);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, large, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(1e-10, 1e-15));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, large);
    }

    SECTION("Value exactly at epsilon boundary succeeds") {
        DdNode *boundary = Cudd_addConst(manager, 1e-6);
        Cudd_Ref(boundary);
        
        // Epsilon is same as value - value is NOT smaller, so should succeed
        DdNode *epsilon = Cudd_addConst(manager, 1e-6);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, boundary, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE_THAT(Cudd_V(result), Catch::Matchers::WithinAbs(1e6, 1.0));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, boundary);
    }

    SECTION("ADD with three levels") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode *var2 = Cudd_addIthVar(manager, 2);
        Cudd_Ref(var2);
        
        DdNode *c1 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(c1);
        DdNode *c2 = Cudd_addConst(manager, 4.0);
        Cudd_Ref(c2);
        DdNode *c3 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(c3);
        DdNode *c4 = Cudd_addConst(manager, 8.0);
        Cudd_Ref(c4);
        
        // Build a 3-level ADD
        DdNode *inner1 = Cudd_addIte(manager, var2, c1, c2);
        Cudd_Ref(inner1);
        DdNode *inner2 = Cudd_addIte(manager, var2, c3, c4);
        Cudd_Ref(inner2);
        DdNode *mid1 = Cudd_addIte(manager, var1, inner1, inner2);
        Cudd_Ref(mid1);
        DdNode *mid2 = Cudd_addIte(manager, var1, inner2, inner1);
        Cudd_Ref(mid2);
        DdNode *f = Cudd_addIte(manager, var0, mid1, mid2);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Verify the result is non-constant
        REQUIRE(!Cudd_IsConstant(result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, mid2);
        Cudd_RecursiveDeref(manager, mid1);
        Cudd_RecursiveDeref(manager, inner2);
        Cudd_RecursiveDeref(manager, inner1);
        Cudd_RecursiveDeref(manager, c4);
        Cudd_RecursiveDeref(manager, c3);
        Cudd_RecursiveDeref(manager, c2);
        Cudd_RecursiveDeref(manager, c1);
        Cudd_RecursiveDeref(manager, var2);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addScalarInverse - Verify mathematical correctness", "[cuddAddInv]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Inverse times original equals one (constant)") {
        DdNode *val = Cudd_addConst(manager, 3.0);
        Cudd_Ref(val);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *inv = Cudd_addScalarInverse(manager, val, epsilon);
        REQUIRE(inv != nullptr);
        Cudd_Ref(inv);
        
        // Multiply inv * val
        DdNode *product = Cudd_addApply(manager, Cudd_addTimes, inv, val);
        REQUIRE(product != nullptr);
        Cudd_Ref(product);
        
        REQUIRE(Cudd_IsConstant(product));
        REQUIRE_THAT(Cudd_V(product), Catch::Matchers::WithinAbs(1.0, 1e-10));
        
        Cudd_RecursiveDeref(manager, product);
        Cudd_RecursiveDeref(manager, inv);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, val);
    }

    SECTION("Inverse times original equals one (non-constant ADD)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(two);
        DdNode *five = Cudd_addConst(manager, 5.0);
        Cudd_Ref(five);
        
        DdNode *f = Cudd_addIte(manager, var0, two, five);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        DdNode *inv = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(inv != nullptr);
        Cudd_Ref(inv);
        
        // Multiply inv * f - should give constant 1.0
        DdNode *product = Cudd_addApply(manager, Cudd_addTimes, inv, f);
        REQUIRE(product != nullptr);
        Cudd_Ref(product);
        
        // Result should be constant 1.0
        REQUIRE(Cudd_IsConstant(product));
        REQUIRE_THAT(Cudd_V(product), Catch::Matchers::WithinAbs(1.0, 1e-10));
        
        Cudd_RecursiveDeref(manager, product);
        Cudd_RecursiveDeref(manager, inv);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, five);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}

// Global variable to track if timeout handler was called
static bool g_timeoutHandlerCalled = false;

// Timeout handler callback
extern "C" {
static void timeoutHandler(DdManager *dd, void *arg) {
    (void)dd;
    (void)arg;
    g_timeoutHandlerCalled = true;
}
}

TEST_CASE("Cudd_addScalarInverse - Timeout handler registration", "[cuddAddInv]") {
    // This test verifies that timeout handler can be registered and used with
    // addScalarInverse. Actual timeout triggering depends on timing and computation
    // size, which is system-dependent and not reliably testable.
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Register timeout handler with addScalarInverse operation") {
        // Reset the flag
        g_timeoutHandlerCalled = false;
        
        // Register a timeout handler
        Cudd_RegisterTimeoutHandler(manager, timeoutHandler, nullptr);
        
        // Verify handler is registered by reading it back
        void *argp = nullptr;
        DD_TOHFP handler = Cudd_ReadTimeoutHandler(manager, &argp);
        REQUIRE(handler == timeoutHandler);
        
        // Build an ADD
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *c1 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(c1);
        DdNode *c2 = Cudd_addConst(manager, 4.0);
        Cudd_Ref(c2);
        
        DdNode *f = Cudd_addIte(manager, var0, c1, c2);
        Cudd_Ref(f);
        
        DdNode *epsilon = Cudd_addConst(manager, 1e-10);
        Cudd_Ref(epsilon);
        
        // Normal operation should complete successfully
        DdNode *result = Cudd_addScalarInverse(manager, f, epsilon);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Cleanup
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, epsilon);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, c2);
        Cudd_RecursiveDeref(manager, c1);
        Cudd_RecursiveDeref(manager, var0);
    }

    Cudd_Quit(manager);
}
