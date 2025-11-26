#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddSubsetSP.c
 * 
 * This file contains comprehensive tests for the cuddSubsetSP module
 * to achieve high code coverage (80%+) of the shortest paths subset
 * extraction heuristic implementation.
 */

TEST_CASE("Cudd_SubsetShortPaths - Constant inputs", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constant one returns itself") {
        DdNode *result = Cudd_SubsetShortPaths(manager, one, 10, 5, 0);
        REQUIRE(result == one);
    }
    
    SECTION("Constant zero returns itself") {
        DdNode *result = Cudd_SubsetShortPaths(manager, zero, 10, 5, 0);
        REQUIRE(result == zero);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Simple variable", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Single variable subset") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // With threshold larger than BDD, should return original
        DdNode *result = Cudd_SubsetShortPaths(manager, x, 1, 100, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of x
        REQUIRE(Cudd_bddLeq(manager, result, x));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Single variable with small threshold") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Small threshold
        DdNode *result = Cudd_SubsetShortPaths(manager, x, 1, 1, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of x
        REQUIRE(Cudd_bddLeq(manager, result, x));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - AND function", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(f);
    
    SECTION("With large threshold (return original)") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 100, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("With hardlimit=0") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("With hardlimit=1") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 2, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - OR function", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddOr(manager, x, y);
    Cudd_Ref(f);
    
    SECTION("OR function subset") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("OR function with hardlimit") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 2, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - XOR function", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddXor(manager, x, y);
    Cudd_Ref(f);
    
    SECTION("XOR function subset") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Complemented function", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(f);
    DdNode *notF = Cudd_Not(f);
    
    SECTION("Complemented AND function") {
        DdNode *result = Cudd_SubsetShortPaths(manager, notF, 2, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of notF
        REQUIRE(Cudd_bddLeq(manager, result, notF));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Complemented with hardlimit") {
        DdNode *result = Cudd_SubsetShortPaths(manager, notF, 2, 3, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of notF
        REQUIRE(Cudd_bddLeq(manager, result, notF));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - numVars=0 default", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(f);
    
    SECTION("numVars=0 uses Cudd_ReadSize default") {
        // When numVars=0, function should use Cudd_ReadSize(dd) as default
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 0, 10, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Large threshold", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    DdNode *f = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(f);
    DdNode *g = Cudd_bddAnd(manager, f, z);
    Cudd_Ref(g);
    
    SECTION("Threshold larger than BDD size") {
        // When threshold > numVars, should just adjust threshold
        DdNode *result = Cudd_SubsetShortPaths(manager, g, 3, 1000, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of g
        REQUIRE(Cudd_bddLeq(manager, result, g));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, g);
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SupersetShortPaths - Basic tests", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constant one superset") {
        DdNode *result = Cudd_SupersetShortPaths(manager, one, 10, 5, 0);
        REQUIRE(result == one);
    }
    
    SECTION("Constant zero superset") {
        DdNode *result = Cudd_SupersetShortPaths(manager, zero, 10, 5, 0);
        REQUIRE(result == zero);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SupersetShortPaths - Variable inputs", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(f);
    
    SECTION("Superset of AND function") {
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 2, 5, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a superset of f
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Superset with hardlimit") {
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 2, 5, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a superset of f
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Superset with numVars=0") {
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 0, 10, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a superset of f
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Complex BDD", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create a more complex BDD with multiple variables
    DdNode *vars[6];
    for (int i = 0; i < 6; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Complex AND-OR structure") {
        // f = (x0 AND x1) OR (x2 AND x3) OR (x4 AND x5)
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(t3);
        
        DdNode *f1 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddOr(manager, f1, t3);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 6, 5, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
    }
    
    SECTION("Complex XOR chain") {
        // f = x0 XOR x1 XOR x2 XOR x3
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 4; i++) {
            DdNode *tmp = Cudd_bddXor(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 8, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Complex with hardlimit=1") {
        // f = (x0 AND x1 AND x2) OR (x3 AND x4 AND x5)
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, t1, vars[2]);
        Cudd_Ref(t2);
        
        DdNode *t3 = Cudd_bddAnd(manager, vars[3], vars[4]);
        Cudd_Ref(t3);
        DdNode *t4 = Cudd_bddAnd(manager, t3, vars[5]);
        Cudd_Ref(t4);
        
        DdNode *f = Cudd_bddOr(manager, t2, t4);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 6, 4, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t4);
    }
    
    for (int i = 0; i < 6; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Various thresholds", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    // Create: (x0 AND x1) OR (x2 AND x3)
    DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
    Cudd_Ref(t1);
    DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
    Cudd_Ref(t2);
    DdNode *f = Cudd_bddOr(manager, t1, t2);
    Cudd_Ref(f);
    
    SECTION("Threshold = 1") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 1, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Threshold = 2") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Threshold = 3") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Threshold = 10") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 10, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, t1);
    Cudd_RecursiveDeref(manager, t2);
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - ITE structure", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    // f = ITE(x, y, z)
    DdNode *f = Cudd_bddIte(manager, x, y, z);
    Cudd_Ref(f);
    
    SECTION("ITE subset") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("ITE with hardlimit") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 3, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a subset of f
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SupersetShortPaths - Complex BDD", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    // Create: (x0 AND x1) OR (x2 AND x3)
    DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
    Cudd_Ref(t1);
    DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
    Cudd_Ref(t2);
    DdNode *f = Cudd_bddOr(manager, t1, t2);
    Cudd_Ref(f);
    
    SECTION("Superset of complex BDD") {
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 4, 5, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a superset of f
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Superset with hardlimit") {
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 4, 5, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be a superset of f
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, t1);
    Cudd_RecursiveDeref(manager, t2);
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Path length edge cases", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Deep BDD chain") {
        // Create a deep chain: x0 AND x1 AND x2 AND ... AND x7
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 8; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 8, 5, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Wide BDD (many OR branches)") {
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // f = x0 OR x1 OR x2 OR ... OR x7
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 8; i++) {
            DdNode *tmp = Cudd_bddOr(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 8, 6, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Mixed parity paths", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Function with odd and even parity paths") {
        // f = (x AND y) OR (NOT x AND z) - has paths of different parities
        DdNode *t1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(t1);
        DdNode *notX = Cudd_Not(x);
        DdNode *t2 = Cudd_bddAnd(manager, notX, z);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
    }
    
    SECTION("Complemented edges") {
        // Create function with complemented edges
        DdNode *notY = Cudd_Not(y);
        DdNode *f = Cudd_bddIte(manager, x, notY, z);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Threshold exceeds numVars", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *f = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(f);
    
    SECTION("threshold > numVars triggers adjustment") {
        // This triggers the threshold > numVars path
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 10, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Very large threshold") {
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 10000, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Multiple calls caching", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    DdNode *f = Cudd_bddAnd(manager, x, Cudd_bddOr(manager, y, z));
    Cudd_Ref(f);
    
    SECTION("Multiple calls with same parameters") {
        DdNode *result1 = Cudd_SubsetShortPaths(manager, f, 3, 3, 0);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        
        DdNode *result2 = Cudd_SubsetShortPaths(manager, f, 3, 3, 0);
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        
        // Both should be subsets of f
        REQUIRE(Cudd_bddLeq(manager, result1, f));
        REQUIRE(Cudd_bddLeq(manager, result2, f));
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Larger BDD for page resizing", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create a moderately large BDD to exercise page resizing code paths
    const int NUM_VARS = 12;
    DdNode *vars[NUM_VARS];
    for (int i = 0; i < NUM_VARS; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Large XOR structure") {
        // XOR chain creates exponential growth
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < NUM_VARS; i++) {
            DdNode *tmp = Cudd_bddXor(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            if (tmp == nullptr) {
                f = nullptr;
                break;
            }
            f = tmp;
            Cudd_Ref(f);
        }
        
        if (f != nullptr) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, NUM_VARS, 20, 0);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
            Cudd_RecursiveDeref(manager, f);
        }
    }
    
    SECTION("Large with hardlimit") {
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 8; i++) {
            DdNode *tmp = Cudd_bddXor(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            if (tmp == nullptr) {
                f = nullptr;
                break;
            }
            f = tmp;
            Cudd_Ref(f);
        }
        
        if (f != nullptr) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 8, 15, 1);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
            Cudd_RecursiveDeref(manager, f);
        }
    }
    
    for (int i = 0; i < NUM_VARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Node reuse paths", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *a = Cudd_bddNewVar(manager);
    DdNode *b = Cudd_bddNewVar(manager);
    DdNode *c = Cudd_bddNewVar(manager);
    DdNode *d = Cudd_bddNewVar(manager);
    Cudd_Ref(a);
    Cudd_Ref(b);
    Cudd_Ref(c);
    Cudd_Ref(d);
    
    SECTION("Shared subexpressions") {
        // Create BDD with shared subexpressions
        DdNode *ab = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(ab);
        DdNode *cd = Cudd_bddAnd(manager, c, d);
        Cudd_Ref(cd);
        
        // f = (a AND b AND c) OR (a AND b AND d) - shares (a AND b)
        DdNode *abc = Cudd_bddAnd(manager, ab, c);
        Cudd_Ref(abc);
        DdNode *abd = Cudd_bddAnd(manager, ab, d);
        Cudd_Ref(abd);
        DdNode *f = Cudd_bddOr(manager, abc, abd);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, abc);
        Cudd_RecursiveDeref(manager, abd);
        Cudd_RecursiveDeref(manager, ab);
        Cudd_RecursiveDeref(manager, cd);
    }
    
    Cudd_RecursiveDeref(manager, a);
    Cudd_RecursiveDeref(manager, b);
    Cudd_RecursiveDeref(manager, c);
    Cudd_RecursiveDeref(manager, d);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Small threshold stress", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[5];
    for (int i = 0; i < 5; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    // Create a BDD with multiple paths
    DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
    Cudd_Ref(t1);
    DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
    Cudd_Ref(t2);
    DdNode *t3 = Cudd_bddOr(manager, t1, t2);
    Cudd_Ref(t3);
    DdNode *f = Cudd_bddAnd(manager, t3, vars[4]);
    Cudd_Ref(f);
    
    SECTION("Very small thresholds") {
        for (int threshold = 1; threshold <= 6; threshold++) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 5, threshold, 0);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    SECTION("Small thresholds with hardlimit") {
        for (int threshold = 1; threshold <= 6; threshold++) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 5, threshold, 1);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, t1);
    Cudd_RecursiveDeref(manager, t2);
    Cudd_RecursiveDeref(manager, t3);
    for (int i = 0; i < 5; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SupersetShortPaths - Various inputs", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Superset of XOR") {
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 2, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Superset of ITE") {
        DdNode *f = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 3, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Superset of complemented function") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *notF = Cudd_Not(f);
        
        DdNode *result = Cudd_SupersetShortPaths(manager, notF, 2, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, notF, result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Deep path coverage", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[10];
    for (int i = 0; i < 10; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Complex nested structure") {
        // Build: ((x0 AND x1) OR (x2 AND x3)) AND ((x4 AND x5) OR (x6 AND x7))
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(t3);
        
        DdNode *t4 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(t4);
        DdNode *t5 = Cudd_bddAnd(manager, vars[6], vars[7]);
        Cudd_Ref(t5);
        DdNode *t6 = Cudd_bddOr(manager, t4, t5);
        Cudd_Ref(t6);
        
        DdNode *f = Cudd_bddAnd(manager, t3, t6);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 8, 6, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t4);
        Cudd_RecursiveDeref(manager, t5);
        Cudd_RecursiveDeref(manager, t6);
    }
    
    SECTION("Complex with hardlimit enabled") {
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(t3);
        DdNode *t4 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(t4);
        DdNode *f = Cudd_bddOr(manager, t3, t4);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 6, 4, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t4);
    }
    
    for (int i = 0; i < 10; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Edge cases for path computation", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Variable itself (shortest path = 1)") {
        DdNode *result = Cudd_SubsetShortPaths(manager, x, 1, 2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, x));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Complemented variable") {
        DdNode *notX = Cudd_Not(x);
        DdNode *result = Cudd_SubsetShortPaths(manager, notX, 1, 2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, notX));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("XNOR (even paths)") {
        DdNode *f = Cudd_bddXnor(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Very large BDD for page resizing", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create a very large BDD to trigger page resizing in NodeDist and Queue pages
    const int NUM_VARS = 16;
    DdNode *vars[NUM_VARS];
    for (int i = 0; i < NUM_VARS; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Large BDD to trigger ResizeNodeDistPages") {
        // Create a large enough BDD structure to trigger page resize
        // XOR chains create exponentially many nodes
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 14; i++) {
            DdNode *tmp = Cudd_bddXor(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            if (tmp == nullptr) {
                f = nullptr;
                break;
            }
            f = tmp;
            Cudd_Ref(f);
        }
        
        if (f != nullptr) {
            int size = Cudd_DagSize(f);
            // Only proceed if BDD is large enough (DEFAULT_NODE_DIST_PAGE_SIZE = 2048)
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 14, size / 2, 0);
            if (result != nullptr) {
                Cudd_Ref(result);
                REQUIRE(Cudd_bddLeq(manager, result, f));
                Cudd_RecursiveDeref(manager, result);
            }
            Cudd_RecursiveDeref(manager, f);
        }
    }
    
    SECTION("Large OR-chain") {
        // Create a wide BDD with many branches
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < NUM_VARS; i++) {
            DdNode *tmp = Cudd_bddOr(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, NUM_VARS, 10, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < NUM_VARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - findShortestPath and threshold paths", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[8];
    for (int i = 0; i < 8; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Threshold triggers findShortestPath") {
        // Create complex function with many paths
        // f = (x0*x1 + x2*x3 + x4*x5 + x6*x7)
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(t3);
        DdNode *t4 = Cudd_bddAnd(manager, vars[6], vars[7]);
        Cudd_Ref(t4);
        
        DdNode *o1 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(o1);
        DdNode *o2 = Cudd_bddOr(manager, t3, t4);
        Cudd_Ref(o2);
        DdNode *f = Cudd_bddOr(manager, o1, o2);
        Cudd_Ref(f);
        
        // Vary threshold to exercise different code paths
        for (int threshold = 1; threshold <= 10; threshold++) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 8, threshold, 0);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
        }
        
        // Test with hardlimit
        for (int threshold = 1; threshold <= 10; threshold++) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 8, threshold, 1);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, o1);
        Cudd_RecursiveDeref(manager, o2);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t4);
    }
    
    for (int i = 0; i < 8; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - maxpath table hit", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[6];
    for (int i = 0; i < 6; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Test maxpathTable lookup hit") {
        // Create function where same nodes appear multiple times
        DdNode *ab = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(ab);
        DdNode *cd = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(cd);
        DdNode *ef = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(ef);
        
        // f = (ab AND cd) OR (ab AND ef) - ab shared
        DdNode *abcd = Cudd_bddAnd(manager, ab, cd);
        Cudd_Ref(abcd);
        DdNode *abef = Cudd_bddAnd(manager, ab, ef);
        Cudd_Ref(abef);
        DdNode *f = Cudd_bddOr(manager, abcd, abef);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 6, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, abcd);
        Cudd_RecursiveDeref(manager, abef);
        Cudd_RecursiveDeref(manager, ab);
        Cudd_RecursiveDeref(manager, cd);
        Cudd_RecursiveDeref(manager, ef);
    }
    
    for (int i = 0; i < 6; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - Both parity distances valid", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[5];
    for (int i = 0; i < 5; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Function with both odd and even parity valid") {
        // Create function where nodes have both odd and even parity paths
        // f = (x0 AND x1) XOR (x2 AND x3) XOR x4
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *f1 = Cudd_bddXor(manager, t1, t2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddXor(manager, f1, vars[4]);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 5, 10, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
    }
    
    for (int i = 0; i < 5; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - subsetNodeTable threshold tracking", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[6];
    for (int i = 0; i < 6; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Hardlimit tracks nodes in subsetNodeTable") {
        // Create function that will generate new nodes
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(t3);
        DdNode *f1 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddOr(manager, f1, t3);
        Cudd_Ref(f);
        
        // With hardlimit=1, subsetNodeTable is used
        for (int threshold = 1; threshold <= 8; threshold++) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 6, threshold, 1);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
    }
    
    for (int i = 0; i < 6; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - regResult and compResult caching", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Test both regResult and compResult paths") {
        // Create function that reaches nodes with different parities
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, vars[2], vars[3]);
        Cudd_Ref(g);
        DdNode *h = Cudd_bddXor(manager, f, g);
        Cudd_Ref(h);
        
        // Multiple calls might hit regResult/compResult cache
        DdNode *result1 = Cudd_SubsetShortPaths(manager, h, 4, 5, 0);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        
        DdNode *result2 = Cudd_SubsetShortPaths(manager, Cudd_Not(h), 4, 5, 0);
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - pathLengthArray updates", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[5];
    for (int i = 0; i < 5; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Test pathLengthArray decrements and increments") {
        // Create function that will update pathLengthArray
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 5; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        for (int threshold = 1; threshold <= 6; threshold++) {
            DdNode *result = Cudd_SubsetShortPaths(manager, f, 5, threshold, 0);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, f));
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < 5; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - tiebreakChild selection", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Test child selection with equal path lengths") {
        // Create function where children have equal path lengths
        // to trigger tiebreaker based on bottom distance
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 4, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - MAXSHORTINT path lengths", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Test distance computations") {
        // Create function with various path configurations
        DdNode *f = Cudd_bddIte(manager, x, y, Cudd_Not(z));
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Complemented edges creating odd/even parity") {
        DdNode *notX = Cudd_Not(x);
        DdNode *f = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, f, z);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, g, 3, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, g));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - CreateBotDist constant child paths", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Function with direct constant children") {
        // x has T=1, E=0 directly
        DdNode *result = Cudd_SubsetShortPaths(manager, x, 1, 2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, x));
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("ITE with one branch being constant") {
        // f = ITE(x, y, 1) = x' + y
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, 3, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - BuildSubsetBdd cache hits", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Shared subexpressions hit cache") {
        // f = (x0 AND x1) OR (x2 AND x1) - x1 shared
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[1]);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 4, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - AssessPathLength edge cases", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[3];
    for (int i = 0; i < 3; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Threshold exactly matches available paths") {
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        int size = Cudd_DagSize(f);
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 2, size, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Threshold less than minimum path") {
        DdNode *f = Cudd_bddAnd(manager, vars[0], Cudd_bddAnd(manager, vars[1], vars[2]));
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, 3, 1, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < 3; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SubsetShortPaths - BFS queue expansion", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create enough nodes to potentially expand the BFS queue
    const int NUM_VARS = 10;
    DdNode *vars[NUM_VARS];
    for (int i = 0; i < NUM_VARS; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Wide BDD structure") {
        // Create many OR branches at same level
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < NUM_VARS; i++) {
            // Create pairs and OR them
            if (i % 2 == 1 && i > 0) {
                DdNode *pair = Cudd_bddAnd(manager, vars[i-1], vars[i]);
                Cudd_Ref(pair);
                DdNode *tmp = Cudd_bddOr(manager, f, pair);
                Cudd_RecursiveDeref(manager, f);
                Cudd_RecursiveDeref(manager, pair);
                f = tmp;
                Cudd_Ref(f);
            }
        }
        
        DdNode *result = Cudd_SubsetShortPaths(manager, f, NUM_VARS, 5, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, result, f));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < NUM_VARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SupersetShortPaths - Edge cases", "[cuddSubsetSP]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Superset with large threshold") {
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 2, 1000, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Superset with numVars=0") {
        DdNode *f = Cudd_bddOr(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 0, 5, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Superset with threshold > numVars") {
        DdNode *f = Cudd_bddXor(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_SupersetShortPaths(manager, f, 2, 100, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_bddLeq(manager, f, result));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
    }
    
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}
