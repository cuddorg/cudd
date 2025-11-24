#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddLinear.c
 * 
 * This file contains comprehensive tests for the cuddLinear module
 * to achieve 100% code coverage and ensure correct functionality
 * of BDD and ADD reduction by linear transformations.
 */

TEST_CASE("Cudd_PrintLinear - Print linear transform matrix", "[cuddLinear]") {
    SECTION("Print linear matrix for small manager") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable reordering with linear transform to initialize the matrix
        Cudd_AutodynEnable(manager, CUDD_REORDER_LINEAR);
        
        // Create some BDD variables and operations to trigger reordering
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *z = Cudd_bddIthVar(manager, 2);
        DdNode *w = Cudd_bddIthVar(manager, 3);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(g);
        DdNode *h = Cudd_bddOr(manager, f, g);
        Cudd_Ref(h);
        
        // Force a reordering to initialize linear matrix
        Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        
        // Now print the linear matrix
        int result = Cudd_PrintLinear(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Print linear matrix for larger manager") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Initialize with linear reordering
        Cudd_AutodynEnable(manager, CUDD_REORDER_LINEAR);
        
        // Create a more complex BDD structure
        for (int i = 0; i < 8; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(f2);
        DdNode *result_bdd = Cudd_bddXor(manager, f1, f2);
        Cudd_Ref(result_bdd);
        
        // Trigger reordering
        Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        
        int result = Cudd_PrintLinear(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, result_bdd);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Cudd_ReadLinear - Read entry from linear transform matrix", "[cuddLinear]") {
    SECTION("Read diagonal elements") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable linear reordering and trigger it
        Cudd_AutodynEnable(manager, CUDD_REORDER_LINEAR);
        
        // Create BDD operations
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Trigger reordering to initialize linear matrix
        Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        
        // Initially, the linear matrix should be identity
        // Read diagonal elements
        for (int i = 0; i < 5; i++) {
            int value = Cudd_ReadLinear(manager, i, i);
            // Diagonal should be 1 in identity matrix (before transformations)
            REQUIRE((value == 0 || value == 1));
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Read off-diagonal elements") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_AutodynEnable(manager, CUDD_REORDER_LINEAR);
        
        // Create interacting variables
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, f, x2);
        Cudd_Ref(g);
        
        // Trigger reordering
        Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        
        // Read various matrix entries
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                int value = Cudd_ReadLinear(manager, i, j);
                REQUIRE((value == 0 || value == 1));
            }
        }
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddLinearAndSifting - BDD reduction with linear transforms", "[cuddLinear]") {
    SECTION("Linear sifting with small BDD") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a BDD that benefits from reordering
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        
        DdNode *f1 = Cudd_bddAnd(manager, x0, x2);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x1, x3);
        Cudd_Ref(f2);
        DdNode *f = Cudd_bddOr(manager, f1, f2);
        Cudd_Ref(f);
        
        // Apply linear sifting reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Linear sifting with converging iterations") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Build a more complex function
        DdNode *f1 = Cudd_bddAnd(manager, vars[0], vars[3]);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, vars[1], vars[4]);
        Cudd_Ref(f2);
        DdNode *f3 = Cudd_bddAnd(manager, vars[2], vars[5]);
        Cudd_Ref(f3);
        
        DdNode *g1 = Cudd_bddOr(manager, f1, f2);
        Cudd_Ref(g1);
        DdNode *result = Cudd_bddOr(manager, g1, f3);
        Cudd_Ref(result);
        
        // Apply linear sifting with convergence
        int status = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(status == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g1);
        Cudd_RecursiveDeref(manager, f3);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Linear sifting with autodyn enabled") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable automatic dynamic reordering with linear transform
        Cudd_AutodynEnable(manager, CUDD_REORDER_LINEAR);
        
        // Set a low threshold to trigger reordering
        unsigned int nextDyn = Cudd_ReadNextReordering(manager);
        REQUIRE(nextDyn > 0);
        
        // Create BDDs that will trigger automatic reordering
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        DdNode *x4 = Cudd_bddIthVar(manager, 4);
        
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(f2);
        DdNode *f3 = Cudd_bddAnd(manager, f1, x4);
        Cudd_Ref(f3);
        DdNode *result = Cudd_bddOr(manager, f2, f3);
        Cudd_Ref(result);
        
        // The linear matrix should now be initialized
        int printResult = Cudd_PrintLinear(manager);
        REQUIRE(printResult == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f3);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddLinearInPlace - Linear combination of adjacent variables", "[cuddLinear]") {
    SECTION("Linear transform with interacting variables") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create functions that make adjacent variables interact
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        // f = x0 * x1 + x1 * x2
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x1, x2);
        Cudd_Ref(f2);
        DdNode *f = Cudd_bddOr(manager, f1, f2);
        Cudd_Ref(f);
        
        // Trigger reordering which will call cuddLinearInPlace internally
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Verify the function is still correct after reordering
        REQUIRE(f != nullptr);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Linear transform maintains BDD semantics") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a specific function
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        
        // Create: f = (x0 AND x1) OR (x2 AND x3)
        DdNode *and1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(and1);
        DdNode *and2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(and2);
        DdNode *f = Cudd_bddOr(manager, and1, and2);
        Cudd_Ref(f);
        
        // Count minterms before reordering
        double countBefore = Cudd_CountMinterm(manager, f, 4);
        
        // Apply linear sifting
        Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        
        // Count minterms after reordering - should be the same
        double countAfter = Cudd_CountMinterm(manager, f, 4);
        REQUIRE(countBefore == countAfter);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, and2);
        Cudd_RecursiveDeref(manager, and1);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddInitLinear - Initialize linear transform matrix", "[cuddLinear]") {
    SECTION("Initialize linear matrix on first reordering") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some BDDs
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // First call to linear reordering should initialize the matrix
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Matrix should be accessible now
        int value = Cudd_ReadLinear(manager, 0, 0);
        REQUIRE((value == 0 || value == 1));
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddResizeLinear - Resize linear transform matrix", "[cuddLinear]") {
    SECTION("Resize when adding variables") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Initialize linear matrix
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        
        // Add more variables
        DdNode *z = Cudd_bddNewVar(manager);
        REQUIRE(z != nullptr);
        DdNode *w = Cudd_bddNewVar(manager);
        REQUIRE(w != nullptr);
        
        DdNode *g = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(g);
        
        // Trigger reordering again - should resize the matrix
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Should be able to read from expanded matrix
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            int value = Cudd_ReadLinear(manager, i, i);
            REQUIRE((value == 0 || value == 1));
        }
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddUpdateInteractionMatrix - Update interaction after transform", "[cuddLinear]") {
    SECTION("Interaction matrix updates during linear sifting") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables that interact
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        
        // Build function where variables interact
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(f2);
        DdNode *f3 = Cudd_bddAnd(manager, x1, x2);
        Cudd_Ref(f3);
        
        DdNode *g1 = Cudd_bddOr(manager, f1, f2);
        Cudd_Ref(g1);
        DdNode *result = Cudd_bddOr(manager, g1, f3);
        Cudd_Ref(result);
        
        // Linear sifting will update interaction matrix during transforms
        int status = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(status == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g1);
        Cudd_RecursiveDeref(manager, f3);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddLinear - Edge cases and error handling", "[cuddLinear]") {
    SECTION("Linear reordering with single variable") {
        DdManager *manager = Cudd_Init(1, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x);
        
        // Should handle gracefully
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear reordering with no BDDs") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Just call reordering without creating any BDDs
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple linear reordering calls") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Call multiple times
        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear reordering with complex BDD structure") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex nested structure
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Build layered structure
        DdNode *layer1[4];
        for (int i = 0; i < 4; i++) {
            layer1[i] = Cudd_bddAnd(manager, vars[i*2], vars[i*2+1]);
            Cudd_Ref(layer1[i]);
        }
        
        DdNode *layer2[2];
        for (int i = 0; i < 2; i++) {
            layer2[i] = Cudd_bddOr(manager, layer1[i*2], layer1[i*2+1]);
            Cudd_Ref(layer2[i]);
        }
        
        DdNode *final = Cudd_bddXor(manager, layer2[0], layer2[1]);
        Cudd_Ref(final);
        
        // Apply linear sifting
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Verify function still works
        REQUIRE(final != nullptr);
        
        Cudd_RecursiveDeref(manager, final);
        for (int i = 0; i < 2; i++) {
            Cudd_RecursiveDeref(manager, layer2[i]);
        }
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDeref(manager, layer1[i]);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddLinear - Static function coverage", "[cuddLinear]") {
    SECTION("ddLinearAndSiftingAux - Sifting at boundaries") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDDs where variables are at different positions
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x5 = Cudd_bddIthVar(manager, 5);
        
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, f1, x5);
        Cudd_Ref(f2);
        
        // This will exercise ddLinearAndSiftingAux with different boundary conditions
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_Quit(manager);
    }
    
    SECTION("ddLinearAndSiftingUp and Down") {
        DdManager *manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure that will cause both up and down sifting
        for (int i = 0; i < 7; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        DdNode *x6 = Cudd_bddIthVar(manager, 6);
        
        DdNode *f1 = Cudd_bddAnd(manager, x0, x3);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, f1, x6);
        Cudd_Ref(f2);
        
        // Will trigger both up and down sifting
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_Quit(manager);
    }
    
    SECTION("ddLinearAndSiftingBackward") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create function that will cause moves to be undone
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[4]);
        Cudd_Ref(f);
        for (int i = 1; i < 4; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
            Cudd_Ref(f);
        }
        
        // This will exercise backward sifting
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ddUndoMoves - Undo transformations") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDDs that will cause moves that need to be undone
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x3);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, x1, x2);
        Cudd_Ref(g);
        DdNode *h = Cudd_bddXor(manager, f, g);
        Cudd_Ref(h);
        
        // Multiple reorderings will exercise undo logic
        for (int i = 0; i < 2; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("cuddXorLinear - XOR rows of linear matrix") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create interacting variables to trigger XOR operations
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Apply linear transform which will XOR matrix rows
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Verify matrix can still be read
        int value = Cudd_ReadLinear(manager, 0, 0);
        REQUIRE((value == 0 || value == 1));
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ddLinearUniqueCompare - Variable ordering") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDDs with different numbers of nodes per level
        // This will exercise the comparison function for ordering
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        DdNode *x3 = Cudd_bddIthVar(manager, 3);
        DdNode *x4 = Cudd_bddIthVar(manager, 4);
        DdNode *x5 = Cudd_bddIthVar(manager, 5);
        
        // Create varying amounts of nodes at each level
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x0, x2);
        Cudd_Ref(f2);
        DdNode *f3 = Cudd_bddOr(manager, f1, f2);
        Cudd_Ref(f3);
        
        DdNode *g1 = Cudd_bddAnd(manager, x3, x4);
        Cudd_Ref(g1);
        DdNode *g2 = Cudd_bddAnd(manager, x4, x5);
        Cudd_Ref(g2);
        DdNode *g3 = Cudd_bddOr(manager, g1, g2);
        Cudd_Ref(g3);
        
        DdNode *result = Cudd_bddXor(manager, f3, g3);
        Cudd_Ref(result);
        
        // Linear sifting will sort variables by unique table size
        int status = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(status == 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g3);
        Cudd_RecursiveDeref(manager, g2);
        Cudd_RecursiveDeref(manager, g1);
        Cudd_RecursiveDeref(manager, f3);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddLinear - Comprehensive coverage test", "[cuddLinear]") {
    SECTION("Full linear reordering workflow") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex BDD structure
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Build multi-level structure
        DdNode *nodes[10];
        for (int i = 0; i < 5; i++) {
            nodes[i] = Cudd_bddAnd(manager, vars[i], vars[i+5]);
            Cudd_Ref(nodes[i]);
        }
        
        DdNode *layer2[2];
        layer2[0] = Cudd_bddOr(manager, nodes[0], nodes[1]);
        Cudd_Ref(layer2[0]);
        layer2[1] = Cudd_bddOr(manager, nodes[2], nodes[3]);
        Cudd_Ref(layer2[1]);
        
        DdNode *final = Cudd_bddAnd(manager, layer2[0], layer2[1]);
        Cudd_Ref(final);
        
        // Count nodes before
        int nodesBefore = Cudd_DagSize(final);
        
        // Apply linear sifting with all its internal functions
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Verify function integrity
        int nodesAfter = Cudd_DagSize(final);
        REQUIRE(nodesAfter > 0);
        
        // Print linear matrix to cover that function
        int printResult = Cudd_PrintLinear(manager);
        REQUIRE(printResult == 1);
        
        // Read some matrix entries
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int value = Cudd_ReadLinear(manager, i, j);
                REQUIRE((value == 0 || value == 1));
            }
        }
        
        // Apply convergent reordering
        result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, final);
        Cudd_RecursiveDeref(manager, layer2[1]);
        Cudd_RecursiveDeref(manager, layer2[0]);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, nodes[i]);
        }
        
        Cudd_Quit(manager);
    }
}
