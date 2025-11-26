#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddGenCof.c
 * 
 * This file contains comprehensive tests for the cuddGenCof module
 * to achieve 80% code coverage.
 */

TEST_CASE("Cudd_bddConstrain - Basic constrain operations", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constrain with constant constraint") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Constrain f with 1 should return f
        DdNode *result = Cudd_bddConstrain(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Constrain with same function") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Constrain x with x
        DdNode *result = Cudd_bddConstrain(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Constrain with complement") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Constrain x with !x
        DdNode *result = Cudd_bddConstrain(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Constrain with complex expressions") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // c = x OR z
        DdNode *c = Cudd_bddOr(manager, x, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Constrain constant function") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Constrain 1 with x should return 1
        DdNode *result = Cudd_bddConstrain(manager, one, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        // Constrain 0 with x should return 0
        result = Cudd_bddConstrain(manager, zero, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddRestrict - Basic restrict operations", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Restrict with constant care set") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Restrict f with 1 should return f
        DdNode *result = Cudd_bddRestrict(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict with same function") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddRestrict(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict with complement") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddRestrict(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict with complex expressions") {
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
        
        // c = x OR y
        DdNode *c = Cudd_bddOr(manager, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict constant function") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddRestrict(manager, one, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        result = Cudd_bddRestrict(manager, zero, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddNPAnd - Non-polluting AND operations", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("NPAnd with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x NPAnd 1 = x
        DdNode *result = Cudd_bddNPAnd(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        // x NPAnd 0 = 0
        result = Cudd_bddNPAnd(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NPAnd with same variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddNPAnd(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // NPAnd is a hybrid of AND and Restrict - just verify it returns a valid result
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NPAnd with complement") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddNPAnd(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NPAnd with complex expressions") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x OR y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        // c = y AND z
        DdNode *c = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddNPAnd(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addConstrain - ADD constrain operations", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    
    SECTION("ADD constrain with constant") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_addConstrain(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD constrain with same ADD") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_addConstrain(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD constrain with complex expressions") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x + y
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(f);
        
        // c = x * y
        DdNode *c = Cudd_addApply(manager, Cudd_addTimes, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_addConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addRestrict - ADD restrict operations", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("ADD restrict with constant") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_addRestrict(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD restrict with same ADD") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_addRestrict(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD restrict with complex expressions") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = (x + y) * z
        DdNode *xpy = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(xpy);
        DdNode *f = Cudd_addApply(manager, Cudd_addTimes, xpy, z);
        Cudd_Ref(f);
        
        // c = x + z
        DdNode *c = Cudd_addApply(manager, Cudd_addPlus, x, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_addRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xpy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddConstrainDecomp - Constrain decomposition", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Decomposition of simple function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **decomp = Cudd_bddConstrainDecomp(manager, f);
        REQUIRE(decomp != nullptr);
        
        // Clean up the decomposition array
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            if (decomp[i] != nullptr) {
                Cudd_RecursiveDeref(manager, decomp[i]);
            }
        }
        FREE(decomp);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Decomposition of complex function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = (x OR y) AND z
        DdNode *xory = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xory);
        DdNode *f = Cudd_bddAnd(manager, xory, z);
        Cudd_Ref(f);
        
        DdNode **decomp = Cudd_bddConstrainDecomp(manager, f);
        REQUIRE(decomp != nullptr);
        
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            if (decomp[i] != nullptr) {
                Cudd_RecursiveDeref(manager, decomp[i]);
            }
        }
        FREE(decomp);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xory);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Decomposition of constant") {
        DdNode **decomp = Cudd_bddConstrainDecomp(manager, one);
        REQUIRE(decomp != nullptr);
        
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            if (decomp[i] != nullptr) {
                Cudd_RecursiveDeref(manager, decomp[i]);
            }
        }
        FREE(decomp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCharToVect - Characteristic to vector", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("CharToVect with simple function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create a function: f = x XOR y (characteristic of a relation)
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **vect = Cudd_bddCharToVect(manager, f);
        REQUIRE(vect != nullptr);
        
        // Clean up
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            if (vect[i] != nullptr) {
                Cudd_RecursiveDeref(manager, vect[i]);
            }
        }
        FREE(vect);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("CharToVect with AND function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND y AND z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(f);
        
        DdNode **vect = Cudd_bddCharToVect(manager, f);
        REQUIRE(vect != nullptr);
        
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            if (vect[i] != nullptr) {
                Cudd_RecursiveDeref(manager, vect[i]);
            }
        }
        FREE(vect);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddMinimize - BDD minimization", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Minimize with constant care set") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddMinimize(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Minimize with same function") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddMinimize(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Minimize with complement") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddMinimize(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Minimize complex expressions") {
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
        
        // c = x OR y
        DdNode *c = Cudd_bddOr(manager, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddMinimize(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddInterpolate - BDD interpolation", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Interpolate with lower = upper") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // When l = u, interpolant is l (or u)
        DdNode *result = Cudd_bddInterpolate(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Interpolate with constants") {
        // When l = 0 and u = 1, any function is valid
        DdNode *result = Cudd_bddInterpolate(manager, zero, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Interpolate with l <= u") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // l = x AND y (subset of x)
        DdNode *l = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(l);
        
        // u = x OR y (superset of l)
        DdNode *u = Cudd_bddOr(manager, x, y);
        Cudd_Ref(u);
        
        DdNode *result = Cudd_bddInterpolate(manager, l, u);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should satisfy l <= result <= u
        REQUIRE(Cudd_bddLeq(manager, l, result) == 1);
        REQUIRE(Cudd_bddLeq(manager, result, u) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, u);
        Cudd_RecursiveDeref(manager, l);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Interpolate complex bounds") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // l = x AND y AND z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *l = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(l);
        
        // u = x OR y OR z
        DdNode *xory = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xory);
        DdNode *u = Cudd_bddOr(manager, xory, z);
        Cudd_Ref(u);
        
        DdNode *result = Cudd_bddInterpolate(manager, l, u);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, u);
        Cudd_RecursiveDeref(manager, xory);
        Cudd_RecursiveDeref(manager, l);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetCompress and Cudd_SupersetCompress", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("SubsetCompress simple function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        int nvars = Cudd_ReadSize(manager);
        DdNode *result = Cudd_SubsetCompress(manager, f, nvars, 10);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Subset should be <= original
        REQUIRE(Cudd_bddLeq(manager, result, f) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("SupersetCompress simple function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        int nvars = Cudd_ReadSize(manager);
        DdNode *result = Cudd_SupersetCompress(manager, f, nvars, 10);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Original should be <= superset
        REQUIRE(Cudd_bddLeq(manager, f, result) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("SubsetCompress with larger BDD") {
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex function
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 5; i++) {
            DdNode *temp = Cudd_bddOr(manager, f, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f);
            f = temp;
        }
        
        int nvars = Cudd_ReadSize(manager);
        DdNode *result = Cudd_SubsetCompress(manager, f, nvars, 3);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("SupersetCompress with larger BDD") {
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex function
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 5; i++) {
            DdNode *temp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f);
            f = temp;
        }
        
        int nvars = Cudd_ReadSize(manager);
        DdNode *result = Cudd_SupersetCompress(manager, f, nvars, 3);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Compress with constant functions") {
        int nvars = Cudd_ReadSize(manager);
        
        DdNode *result = Cudd_SubsetCompress(manager, one, nvars, 10);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        result = Cudd_SupersetCompress(manager, zero, nvars, 10);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Deep recursion tests for constrain functions", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Multi-level constrain") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build f = v0 AND v1 AND v2
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 3; i++) {
            DdNode *temp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f);
            f = temp;
        }
        
        // Build c = v0 OR v3 OR v4 OR v5
        DdNode *c = vars[0];
        Cudd_Ref(c);
        for (int i = 3; i < 6; i++) {
            DdNode *temp = Cudd_bddOr(manager, c, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, c);
            c = temp;
        }
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Multi-level restrict") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build f = (v0 OR v1) AND (v2 OR v3)
        DdNode *v0orv1 = Cudd_bddOr(manager, vars[0], vars[1]);
        Cudd_Ref(v0orv1);
        DdNode *v2orv3 = Cudd_bddOr(manager, vars[2], vars[3]);
        Cudd_Ref(v2orv3);
        DdNode *f = Cudd_bddAnd(manager, v0orv1, v2orv3);
        Cudd_Ref(f);
        
        // Build c = v0 AND v2 AND v4
        DdNode *c = Cudd_bddAnd(manager, vars[0], vars[2]);
        Cudd_Ref(c);
        DdNode *c2 = Cudd_bddAnd(manager, c, vars[4]);
        Cudd_Ref(c2);
        
        DdNode *result = Cudd_bddRestrict(manager, f, c2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c2);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v2orv3);
        Cudd_RecursiveDeref(manager, v0orv1);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Complemented node handling") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = !x AND y
        DdNode *f = Cudd_bddAnd(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        
        // c = !y OR z
        DdNode *c = Cudd_bddOr(manager, Cudd_Not(y), z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Edge cases and boundary conditions", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constrain f implies c") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x AND y (implies x)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // c = x
        DdNode *result = Cudd_bddConstrain(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict with XOR function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x XOR y
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddRestrict(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NPAnd with ITE") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = ITE(x, y, z)
        DdNode *f = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(f);
        
        // c = x OR y
        DdNode *c = Cudd_bddOr(manager, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddNPAnd(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Minimize with XNOR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x XNOR y
        DdNode *f = Cudd_bddXnor(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddMinimize(manager, f, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Additional coverage for recursive paths", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Constrain with skipped variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // f = x AND w (skips y, z)
        DdNode *f = Cudd_bddAnd(manager, x, w);
        Cudd_Ref(f);
        
        // c = y OR z
        DdNode *c = Cudd_bddOr(manager, y, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict with different top variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = y (top var is 1)
        // c = x AND z (top var is 0)
        DdNode *c = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddRestrict(manager, y, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD operations with arithmetic") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = (x - y) + z
        DdNode *xmy = Cudd_addApply(manager, Cudd_addMinus, x, y);
        Cudd_Ref(xmy);
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, xmy, z);
        Cudd_Ref(f);
        
        // c = x * y
        DdNode *c = Cudd_addApply(manager, Cudd_addTimes, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_addConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xmy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}
