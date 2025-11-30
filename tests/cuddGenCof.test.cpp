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

TEST_CASE("Additional constrain and restrict tests", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constrain with different variable levels") {
        // Create variables to exercise different topVar conditions
        DdNode *a = Cudd_bddNewVar(manager);
        DdNode *b = Cudd_bddNewVar(manager);
        DdNode *c = Cudd_bddNewVar(manager);
        DdNode *d = Cudd_bddNewVar(manager);
        Cudd_Ref(a); Cudd_Ref(b); Cudd_Ref(c); Cudd_Ref(d);
        
        // f uses only higher level vars, c uses lower level vars
        DdNode *f = Cudd_bddAnd(manager, c, d);
        Cudd_Ref(f);
        DdNode *constraint = Cudd_bddOr(manager, a, b);
        Cudd_Ref(constraint);
        
        DdNode *result = Cudd_bddConstrain(manager, f, constraint);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, constraint);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, d);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, a);
    }
    
    SECTION("Restrict when f implies c") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // f = x AND y, c = x OR y (f implies c)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *c = Cudd_bddOr(manager, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NPAnd with mixed variable levels") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f uses vars[0] and vars[2]
        DdNode *f = Cudd_bddOr(manager, vars[0], vars[2]);
        Cudd_Ref(f);
        
        // g uses vars[1] and vars[3]
        DdNode *g = Cudd_bddAnd(manager, vars[1], vars[3]);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddNPAnd(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Constrain with both branches equal") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // Test case where both cofactors result in same value
        DdNode *result = Cudd_bddConstrain(manager, y, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Additional squeeze and LICompaction tests", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Squeeze with tight bounds") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // l = x AND y
        DdNode *l = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(l);
        
        // u = x OR y
        DdNode *u = Cudd_bddOr(manager, x, y);
        Cudd_Ref(u);
        
        DdNode *result = Cudd_bddSqueeze(manager, l, u);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Verify l <= result <= u
        REQUIRE(Cudd_bddLeq(manager, l, result) == 1);
        REQUIRE(Cudd_bddLeq(manager, result, u) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, u);
        Cudd_RecursiveDeref(manager, l);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("LICompaction with various bounds") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y); Cudd_Ref(z);
        
        // f = (x AND y) OR z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        // c = x OR z
        DdNode *c = Cudd_bddOr(manager, x, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddLICompaction(manager, f, c);
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
    
    SECTION("Squeeze with equal bounds") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // When l = u, result should be l
        DdNode *result = Cudd_bddSqueeze(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("LICompaction with constant care set") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_bddLICompaction(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Complex recursive constrain paths", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Deep BDD constrain") {
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex f: (v0 AND v1) OR (v2 AND v3) OR (v4 AND v5) OR (v6 AND v7)
        DdNode *f = zero;
        Cudd_Ref(f);
        for (int i = 0; i < 8; i += 2) {
            DdNode *pair = Cudd_bddAnd(manager, vars[i], vars[i+1]);
            Cudd_Ref(pair);
            DdNode *newF = Cudd_bddOr(manager, f, pair);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, pair);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }
        
        // Build c: v0 OR v2 OR v4 OR v6
        DdNode *c = zero;
        Cudd_Ref(c);
        for (int i = 0; i < 8; i += 2) {
            DdNode *newC = Cudd_bddOr(manager, c, vars[i]);
            Cudd_Ref(newC);
            Cudd_RecursiveDeref(manager, c);
            c = newC;
        }
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Deep BDD restrict") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build f: (v0 XOR v1) AND (v2 XOR v3) AND (v4 XOR v5)
        DdNode *f = one;
        Cudd_Ref(f);
        for (int i = 0; i < 6; i += 2) {
            DdNode *xorPair = Cudd_bddXor(manager, vars[i], vars[i+1]);
            Cudd_Ref(xorPair);
            DdNode *newF = Cudd_bddAnd(manager, f, xorPair);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, xorPair);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }
        
        // Build c: v0 AND v2 AND v4
        DdNode *c = one;
        Cudd_Ref(c);
        for (int i = 0; i < 6; i += 2) {
            DdNode *newC = Cudd_bddAnd(manager, c, vars[i]);
            Cudd_Ref(newC);
            Cudd_RecursiveDeref(manager, c);
            c = newC;
        }
        
        DdNode *result = Cudd_bddRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("ADD constrain with multiple levels") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_addIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Build f = ((v0 + v1) * v2) + v3
        DdNode *sum = Cudd_addApply(manager, Cudd_addPlus, vars[0], vars[1]);
        Cudd_Ref(sum);
        DdNode *prod = Cudd_addApply(manager, Cudd_addTimes, sum, vars[2]);
        Cudd_Ref(prod);
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, prod, vars[3]);
        Cudd_Ref(f);
        
        // c = v0 * v2
        DdNode *c = Cudd_addApply(manager, Cudd_addTimes, vars[0], vars[2]);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_addConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, prod);
        Cudd_RecursiveDeref(manager, sum);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Interpolate and Minimize additional tests", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Interpolate with wide gap") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // l = v0 AND v1 AND v2 AND v3 (very small)
        DdNode *l = one;
        Cudd_Ref(l);
        for (int i = 0; i < 4; i++) {
            DdNode *newL = Cudd_bddAnd(manager, l, vars[i]);
            Cudd_Ref(newL);
            Cudd_RecursiveDeref(manager, l);
            l = newL;
        }
        
        // u = v0 OR v1 OR v2 OR v3 (very large)
        DdNode *u = zero;
        Cudd_Ref(u);
        for (int i = 0; i < 4; i++) {
            DdNode *newU = Cudd_bddOr(manager, u, vars[i]);
            Cudd_Ref(newU);
            Cudd_RecursiveDeref(manager, u);
            u = newU;
        }
        
        DdNode *result = Cudd_bddInterpolate(manager, l, u);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, l, result) == 1);
        REQUIRE(Cudd_bddLeq(manager, result, u) == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, u);
        Cudd_RecursiveDeref(manager, l);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Minimize with implication") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y); Cudd_Ref(z);
        
        // f = x -> y (i.e., !x OR y)
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        
        // c = x AND z
        DdNode *c = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddMinimize(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Minimize returning original") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // Small f with large c - minimize should return f
        DdNode *f = x;
        Cudd_Ref(f);
        DdNode *c = Cudd_bddOr(manager, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddMinimize(manager, f, c);
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

TEST_CASE("CharToVect and ConstrainDecomp additional tests", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("CharToVect with XNOR function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // f = x XNOR y (equivalence relation)
        DdNode *f = Cudd_bddXnor(manager, x, y);
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
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ConstrainDecomp with multi-variable function") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f = (v0 AND v1) OR (v2 AND v3)
        DdNode *pair1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(pair1);
        DdNode *pair2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(pair2);
        DdNode *f = Cudd_bddOr(manager, pair1, pair2);
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
        Cudd_RecursiveDeref(manager, pair2);
        Cudd_RecursiveDeref(manager, pair1);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("CharToVect with implication") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y); Cudd_Ref(z);
        
        // f = (x AND y) -> z (i.e., !(x AND y) OR z)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(xy), z);
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

TEST_CASE("Compress functions additional tests", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("SubsetCompress with tight threshold") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex function
        DdNode *f = zero;
        Cudd_Ref(f);
        for (int i = 0; i < 6; i += 2) {
            DdNode *pair = Cudd_bddAnd(manager, vars[i], vars[i+1]);
            Cudd_Ref(pair);
            DdNode *newF = Cudd_bddOr(manager, f, pair);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, pair);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }
        
        int nvars = Cudd_ReadSize(manager);
        
        // Very tight threshold
        DdNode *result = Cudd_SubsetCompress(manager, f, nvars, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f) == 1);
        Cudd_RecursiveDeref(manager, result);
        
        // Normal threshold
        result = Cudd_SubsetCompress(manager, f, nvars, 10);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f) == 1);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("SupersetCompress with tight threshold") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex function
        DdNode *f = one;
        Cudd_Ref(f);
        for (int i = 0; i < 6; i++) {
            DdNode *newF = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }
        
        int nvars = Cudd_ReadSize(manager);
        
        // Very tight threshold
        DdNode *result = Cudd_SupersetCompress(manager, f, nvars, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result) == 1);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Complemented edge handling in constrain", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constrain with complemented constraint") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y); Cudd_Ref(z);
        
        // f = x AND y (regular)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // c = !(y AND z) - complemented constraint
        DdNode *yz = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(yz);
        DdNode *c = Cudd_Not(yz);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Constrain with both complemented") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // f = !(x OR y)
        DdNode *xory = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xory);
        DdNode *f = Cudd_Not(xory);
        
        // c = !(x AND y)
        DdNode *xandy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xandy);
        DdNode *c = Cudd_Not(xandy);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, xandy);
        Cudd_RecursiveDeref(manager, xory);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Restrict with complemented constraint at specific level") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f = v0 AND v2
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[2]);
        Cudd_Ref(f);
        
        // c = !(v1 OR v3) - complemented, uses different vars
        DdNode *v1orv3 = Cudd_bddOr(manager, vars[1], vars[3]);
        Cudd_Ref(v1orv3);
        DdNode *c = Cudd_Not(v1orv3);
        
        DdNode *result = Cudd_bddRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, v1orv3);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("NPAnd with complemented operands") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y); Cudd_Ref(z);
        
        // f = !(x AND y)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_Not(xy);
        
        // g = !(y OR z)
        DdNode *yz = Cudd_bddOr(manager, y, z);
        Cudd_Ref(yz);
        DdNode *g = Cudd_Not(yz);
        
        DdNode *result = Cudd_bddNPAnd(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Special variable level cases", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constrain when topF > topC") {
        // Variables at specific levels
        DdNode *v0 = Cudd_bddIthVar(manager, 0);  // lowest index = highest in order
        DdNode *v1 = Cudd_bddIthVar(manager, 1);
        DdNode *v2 = Cudd_bddIthVar(manager, 2);
        DdNode *v3 = Cudd_bddIthVar(manager, 3);  // highest index = lowest in order
        Cudd_Ref(v0); Cudd_Ref(v1); Cudd_Ref(v2); Cudd_Ref(v3);
        
        // f uses variables lower in order (v2, v3)
        DdNode *f = Cudd_bddAnd(manager, v2, v3);
        Cudd_Ref(f);
        
        // c uses variables higher in order (v0, v1)
        DdNode *c = Cudd_bddOr(manager, v0, v1);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v3);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v0);
    }
    
    SECTION("Restrict when topF < topC") {
        DdNode *v0 = Cudd_bddIthVar(manager, 0);
        DdNode *v1 = Cudd_bddIthVar(manager, 1);
        DdNode *v2 = Cudd_bddIthVar(manager, 2);
        DdNode *v3 = Cudd_bddIthVar(manager, 3);
        Cudd_Ref(v0); Cudd_Ref(v1); Cudd_Ref(v2); Cudd_Ref(v3);
        
        // f uses variables higher in order (v0, v1)
        DdNode *f = Cudd_bddOr(manager, v0, v1);
        Cudd_Ref(f);
        
        // c uses variables lower in order (v2, v3)
        DdNode *c = Cudd_bddAnd(manager, v2, v3);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v3);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v0);
    }
    
    SECTION("NPAnd when topF != topG") {
        DdNode *v0 = Cudd_bddIthVar(manager, 0);
        DdNode *v1 = Cudd_bddIthVar(manager, 1);
        DdNode *v2 = Cudd_bddIthVar(manager, 2);
        DdNode *v3 = Cudd_bddIthVar(manager, 3);
        Cudd_Ref(v0); Cudd_Ref(v1); Cudd_Ref(v2); Cudd_Ref(v3);
        
        // f uses v0 and v2
        DdNode *f = Cudd_bddXor(manager, v0, v2);
        Cudd_Ref(f);
        
        // g uses v1 and v3
        DdNode *g = Cudd_bddXor(manager, v1, v3);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddNPAnd(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v3);
        Cudd_RecursiveDeref(manager, v2);
        Cudd_RecursiveDeref(manager, v1);
        Cudd_RecursiveDeref(manager, v0);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Specific branch coverage for cuddBddConstrainRecur", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Case: Cv == zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // Build f and c such that Cv becomes zero during recursion
        // c = x (when x is true, Cv = one; when x is false, Cv = zero)
        DdNode *result = Cudd_bddConstrain(manager, y, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Case: Cnv == zero") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x); Cudd_Ref(y);
        
        // Build f and c such that Cnv becomes zero
        // c = !x (when x is true, Cnv = zero)
        DdNode *result = Cudd_bddConstrain(manager, y, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Constrain exercise multiple recursive calls") {
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Complex f to exercise many branches
        // f = (v0 XOR v1) AND (v2 OR v3) AND v4
        DdNode *xor01 = Cudd_bddXor(manager, vars[0], vars[1]);
        Cudd_Ref(xor01);
        DdNode *or23 = Cudd_bddOr(manager, vars[2], vars[3]);
        Cudd_Ref(or23);
        DdNode *temp = Cudd_bddAnd(manager, xor01, or23);
        Cudd_Ref(temp);
        DdNode *f = Cudd_bddAnd(manager, temp, vars[4]);
        Cudd_Ref(f);
        
        // Complex c
        // c = v0 OR (v2 AND v4)
        DdNode *and24 = Cudd_bddAnd(manager, vars[2], vars[4]);
        Cudd_Ref(and24);
        DdNode *c = Cudd_bddOr(manager, vars[0], and24);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_bddConstrain(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, and24);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, or23);
        Cudd_RecursiveDeref(manager, xor01);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("ADD restrict and constrain edge cases", "[cuddGenCof]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *addOne = Cudd_ReadOne(manager);
    DdNode *addZero = Cudd_ReadZero(manager);
    
    SECTION("ADD constrain with constant result") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        // Constrain x with x should give addOne for ADDs
        DdNode *result = Cudd_addConstrain(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD restrict recursive path") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x); Cudd_Ref(y); Cudd_Ref(z);
        
        // Build more complex ADD
        // f = x + (y * z)
        DdNode *yz = Cudd_addApply(manager, Cudd_addTimes, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, x, yz);
        Cudd_Ref(f);
        
        // c = x * y
        DdNode *c = Cudd_addApply(manager, Cudd_addTimes, x, y);
        Cudd_Ref(c);
        
        DdNode *result = Cudd_addRestrict(manager, f, c);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, c);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}
