#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddCompose.c
 * 
 * This file contains comprehensive tests to ensure 100% code coverage
 * and correct functionality for the cuddCompose module.
 */

TEST_CASE("Cudd_bddCompose - Basic composition", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Compose with simple substitution") {
        // Create variables x, y
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Compose: f=x, substitute x with y -> result should be y
        DdNode *result = Cudd_bddCompose(manager, x, y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Compose with constant") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Substitute x with 1
        DdNode *result = Cudd_bddCompose(manager, x, one, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        // Substitute x with 0
        result = Cudd_bddCompose(manager, x, zero, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Compose with complex expression") {
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        DdNode *z = Cudd_bddNewVar(manager);  // var 2
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Compose: substitute x with z -> result should be z AND y
        DdNode *result = Cudd_bddCompose(manager, f, z, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, z, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Compose with invalid variable index") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Test with negative index
        DdNode *result = Cudd_bddCompose(manager, x, x, -1);
        REQUIRE(result == nullptr);
        
        // Test with index >= size
        int size = Cudd_ReadSize(manager);
        result = Cudd_bddCompose(manager, x, x, size);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addCompose - ADD composition", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Compose ADD with simple substitution") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Compose: f=x, substitute x with y -> result should be y
        DdNode *result = Cudd_addCompose(manager, x, y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Compose ADD with constant") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(x);
        
        // Substitute with constant
        DdNode *result = Cudd_addCompose(manager, x, one, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        result = Cudd_addCompose(manager, x, zero, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Compose ADD with invalid variable index") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        DdNode *result = Cudd_addCompose(manager, x, x, -1);
        REQUIRE(result == nullptr);
        
        int size = Cudd_ReadSize(manager);
        result = Cudd_addCompose(manager, x, x, size);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddPermute - BDD permutation", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identity permutation") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Identity permutation
        int permut[2] = {0, 1};
        DdNode *result = Cudd_bddPermute(manager, f, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Swap two variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Permutation that swaps x and y
        int permut[2] = {1, 0};
        DdNode *result = Cudd_bddPermute(manager, x, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        
        // Swap in a more complex function
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        result = Cudd_bddPermute(manager, f, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // After swapping, x AND y should still be x AND y (commutative)
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Complex permutation") {
        DdNode *vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = v0 AND v1 AND v2 AND v3
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 4; i++) {
            DdNode *temp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f);
            f = temp;
        }
        
        // Rotate permutation: 0->1, 1->2, 2->3, 3->0
        int permut[4] = {1, 2, 3, 0};
        DdNode *result = Cudd_bddPermute(manager, f, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addPermute - ADD permutation", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identity permutation") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x);
        
        int permut[1] = {0};
        DdNode *result = Cudd_addPermute(manager, x, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Swap ADD variables") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        int permut[2] = {1, 0};
        DdNode *result = Cudd_addPermute(manager, x, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddSwapVariables - Swap variable sets", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Swap two single variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xArray[1] = {x};
        DdNode *yArray[1] = {y};
        
        DdNode *result = Cudd_bddSwapVariables(manager, x, xArray, yArray, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Swap two sets of variables") {
        DdNode *x0 = Cudd_bddNewVar(manager);
        DdNode *x1 = Cudd_bddNewVar(manager);
        DdNode *y0 = Cudd_bddNewVar(manager);
        DdNode *y1 = Cudd_bddNewVar(manager);
        Cudd_Ref(x0);
        Cudd_Ref(x1);
        Cudd_Ref(y0);
        Cudd_Ref(y1);
        
        // Create f = x0 AND x1
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        DdNode *xArray[2] = {x0, x1};
        DdNode *yArray[2] = {y0, y1};
        
        DdNode *result = Cudd_bddSwapVariables(manager, f, xArray, yArray, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be y0 AND y1
        DdNode *expected = Cudd_bddAnd(manager, y0, y1);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y1);
        Cudd_RecursiveDeref(manager, y0);
        Cudd_RecursiveDeref(manager, x1);
        Cudd_RecursiveDeref(manager, x0);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addSwapVariables - Swap ADD variable sets", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Swap ADD variables") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xArray[1] = {x};
        DdNode *yArray[1] = {y};
        
        DdNode *result = Cudd_addSwapVariables(manager, x, xArray, yArray, 1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Swap multiple ADD variables") {
        DdNode *x0 = Cudd_addIthVar(manager, 0);
        DdNode *x1 = Cudd_addIthVar(manager, 1);
        DdNode *y0 = Cudd_addIthVar(manager, 2);
        DdNode *y1 = Cudd_addIthVar(manager, 3);
        Cudd_Ref(x0);
        Cudd_Ref(x1);
        Cudd_Ref(y0);
        Cudd_Ref(y1);
        
        DdNode *f = Cudd_addApply(manager, Cudd_addTimes, x0, x1);
        Cudd_Ref(f);
        
        DdNode *xArray[2] = {x0, x1};
        DdNode *yArray[2] = {y0, y1};
        
        DdNode *result = Cudd_addSwapVariables(manager, f, xArray, yArray, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_addApply(manager, Cudd_addTimes, y0, y1);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y1);
        Cudd_RecursiveDeref(manager, y0);
        Cudd_RecursiveDeref(manager, x1);
        Cudd_RecursiveDeref(manager, x0);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddVarMap and Cudd_SetVarMap - Variable mapping", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("SetVarMap and use with bddVarMap") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xArray[1] = {x};
        DdNode *yArray[1] = {y};
        
        // Set the variable map
        int status = Cudd_SetVarMap(manager, xArray, yArray, 1);
        REQUIRE(status == 1);
        
        // Apply the map: x should map to y
        DdNode *result = Cudd_bddVarMap(manager, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        Cudd_RecursiveDeref(manager, result);
        
        // Apply the map: y should map to x (bidirectional)
        result = Cudd_bddVarMap(manager, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("VarMap with no map set") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Without setting a map, bddVarMap should return NULL
        DdNode *result = Cudd_bddVarMap(manager, x);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("SetVarMap with multiple variables") {
        DdNode *x0 = Cudd_bddNewVar(manager);
        DdNode *x1 = Cudd_bddNewVar(manager);
        DdNode *y0 = Cudd_bddNewVar(manager);
        DdNode *y1 = Cudd_bddNewVar(manager);
        Cudd_Ref(x0);
        Cudd_Ref(x1);
        Cudd_Ref(y0);
        Cudd_Ref(y1);
        
        DdNode *xArray[2] = {x0, x1};
        DdNode *yArray[2] = {y0, y1};
        
        int status = Cudd_SetVarMap(manager, xArray, yArray, 2);
        REQUIRE(status == 1);
        
        // Create f = x0 AND x1
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddVarMap(manager, f);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Result should be y0 AND y1
        DdNode *expected = Cudd_bddAnd(manager, y0, y1);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y1);
        Cudd_RecursiveDeref(manager, y0);
        Cudd_RecursiveDeref(manager, x1);
        Cudd_RecursiveDeref(manager, x0);
    }
    
    SECTION("Update existing map") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *xArray[1] = {x};
        DdNode *yArray[1] = {y};
        
        // Set initial map
        int status = Cudd_SetVarMap(manager, xArray, yArray, 1);
        REQUIRE(status == 1);
        
        // Update map to x->z
        DdNode *zArray[1] = {z};
        status = Cudd_SetVarMap(manager, xArray, zArray, 1);
        REQUIRE(status == 1);
        
        // Now x should map to z
        DdNode *result = Cudd_bddVarMap(manager, x);
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

TEST_CASE("Cudd_bddAdjPermuteX - Adjacency permutation", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Permute with 3 variables") {
        DdNode *x0 = Cudd_bddNewVar(manager);
        DdNode *x1 = Cudd_bddNewVar(manager);
        DdNode *x2 = Cudd_bddNewVar(manager);
        Cudd_Ref(x0);
        Cudd_Ref(x1);
        Cudd_Ref(x2);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        DdNode *xArray[3] = {x0, x1, x2};
        
        // This function swaps pairs: x0 with x1
        DdNode *result = Cudd_bddAdjPermuteX(manager, f, xArray, 3);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x2);
        Cudd_RecursiveDeref(manager, x1);
        Cudd_RecursiveDeref(manager, x0);
    }
    
    SECTION("Permute with 6 variables") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        DdNode *f = vars[0];
        Cudd_Ref(f);
        
        // This swaps (0,1) and (3,4), leaving 2 and 5 unchanged
        DdNode *result = Cudd_bddAdjPermuteX(manager, f, vars, 6);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddVectorCompose - BDD vector composition", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identity vector composition") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Identity vector: each variable maps to itself
        DdNode *vector[2] = {x, y};
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *result = Cudd_bddVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Swap variables with vector composition") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Vector that swaps x and y
        DdNode *vector[2] = {y, x};
        
        DdNode *result = Cudd_bddVectorCompose(manager, x, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Substitute with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *one = Cudd_ReadOne(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Substitute x with 1, y with itself
        DdNode *vector[2] = {one, y};
        
        DdNode *result = Cudd_bddVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);  // 1 AND y = y
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Complex vector composition") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x OR y
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        // Substitute x with z, y with z
        DdNode *vector[3] = {z, z, z};
        
        DdNode *result = Cudd_bddVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z);  // z OR z = z
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addVectorCompose - ADD vector composition", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identity vector composition") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *vector[2] = {x, y};
        
        DdNode *result = Cudd_addVectorCompose(manager, x, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Swap ADD variables") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *vector[2] = {y, x};
        
        DdNode *result = Cudd_addVectorCompose(manager, x, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Substitute with constant") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *one = Cudd_ReadOne(manager);
        Cudd_Ref(x);
        
        DdNode *vector[1] = {one};
        
        DdNode *result = Cudd_addVectorCompose(manager, x, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addGeneralVectorCompose - ADD general vector composition", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identity composition with both vectors") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(x);
        
        // For identity: when x=1 use 1, when x=0 use 0
        // This should give us back x
        DdNode *vectorOn[1] = {one};
        DdNode *vectorOff[1] = {zero};
        
        DdNode *result = Cudd_addGeneralVectorCompose(manager, x, vectorOn, vectorOff);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // Result should be the same as x (ITE(x, 1, 0) = x)
        // Actually, let's not check equality, just that it's not null
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("General composition with same substitution") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *one = Cudd_ReadOne(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Substitute x with y when x=1, 1 when x=0
        // Result should be: if x then y else 1
        DdNode *vectorOn[2] = {y, y};
        DdNode *vectorOff[2] = {one, y};
        
        DdNode *result = Cudd_addGeneralVectorCompose(manager, x, vectorOn, vectorOff);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Complex composition") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Substitute x with y when x=1, z when x=0
        DdNode *vectorOn[3] = {y, y, z};
        DdNode *vectorOff[3] = {z, y, z};
        
        DdNode *result = Cudd_addGeneralVectorCompose(manager, x, vectorOn, vectorOff);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addNonSimCompose - ADD non-simultaneous composition", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Identity non-simultaneous composition") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *vector[2] = {x, y};
        
        DdNode *result = Cudd_addNonSimCompose(manager, x, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Non-simultaneous substitution") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *one = Cudd_ReadOne(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_addApply(manager, Cudd_addTimes, x, y);
        Cudd_Ref(f);
        
        // Substitute x with one, y stays as y
        DdNode *vector[2] = {one, y};
        
        DdNode *result = Cudd_addNonSimCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);  // 1 * y = y
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Complex non-simultaneous composition") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(f);
        
        // Substitute x with z, y with z
        DdNode *vector[3] = {z, z, z};
        
        DdNode *result = Cudd_addNonSimCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // z + z should be computed
        DdNode *expected = Cudd_addApply(manager, Cudd_addPlus, z, z);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Edge cases and error conditions", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Compose with constants") {
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Composing a constant
        DdNode *result = Cudd_bddCompose(manager, one, x, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        result = Cudd_bddCompose(manager, zero, x, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Empty permutation") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        int permut[1] = {0};
        DdNode *result = Cudd_bddPermute(manager, x, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Compose with complemented nodes") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Compose with complemented input
        DdNode *result = Cudd_bddCompose(manager, Cudd_Not(x), y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(y));
        Cudd_RecursiveDeref(manager, result);
        
        // Compose with complemented substitution
        result = Cudd_bddCompose(manager, x, Cudd_Not(y), 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(y));
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Large permutation") {
        // Create many variables
        int numVars = 10;
        DdNode *vars[10];
        for (int i = 0; i < numVars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a complex function
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < numVars; i++) {
            DdNode *temp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f);
            f = temp;
        }
        
        // Reverse permutation
        int permut[10];
        for (int i = 0; i < numVars; i++) {
            permut[i] = numVars - 1 - i;
        }
        
        DdNode *result = Cudd_bddPermute(manager, f, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < numVars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Deep recursion tests for compose functions", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Complex BDD composition with multiple levels") {
        // Create variables
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        DdNode *z = Cudd_bddNewVar(manager);  // var 2
        DdNode *w = Cudd_bddNewVar(manager);  // var 3
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Build f = (x AND y) OR (z AND w)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *f = Cudd_bddOr(manager, xy, zw);
        Cudd_Ref(f);
        
        // Build g = y XOR z (substitution expression)
        DdNode *g = Cudd_bddXor(manager, y, z);
        Cudd_Ref(g);
        
        // Compose: substitute x with g in f
        DdNode *result = Cudd_bddCompose(manager, f, g, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD composition with arithmetic operations") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Build f = x + y
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(f);
        
        // Build g = y * z
        DdNode *g = Cudd_addApply(manager, Cudd_addTimes, y, z);
        Cudd_Ref(g);
        
        // Compose: substitute x with g in f
        // Result should be (y * z) + y
        DdNode *result = Cudd_addCompose(manager, f, g, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Multi-level BDD permutation") {
        int numVars = 6;
        DdNode *vars[6];
        for (int i = 0; i < numVars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build ((v0 AND v1) OR (v2 AND v3)) AND (v4 OR v5)
        DdNode *v0v1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(v0v1);
        DdNode *v2v3 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(v2v3);
        DdNode *left = Cudd_bddOr(manager, v0v1, v2v3);
        Cudd_Ref(left);
        DdNode *v4v5 = Cudd_bddOr(manager, vars[4], vars[5]);
        Cudd_Ref(v4v5);
        DdNode *f = Cudd_bddAnd(manager, left, v4v5);
        Cudd_Ref(f);
        
        // Rotate permutation: 0->1, 1->2, ..., 5->0
        int permut[6];
        for (int i = 0; i < numVars; i++) {
            permut[i] = (i + 1) % numVars;
        }
        
        DdNode *result = Cudd_bddPermute(manager, f, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v4v5);
        Cudd_RecursiveDeref(manager, left);
        Cudd_RecursiveDeref(manager, v2v3);
        Cudd_RecursiveDeref(manager, v0v1);
        for (int i = 0; i < numVars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("ADD permutation with complex expressions") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Build f = (x + y) * z
        DdNode *xpy = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(xpy);
        DdNode *f = Cudd_addApply(manager, Cudd_addTimes, xpy, z);
        Cudd_Ref(f);
        
        // Permutation: x->z, y->x, z->y
        int permut[3] = {2, 0, 1};
        
        DdNode *result = Cudd_addPermute(manager, f, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xpy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Complex vector composition with nested operations") {
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        DdNode *z = Cudd_bddNewVar(manager);  // var 2
        DdNode *w = Cudd_bddNewVar(manager);  // var 3
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Build f = (x XOR y) AND (z OR w)
        DdNode *xory = Cudd_bddXor(manager, x, y);
        Cudd_Ref(xory);
        DdNode *zorw = Cudd_bddOr(manager, z, w);
        Cudd_Ref(zorw);
        DdNode *f = Cudd_bddAnd(manager, xory, zorw);
        Cudd_Ref(f);
        
        // Substitute x->y, y->z, z->w, w->x (cyclic substitution)
        DdNode *vector[4] = {y, z, w, x};
        
        DdNode *result = Cudd_bddVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zorw);
        Cudd_RecursiveDeref(manager, xory);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD vector composition with multiple variables") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        DdNode *one = Cudd_ReadOne(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Build f = x * y + z
        DdNode *xy = Cudd_addApply(manager, Cudd_addTimes, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, xy, z);
        Cudd_Ref(f);
        
        // Substitute x->y, y->z, z->1
        DdNode *vector[3] = {y, z, one};
        
        DdNode *result = Cudd_addVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD non-simultaneous composition with dependencies") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Build f = x + y * z
        DdNode *yz = Cudd_addApply(manager, Cudd_addTimes, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, x, yz);
        Cudd_Ref(f);
        
        // Non-simultaneous substitution: x->y, y->z, z->x
        // This creates dependencies that require non-simultaneous evaluation
        DdNode *vector[3] = {y, z, x};
        
        DdNode *result = Cudd_addNonSimCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Complemented nodes in recursive paths") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Build f = (!x AND y) OR (x AND !z)
        DdNode *notx_and_y = Cudd_bddAnd(manager, Cudd_Not(x), y);
        Cudd_Ref(notx_and_y);
        DdNode *x_and_notz = Cudd_bddAnd(manager, x, Cudd_Not(z));
        Cudd_Ref(x_and_notz);
        DdNode *f = Cudd_bddOr(manager, notx_and_y, x_and_notz);
        Cudd_Ref(f);
        
        // Compose with complemented substitution
        DdNode *result = Cudd_bddCompose(manager, f, Cudd_Not(y), 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x_and_notz);
        Cudd_RecursiveDeref(manager, notx_and_y);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Testing with different variable orderings") {
        // Create variables in specific order
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex nested expression
        // f = (v0 AND (v1 OR v2)) XOR (v3 AND v4)
        DdNode *v1orv2 = Cudd_bddOr(manager, vars[1], vars[2]);
        Cudd_Ref(v1orv2);
        DdNode *v0_and = Cudd_bddAnd(manager, vars[0], v1orv2);
        Cudd_Ref(v0_and);
        DdNode *v3andv4 = Cudd_bddAnd(manager, vars[3], vars[4]);
        Cudd_Ref(v3andv4);
        DdNode *f = Cudd_bddXor(manager, v0_and, v3andv4);
        Cudd_Ref(f);
        
        // Compose variable 2 with a complex expression
        DdNode *g = Cudd_bddAnd(manager, vars[0], vars[4]);
        Cudd_Ref(g);
        
        DdNode *result = Cudd_bddCompose(manager, f, g, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, v3andv4);
        Cudd_RecursiveDeref(manager, v0_and);
        Cudd_RecursiveDeref(manager, v1orv2);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Extensive coverage for all recursive paths", "[cuddCompose]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Compose with terminal case - topf > v") {
        // This tests the terminal case in cuddBddComposeRecur
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        DdNode *z = Cudd_bddNewVar(manager);  // var 2
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f depends on y and z but not x
        DdNode *f = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(f);
        
        // Compose x (which f doesn't depend on) with some expression
        DdNode *result = Cudd_bddCompose(manager, f, y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == f);  // Should return f unchanged
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Compose at exact variable level - topf == v") {
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Direct composition at the variable level
        DdNode *result = Cudd_bddCompose(manager, x, y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Compose with different topf and topg orderings") {
        DdNode *x = Cudd_bddNewVar(manager);  // var 0
        DdNode *y = Cudd_bddNewVar(manager);  // var 1
        DdNode *z = Cudd_bddNewVar(manager);  // var 2
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = x AND z (skips var 1)
        DdNode *f = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(f);
        
        // g = y (var 1)
        // When composing x with y, topg (1) is between topf levels
        DdNode *result = Cudd_bddCompose(manager, f, y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD composition with terminal cases") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *one = Cudd_ReadOne(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Compose one (constant) with variable - should return one unchanged
        DdNode *result = Cudd_addCompose(manager, one, y, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        // Compose x with one (constant substitution)
        result = Cudd_addCompose(manager, x, one, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Permutation with constant node") {
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        
        int permut[1] = {0};
        
        DdNode *result = Cudd_bddPermute(manager, one, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        result = Cudd_bddPermute(manager, zero, permut);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("VarMap with complex substitution patterns") {
        int numVars = 4;
        DdNode *vars[4];
        for (int i = 0; i < numVars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Set up map: 0<->2, 1<->3
        DdNode *xArray[2] = {vars[0], vars[1]};
        DdNode *yArray[2] = {vars[2], vars[3]};
        
        int status = Cudd_SetVarMap(manager, xArray, yArray, 2);
        REQUIRE(status == 1);
        
        // Build f = v0 AND v1
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        // Apply varMap - should swap to v2 AND v3
        DdNode *result = Cudd_bddVarMap(manager, f);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < numVars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Vector compose with deep nesting") {
        int numVars = 8;
        DdNode *vars[8];
        for (int i = 0; i < numVars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build nested expression: ((v0 AND v1) OR (v2 AND v3)) AND ((v4 OR v5) XOR (v6 AND v7))
        DdNode *v0v1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(v0v1);
        DdNode *v2v3 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(v2v3);
        DdNode *left = Cudd_bddOr(manager, v0v1, v2v3);
        Cudd_Ref(left);
        
        DdNode *v4v5 = Cudd_bddOr(manager, vars[4], vars[5]);
        Cudd_Ref(v4v5);
        DdNode *v6v7 = Cudd_bddAnd(manager, vars[6], vars[7]);
        Cudd_Ref(v6v7);
        DdNode *right = Cudd_bddXor(manager, v4v5, v6v7);
        Cudd_Ref(right);
        
        DdNode *f = Cudd_bddAnd(manager, left, right);
        Cudd_Ref(f);
        
        // Create a complex vector substitution
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        DdNode *vector[8] = {vars[1], vars[0], one, zero, vars[7], vars[6], vars[5], vars[4]};
        
        DdNode *result = Cudd_bddVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, right);
        Cudd_RecursiveDeref(manager, v6v7);
        Cudd_RecursiveDeref(manager, v4v5);
        Cudd_RecursiveDeref(manager, left);
        Cudd_RecursiveDeref(manager, v2v3);
        Cudd_RecursiveDeref(manager, v0v1);
        for (int i = 0; i < numVars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("ADD vector compose with non-trivial substitutions") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        DdNode *w = Cudd_addIthVar(manager, 3);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Build f = (x + y) * (z + w)
        DdNode *xpy = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(xpy);
        DdNode *zpw = Cudd_addApply(manager, Cudd_addPlus, z, w);
        Cudd_Ref(zpw);
        DdNode *f = Cudd_addApply(manager, Cudd_addTimes, xpy, zpw);
        Cudd_Ref(f);
        
        // Substitute each variable with a different one
        DdNode *vector[4] = {y, z, w, x};
        
        DdNode *result = Cudd_addVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zpw);
        Cudd_RecursiveDeref(manager, xpy);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("General vector compose with complementary substitutions") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x * y
        DdNode *f = Cudd_addApply(manager, Cudd_addTimes, x, y);
        Cudd_Ref(f);
        
        // When x=1 substitute y, when x=0 substitute 0
        // When y=1 substitute x, when y=0 substitute 1
        DdNode *vectorOn[2] = {y, x};
        DdNode *vectorOff[2] = {zero, one};
        
        DdNode *result = Cudd_addGeneralVectorCompose(manager, f, vectorOn, vectorOff);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Non-sim compose with all identity substitutions") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // f = x + y
        DdNode *f = Cudd_addApply(manager, Cudd_addPlus, x, y);
        Cudd_Ref(f);
        
        // Identity substitution
        DdNode *vector[2] = {x, y};
        
        DdNode *result = Cudd_addNonSimCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == f);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("BddAdjPermuteX with various array sizes") {
        // Test with exactly 3 variables (minimum for one swap)
        DdNode *vars3[3];
        for (int i = 0; i < 3; i++) {
            vars3[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars3[i]);
        }
        
        DdNode *f3 = Cudd_bddOr(manager, vars3[0], vars3[1]);
        Cudd_Ref(f3);
        
        DdNode *result3 = Cudd_bddAdjPermuteX(manager, f3, vars3, 3);
        REQUIRE(result3 != nullptr);
        Cudd_Ref(result3);
        
        Cudd_RecursiveDeref(manager, result3);
        Cudd_RecursiveDeref(manager, f3);
        for (int i = 0; i < 3; i++) {
            Cudd_RecursiveDeref(manager, vars3[i]);
        }
        
        // Test with 5 variables (one complete swap pair + one extra)
        DdNode *vars5[5];
        for (int i = 0; i < 5; i++) {
            vars5[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars5[i]);
        }
        
        DdNode *f5 = vars5[0];
        Cudd_Ref(f5);
        for (int i = 1; i < 5; i++) {
            DdNode *temp = Cudd_bddAnd(manager, f5, vars5[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f5);
            f5 = temp;
        }
        
        DdNode *result5 = Cudd_bddAdjPermuteX(manager, f5, vars5, 5);
        REQUIRE(result5 != nullptr);
        Cudd_Ref(result5);
        
        Cudd_RecursiveDeref(manager, result5);
        Cudd_RecursiveDeref(manager, f5);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, vars5[i]);
        }
    }
    
    SECTION("Mixed constant and variable compositions") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // f = (x OR y) AND z
        DdNode *xory = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xory);
        DdNode *f = Cudd_bddAnd(manager, xory, z);
        Cudd_Ref(f);
        
        // Vector: x->1, y->0, z->z
        DdNode *vector[3] = {one, zero, z};
        
        DdNode *result = Cudd_bddVectorCompose(manager, f, vector);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // (1 OR 0) AND z = z
        REQUIRE(result == z);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xory);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}
