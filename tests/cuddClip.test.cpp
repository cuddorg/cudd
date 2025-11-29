#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddClip.c
 * 
 * This file contains comprehensive tests for the cuddClip module
 * to achieve high code coverage (90%+) and ensure correct functionality.
 */

TEST_CASE("Cudd_bddClippingAnd - Terminal cases", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("f == zero returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, zero, x, 10, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("g == zero returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, zero, 10, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("f == NOT(g) returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, Cudd_Not(x), 10, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("f == g returns f") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("g == one returns f") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, one, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("f == one returns g") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, one, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAnd - Distance zero cases", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("distance=0, f <= g returns f (under approximation)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x AND y, g = x, so f <= g
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAnd(manager, f, x, 0, 0);
        Cudd_Ref(result);
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("distance=0, g <= f returns g (under approximation)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x, g = x AND y, so g <= f
        DdNode *g = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, g, 0, 0);
        Cudd_Ref(result);
        REQUIRE(result == g);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("distance=0, direction=0 (under) returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x, g = y - neither implies the other
        DdNode *result = Cudd_bddClippingAnd(manager, x, y, 0, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("distance=0, direction=1 (over) returns one") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x, g = y - neither implies the other
        DdNode *result = Cudd_bddClippingAnd(manager, x, y, 0, 1);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("distance=0, direction=1, f <= NOT(g) returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x AND y, g = NOT(x) OR NOT(y), f <= NOT(g)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *notX = Cudd_Not(x);
        DdNode *notY = Cudd_Not(y);
        DdNode *g = Cudd_bddOr(manager, notX, notY);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAnd(manager, f, g, 0, 1);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAnd - Recursive cases", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Basic AND with sufficient depth") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, y, 10, 0);
        Cudd_Ref(result);
        
        // With sufficient depth, should equal x AND y
        DdNode *expected = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Pointer ordering (f > g swap)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Test both orderings - should get same result
        DdNode *result1 = Cudd_bddClippingAnd(manager, x, y, 10, 0);
        Cudd_Ref(result1);
        DdNode *result2 = Cudd_bddClippingAnd(manager, y, x, 10, 0);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Complemented node handling") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *notX = Cudd_Not(x);
        
        DdNode *result = Cudd_bddClippingAnd(manager, notX, y, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Different variable levels (topf < topg)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on x (level 0), g depends on y,z (higher levels)
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAnd(manager, x, g, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, x, g);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Different variable levels (topf > topg)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on y,z (higher levels), g depends on x (level 0)
        DdNode *f = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAnd(manager, f, x, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, f, x);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Equal cofactors (t == e)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create function where cofactors are equal: f = y, g = y
        DdNode *result = Cudd_bddClippingAnd(manager, y, y, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Complemented t cofactor path") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create scenario where t will be complemented
        DdNode *notY = Cudd_Not(y);
        DdNode *f = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAnd(manager, f, y, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, f, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAnd - Cache behavior", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Cache hit with ref > 1") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        // Extra refs to trigger cache insertion
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // First call - cache miss
        DdNode *result1 = Cudd_bddClippingAnd(manager, x, y, 10, 0);
        Cudd_Ref(result1);
        
        // Second call - should hit cache
        DdNode *result2 = Cudd_bddClippingAnd(manager, x, y, 10, 0);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Direction 0 vs direction 1 use different cache ops") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result0 = Cudd_bddClippingAnd(manager, x, y, 10, 0);
        Cudd_Ref(result0);
        DdNode *result1 = Cudd_bddClippingAnd(manager, x, y, 10, 1);
        Cudd_Ref(result1);
        
        // Both should give same result for sufficient depth
        REQUIRE(result0 == result1);
        
        Cudd_RecursiveDeref(manager, result0);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Terminal cases", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("f == zero returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, zero, x, y, 10, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("g == zero returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, zero, y, 10, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("f == NOT(g) returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, Cudd_Not(x), y, 10, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("f == one AND g == one returns one") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, one, one, x, 10, 0);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("cube == one falls through to clipping and") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, y, one, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddClippingAnd(manager, x, y, 10, 0);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("f == one uses exist abstract on g") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *g = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, one, g, x, 10, 0);
        Cudd_Ref(result);
        
        // Should be exists x. (x AND y) = y
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("f == g uses exist abstract on g") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, f, x, 10, 0);
        Cudd_Ref(result);
        
        // Should be exists x. (x AND y) = y
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("g == one uses exist abstract on f") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, one, x, 10, 0);
        Cudd_Ref(result);
        
        // Should be exists x. (x AND y) = y
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Distance zero", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("distance=0, direction=0 returns zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, y, z, 0, 0);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("distance=0, direction=1 returns one") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, y, z, 0, 1);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Recursive cases", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Basic abstraction with sufficient depth") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, one, x, 10, 0);
        Cudd_Ref(result);
        
        // exists x. (x AND y) = y
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Cube variable above top of f and g (topcube < top)") {
        DdNode *w = Cudd_bddNewVar(manager);
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(w);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f and g depend on x and y, cube is w (above them in order)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, y, w, 10, 0);
        Cudd_Ref(result);
        
        // w is not in f or g, so abstraction is essentially AND
        DdNode *expected = Cudd_bddAnd(manager, f, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Pointer ordering (f > g swap)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Test both orderings
        DdNode *result1 = Cudd_bddClippingAndAbstract(manager, x, y, z, 10, 0);
        Cudd_Ref(result1);
        DdNode *result2 = Cudd_bddClippingAndAbstract(manager, y, x, z, 10, 0);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("topcube == top with early termination (t == one)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create scenario where t=one for early termination
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, one, x, 10, 0);
        Cudd_Ref(result);
        
        // exists x. x = 1
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Equal cofactors (t == e) without abstraction") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // When f and g don't depend on x, cofactors t and e are equal
        DdNode *result = Cudd_bddClippingAndAbstract(manager, y, z, x, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Complemented f handling") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *notX = Cudd_Not(x);
        DdNode *f = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, z, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Complemented g handling") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *notY = Cudd_Not(y);
        DdNode *g = Cudd_bddAnd(manager, notY, z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, g, y, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Complemented t cofactor in unique inter") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create scenario where t is complemented but topcube != top
        DdNode *notY = Cudd_Not(y);
        DdNode *f = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, z, y, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("topf != top case") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // g has top variable, f doesn't
        DdNode *g = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, z, g, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("topg != top case") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f has top variable, g doesn't
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, z, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Cache behavior", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Cache hit with ref > 1") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // First call
        DdNode *result1 = Cudd_bddClippingAndAbstract(manager, x, y, z, 10, 0);
        Cudd_Ref(result1);
        
        // Second call - should hit cache
        DdNode *result2 = Cudd_bddClippingAndAbstract(manager, x, y, z, 10, 0);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Different directions use different cache tags") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result0 = Cudd_bddClippingAndAbstract(manager, x, y, z, 10, 0);
        Cudd_Ref(result0);
        DdNode *result1 = Cudd_bddClippingAndAbstract(manager, x, y, z, 10, 1);
        Cudd_Ref(result1);
        
        // Should get same results with sufficient depth
        REQUIRE(result0 == result1);
        
        Cudd_RecursiveDeref(manager, result0);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Abstraction paths", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("topcube == top triggers abstraction (OR of cofactors)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Abstracting x from (x AND y) AND one
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, one, x, 10, 0);
        Cudd_Ref(result);
        
        // exists x. (x AND y) = y
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("topcube != top skips abstraction") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // z is after x,y in order, so topcube != top
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, one, z, 10, 0);
        Cudd_Ref(result);
        
        // z not in f, so result is just f
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAnd - Complex scenarios", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Multi-variable BDDs") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f = x0 AND x1
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        // g = x2 AND x3
        DdNode *g = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddClippingAnd(manager, f, g, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, f, g);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Nested complemented edges") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = NOT(x AND y)
        DdNode *temp = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(temp);
        DdNode *f = Cudd_Not(temp);
        
        // g = NOT(y AND z)
        DdNode *temp2 = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(temp2);
        DdNode *g = Cudd_Not(temp2);
        
        DdNode *result = Cudd_bddClippingAnd(manager, f, g, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, f, g);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, temp2);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Complex scenarios", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Multi-variable cube") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND y AND z
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *f2 = Cudd_bddAnd(manager, f, z);
        Cudd_Ref(f2);
        Cudd_RecursiveDeref(manager, f);
        
        // cube = x AND y
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f2, one, cube, 10, 0);
        Cudd_Ref(result);
        
        // exists x,y. (x AND y AND z) = z
        REQUIRE(result == z);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("AND and abstract with non-trivial result") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND z
        DdNode *f = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(f);
        
        // g = y AND z
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        // Abstract x and y
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, g, cube, 10, 0);
        Cudd_Ref(result);
        
        // exists x,y. ((x AND z) AND (y AND z)) = exists x,y. (x AND y AND z) = z
        REQUIRE(result == z);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAnd - Approximation quality", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Under approximation is subset of exact") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *exact = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(exact);
        
        DdNode *under = Cudd_bddClippingAnd(manager, x, y, 1, 0);
        Cudd_Ref(under);
        
        // Under approximation should imply exact: under <= exact
        int leq = Cudd_bddLeq(manager, under, exact);
        REQUIRE(leq == 1);
        
        Cudd_RecursiveDeref(manager, under);
        Cudd_RecursiveDeref(manager, exact);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Over approximation is superset of exact") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *exact = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(exact);
        
        DdNode *over = Cudd_bddClippingAnd(manager, x, y, 1, 1);
        Cudd_Ref(over);
        
        // Exact should imply over approximation: exact <= over
        int leq = Cudd_bddLeq(manager, exact, over);
        REQUIRE(leq == 1);
        
        Cudd_RecursiveDeref(manager, over);
        Cudd_RecursiveDeref(manager, exact);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Early termination t==one", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Early return when t is one with topcube == top") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        // Extra refs for cache insertion
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x OR y, when we abstract x, the then-cofactor is one
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, one, x, 10, 0);
        Cudd_Ref(result);
        
        // exists x. (x OR y) = 1
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - t==e without abstraction", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Equal cofactors when variable not in functions") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // f = y, g = z, cube = w (w is after y,z in order)
        // When processing at level of y or z, cofactors w.r.t. w are equal
        DdNode *result = Cudd_bddClippingAndAbstract(manager, y, z, w, 10, 0);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Complemented t in unique inter", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create complemented t with topcube != top") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f with complemented then-cofactor
        // f = NOT(x) AND y, so when x=1, f=0 (complemented)
        DdNode *notX = Cudd_Not(x);
        DdNode *f = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(f);
        
        // g = z, cube = y (not at top level of f)
        // This should create scenario where t != e and t might be complemented
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, z, y, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Force complemented t path more directly") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Create f = (NOT x) AND y AND z
        DdNode *notX = Cudd_Not(x);
        DdNode *tmp1 = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(tmp1);
        DdNode *f = Cudd_bddAnd(manager, tmp1, z);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, tmp1);
        
        // g = w, cube = w (after x,y,z)
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, w, w, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, w);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Complex cube paths", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Cube variable at different levels triggers different paths") {
        DdNode *a = Cudd_bddNewVar(manager);
        DdNode *b = Cudd_bddNewVar(manager);
        DdNode *c = Cudd_bddNewVar(manager);
        DdNode *d = Cudd_bddNewVar(manager);
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(c);
        Cudd_Ref(d);
        
        // f = a AND b, g = c AND d
        DdNode *f = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, c, d);
        Cudd_Ref(g);
        
        // Create cube = a AND c (some variables in f, some in g)
        DdNode *cube = Cudd_bddAnd(manager, a, c);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, g, cube, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, a);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, d);
    }
    
    SECTION("Test the abstraction OR path with different cofactors") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x XOR y (different then/else cofactors)
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        // Abstract x - this triggers OR of different cofactors
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, z, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - t==one early termination", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Force t==one with topcube==top for cache insert") {
        // Create variables
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(x);  // Extra ref so F->ref != 1
        
        // f = x, g = one, cube = x
        // When abstracting x from (x AND one), t-cofactor (with x=1) is one
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, one, x, 10, 0);
        Cudd_Ref(result);
        
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Another path to t==one early termination") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(x);  // Extra ref
        Cudd_Ref(y);  // Extra ref
        
        // f = x OR one = one, so try with f depending on x
        // f = x, g = y OR one = one doesn't work
        // Try: f and g both have x, and when x=1, result is one
        DdNode *f = x;  // f|x=1 = 1
        DdNode *g = x;  // g|x=1 = 1
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, g, x, 10, 0);
        Cudd_Ref(result);
        
        // exists x. (x AND x) = exists x. x = 1
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - t==e path", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Force t==e when topcube != top") {
        // Need: topcube != top, and t == e
        // This happens when f and g don't depend on the top variable being processed
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND y (top var is x)
        // g = z (doesn't depend on x)
        // cube = z (after x,y in order, so topcube > top)
        // When processing x: ft=y, fe=0, gt=ge=z
        // This gives different t and e...
        
        // Actually need both f and g to not depend on the variable at the current level
        // f = y, g = z, process at level of x (but x is before y,z)
        // Better: f = y AND z, g = y AND z (same), cube = x
        DdNode *f = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, f, x, 10, 0);
        Cudd_Ref(result);
        
        // Since f doesn't depend on x, abstracting x gives f
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Hit t==one && topcube==top", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Create scenario where t==one during abstraction") {
        // We need: topf == top, topcube == top, and t becomes one
        // f depends on x, we abstract x, ft AND gt must give one
        
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(x);  // Extra ref for cache path
        Cudd_Ref(y);  // Extra ref for cache path
        
        // f = x (then-cof = 1, else-cof = 0)
        // g = one
        // cube = x
        // At top level: ft=1, fe=0, gt=ge=1
        // t = ClipAndAbsRecur(1, 1, cuddT(x), ...) = 1 (terminal case f==one && g==one)
        // So t==one and topcube==top should be true!
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, x, one, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Another attempt with OR function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x OR y, cube = x
        // ft = 1 (x=1: 1 OR y = 1), fe = y (x=0: 0 OR y = y)
        // gt = ge = one (since g = one)
        // t = ClipAndAbsRecur(1, 1, Cube) = 1
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, f, one, x, 10, 0);
        Cudd_Ref(result);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddClippingAndAbstract - Hit t==e with topcube!=top", "[cuddClip]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create scenario where t==e and topcube!=top") {
        // Need: topcube > top (cube variable comes after current top)
        // AND t == e (same results for then and else branches)
        
        DdNode *x = Cudd_bddNewVar(manager);  // level 0
        DdNode *y = Cudd_bddNewVar(manager);  // level 1
        DdNode *z = Cudd_bddNewVar(manager);  // level 2
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x, g = x (both depend on x the same way)
        // cube = y (after x in order, so topcube > top when processing x)
        // At level x: ft=1, fe=0, gt=1, ge=0
        // Cube = y (since topcube != top)
        // t = ClipAndAbsRecur(1, 1, y) = 1
        // e = ClipAndAbsRecur(0, 0, y) = 0
        // So t != e... this won't work
        
        // Need f and g to have same cofactors at some level where cube is below
        // f = y, g = y, cube = z
        // At level y: ft=1, fe=0, gt=1, ge=0, Cube=z (topcube > top)
        // t = ClipAndAbsRecur(1, 1, z) = 1
        // e = ClipAndAbsRecur(0, 0, z) = 0
        // Still t != e
        
        // Try: f = y AND z, g = y AND z, cube = x (x before y,z)
        // This goes to topcube < top path instead...
        
        // Actually, for t==e with topcube != top:
        // We need the then and else cofactors of f and g to produce same result
        // when ANDed together
        
        // f = z, g = z, cube = y (y before z)
        // topcube = level(y), topf = topg = level(z)
        // Since topcube < top, this triggers the skip-cube path
        
        // f = y, g = y (same), cube = z (z after y)
        // top = level(y), topcube = level(z)
        // Since topcube > top: Cube = z
        // ft=1, fe=0, gt=1, ge=0
        // t = recur(1,1,z) = 1, e = recur(0,0,z) = 0
        // t != e
        
        // For t==e: need ft AND gt == fe AND ge
        // f = const, g = const won't work (terminal cases)
        // f = something that doesn't depend on top variable
        // But if f doesn't depend on x, then topf > top, so topf != top
        // In that case: ft = fe = f, and if topg != top too: gt = ge = g
        // Then t = recur(f, g, Cube), e = recur(f, g, Cube), so t == e!
        
        // f = y, g = z, cube = x (x is first variable)
        // top = min(level(y), level(z)) = level(y) since y comes first
        // Actually x is first, so top = level(x)
        // topf = level(y), topg = level(z), top = min(topf, topg)
        // Since both y and z don't have x, topf > level(x) and topg > level(x)
        // This means we skip to the cube < top case again
        
        // The t==e case happens when:
        // - topcube >= top (not in the topcube < top early return)
        // - topcube != top (in the else branch of if (topcube == top))
        // - t == e
        
        // Let's trace through:
        // f = y AND z, g = y AND z, cube = z (same level as part of f,g)
        // top = level(y) (top variable in f AND g)
        // topcube = level(z)
        // Since topcube > top: Cube = z (not abstracted yet)
        // topf = level(y), so topf == top
        // ft = z, fe = 0 (ITE(y, z, 0))
        // gt = z, ge = 0
        // t = recur(z, z, z) -> will abstract z
        // e = recur(0, 0, z) = 0
        // t != e
        
        // Very hard to hit this path! Let me try with f and g not depending on top
        // f = z, g = z, but processed at level before z
        // Need a variable before z but after cube...
        
        DdNode *result = Cudd_bddClippingAndAbstract(manager, z, z, y, 10, 0);
        Cudd_Ref(result);
        // Abstract y from (z AND z) = z (y not in function)
        REQUIRE(result == z);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    Cudd_Quit(manager);
}
