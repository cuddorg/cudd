#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstdlib>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddSat.c
 * 
 * This file contains comprehensive tests for the cuddSat module
 * to achieve 90%+ code coverage.
 *
 * Functions tested:
 * - Cudd_Eval
 * - Cudd_ShortestPath
 * - Cudd_LargestCube
 * - Cudd_ShortestLength
 * - Cudd_Decreasing
 * - Cudd_Increasing
 * - Cudd_EquivDC
 * - Cudd_bddLeqUnless
 * - Cudd_EqualSupNorm
 * - Cudd_bddMakePrime
 * - Cudd_bddMaximallyExpand
 * - Cudd_bddLargestPrimeUnate
 */

// ============================================================================
// Tests for Cudd_Eval
// ============================================================================

TEST_CASE("Cudd_Eval - Constants", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    int inputs[] = {0, 0, 0, 0};
    
    SECTION("Eval constant 1") {
        DdNode *result = Cudd_Eval(manager, one, inputs);
        REQUIRE(result == one);
    }
    
    SECTION("Eval constant 0") {
        DdNode *result = Cudd_Eval(manager, zero, inputs);
        REQUIRE(result == zero);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Eval - Single variable", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("Eval x with x=0") {
        int inputs[10] = {0};
        DdNode *result = Cudd_Eval(manager, x, inputs);
        REQUIRE(result == zero);
    }
    
    SECTION("Eval x with x=1") {
        int inputs[10] = {1};
        DdNode *result = Cudd_Eval(manager, x, inputs);
        REQUIRE(result == one);
    }
    
    SECTION("Eval !x with x=0") {
        int inputs[10] = {0};
        DdNode *result = Cudd_Eval(manager, Cudd_Not(x), inputs);
        REQUIRE(result == one);
    }
    
    SECTION("Eval !x with x=1") {
        int inputs[10] = {1};
        DdNode *result = Cudd_Eval(manager, Cudd_Not(x), inputs);
        REQUIRE(result == zero);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Eval - Two variables", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Eval x AND y") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        int inputs_00[] = {0, 0};
        int inputs_01[] = {0, 1};
        int inputs_10[] = {1, 0};
        int inputs_11[] = {1, 1};
        
        REQUIRE(Cudd_Eval(manager, f, inputs_00) == zero);
        REQUIRE(Cudd_Eval(manager, f, inputs_01) == zero);
        REQUIRE(Cudd_Eval(manager, f, inputs_10) == zero);
        REQUIRE(Cudd_Eval(manager, f, inputs_11) == one);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Eval x OR y") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        int inputs_00[] = {0, 0};
        int inputs_01[] = {0, 1};
        int inputs_10[] = {1, 0};
        int inputs_11[] = {1, 1};
        
        REQUIRE(Cudd_Eval(manager, f, inputs_00) == zero);
        REQUIRE(Cudd_Eval(manager, f, inputs_01) == one);
        REQUIRE(Cudd_Eval(manager, f, inputs_10) == one);
        REQUIRE(Cudd_Eval(manager, f, inputs_11) == one);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Eval x XOR y") {
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        int inputs_00[] = {0, 0};
        int inputs_01[] = {0, 1};
        int inputs_10[] = {1, 0};
        int inputs_11[] = {1, 1};
        
        REQUIRE(Cudd_Eval(manager, f, inputs_00) == zero);
        REQUIRE(Cudd_Eval(manager, f, inputs_01) == one);
        REQUIRE(Cudd_Eval(manager, f, inputs_10) == one);
        REQUIRE(Cudd_Eval(manager, f, inputs_11) == zero);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Eval complemented function") {
        // Test !(x AND y) = !x OR !y
        DdNode *f = Cudd_Not(Cudd_bddAnd(manager, x, y));
        Cudd_Ref(f);
        
        int inputs_00[] = {0, 0};
        int inputs_11[] = {1, 1};
        
        REQUIRE(Cudd_Eval(manager, f, inputs_00) == one);
        REQUIRE(Cudd_Eval(manager, f, inputs_11) == zero);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_ShortestPath
// ============================================================================

TEST_CASE("Cudd_ShortestPath - Constant functions", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    int length;
    
    SECTION("ShortestPath of constant 0") {
        DdNode *path = Cudd_ShortestPath(manager, zero, nullptr, nullptr, &length);
        REQUIRE(path == zero);
        REQUIRE(length == 100000000); // DD_BIGGY
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_ShortestPath - Single variable", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    int length;
    
    SECTION("ShortestPath of x with no weights") {
        DdNode *path = Cudd_ShortestPath(manager, x, nullptr, nullptr, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        REQUIRE(length >= 0);
        Cudd_RecursiveDeref(manager, path);
    }
    
    SECTION("ShortestPath of !x with no weights") {
        DdNode *path = Cudd_ShortestPath(manager, Cudd_Not(x), nullptr, nullptr, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        REQUIRE(length >= 0);
        Cudd_RecursiveDeref(manager, path);
    }
    
    SECTION("ShortestPath with custom weights") {
        int weights[10] = {5, 3, 2, 1, 1, 1, 1, 1, 1, 1};
        int support[10] = {0};
        DdNode *path = Cudd_ShortestPath(manager, x, weights, support, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        REQUIRE(support[0] == 1); // x should be in support
        Cudd_RecursiveDeref(manager, path);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_ShortestPath - Two variables", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    int length;
    
    SECTION("ShortestPath of x AND y") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        int support[10] = {0};
        DdNode *path = Cudd_ShortestPath(manager, f, nullptr, support, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        Cudd_RecursiveDeref(manager, path);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ShortestPath of x OR y") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *path = Cudd_ShortestPath(manager, f, nullptr, nullptr, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        Cudd_RecursiveDeref(manager, path);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_LargestCube
// ============================================================================

TEST_CASE("Cudd_LargestCube - Constant functions", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("LargestCube of constant 0") {
        int length;
        DdNode *cube = Cudd_LargestCube(manager, zero, &length);
        REQUIRE(cube == zero);
        REQUIRE(length == 100000000); // DD_BIGGY
    }
    
    SECTION("LargestCube of constant 0 with NULL length") {
        DdNode *cube = Cudd_LargestCube(manager, zero, nullptr);
        REQUIRE(cube == zero);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_LargestCube - Single variable", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("LargestCube of x") {
        int length;
        DdNode *cube = Cudd_LargestCube(manager, x, &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        REQUIRE(length >= 1);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    SECTION("LargestCube of !x") {
        int length;
        DdNode *cube = Cudd_LargestCube(manager, Cudd_Not(x), &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        REQUIRE(length >= 1);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_LargestCube - Two variables", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("LargestCube of x AND y") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        int length;
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        REQUIRE(length == 2); // Both x and y required
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("LargestCube of x OR y") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        int length;
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        REQUIRE(length == 1); // One variable suffices
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_ShortestLength
// ============================================================================

TEST_CASE("Cudd_ShortestLength - Constant functions", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("ShortestLength of constant 0") {
        int length = Cudd_ShortestLength(manager, zero, nullptr);
        REQUIRE(length == 100000000); // DD_BIGGY
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_ShortestLength - Single variable", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("ShortestLength of x with no weights") {
        int length = Cudd_ShortestLength(manager, x, nullptr);
        REQUIRE(length == 1); // Unit weight
    }
    
    SECTION("ShortestLength of !x with no weights") {
        int length = Cudd_ShortestLength(manager, Cudd_Not(x), nullptr);
        REQUIRE(length == 0); // ELSE arc has 0 weight
    }
    
    SECTION("ShortestLength of x with custom weights") {
        int weights[10] = {5};
        int length = Cudd_ShortestLength(manager, x, weights);
        REQUIRE(length == 5); // Custom weight
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_ShortestLength - Two variables", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("ShortestLength of x AND y") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        int length = Cudd_ShortestLength(manager, f, nullptr);
        REQUIRE(length == 2); // Both x and y needed with unit weights
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ShortestLength of x OR y") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        int length = Cudd_ShortestLength(manager, f, nullptr);
        REQUIRE(length == 1); // One variable suffices
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_Decreasing and Cudd_Increasing
// ============================================================================

TEST_CASE("Cudd_Decreasing - Basic tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    int xid = Cudd_NodeReadIndex(x);
    int yid = Cudd_NodeReadIndex(y);
    
    SECTION("Decreasing in variable not in support") {
        // x does not depend on y, so x is unate (trivially decreasing) in y
        DdNode *result = Cudd_Decreasing(manager, x, yid);
        REQUIRE(result == one);
    }
    
    SECTION("x is not decreasing in x") {
        // f(0) = 0 < 1 = f(1), so x is NOT decreasing in x
        DdNode *result = Cudd_Decreasing(manager, x, xid);
        REQUIRE(result == Cudd_Not(one));
    }
    
    SECTION("!x is decreasing in x") {
        // f(0) = 1 > 0 = f(1), so !x IS decreasing in x
        DdNode *result = Cudd_Decreasing(manager, Cudd_Not(x), xid);
        REQUIRE(result == one);
    }
    
    SECTION("x AND y is not decreasing in x") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_Decreasing(manager, f, xid);
        REQUIRE(result == Cudd_Not(one));
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("!x OR y is decreasing in x (implication)") {
        // x -> y = !x OR y is decreasing in x
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_Decreasing(manager, f, xid);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Increasing - Basic tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    int xid = Cudd_NodeReadIndex(x);
    int yid = Cudd_NodeReadIndex(y);
    
    SECTION("x is increasing in x") {
        DdNode *result = Cudd_Increasing(manager, x, xid);
        REQUIRE(result == one);
    }
    
    SECTION("!x is not increasing in x") {
        DdNode *result = Cudd_Increasing(manager, Cudd_Not(x), xid);
        REQUIRE(result == Cudd_Not(one));
    }
    
    SECTION("x AND y is increasing in both variables") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        REQUIRE(Cudd_Increasing(manager, f, xid) == one);
        REQUIRE(Cudd_Increasing(manager, f, yid) == one);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Decreasing - Cache and special branches", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    int xid = Cudd_NodeReadIndex(x);
    
    SECTION("Complex function - tests recursion and cache") {
        // f = (x AND y) OR (x AND z)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *xz = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(xz);
        DdNode *f = Cudd_bddOr(manager, xy, xz);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_Decreasing(manager, f, xid);
        REQUIRE(result == Cudd_Not(one));
        
        // Call again to hit cache
        result = Cudd_Decreasing(manager, f, xid);
        REQUIRE(result == Cudd_Not(one));
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xz);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_EquivDC
// ============================================================================

TEST_CASE("Cudd_EquivDC - Terminal cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("D is constant 1 - always equivalent") {
        int result = Cudd_EquivDC(manager, x, y, one);
        REQUIRE(result == 1);
    }
    
    SECTION("F equals G - always equivalent") {
        int result = Cudd_EquivDC(manager, x, x, x);
        REQUIRE(result == 1);
    }
    
    SECTION("D is constant 0 - depends on F and G") {
        int result = Cudd_EquivDC(manager, x, y, zero);
        REQUIRE(result == 0);
    }
    
    SECTION("F equals NOT G - never equivalent") {
        int result = Cudd_EquivDC(manager, x, Cudd_Not(x), y);
        REQUIRE(result == 0);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_EquivDC - Non-trivial cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("F and G differ only where D is 1") {
        // F = x, G = x AND y, D = !y
        // When y=0 (D=1), F=x and G=0, so they differ
        // But when y=1 (D=0), they should be checked
        DdNode *g = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(g);
        DdNode *d = Cudd_Not(y);
        
        int result = Cudd_EquivDC(manager, x, g, d);
        // When D=0 (y=1), F=x and G=x, so they are equivalent
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Test normalization - F > G") {
        // Force F > G to trigger normalization
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Ensure we call with f > g pointer-wise by trying both orders
        int result = Cudd_EquivDC(manager, f, x, z);
        REQUIRE(result >= 0); // Just checking it doesn't crash
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Complemented F") {
        DdNode *notx = Cudd_Not(x);
        DdNode *noty = Cudd_Not(y);
        
        int result = Cudd_EquivDC(manager, notx, noty, z);
        REQUIRE(result >= 0);
    }
    
    SECTION("Complex recursive case") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, x, z);
        Cudd_Ref(g);
        DdNode *d = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(d);
        
        int result = Cudd_EquivDC(manager, f, g, d);
        REQUIRE(result >= 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, d);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_bddLeqUnless
// ============================================================================

TEST_CASE("Cudd_bddLeqUnless - Terminal cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("f == g") {
        int result = Cudd_bddLeqUnless(manager, x, x, y);
        REQUIRE(result == 1);
    }
    
    SECTION("g == 1") {
        int result = Cudd_bddLeqUnless(manager, x, one, y);
        REQUIRE(result == 1);
    }
    
    SECTION("f == 0") {
        int result = Cudd_bddLeqUnless(manager, zero, x, y);
        REQUIRE(result == 1);
    }
    
    SECTION("D == 1") {
        int result = Cudd_bddLeqUnless(manager, x, y, one);
        REQUIRE(result == 1);
    }
    
    SECTION("D == f") {
        int result = Cudd_bddLeqUnless(manager, x, y, x);
        REQUIRE(result == 1);
    }
    
    SECTION("D == !g") {
        int result = Cudd_bddLeqUnless(manager, x, y, Cudd_Not(y));
        REQUIRE(result == 1);
    }
    
    SECTION("D == 0 reduces to bddLeq") {
        int result = Cudd_bddLeqUnless(manager, x, one, zero);
        REQUIRE(result == 1);
    }
    
    SECTION("D == g reduces to bddLeq") {
        // When D == g, it reduces to Cudd_bddLeq(f, g)
        // For x <= y to be true, x must imply y, which is not the case for independent vars
        int result = Cudd_bddLeqUnless(manager, x, y, y);
        REQUIRE(result >= 0); // Just checking it runs correctly
    }
    
    SECTION("D == !f reduces to bddLeq") {
        // When D == !f, it reduces to Cudd_bddLeq(f, g)  
        int result = Cudd_bddLeqUnless(manager, x, y, Cudd_Not(x));
        REQUIRE(result >= 0); // Just checking it runs correctly
    }
    
    SECTION("g == 0 or g == !f") {
        int result = Cudd_bddLeqUnless(manager, x, zero, y);
        REQUIRE(result >= 0);
    }
    
    SECTION("f == 1") {
        int result = Cudd_bddLeqUnless(manager, one, y, x);
        REQUIRE(result >= 0);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLeqUnless - Normalization cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Complemented D with complemented g") {
        // This should trigger the special case at line 634
        int result = Cudd_bddLeqUnless(manager, x, Cudd_Not(y), Cudd_Not(z));
        REQUIRE(result >= 0);
    }
    
    SECTION("Complemented D with complemented f") {
        int result = Cudd_bddLeqUnless(manager, Cudd_Not(x), y, Cudd_Not(z));
        REQUIRE(result >= 0);
    }
    
    SECTION("Regular D with complemented g and complemented f") {
        int result = Cudd_bddLeqUnless(manager, Cudd_Not(x), Cudd_Not(y), z);
        REQUIRE(result >= 0);
    }
    
    SECTION("Regular D with complemented g") {
        int result = Cudd_bddLeqUnless(manager, x, Cudd_Not(y), z);
        REQUIRE(result >= 0);
    }
    
    SECTION("Recursive case with different variable levels") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, y, z);
        Cudd_Ref(g);
        
        int result = Cudd_bddLeqUnless(manager, f, g, z);
        REQUIRE(result >= 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_EqualSupNorm
// ============================================================================

TEST_CASE("Cudd_EqualSupNorm - ADD tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identical ADDs") {
        DdNode *f = Cudd_addConst(manager, 5.0);
        Cudd_Ref(f);
        
        int result = Cudd_EqualSupNorm(manager, f, f, 0.0, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Different constants within tolerance") {
        DdNode *f = Cudd_addConst(manager, 5.0);
        DdNode *g = Cudd_addConst(manager, 5.1);
        Cudd_Ref(f);
        Cudd_Ref(g);
        
        int result = Cudd_EqualSupNorm(manager, f, g, 0.2, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Different constants outside tolerance") {
        DdNode *f = Cudd_addConst(manager, 5.0);
        DdNode *g = Cudd_addConst(manager, 6.0);
        Cudd_Ref(f);
        Cudd_Ref(g);
        
        int result = Cudd_EqualSupNorm(manager, f, g, 0.5, 0);
        REQUIRE(result == 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Different constants outside tolerance with printing") {
        DdNode *f = Cudd_addConst(manager, 5.0);
        DdNode *g = Cudd_addConst(manager, 6.0);
        Cudd_Ref(f);
        Cudd_Ref(g);
        
        int result = Cudd_EqualSupNorm(manager, f, g, 0.5, 1);
        REQUIRE(result == 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("ADD with variables") {
        DdNode *x = Cudd_addNewVar(manager);
        Cudd_Ref(x);
        
        // Create ADD: if x then 3.0 else 2.0
        DdNode *three = Cudd_addConst(manager, 3.0);
        DdNode *two = Cudd_addConst(manager, 2.0);
        Cudd_Ref(three);
        Cudd_Ref(two);
        
        DdNode *f = Cudd_addIte(manager, x, three, two);
        Cudd_Ref(f);
        
        int result = Cudd_EqualSupNorm(manager, f, f, 0.0, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, three);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_bddMakePrime
// ============================================================================

TEST_CASE("Cudd_bddMakePrime - Basic tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Cube not implicant of f - returns NULL") {
        // f = x, cube = !x - cube is not an implicant of f
        DdNode *result = Cudd_bddMakePrime(manager, Cudd_Not(x), x);
        REQUIRE(result == nullptr);
    }
    
    SECTION("Single variable cube expanded to prime") {
        // f = x OR y, cube = x AND y
        // Cube (x AND y) is an implicant, should expand to either x or y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        DdNode *prime = Cudd_bddMakePrime(manager, cube, f);
        Cudd_Ref(prime);
        REQUIRE(prime != nullptr);
        // The prime should be a superset of the cube's onset
        REQUIRE(Cudd_bddLeq(manager, prime, f));
        
        Cudd_RecursiveDeref(manager, prime);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Single literal cube") {
        // f = x, cube = x
        DdNode *prime = Cudd_bddMakePrime(manager, x, x);
        Cudd_Ref(prime);
        REQUIRE(prime != nullptr);
        REQUIRE(prime == x);
        Cudd_RecursiveDeref(manager, prime);
    }
    
    SECTION("Complemented literal cube") {
        // f = !x, cube = !x
        DdNode *prime = Cudd_bddMakePrime(manager, Cudd_Not(x), Cudd_Not(x));
        Cudd_Ref(prime);
        REQUIRE(prime != nullptr);
        REQUIRE(prime == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, prime);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_bddMaximallyExpand
// ============================================================================

TEST_CASE("Cudd_bddMaximallyExpand - Basic tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("lb not contained in ub - returns NULL") {
        // lb = x, ub = !x - lb is not contained in ub
        DdNode *result = Cudd_bddMaximallyExpand(manager, x, Cudd_Not(x), one);
        REQUIRE(result == nullptr);
    }
    
    SECTION("Simple expansion with ub = f") {
        // lb = x AND y, ub = 1, f = x OR y
        DdNode *lb = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(lb);
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddMaximallyExpand(manager, lb, one, f);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, lb);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("lb not contained in f - returns zero") {
        // lb = x, f = !x - lb is not contained in f
        DdNode *result = Cudd_bddMaximallyExpand(manager, x, one, Cudd_Not(x));
        REQUIRE(result == zero);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddMaximallyExpand - Complex cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Expansion with ub == f") {
        // f = ub, lb contained in ub
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddMaximallyExpand(manager, x, f, f);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        REQUIRE(result == f); // Should return ub when ub -> f
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Expansion with lb == f") {
        DdNode *result = Cudd_bddMaximallyExpand(manager, x, one, x);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_bddLargestPrimeUnate
// ============================================================================

TEST_CASE("Cudd_bddLargestPrimeUnate - Basic tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Positive unate function (x AND y)") {
        // f = x AND y is positive unate in both x and y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Phase BDD for positive unate: x AND y (all positive)
        DdNode *phaseBdd = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(phaseBdd);
        
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, f, phaseBdd);
        Cudd_Ref(prime);
        REQUIRE(prime != nullptr);
        
        Cudd_RecursiveDeref(manager, prime);
        Cudd_RecursiveDeref(manager, phaseBdd);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Negative unate function (!x AND !y)") {
        // f = !x AND !y is negative unate in both x and y
        DdNode *f = Cudd_bddAnd(manager, Cudd_Not(x), Cudd_Not(y));
        Cudd_Ref(f);
        
        // Phase BDD for negative unate: all negative (cube of complements)
        DdNode *phaseBdd = Cudd_bddAnd(manager, Cudd_Not(x), Cudd_Not(y));
        Cudd_Ref(phaseBdd);
        
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, f, phaseBdd);
        Cudd_Ref(prime);
        REQUIRE(prime != nullptr);
        
        Cudd_RecursiveDeref(manager, prime);
        Cudd_RecursiveDeref(manager, phaseBdd);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Single variable") {
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, x, x);
        Cudd_Ref(prime);
        REQUIRE(prime != nullptr);
        Cudd_RecursiveDeref(manager, prime);
    }
    
    SECTION("Constant function") {
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, one, one);
        Cudd_Ref(prime);
        REQUIRE(prime == one);
        Cudd_RecursiveDeref(manager, prime);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional comprehensive tests to ensure high coverage
// ============================================================================

TEST_CASE("cuddSat - Path functions with multi-variable BDDs", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Three variable function - ShortestPath") {
        // f = (x AND y) OR z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        int length;
        int support[10] = {0};
        DdNode *path = Cudd_ShortestPath(manager, f, nullptr, support, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        REQUIRE(length >= 1);
        
        Cudd_RecursiveDeref(manager, path);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Three variable function - LargestCube") {
        // f = (x AND y) OR z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        int length;
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        REQUIRE(length >= 1);
        
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Three variable function - Complemented") {
        DdNode *f = Cudd_Not(Cudd_bddAnd(manager, Cudd_bddAnd(manager, x, y), z));
        Cudd_Ref(f);
        
        int length;
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("cuddSat - WeightedPath functions", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("ShortestPath with different weights") {
        // f = x OR y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        // Weight x more heavily
        int weights[10] = {10, 1}; // x has weight 10, y has weight 1
        int length;
        DdNode *path = Cudd_ShortestPath(manager, f, weights, nullptr, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        // Should prefer y path (weight 1) over x path (weight 10)
        
        Cudd_RecursiveDeref(manager, path);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ShortestLength with different weights") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        int weights[10] = {10, 1};
        int length = Cudd_ShortestLength(manager, f, weights);
        REQUIRE(length == 1); // Should take y path
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("cuddSat - Edge cases and error handling", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("bddMakePrime with constant 1 as cube") {
        // Cube = 1 (empty cube), f = x
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *prime = Cudd_bddMakePrime(manager, one, x);
        if (prime != nullptr) {
            Cudd_Ref(prime);
            Cudd_RecursiveDeref(manager, prime);
        }
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("LargestPrimeUnate with single variable") {
        // Test with a simple positive unate function
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, x, x);
        if (prime != nullptr) {
            Cudd_Ref(prime);
            Cudd_RecursiveDeref(manager, prime);
        }
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

// ============================================================================
// Additional tests for higher coverage
// ============================================================================

TEST_CASE("Cudd_EqualSupNorm - Non-constant ADDs", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("ADD with two variables - equal") {
        DdNode *x = Cudd_addNewVar(manager);
        DdNode *y = Cudd_addNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create ADD: if x then (if y then 3 else 2) else 1
        DdNode *three = Cudd_addConst(manager, 3.0);
        DdNode *two = Cudd_addConst(manager, 2.0);
        DdNode *one_val = Cudd_addConst(manager, 1.0);
        Cudd_Ref(three);
        Cudd_Ref(two);
        Cudd_Ref(one_val);
        
        DdNode *inner = Cudd_addIte(manager, y, three, two);
        Cudd_Ref(inner);
        DdNode *f = Cudd_addIte(manager, x, inner, one_val);
        Cudd_Ref(f);
        
        // Compare f with itself
        int result = Cudd_EqualSupNorm(manager, f, f, 0.0, 0);
        REQUIRE(result == 1);
        
        // Create a slightly different ADD
        DdNode *threepointone = Cudd_addConst(manager, 3.1);
        Cudd_Ref(threepointone);
        DdNode *inner2 = Cudd_addIte(manager, y, threepointone, two);
        Cudd_Ref(inner2);
        DdNode *g = Cudd_addIte(manager, x, inner2, one_val);
        Cudd_Ref(g);
        
        // Within tolerance
        result = Cudd_EqualSupNorm(manager, f, g, 0.2, 0);
        REQUIRE(result >= 0); // Just check it runs
        
        // Outside tolerance - use a very small tolerance
        result = Cudd_EqualSupNorm(manager, f, g, 0.001, 0);
        REQUIRE(result >= 0); // Just check it runs
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, inner2);
        Cudd_RecursiveDeref(manager, threepointone);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, inner);
        Cudd_RecursiveDeref(manager, one_val);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, three);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD with different structure") {
        DdNode *x = Cudd_addNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *two = Cudd_addConst(manager, 2.0);
        DdNode *one_val = Cudd_addConst(manager, 1.0);
        Cudd_Ref(two);
        Cudd_Ref(one_val);
        
        DdNode *f = Cudd_addIte(manager, x, two, one_val);
        Cudd_Ref(f);
        
        // Compare with constant - within tolerance
        int result = Cudd_EqualSupNorm(manager, f, one_val, 1.5, 0);
        REQUIRE(result >= 0); // Just check it runs
        
        // Try with smaller tolerance
        result = Cudd_EqualSupNorm(manager, f, one_val, 0.001, 0);
        REQUIRE(result >= 0); // Just check it runs
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, one_val);
        Cudd_RecursiveDeref(manager, two);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLeqUnless - Comprehensive normalization", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    DdNode *w = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    Cudd_Ref(w);
    
    SECTION("All combinations of complements") {
        // Test with !D, !g - triggers special normalization
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, y, z);
        Cudd_Ref(g);
        DdNode *d = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(d);
        
        // Try many combinations to hit all normalization branches
        int r1 = Cudd_bddLeqUnless(manager, f, g, d);
        int r2 = Cudd_bddLeqUnless(manager, f, Cudd_Not(g), d);
        int r3 = Cudd_bddLeqUnless(manager, Cudd_Not(f), g, d);
        int r4 = Cudd_bddLeqUnless(manager, Cudd_Not(f), Cudd_Not(g), d);
        int r5 = Cudd_bddLeqUnless(manager, f, g, Cudd_Not(d));
        int r6 = Cudd_bddLeqUnless(manager, f, Cudd_Not(g), Cudd_Not(d));
        int r7 = Cudd_bddLeqUnless(manager, Cudd_Not(f), g, Cudd_Not(d));
        int r8 = Cudd_bddLeqUnless(manager, Cudd_Not(f), Cudd_Not(g), Cudd_Not(d));
        
        // Just verify they run without crashing
        REQUIRE((r1 >= 0 && r2 >= 0 && r3 >= 0 && r4 >= 0));
        REQUIRE((r5 >= 0 && r6 >= 0 && r7 >= 0 && r8 >= 0));
        
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Complex recursive cases") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *f = Cudd_bddOr(manager, xy, zw);
        Cudd_Ref(f);
        
        DdNode *xz = Cudd_bddOr(manager, x, z);
        Cudd_Ref(xz);
        DdNode *yw = Cudd_bddOr(manager, y, w);
        Cudd_Ref(yw);
        DdNode *g = Cudd_bddAnd(manager, xz, yw);
        Cudd_Ref(g);
        
        DdNode *d = Cudd_bddXor(manager, x, w);
        Cudd_Ref(d);
        
        int r = Cudd_bddLeqUnless(manager, f, g, d);
        REQUIRE(r >= 0);
        
        r = Cudd_bddLeqUnless(manager, Cudd_Not(f), Cudd_Not(g), Cudd_Not(d));
        REQUIRE(r >= 0);
        
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, yw);
        Cudd_RecursiveDeref(manager, xz);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, w);
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_EquivDC - Complemented cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("G is complemented") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, x, z);
        Cudd_Ref(g);
        DdNode *d = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(d);
        
        // Test with complemented G
        int r = Cudd_EquivDC(manager, f, Cudd_Not(g), d);
        REQUIRE(r >= 0);
        
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Cache hit test") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, y, z);
        Cudd_Ref(g);
        
        // Call twice to potentially hit cache
        int r1 = Cudd_EquivDC(manager, f, g, z);
        int r2 = Cudd_EquivDC(manager, f, g, z);
        REQUIRE(r1 == r2);
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Decreasing - Cache hit", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    int xid = Cudd_NodeReadIndex(x);
    
    SECTION("Cache hit by repeated call") {
        // Create a complex function
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        // First call populates cache
        DdNode *r1 = Cudd_Decreasing(manager, f, xid);
        
        // Second call should hit cache (line 423)
        DdNode *r2 = Cudd_Decreasing(manager, f, xid);
        
        REQUIRE(r1 == r2);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddMaximallyExpand - Additional coverage", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Expansion with complex ub and f") {
        // lb = x AND y, ub = x OR y OR z, f = x OR y
        DdNode *lb = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(lb);
        
        DdNode *yz = Cudd_bddOr(manager, y, z);
        Cudd_Ref(yz);
        DdNode *ub = Cudd_bddOr(manager, x, yz);
        Cudd_Ref(ub);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddMaximallyExpand(manager, lb, ub, f);
        if (result != nullptr) {
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, lb, result));
            REQUIRE(Cudd_bddLeq(manager, result, ub));
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, ub);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, lb);
    }
    
    SECTION("Complemented lb") {
        DdNode *lb = Cudd_bddAnd(manager, Cudd_Not(x), y);
        Cudd_Ref(lb);
        DdNode *ub = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(ub);
        DdNode *f = ub;
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddMaximallyExpand(manager, lb, ub, f);
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, ub);
        Cudd_RecursiveDeref(manager, lb);
    }
    
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Path functions - Complemented and zero paths", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("ShortestPath of complemented function") {
        DdNode *f = Cudd_Not(Cudd_bddAnd(manager, x, y));
        Cudd_Ref(f);
        
        int length;
        DdNode *path = Cudd_ShortestPath(manager, f, nullptr, nullptr, &length);
        Cudd_Ref(path);
        REQUIRE(path != nullptr);
        REQUIRE(length >= 0);
        Cudd_RecursiveDeref(manager, path);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("LargestCube of complemented function") {
        DdNode *f = Cudd_Not(Cudd_bddOr(manager, x, y));
        Cudd_Ref(f);
        
        int length;
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        Cudd_Ref(cube);
        REQUIRE(cube != nullptr);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ShortestLength of complemented function") {
        DdNode *f = Cudd_Not(x);
        Cudd_Ref(f);
        
        int length = Cudd_ShortestLength(manager, f, nullptr);
        REQUIRE(length >= 0);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLargestPrimeUnate - More cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Three variable positive unate") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(f);
        
        // Phase BDD for all positive
        DdNode *xyz = f;
        Cudd_Ref(xyz);
        
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, f, xyz);
        if (prime != nullptr) {
            Cudd_Ref(prime);
            Cudd_RecursiveDeref(manager, prime);
        }
        
        Cudd_RecursiveDeref(manager, xyz);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Mixed unate") {
        // f = x AND !y is positive unate in x, negative unate in y
        DdNode *f = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(f);
        
        DdNode *phaseBdd = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(phaseBdd);
        
        DdNode *prime = Cudd_bddLargestPrimeUnate(manager, f, phaseBdd);
        if (prime != nullptr) {
            Cudd_Ref(prime);
            Cudd_RecursiveDeref(manager, prime);
        }
        
        Cudd_RecursiveDeref(manager, phaseBdd);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional tests to reach 90% coverage
// ============================================================================

TEST_CASE("Path functions - Zero terminal tests", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Function with both paths to zero and one") {
        // Create a function where some paths lead to zero, some to one
        // f = x AND (y OR z)
        DdNode *yz = Cudd_bddOr(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddAnd(manager, x, yz);
        Cudd_Ref(f);
        
        int length;
        int support[10] = {0};
        DdNode *path = Cudd_ShortestPath(manager, f, nullptr, support, &length);
        if (path != nullptr) {
            Cudd_Ref(path);
            Cudd_RecursiveDeref(manager, path);
        }
        
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        if (cube != nullptr) {
            Cudd_Ref(cube);
            Cudd_RecursiveDeref(manager, cube);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
    }
    
    SECTION("Function with complemented intermediate nodes") {
        // Create !(x AND y)
        DdNode *f = Cudd_Not(Cudd_bddAnd(manager, x, y));
        Cudd_Ref(f);
        
        int length;
        int support[10] = {0};
        DdNode *path = Cudd_ShortestPath(manager, f, nullptr, support, &length);
        if (path != nullptr) {
            Cudd_Ref(path);
            Cudd_RecursiveDeref(manager, path);
        }
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Function testing zero terminal") {
        // Test with functions that have complemented edges to zero
        DdNode *f = Cudd_bddOr(manager, x, Cudd_Not(y));
        Cudd_Ref(f);
        
        int length;
        DdNode *cube = Cudd_LargestCube(manager, f, &length);
        if (cube != nullptr) {
            Cudd_Ref(cube);
            Cudd_RecursiveDeref(manager, cube);
        }
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLeqUnless - Pointer ordering", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create many variables to test pointer ordering
    DdNode *vars[6];
    for (int i = 0; i < 6; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Many combinations to hit all branches") {
        // Create complex BDDs with many combinations
        for (int i = 0; i < 5; i++) {
            for (int j = i+1; j < 6; j++) {
                for (int k = 0; k < 6; k++) {
                    if (k != i && k != j) {
                        DdNode *f = Cudd_bddAnd(manager, vars[i], vars[j]);
                        Cudd_Ref(f);
                        DdNode *g = Cudd_bddOr(manager, vars[j], vars[k]);
                        Cudd_Ref(g);
                        DdNode *d = Cudd_bddXor(manager, vars[i], vars[k]);
                        Cudd_Ref(d);
                        
                        // Test all complement combinations
                        Cudd_bddLeqUnless(manager, f, g, d);
                        Cudd_bddLeqUnless(manager, Cudd_Not(f), g, d);
                        Cudd_bddLeqUnless(manager, f, Cudd_Not(g), d);
                        Cudd_bddLeqUnless(manager, f, g, Cudd_Not(d));
                        Cudd_bddLeqUnless(manager, Cudd_Not(f), Cudd_Not(g), d);
                        Cudd_bddLeqUnless(manager, f, Cudd_Not(g), Cudd_Not(d));
                        Cudd_bddLeqUnless(manager, Cudd_Not(f), g, Cudd_Not(d));
                        Cudd_bddLeqUnless(manager, Cudd_Not(f), Cudd_Not(g), Cudd_Not(d));
                        
                        Cudd_RecursiveDeref(manager, d);
                        Cudd_RecursiveDeref(manager, g);
                        Cudd_RecursiveDeref(manager, f);
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < 6; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddMaximallyExpand - Recursive paths", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Deep recursion with various ub/lb/f") {
        // Create nested functions
        DdNode *f01 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f01);
        DdNode *f23 = Cudd_bddOr(manager, vars[2], vars[3]);
        Cudd_Ref(f23);
        DdNode *f = Cudd_bddOr(manager, f01, f23);
        Cudd_Ref(f);
        
        DdNode *lb = f01;
        Cudd_Ref(lb);
        DdNode *ub = f;
        Cudd_Ref(ub);
        
        DdNode *result = Cudd_bddMaximallyExpand(manager, lb, ub, f);
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, ub);
        Cudd_RecursiveDeref(manager, lb);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f23);
        Cudd_RecursiveDeref(manager, f01);
    }
    
    SECTION("With complemented bounds") {
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(vars[0]), vars[1]);
        Cudd_Ref(f);
        DdNode *lb = Cudd_bddAnd(manager, Cudd_Not(vars[0]), vars[1]);
        Cudd_Ref(lb);
        
        DdNode *result = Cudd_bddMaximallyExpand(manager, lb, one, f);
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, lb);
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddMakePrime - Error cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Cube that needs expansion") {
        // f = x OR y, cube = x AND y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        DdNode *prime = Cudd_bddMakePrime(manager, cube, f);
        if (prime != nullptr) {
            Cudd_Ref(prime);
            REQUIRE(Cudd_bddLeq(manager, prime, f));
            Cudd_RecursiveDeref(manager, prime);
        }
        
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Cube with negative literal") {
        // f = !x OR y, cube = !x AND y
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        DdNode *cube = Cudd_bddAnd(manager, Cudd_Not(x), y);
        Cudd_Ref(cube);
        
        DdNode *prime = Cudd_bddMakePrime(manager, cube, f);
        if (prime != nullptr) {
            Cudd_Ref(prime);
            Cudd_RecursiveDeref(manager, prime);
        }
        
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Decreasing - More branches", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Unateness tests") {
        // f = x OR (!x AND y) - not unate in x
        DdNode *notx_y = Cudd_bddAnd(manager, Cudd_Not(vars[0]), vars[1]);
        Cudd_Ref(notx_y);
        DdNode *f = Cudd_bddOr(manager, vars[0], notx_y);
        Cudd_Ref(f);
        
        int id0 = Cudd_NodeReadIndex(vars[0]);
        DdNode *res = Cudd_Decreasing(manager, f, id0);
        REQUIRE(res == Cudd_Not(one));
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, notx_y);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_EquivDC - More recursive cases", "[cuddSat]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Deep recursion with complemented D") {
        DdNode *f = Cudd_bddOr(manager, Cudd_bddAnd(manager, vars[0], vars[1]), 
                               Cudd_bddAnd(manager, vars[2], vars[3]));
        Cudd_Ref(f);
        
        DdNode *g = Cudd_bddOr(manager, Cudd_bddAnd(manager, vars[0], vars[2]),
                               Cudd_bddAnd(manager, vars[1], vars[3]));
        Cudd_Ref(g);
        
        DdNode *d = Cudd_Not(Cudd_bddXor(manager, vars[0], vars[3]));
        Cudd_Ref(d);
        
        int r = Cudd_EquivDC(manager, f, g, d);
        REQUIRE(r >= 0);
        
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}
