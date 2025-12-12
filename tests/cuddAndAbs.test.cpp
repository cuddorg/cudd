#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddAndAbs.c
 * 
 * This file contains comprehensive tests for the cuddAndAbs module
 * to achieve high code coverage (target: 90%+).
 * 
 * The module contains:
 * - Cudd_bddAndAbstract: AND two BDDs and existentially abstract cube variables
 * - Cudd_bddAndAbstractLimit: Same as above with a node limit
 * - cuddBddAndAbstractRecur: Internal recursive implementation
 */

TEST_CASE("Cudd_bddAndAbstract - Terminal cases", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("f == zero returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddAndAbstract(manager, zero, x, x);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("g == zero returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddAndAbstract(manager, x, zero, x);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("f == Cudd_Not(g) returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // x AND NOT(x) = zero
        DdNode *result = Cudd_bddAndAbstract(manager, x, Cudd_Not(x), y);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("f == one && g == one returns one") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddAndAbstract(manager, one, one, x);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - cube == one path", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("With empty cube, returns AND of f and g") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // AndAbstract(x, y, one) = x AND y
        DdNode *result = Cudd_bddAndAbstract(manager, x, y, one);
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - f == one or f == g paths", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("f == one returns ExistAbstract(g, cube)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create g = x AND y
        DdNode *g = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(g);
        
        // AndAbstract(one, g, x) = ExistAbstract(g, x) = y
        DdNode *result = Cudd_bddAndAbstract(manager, one, g, x);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("f == g returns ExistAbstract(g, cube)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // AndAbstract(f, f, x) = ExistAbstract(f, x) = y
        DdNode *result = Cudd_bddAndAbstract(manager, f, f, x);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - g == one path", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("g == one returns ExistAbstract(f, cube)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // AndAbstract(f, one, x) = ExistAbstract(f, x) = y
        DdNode *result = Cudd_bddAndAbstract(manager, f, one, x);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - cube above top variable", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Cube variables above f,g top returns AND") {
        // Create variables in order - x comes before y,z in variable ordering
        DdNode *x = Cudd_bddNewVar(manager);  // index 0
        DdNode *y = Cudd_bddNewVar(manager);  // index 1
        DdNode *z = Cudd_bddNewVar(manager);  // index 2
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f and g depend on y and z only (not x)
        DdNode *f = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(f);
        
        // cube = x, which is higher in order than top(f) and top(g)
        // When cube < top, we skip down the cube, which exercises the while loop
        DdNode *result = Cudd_bddAndAbstract(manager, f, z, x);
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddAnd(manager, f, z);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Skip cube variables until cube becomes one") {
        // This tests the while loop exit via cube == one
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on z (lower in order)
        // cube = x (higher in order than z), cube will be skipped entirely
        DdNode *result = Cudd_bddAndAbstract(manager, z, z, x);
        Cudd_Ref(result);
        // After skipping x, cube becomes one, so result is AND(z, z) = z
        REQUIRE(result == z);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Cache behavior", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Cache hit with ref != 1") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f = x AND y with multiple refs
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        Cudd_Ref(f);  // Extra ref to ensure ref != 1
        
        // First call populates cache
        DdNode *result1 = Cudd_bddAndAbstract(manager, f, z, x);
        Cudd_Ref(result1);
        
        // Second call should hit cache
        DdNode *result2 = Cudd_bddAndAbstract(manager, f, z, x);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Argument swapping for cache", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("f > g triggers swap") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        // Call with f and g in both orders - results should be same
        DdNode *result1 = Cudd_bddAndAbstract(manager, f, g, y);
        Cudd_Ref(result1);
        DdNode *result2 = Cudd_bddAndAbstract(manager, g, f, y);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Quantify path (topcube == top)", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("t == one early return") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x OR y, g = x
        // f AND g = (x OR y) AND x = x
        // exists x. x = 1 (since x is true for some values)
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, x, x);
        Cudd_Ref(result);
        REQUIRE(result == one);  // exists x. ((x OR y) AND x) = exists x. x = 1
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("t == fe early return") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create scenario where t == fe (else cofactor of f)
        // f = x implies y = NOT x OR y
        DdNode *notX = Cudd_Not(x);
        DdNode *f = Cudd_bddOr(manager, notX, y);
        Cudd_Ref(f);
        
        // g = y
        // cube = x
        // ft = y, fe = one (from NOT x OR y with x = 0 gives one)
        // When t == fe (which is one), early return
        DdNode *result = Cudd_bddAndAbstract(manager, f, y, x);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("t == ge early return") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create scenario where t == ge (else cofactor of g)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // g = x implies z = NOT x OR z  
        DdNode *notX = Cudd_Not(x);
        DdNode *g = Cudd_bddOr(manager, notX, z);
        Cudd_Ref(g);
        
        // cube = x
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, x);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("t == Cudd_Not(fe) path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x (so ft = 1, fe = 0)
        // g = y
        // cube = x
        // After recursion on t: t = y (from 1 AND y)
        // fe = 0, so Cudd_Not(fe) = 1
        // t != 1 (t = y), so t != Cudd_Not(fe)
        // Let's construct different scenario
        
        // f = x XOR y (so ft = NOT y, fe = y when x is top)
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        // g = one
        // cube = x
        // ft = NOT y, fe = y
        // t = AndAbstract(ft=NOT y, gt=1, Cube) = NOT y
        // t == Cudd_Not(fe) = Cudd_Not(y) - YES!
        // This triggers: e = ExistAbstract(ge, Cube)
        DdNode *result = Cudd_bddAndAbstract(manager, f, one, x);
        Cudd_Ref(result);
        
        // exists x. (x XOR y) = 1 (since XOR is 1 for either x=0 or x=1)
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("t == Cudd_Not(ge) path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create g = x XOR y (so gt = NOT y, ge = y when x is top)
        DdNode *g = Cudd_bddXor(manager, x, y);
        Cudd_Ref(g);
        
        // f = one
        // cube = x
        // gt = NOT y, ge = y
        // t = AndAbstract(ft=1, gt=NOT y, Cube) = NOT y
        // t == Cudd_Not(ge) = Cudd_Not(y) - YES!
        // This triggers: e = ExistAbstract(fe, Cube)
        DdNode *result = Cudd_bddAndAbstract(manager, one, g, x);
        Cudd_Ref(result);
        
        // exists x. (x XOR y) = 1
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("t == e path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create f and g such that t == e after recursion
        // f = y (doesn't depend on x), g = y
        // cube = x
        // ft = fe = y, gt = ge = y
        // t = AndAbstract(y, y, cube') = y
        // e = AndAbstract(y, y, cube') = y
        // t == e triggers simple return
        DdNode *result = Cudd_bddAndAbstract(manager, y, y, x);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("t != e with OR reduction") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create complex case where t != e
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(g);
        
        // cube = x
        // ft = y, fe = 0, gt = z, ge = 0
        // t = AndAbstract(y, z, cube') = y AND z
        // e = AndAbstract(0, 0, cube') = 0
        // t != e, so compute r = NOT(NOT t AND NOT e) = t OR e = y AND z
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, x);
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Non-quantify path (topcube != top)", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("t == e in non-quantify path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on x, cube depends on z (different levels)
        // f = x, g = x
        // cube = z (z is below x in ordering)
        // topcube > top, so non-quantify path
        DdNode *result = Cudd_bddAndAbstract(manager, x, x, z);
        Cudd_Ref(result);
        REQUIRE(result == x);  // x AND x = x, no abstraction of z needed
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("t != e with complemented t") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create scenario with complemented result
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, Cudd_Not(x), z);
        Cudd_Ref(g);
        
        // cube depends on variable below x
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(w);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, w);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    SECTION("t != e with non-complemented t") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND y, g = x AND z
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(g);
        
        // cube below top of f and g
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(w);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, w);
        Cudd_Ref(result);
        
        // No abstraction occurs since w is not in f or g
        DdNode *expected = Cudd_bddAnd(manager, f, g);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Complemented inputs", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Complemented f") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = NOT(x AND y), g = x
        DdNode *temp = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(temp);
        DdNode *f = Cudd_Not(temp);
        
        // cube = x
        DdNode *result = Cudd_bddAndAbstract(manager, f, x, x);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Complemented g") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x, g = NOT(x AND y)
        DdNode *temp = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(temp);
        DdNode *g = Cudd_Not(temp);
        
        // cube = x
        DdNode *result = Cudd_bddAndAbstract(manager, x, g, x);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Both f and g complemented") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_Not(x);
        DdNode *g = Cudd_Not(y);
        
        // cube = x
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, x);
        Cudd_Ref(result);
        
        // exists x. (NOT x AND NOT y) = NOT y
        REQUIRE(result == Cudd_Not(y));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstractLimit - Basic functionality", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Limit large enough - succeeds") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        // With large limit, should succeed
        DdNode *result = Cudd_bddAndAbstractLimit(manager, f, g, x, 10000);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Verify same result as unlimited version
        DdNode *expected = Cudd_bddAndAbstract(manager, f, g, x);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Limit zero - may fail for complex operations") {
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create somewhat complex BDD
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 5; i++) {
            DdNode *tmp = Cudd_bddXor(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        DdNode *g = vars[5];
        Cudd_Ref(g);
        for (int i = 6; i < 10; i++) {
            DdNode *tmp = Cudd_bddXor(manager, g, vars[i]);
            Cudd_RecursiveDeref(manager, g);
            g = tmp;
            Cudd_Ref(g);
        }
        
        // Create cube
        DdNode *cube = vars[0];
        Cudd_Ref(cube);
        for (int i = 1; i < 5; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, cube, vars[i]);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
            Cudd_Ref(cube);
        }
        
        // With limit 0, may return NULL if complex
        DdNode *result = Cudd_bddAndAbstractLimit(manager, f, g, cube, 0);
        // Just verify it doesn't crash - may or may not return NULL
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        for (int i = 0; i < 10; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Various limits") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Simple operations should work even with limit 0
        DdNode *result1 = Cudd_bddAndAbstractLimit(manager, x, y, x, 0);
        if (result1 != nullptr) {
            Cudd_Ref(result1);
            REQUIRE(result1 == y);
            Cudd_RecursiveDeref(manager, result1);
        }
        
        DdNode *result2 = Cudd_bddAndAbstractLimit(manager, x, y, x, 1);
        if (result2 != nullptr) {
            Cudd_Ref(result2);
            REQUIRE(result2 == y);
            Cudd_RecursiveDeref(manager, result2);
        }
        
        DdNode *result3 = Cudd_bddAndAbstractLimit(manager, x, y, x, 100);
        REQUIRE(result3 != nullptr);
        Cudd_Ref(result3);
        REQUIRE(result3 == y);
        Cudd_RecursiveDeref(manager, result3);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Complex scenarios for coverage", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Deep recursion with many variables") {
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex functions
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 4; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        DdNode *g = vars[4];
        Cudd_Ref(g);
        for (int i = 5; i < 8; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, g, vars[i]);
            Cudd_RecursiveDeref(manager, g);
            g = tmp;
            Cudd_Ref(g);
        }
        
        // Abstract first 4 variables
        DdNode *cube = vars[0];
        Cudd_Ref(cube);
        for (int i = 1; i < 4; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, cube, vars[i]);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
            Cudd_Ref(cube);
        }
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, cube);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Multiple cube variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // f = x AND y AND z AND w
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *tmp = Cudd_bddAnd(manager, f, z);
        Cudd_RecursiveDeref(manager, f);
        f = tmp;
        Cudd_Ref(f);
        tmp = Cudd_bddAnd(manager, f, w);
        Cudd_RecursiveDeref(manager, f);
        f = tmp;
        Cudd_Ref(f);
        
        // cube = x AND y
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        // exists x,y. (x AND y AND z AND w) = z AND w
        DdNode *result = Cudd_bddAndAbstract(manager, f, one, cube);
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    SECTION("Test topf == top vs topg == top branches") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on x (top variable)
        // g depends on y (not top variable for f)
        // This tests the topf == top and topg != top branches
        DdNode *f = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, y, x);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Test topf != top and topg == top branches") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on y (not top variable for g)
        // g depends on x (top variable)
        DdNode *g = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddAndAbstract(manager, y, g, x);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Additional edge cases", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Test ref==1 path (no cache)") {
        // Create nodes without extra references to test the non-caching path
        // When F->ref == 1 and G->ref == 1, results are not cached
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, y, x);
        REQUIRE(result == y);
    }
    
    SECTION("Multiple calls for cache testing") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        Cudd_Ref(f);  // Extra ref for cache insertion
        
        // Multiple calls to ensure cache is used
        for (int i = 0; i < 5; i++) {
            DdNode *result = Cudd_bddAndAbstract(manager, f, z, x);
            Cudd_Ref(result);
            REQUIRE(result != nullptr);
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Test cuddUniqueInter with complemented t") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Create scenario where t is complemented in the non-quantify path
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, Cudd_Not(x), z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, w);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    SECTION("Test cuddUniqueInter with non-complemented t") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Create scenario where t is not complemented
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, w);
        Cudd_Ref(result);
        
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Correctness verification", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("AndAbstract equals And followed by ExistAbstract") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        // Method 1: AndAbstract
        DdNode *result1 = Cudd_bddAndAbstract(manager, f, g, y);
        Cudd_Ref(result1);
        
        // Method 2: And followed by ExistAbstract
        DdNode *andResult = Cudd_bddAnd(manager, f, g);
        Cudd_Ref(andResult);
        DdNode *result2 = Cudd_bddExistAbstract(manager, andResult, y);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, andResult);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Matrix multiplication use case") {
        // AndAbstract is used for semiring matrix multiplication
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Simulate relation composition
        // R1(x,y) = x AND y
        // R2(y,z) = y AND z
        // R1 . R2 (x,z) = exists y. (R1(x,y) AND R2(y,z))
        DdNode *R1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(R1);
        DdNode *R2 = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(R2);
        
        DdNode *composition = Cudd_bddAndAbstract(manager, R1, R2, y);
        Cudd_Ref(composition);
        
        // Composition should be x AND z
        DdNode *expected = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(expected);
        REQUIRE(composition == expected);
        
        Cudd_RecursiveDeref(manager, composition);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, R1);
        Cudd_RecursiveDeref(manager, R2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Coverage for remaining paths", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Test t == Cudd_Not(ge) branch") {
        // Test the special case where t equals the complement of ge
        // This triggers an optimization where e is computed via ExistAbstract
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x, g = x XOR y
        // For x XOR y: gt = NOT y, ge = y (when x is at top)
        // ft = 1, fe = 0
        // t = AndAbstract(1, NOT y, Cube) = NOT y (since 1 AND NOT y = NOT y)
        // Cudd_Not(ge) = Cudd_Not(y)
        // t == Cudd_Not(ge) triggers the branch
        DdNode *g = Cudd_bddXor(manager, x, y);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddAndAbstract(manager, x, g, x);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Test t == e in quantify path (lines 302-303)") {
        // To hit lines 302-303, we need t == e in the quantify branch
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = y, g = y (both don't depend on x)
        // When we abstract x, ft = fe = y and gt = ge = y
        // t = AndAbstract(y, y, Cube) = y
        // e = AndAbstract(y, y, Cube) = y
        // t == e, so we hit the simple return path
        DdNode *result = Cudd_bddAndAbstract(manager, y, y, x);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Test non-quantify path with t == e (lines 328-329)") {
        // In non-quantify path (topcube != top), we need t == e
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);  // z is below x in ordering
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = y, g = y (both depend on y, not x or z)
        // cube = z (below both f and g's top variables)
        // When processing y (top), topcube > top, so non-quantify path
        // ft = fe = y, gt = ge = y
        // t = AndAbstract(y, y, cube) = y
        // e = AndAbstract(y, y, cube) = y
        // t == e, so we hit lines 328-329
        DdNode *result = Cudd_bddAndAbstract(manager, y, y, z);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Test non-quantify path with Cudd_IsComplement(t) (lines 333-340)") {
        // Need t to be complemented in non-quantify path
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);  // w is after z in ordering
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Create f and g such that t will be complemented
        // f = NOT x (complemented), g = NOT y (complemented)
        // cube = w (below x and y)
        DdNode *f = Cudd_Not(x);
        DdNode *g = Cudd_Not(y);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, w);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    SECTION("Test cube variable skipping with multiple cube vars above top") {
        // Test when cube has variables above top of f and g
        // The while loop skips cube variables until reaching top(f,g)
        DdNode *x = Cudd_bddNewVar(manager);  // index 0
        DdNode *y = Cudd_bddNewVar(manager);  // index 1
        DdNode *z = Cudd_bddNewVar(manager);  // index 2
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f and g depend only on z (lowest level)
        // cube = x AND y (both above z)
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        // When processing, x is first in cube but z is top of f,g
        // So we skip x, then y, until cube becomes one
        DdNode *result = Cudd_bddAndAbstract(manager, z, z, cube);
        Cudd_Ref(result);
        REQUIRE(result == z);  // No abstraction since x,y not in z
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Test more complex cube skipping") {
        // More thorough test of the while loop for cube skipping
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f depends on vars[4] and vars[5] (bottom of ordering)
        DdNode *f = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(f);
        
        // cube = vars[0] AND vars[1] AND vars[2] (all above vars[4])
        DdNode *cube = vars[0];
        Cudd_Ref(cube);
        for (int i = 1; i <= 2; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, cube, vars[i]);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
            Cudd_Ref(cube);
        }
        
        // This should skip through vars[0], vars[1], vars[2] in cube
        DdNode *result = Cudd_bddAndAbstract(manager, f, f, cube);
        Cudd_Ref(result);
        REQUIRE(result == f);  // No abstraction since cube vars not in f
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Test non-quantify path with non-complemented t (lines 342)") {
        // Need t to NOT be complemented in non-quantify path
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // f = x, g = y (non-complemented)
        // cube = w (below x and y)
        DdNode *result = Cudd_bddAndAbstract(manager, x, y, w);
        Cudd_Ref(result);
        
        // Result should be x AND y since w not in either
        DdNode *expected = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - More coverage tests", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Test with complex BDDs for t != e in non-quantify path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        DdNode *v = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        Cudd_Ref(v);
        
        // Create complex functions
        DdNode *f = Cudd_bddOr(manager, Cudd_bddAnd(manager, x, y), z);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, Cudd_bddAnd(manager, x, z), w);
        Cudd_Ref(g);
        
        // cube = v (at the bottom)
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, v);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, v);
    }
    
    SECTION("Test t != e with complemented result in quantify path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f and g where t and e will differ
        // f = x IMPLIES y = NOT x OR y
        DdNode *notX = Cudd_Not(x);
        DdNode *f = Cudd_bddOr(manager, notX, y);
        Cudd_Ref(f);
        
        // g = x IMPLIES z = NOT x OR z
        DdNode *g = Cudd_bddOr(manager, notX, z);
        Cudd_Ref(g);
        
        // cube = x
        // This exercises the OR reduction in quantify path
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, x);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Test with multiple levels in non-quantify path") {
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create multi-level BDD
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        DdNode *tmp = Cudd_bddAnd(manager, f, vars[2]);
        Cudd_RecursiveDeref(manager, f);
        f = tmp;
        Cudd_Ref(f);
        
        DdNode *g = Cudd_bddAnd(manager, vars[0], vars[3]);
        Cudd_Ref(g);
        
        // cube = vars[4] (at bottom)
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, vars[4]);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Cube variable skipping loop", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Test the while loop that skips cube variables above top(f,g)
    // This loop iterates when multiple cube variables are above the BDD's top variable
    
    SECTION("Multiple cube var skip iterations") {
        // Create 5 variables - a, b, c are in cube (high order), d, e are in f,g (low order)
        DdNode *a = Cudd_bddNewVar(manager);  // index 0, highest
        DdNode *b = Cudd_bddNewVar(manager);  // index 1
        DdNode *c = Cudd_bddNewVar(manager);  // index 2
        DdNode *d = Cudd_bddNewVar(manager);  // index 3
        DdNode *e = Cudd_bddNewVar(manager);  // index 4, lowest
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(c);
        Cudd_Ref(d);
        Cudd_Ref(e);
        
        // f and g depend on d and e (low level variables)
        DdNode *f = Cudd_bddAnd(manager, d, e);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, d, e);
        Cudd_Ref(g);
        
        // Create cube with a, b, c (all above d and e in ordering)
        // cube = a AND b AND c
        DdNode *cube = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(cube);
        DdNode *tmp = Cudd_bddAnd(manager, cube, c);
        Cudd_RecursiveDeref(manager, cube);
        cube = tmp;
        Cudd_Ref(cube);
        
        // When processing AndAbstract:
        // - top of f and g is at level of d (index 3)
        // - topcube starts at level of a (index 0)
        // - Loop iteration 1: topcube=0 < top=3, skip a, cube = cuddT(cube) = b AND c
        //   Line 232: cube != one, continue
        //   Line 235: topcube = level of b (index 1)
        // - Loop iteration 2: topcube=1 < top=3, skip b, cube = cuddT(cube) = c
        //   Line 232: cube != one, continue
        //   Line 235: topcube = level of c (index 2)
        // - Loop iteration 3: topcube=2 < top=3, skip c, cube = cuddT(cube) = one
        //   Line 232: cube == one, return And(f, g)
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, cube);
        Cudd_Ref(result);
        
        // Since all cube vars are above f,g, and cube becomes one,
        // result should be f AND g
        DdNode *expected = Cudd_bddAnd(manager, f, g);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, a);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, e);
    }
    
    SECTION("Two cube vars above top, loop continues") {
        DdNode *a = Cudd_bddNewVar(manager);  // index 0
        DdNode *b = Cudd_bddNewVar(manager);  // index 1
        DdNode *c = Cudd_bddNewVar(manager);  // index 2
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(c);
        
        // f depends on c only
        DdNode *f = c;
        DdNode *g = c;
        
        // cube = a AND b (both above c)
        DdNode *cube = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(cube);
        
        // Loop iterations:
        // 1. topcube = 0 < top = 2, skip a, cube = b
        //    Line 235: topcube = 1
        // 2. topcube = 1 < top = 2, skip b, cube = one
        //    Line 232: return And(f, g)
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, cube);
        Cudd_Ref(result);
        REQUIRE(result == c);  // c AND c = c
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, a);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, c);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Additional coverage for t==e", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Complex scenario for quantify t==e") {
        // Create variables with specific ordering
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
        
        // g = (x AND NOT y) OR z  
        DdNode *xnoty = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(xnoty);
        DdNode *g = Cudd_bddOr(manager, xnoty, z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, y);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, xnoty);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Non-quantify path with simple t==e") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // f = x, g = x (both same)
        // cube = w (below x level)
        // At x level (non-quantify since w level > x level):
        // ft = 1, fe = 0, gt = 1, ge = 0
        // t = AndAbstract(1, 1, w) = 1
        // e = AndAbstract(0, 0, w) = 0
        // t != e, so we build a node
        
        // Try f = z, g = z (both same, below x)
        // cube = w
        // At z level: ft = 1, fe = 0, gt = 1, ge = 0
        // Same issue
        
        // Actually for t == e we need the cofactors to produce same result
        // f = y (doesn't depend on x), g = y
        // cube = w (at bottom)
        // The top of f and g is y
        // At y level (non-quantify since w > y in perm):
        // ft = 1, fe = 0, gt = 1, ge = 0
        // t = AndAbstract(1, 1, w) = 1
        // e = AndAbstract(0, 0, w) = 0
        // t != e
        
        // The only way t == e is if the then and else cofactors are identical
        // f = z (below y), g = z
        // At processing level (top of f,g is z):
        // If cube = w (below z), then non-quantify at z
        // ft = 1, fe = 0, gt = 1, ge = 0 (for z)
        // This gives t = 1, e = 0, not equal
        
        // Actually, let's think about when ft == fe AND gt == ge
        // That only happens if f and g don't depend on the current variable at all
        // But that means the variable isn't at top!
        
        // The real case is that during recursion, at some level below,
        // t and e happen to be equal
        
        DdNode *result = Cudd_bddAndAbstract(manager, y, y, w);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndAbstract - Push for 90% coverage", "[cuddAndAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Test with symmetric BDDs for t==e") {
        // Create symmetric functions where then and else branches give same result
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x XOR y (symmetric in x)
        // When x=1: f = NOT y
        // When x=0: f = y
        // These are NOT equal, so t != e at x level
        
        // Try f = y (constant w.r.t. x)
        // Actually, let me try a different approach:
        // Create f such that ft AND gt equals fe AND ge during the recursion
        
        DdNode *fxy = Cudd_bddXor(manager, x, y);
        Cudd_Ref(fxy);
        DdNode *gxy = Cudd_bddXor(manager, x, y);
        Cudd_Ref(gxy);
        
        // f = g = x XOR y
        // cube = x
        // At x level (in cube):
        // ft = NOT y, fe = y
        // gt = NOT y, ge = y
        // t = AndAbstract(NOT y, NOT y, Cube') = NOT y (if Cube' = one)
        // t != one, t != fe = y, t != ge = y
        // Cudd_Not(fe) = NOT y, so t == Cudd_Not(fe) triggers special path
        // This triggers e = ExistAbstract(ge, Cube)
        
        DdNode *result = Cudd_bddAndAbstract(manager, fxy, gxy, x);
        Cudd_Ref(result);
        REQUIRE(result == one);  // exists x. (x XOR y AND x XOR y) = exists x. (x XOR y) = 1
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, fxy);
        Cudd_RecursiveDeref(manager, gxy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Larger BDDs to exercise more paths") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex interleaved functions
        // f = (v0 AND v1) OR (v2 AND v3)
        DdNode *p1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(p1);
        DdNode *p2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(p2);
        DdNode *f = Cudd_bddOr(manager, p1, p2);
        Cudd_Ref(f);
        
        // g = (v0 AND v2) OR (v1 AND v3)
        DdNode *q1 = Cudd_bddAnd(manager, vars[0], vars[2]);
        Cudd_Ref(q1);
        DdNode *q2 = Cudd_bddAnd(manager, vars[1], vars[3]);
        Cudd_Ref(q2);
        DdNode *g = Cudd_bddOr(manager, q1, q2);
        Cudd_Ref(g);
        
        // cube = v0 AND v1
        DdNode *cube = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, cube);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, p1);
        Cudd_RecursiveDeref(manager, p2);
        Cudd_RecursiveDeref(manager, q1);
        Cudd_RecursiveDeref(manager, q2);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Different variable orderings") {
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f depends on v0, v2, v4 (odd indexed vars)
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[2]);
        Cudd_Ref(f);
        DdNode *tmp = Cudd_bddAnd(manager, f, vars[4]);
        Cudd_RecursiveDeref(manager, f);
        f = tmp;
        Cudd_Ref(f);
        
        // g depends on v1, v3 (even indexed vars)
        DdNode *g = Cudd_bddAnd(manager, vars[1], vars[3]);
        Cudd_Ref(g);
        
        // cube = v0 AND v1 (interleaved)
        DdNode *cube = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, cube);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Stress test with many operations") {
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Multiple AndAbstract operations
        for (int test = 0; test < 10; test++) {
            int fIdx = test % 8;
            int gIdx = (test + 1) % 8;
            int cubeIdx = (test + 2) % 8;
            
            DdNode *f = Cudd_bddAnd(manager, vars[fIdx], vars[(fIdx + 3) % 8]);
            Cudd_Ref(f);
            DdNode *g = Cudd_bddAnd(manager, vars[gIdx], vars[(gIdx + 4) % 8]);
            Cudd_Ref(g);
            
            DdNode *result = Cudd_bddAndAbstract(manager, f, g, vars[cubeIdx]);
            Cudd_Ref(result);
            REQUIRE(result != nullptr);
            
            Cudd_RecursiveDeref(manager, result);
            Cudd_RecursiveDeref(manager, f);
            Cudd_RecursiveDeref(manager, g);
        }
        
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Test with complemented cube") {
        // This should work or return appropriate behavior
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Use a regular cube
        DdNode *result = Cudd_bddAndAbstract(manager, f, x, x);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Complex AndAbstract scenario") {
        // Test complex BDD structures to exercise various code paths
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // f = (x AND y) OR z
        DdNode *f = Cudd_bddOr(manager, Cudd_bddAnd(manager, x, y), z);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, Cudd_bddAnd(manager, Cudd_Not(x), y), z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddAndAbstract(manager, f, g, x);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}
