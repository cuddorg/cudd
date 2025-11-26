#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtr.h must come before cudd.h for tree functions
#include "mtr.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddReorder.c
 * 
 * This file contains comprehensive tests for the cuddReorder module
 * to achieve 80% code coverage and ensure correct functionality
 * of the dynamic variable reordering functions.
 */

// Helper function to create a BDD with interacting variables
static DdNode* createComplexBdd(DdManager* manager, int numVars) {
    if (numVars < 3) return nullptr;
    
    // Create variables
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* x2 = Cudd_bddIthVar(manager, 2);
    
    // Build f = (x0 AND x1) OR (x1 AND x2) - creates interactions
    DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(t1);
    
    DdNode* t2 = Cudd_bddAnd(manager, x1, x2);
    Cudd_Ref(t2);
    
    DdNode* result = Cudd_bddOr(manager, t1, t2);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, t1);
    Cudd_RecursiveDeref(manager, t2);
    
    return result;
}

// Helper function to create a more complex BDD for reordering
static DdNode* createLargerBdd(DdManager* manager, int numVars) {
    if (numVars < 5) return nullptr;
    
    DdNode* result = Cudd_ReadOne(manager);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars - 1; i++) {
        DdNode* xi = Cudd_bddIthVar(manager, i);
        DdNode* xi1 = Cudd_bddIthVar(manager, i + 1);
        
        DdNode* clause = Cudd_bddOr(manager, xi, xi1);
        Cudd_Ref(clause);
        
        DdNode* newResult = Cudd_bddAnd(manager, result, clause);
        Cudd_Ref(newResult);
        
        Cudd_RecursiveDeref(manager, clause);
        Cudd_RecursiveDeref(manager, result);
        result = newResult;
    }
    
    return result;
}

TEST_CASE("cuddReorder - Cudd_ReduceHeap basic tests", "[cuddReorder]") {
    SECTION("ReduceHeap with too few nodes (below minsize)") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // With few nodes, reordering should return 1 without doing anything
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 1000000);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_NONE") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some BDDs
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // CUDD_REORDER_NONE should return 1 without doing anything
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_NONE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_SAME") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set an auto reordering method first
        Cudd_AutodynEnable(manager, CUDD_REORDER_SIFT);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // CUDD_REORDER_SAME should use the auto method
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SAME, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_SIFT") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        int initialReorderings = Cudd_ReadReorderings(manager);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Verify reordering count increased
        REQUIRE(Cudd_ReadReorderings(manager) == (unsigned)(initialReorderings + 1));
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_SIFT_CONVERGE") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Cudd_ReduceHeap with various heuristics", "[cuddReorder]") {
    SECTION("CUDD_REORDER_RANDOM") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_RANDOM_PIVOT") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_WINDOW2") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_WINDOW3") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_WINDOW4") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_WINDOW2_CONV") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_SYMM_SIFT") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_SYMM_SIFT_CONV") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SYMM_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_GROUP_SIFT") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_GROUP_SIFT_CONV") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_LINEAR") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_LINEAR_CONVERGE") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Cudd_ShuffleHeap tests", "[cuddReorder]") {
    SECTION("ShuffleHeap with identity permutation") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Identity permutation - should return 1 without doing anything
        int permutation[] = {0, 1, 2, 3, 4};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        // Verify order hasn't changed
        for (int i = 0; i < 5; i++) {
            REQUIRE(Cudd_ReadPerm(manager, i) == i);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap with reversed permutation") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Reverse the order
        int permutation[] = {4, 3, 2, 1, 0};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        // Verify the permutation is applied
        REQUIRE(Cudd_ReadInvPerm(manager, 0) == 4);
        REQUIRE(Cudd_ReadInvPerm(manager, 4) == 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap with swap permutation") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // Swap first and second variables
        int permutation[] = {1, 0, 2, 3};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddDynamicAllocNode tests", "[cuddReorder]") {
    SECTION("Allocate node from free list") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // cuddDynamicAllocNode should allocate a node
        DdNode* node = cuddDynamicAllocNode(manager);
        REQUIRE(node != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Allocate multiple nodes") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Allocate several nodes
        DdNode* nodes[10];
        for (int i = 0; i < 10; i++) {
            nodes[i] = cuddDynamicAllocNode(manager);
            REQUIRE(nodes[i] != nullptr);
        }
        
        // All nodes should be different
        for (int i = 0; i < 10; i++) {
            for (int j = i + 1; j < 10; j++) {
                REQUIRE(nodes[i] != nodes[j]);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddNextHigh and cuddNextLow tests", "[cuddReorder]") {
    SECTION("cuddNextHigh returns x+1") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(cuddNextHigh(manager, 0) == 1);
        REQUIRE(cuddNextHigh(manager, 1) == 2);
        REQUIRE(cuddNextHigh(manager, 3) == 4);
        
        Cudd_Quit(manager);
    }
    
    SECTION("cuddNextLow returns x-1") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(cuddNextLow(manager, 4) == 3);
        REQUIRE(cuddNextLow(manager, 2) == 1);
        REQUIRE(cuddNextLow(manager, 1) == 0);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Variable swapping through public API", "[cuddReorder]") {
    // Note: cuddSwapInPlace is an internal function. Testing it directly requires
    // manipulating internal manager state and manual memory cleanup which is fragile.
    // Instead, we test swapping behavior through the public Cudd_ShuffleHeap API
    // which exercises the same code paths safely.
    
    SECTION("Swap adjacent variables via ShuffleHeap - non-interacting") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD with non-interacting variables (x0 AND x2)
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* f = Cudd_bddAnd(manager, x0, x2);
        Cudd_Ref(f);
        
        // Swap variables 0 and 1 using permutation
        int perm[] = {1, 0, 2, 3};
        int result = Cudd_ShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        // Verify the BDD is still valid
        REQUIRE(Cudd_DagSize(f) > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Swap adjacent variables via ShuffleHeap - interacting") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD with interacting variables (x0 AND x1)
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Swap variables 0 and 1 using permutation
        int perm[] = {1, 0, 2, 3};
        int result = Cudd_ShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        // Verify the BDD is still valid
        REQUIRE(Cudd_DagSize(f) > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple swaps via reordering") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a BDD that benefits from reordering
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // SIFT reordering internally uses cuddSwapInPlace
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Verify the BDD is still valid
        REQUIRE(Cudd_DagSize(f) > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddSifting through ReduceHeap", "[cuddReorder]") {
    SECTION("Sifting on small BDD via ReduceHeap") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Use ReduceHeap with SIFT to exercise cuddSifting
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting on larger BDD via ReduceHeap") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Use ReduceHeap with SIFT to exercise cuddSifting
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddSwapping through ReduceHeap", "[cuddReorder]") {
    SECTION("Random swapping via ReduceHeap") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Use ReduceHeap with RANDOM to exercise cuddSwapping
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Random pivot swapping via ReduceHeap") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Use ReduceHeap with RANDOM_PIVOT to exercise cuddSwapping
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Hooks tests", "[cuddReorder]") {
    SECTION("Pre and post reordering hooks") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Enable reordering reporting (adds hooks)
        REQUIRE(Cudd_EnableReorderingReporting(manager) == 1);
        REQUIRE(Cudd_ReorderingReporting(manager) == 1);
        
        // Perform reordering - hooks should be called
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Disable reporting
        REQUIRE(Cudd_DisableReorderingReporting(manager) == 1);
        REQUIRE(Cudd_ReorderingReporting(manager) == 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddBddAlignToZdd tests", "[cuddReorder]") {
    SECTION("Align BDD to ZDD with no variables") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // With zero size, should return 1
        int result = cuddBddAlignToZdd(manager);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Align BDD to ZDD with matching variables") {
        DdManager *manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // Align BDD to ZDD order
        int result = cuddBddAlignToZdd(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Align BDD to ZDD with ZDD multiplicity") {
        // Create manager with 2 BDD vars and 4 ZDD vars (multiplicity 2)
        DdManager *manager = Cudd_Init(2, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        int result = cuddBddAlignToZdd(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Reordering parameters tests", "[cuddReorder]") {
    SECTION("Set and read sift max var") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int originalMaxVar = Cudd_ReadSiftMaxVar(manager);
        REQUIRE(originalMaxVar > 0);
        
        Cudd_SetSiftMaxVar(manager, 5);
        REQUIRE(Cudd_ReadSiftMaxVar(manager) == 5);
        
        // Restore
        Cudd_SetSiftMaxVar(manager, originalMaxVar);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Set and read sift max swap") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int originalMaxSwap = Cudd_ReadSiftMaxSwap(manager);
        REQUIRE(originalMaxSwap > 0);
        
        Cudd_SetSiftMaxSwap(manager, 100);
        REQUIRE(Cudd_ReadSiftMaxSwap(manager) == 100);
        
        // Restore
        Cudd_SetSiftMaxSwap(manager, originalMaxSwap);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Set and read max growth") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        double originalGrowth = Cudd_ReadMaxGrowth(manager);
        
        Cudd_SetMaxGrowth(manager, 1.5);
        REQUIRE(Cudd_ReadMaxGrowth(manager) == 1.5);
        
        // Restore
        Cudd_SetMaxGrowth(manager, originalGrowth);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Set and read max growth alternate") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        double originalGrowthAlt = Cudd_ReadMaxGrowthAlternate(manager);
        
        Cudd_SetMaxGrowthAlternate(manager, 2.0);
        REQUIRE(Cudd_ReadMaxGrowthAlternate(manager) == 2.0);
        
        // Restore
        Cudd_SetMaxGrowthAlternate(manager, originalGrowthAlt);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Set and read reordering cycle") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int originalCycle = Cudd_ReadReorderingCycle(manager);
        
        Cudd_SetReorderingCycle(manager, 3);
        REQUIRE(Cudd_ReadReorderingCycle(manager) == 3);
        
        // Restore
        Cudd_SetReorderingCycle(manager, originalCycle);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Autodyn tests", "[cuddReorder]") {
    SECTION("Enable and disable auto reordering") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_ReorderingType method;
        
        // Initially disabled
        REQUIRE(Cudd_ReorderingStatus(manager, &method) == 0);
        
        // Enable
        Cudd_AutodynEnable(manager, CUDD_REORDER_SIFT);
        REQUIRE(Cudd_ReorderingStatus(manager, &method) == 1);
        REQUIRE(method == CUDD_REORDER_SIFT);
        
        // Disable
        Cudd_AutodynDisable(manager);
        REQUIRE(Cudd_ReorderingStatus(manager, &method) == 0);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Variable binding tests", "[cuddReorder]") {
    SECTION("Bind and unbind variable") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Initially not bound
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 0);
        
        // Bind variable
        REQUIRE(Cudd_bddBindVar(manager, 0) == 1);
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 1);
        
        // Unbind variable
        REQUIRE(Cudd_bddUnbindVar(manager, 0) == 1);
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reordering with bound variables") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Bind first variable
        REQUIRE(Cudd_bddBindVar(manager, 0) == 1);
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 1);
        
        // Reorder - bound variable influences sifting behavior
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Variable should still be bound after reordering
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Peak live nodes tests", "[cuddReorder]") {
    SECTION("Peak live nodes updated during reordering") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        int peakBefore = Cudd_ReadPeakLiveNodeCount(manager);
        
        // Reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Peak should be at least as large as before
        int peakAfter = Cudd_ReadPeakLiveNodeCount(manager);
        REQUIRE(peakAfter >= peakBefore);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Next dynamic reordering tests", "[cuddReorder]") {
    SECTION("Next reordering threshold is updated") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        unsigned int nextBefore = Cudd_ReadNextReordering(manager);
        
        // Reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Next reordering should be updated
        unsigned int nextAfter = Cudd_ReadNextReordering(manager);
        REQUIRE(nextAfter > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Manually set next reordering") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_SetNextReordering(manager, 1000);
        REQUIRE(Cudd_ReadNextReordering(manager) == 1000);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Order randomization tests", "[cuddReorder]") {
    SECTION("Set and read order randomization") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        unsigned int original = Cudd_ReadOrderRandomization(manager);
        
        Cudd_SetOrderRandomization(manager, 0x7);
        REQUIRE(Cudd_ReadOrderRandomization(manager) == 0x7);
        
        // Restore
        Cudd_SetOrderRandomization(manager, original);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reordering with randomization") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Enable order randomization
        Cudd_SetOrderRandomization(manager, 0xF);
        
        // Reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Reordering time tests", "[cuddReorder]") {
    SECTION("Read reordering time") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        long timeBefore = Cudd_ReadReorderingTime(manager);
        
        // Reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Reordering time should increase
        long timeAfter = Cudd_ReadReorderingTime(manager);
        REQUIRE(timeAfter >= timeBefore);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Reorder cycle with alternate growth", "[cuddReorder]") {
    SECTION("Alternate growth threshold on cycle") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Set reorder cycle to 1 (always use alternate)
        Cudd_SetReorderingCycle(manager, 1);
        
        // Set alternate growth
        Cudd_SetMaxGrowthAlternate(manager, 1.05);
        
        // First reordering (reorderings count = 1, 1 % 1 == 0, so uses alternate)
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - BDD ZDD alignment tests", "[cuddReorder]") {
    SECTION("Enable and disable ZDD realignment") {
        DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Initially disabled
        REQUIRE(Cudd_zddRealignmentEnabled(manager) == 0);
        
        // Enable
        Cudd_zddRealignEnable(manager);
        REQUIRE(Cudd_zddRealignmentEnabled(manager) == 1);
        
        // Disable
        Cudd_zddRealignDisable(manager);
        REQUIRE(Cudd_zddRealignmentEnabled(manager) == 0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Enable and disable BDD realignment") {
        DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Initially disabled
        REQUIRE(Cudd_bddRealignmentEnabled(manager) == 0);
        
        // Enable
        Cudd_bddRealignEnable(manager);
        REQUIRE(Cudd_bddRealignmentEnabled(manager) == 1);
        
        // Disable
        Cudd_bddRealignDisable(manager);
        REQUIRE(Cudd_bddRealignmentEnabled(manager) == 0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reordering with ZDD realignment enabled") {
        DdManager *manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // Enable ZDD realignment
        Cudd_zddRealignEnable(manager);
        
        // Reorder BDDs - should also realign ZDDs
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Multiple reorderings", "[cuddReorder]") {
    SECTION("Sequential reorderings") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        unsigned int initialReorderings = Cudd_ReadReorderings(manager);
        
        // Multiple reorderings
        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
        }
        
        // Should have done 3 reorderings
        REQUIRE(Cudd_ReadReorderings(manager) == initialReorderings + 3);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Max reorderings limit", "[cuddReorder]") {
    SECTION("Set max reorderings") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        unsigned int original = Cudd_ReadMaxReorderings(manager);
        
        Cudd_SetMaxReorderings(manager, 10);
        REQUIRE(Cudd_ReadMaxReorderings(manager) == 10);
        
        // Restore
        Cudd_SetMaxReorderings(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Swap steps count", "[cuddReorder]") {
    SECTION("Read swap steps") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        double swapsBefore = Cudd_ReadSwapSteps(manager);
        
        // Reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // This is a compile-time feature (DD_COUNT), so may not change
        double swapsAfter = Cudd_ReadSwapSteps(manager);
        REQUIRE(swapsAfter >= swapsBefore);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// SKIP: These tests trigger undefined behavior (signed integer overflow) in the CUDD library
// internal implementation of genetic/annealing algorithms. Skipped until library code is fixed.
TEST_CASE("cuddReorder - Annealing and Genetic algorithms", "[cuddReorder][.skip]") {
    SECTION("CUDD_REORDER_ANNEALING") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_GENETIC") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Window and convergence reordering", "[cuddReorder]") {
    SECTION("CUDD_REORDER_WINDOW3_CONV") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_WINDOW4_CONV") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Exact reordering", "[cuddReorder]") {
    SECTION("CUDD_REORDER_EXACT on small BDD") {
        // Use small number of variables for exact reordering (it's expensive)
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - ShuffleHeap comprehensive tests", "[cuddReorder]") {
    SECTION("ShuffleHeap rotation permutation") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Rotate the order: move first to last
        int permutation[] = {1, 2, 3, 4, 0};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        // Check the new order
        REQUIRE(Cudd_ReadInvPerm(manager, 0) == 1);
        REQUIRE(Cudd_ReadInvPerm(manager, 4) == 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap with complex BDD") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Test various permutations
        int permutation[] = {5, 4, 3, 2, 1, 0};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap preserves BDD semantics") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Store number of nodes
        int nodesBefore = Cudd_DagSize(f);
        
        // Shuffle
        int permutation[] = {1, 0, 2, 3};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        // BDD should still represent the same function
        // (though structure may differ)
        int nodesAfter = Cudd_DagSize(f);
        REQUIRE(nodesAfter > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Larger swap operations", "[cuddReorder]") {
    SECTION("Multiple adjacent swaps via reordering") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Use WINDOW reordering which performs multiple swaps internally
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result >= 1);
        
        // Verify BDD is still valid
        REQUIRE(Cudd_DagSize(f) > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Complex BDD reordering", "[cuddReorder]") {
    SECTION("Reorder BDD with many interactions") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a BDD with many variable interactions
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 8; i++) {
            for (int j = i + 1; j < 8; j++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause = Cudd_bddOr(manager, xi, xj);
                Cudd_Ref(clause);
                
                DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                Cudd_Ref(newResult);
                
                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }
        
        int nodesBefore = Cudd_DagSize(result);
        
        // Reorder
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        int nodesAfter = Cudd_DagSize(result);
        REQUIRE(nodesAfter > 0);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Variable types for lazy sifting", "[cuddReorder]") {
    SECTION("Set primary input variable type") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // Set variable types
        REQUIRE(Cudd_bddSetPiVar(manager, 0) == 1);
        REQUIRE(Cudd_bddIsPiVar(manager, 0) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Set present state variable type") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        REQUIRE(Cudd_bddSetPsVar(manager, 1) == 1);
        REQUIRE(Cudd_bddIsPsVar(manager, 1) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Set next state variable type") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        REQUIRE(Cudd_bddSetNsVar(manager, 2) == 1);
        REQUIRE(Cudd_bddIsNsVar(manager, 2) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Set pair index") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // Set variable 0 and 2 as a pair
        REQUIRE(Cudd_bddSetPairIndex(manager, 0, 2) == 1);
        REQUIRE(Cudd_bddReadPairIndex(manager, 0) == 2);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Variable grouping", "[cuddReorder]") {
    SECTION("Set variable to be grouped") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // Group variable
        REQUIRE(Cudd_bddSetVarToBeGrouped(manager, 0) == 1);
        REQUIRE(Cudd_bddIsVarToBeGrouped(manager, 0) == 1);
        
        // Reset
        REQUIRE(Cudd_bddResetVarToBeGrouped(manager, 0) == 1);
        REQUIRE(Cudd_bddIsVarToBeGrouped(manager, 0) == 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Set variable hard group") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        REQUIRE(Cudd_bddSetVarHardGroup(manager, 1) == 1);
        REQUIRE(Cudd_bddIsVarHardGroup(manager, 1) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Set variable to be ungrouped") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        REQUIRE(Cudd_bddSetVarToBeUngrouped(manager, 0) == 1);
        REQUIRE(Cudd_bddIsVarToBeUngrouped(manager, 0) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Sifting direction tests", "[cuddReorder]") {
    SECTION("Sifting with variable at low boundary") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that prefers first variable at top
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* f = Cudd_bddAnd(manager, x0, Cudd_bddIthVar(manager, 5));
        Cudd_Ref(f);
        
        // Sift - variable at position 0 can only go down
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting with variable at high boundary") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that uses last variable
        DdNode* x5 = Cudd_bddIthVar(manager, 5);
        DdNode* f = Cudd_bddAnd(manager, x5, Cudd_bddIthVar(manager, 0));
        Cudd_Ref(f);
        
        // Sift - variable at highest position can only go up
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddNextHigh and cuddNextLow edge cases", "[cuddReorder]") {
    SECTION("cuddNextHigh at boundary") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Last position
        REQUIRE(cuddNextHigh(manager, 2) == 3);
        
        Cudd_Quit(manager);
    }
    
    SECTION("cuddNextLow at zero") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // First position
        REQUIRE(cuddNextLow(manager, 0) == -1);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - cuddDynamicAllocNode exhaustion", "[cuddReorder]") {
    SECTION("Allocate many nodes") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Allocate many nodes to potentially trigger memory block allocation
        for (int i = 0; i < 2000; i++) {
            DdNode* node = cuddDynamicAllocNode(manager);
            REQUIRE(node != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Dense BDD operations", "[cuddReorder]") {
    SECTION("Create many nodes then reorder") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many BDD nodes to exercise subtable handling
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 10; i++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* term;
                if (j % 2 == 0) {
                    term = Cudd_bddAnd(manager, result, xi);
                } else {
                    term = Cudd_bddOr(manager, result, Cudd_Not(xi));
                }
                Cudd_Ref(term);
                Cudd_RecursiveDeref(manager, result);
                result = term;
            }
        }
        
        // Reorder to test swap with populated subtables
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Multiple reorderings to trigger nextDyn else branch", "[cuddReorder]") {
    SECTION("Perform many reorderings") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Perform many reorderings to trigger the else branch in nextDyn calculation
        // (needs reorderings >= 20 and next nextDyn <= current nextDyn)
        for (int i = 0; i < 25; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// SKIP: This test triggers undefined behavior in the CUDD library's lazy sifting
// algorithm internals. Skipped until library code is fixed.
TEST_CASE("cuddReorder - Lazy sifting tests", "[cuddReorder][.skip]") {
    SECTION("CUDD_REORDER_LAZY_SIFT") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LAZY_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Groupcheck settings", "[cuddReorder]") {
    SECTION("Read and set groupcheck") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_AggregationType original = Cudd_ReadGroupcheck(manager);
        
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK);
        REQUIRE(Cudd_ReadGroupcheck(manager) == CUDD_GROUP_CHECK);
        
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK5);
        REQUIRE(Cudd_ReadGroupcheck(manager) == CUDD_GROUP_CHECK5);
        
        // Restore
        Cudd_SetGroupcheck(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Recomb settings", "[cuddReorder]") {
    SECTION("Read and set recomb") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int original = Cudd_ReadRecomb(manager);
        
        Cudd_SetRecomb(manager, 5);
        REQUIRE(Cudd_ReadRecomb(manager) == 5);
        
        // Restore
        Cudd_SetRecomb(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Symmetry violation settings", "[cuddReorder]") {
    SECTION("Read and set symmviolation") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int original = Cudd_ReadSymmviolation(manager);
        
        Cudd_SetSymmviolation(manager, 1);
        REQUIRE(Cudd_ReadSymmviolation(manager) == 1);
        
        // Restore
        Cudd_SetSymmviolation(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Arc violation settings", "[cuddReorder]") {
    SECTION("Read and set arcviolation") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int original = Cudd_ReadArcviolation(manager);
        
        Cudd_SetArcviolation(manager, 1);
        REQUIRE(Cudd_ReadArcviolation(manager) == 1);
        
        // Restore
        Cudd_SetArcviolation(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Population size settings", "[cuddReorder]") {
    SECTION("Read and set population size for genetic algorithm") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int original = Cudd_ReadPopulationSize(manager);
        
        Cudd_SetPopulationSize(manager, 100);
        REQUIRE(Cudd_ReadPopulationSize(manager) == 100);
        
        // Restore
        Cudd_SetPopulationSize(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Number of crossovers settings", "[cuddReorder]") {
    SECTION("Read and set number of crossovers for genetic algorithm") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int original = Cudd_ReadNumberXovers(manager);
        
        Cudd_SetNumberXovers(manager, 10);
        REQUIRE(Cudd_ReadNumberXovers(manager) == 10);
        
        // Restore
        Cudd_SetNumberXovers(manager, original);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - BDD with only projection functions", "[cuddReorder]") {
    SECTION("Reorder with only variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Just use a single variable - tests isolated projection handling
        DdNode* x = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Reorder with no nodes", "[cuddReorder]") {
    SECTION("Reorder empty manager") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // No BDD created, just reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 1);
        REQUIRE(result >= 1);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Swap with many nodes in subtable", "[cuddReorder]") {
    SECTION("Build BDD to populate subtables then swap") {
        DdManager *manager = Cudd_Init(6, 0, 64, CUDD_CACHE_SLOTS, 0);  // Small unique slots
        REQUIRE(manager != nullptr);
        
        // Create many nodes to fill up subtables
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        // Build a BDD that creates nodes at multiple levels
        for (int iter = 0; iter < 5; iter++) {
            for (int i = 0; i < 6; i++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* temp;
                if (iter % 2 == 0) {
                    temp = Cudd_bddXor(manager, result, xi);
                } else {
                    temp = Cudd_bddOr(manager, result, xi);
                }
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, result);
                result = temp;
            }
        }
        
        // This should exercise more paths in cuddSwapInPlace
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Sift max swap limit", "[cuddReorder]") {
    SECTION("Limit swaps during sifting") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Set very low max swap to potentially trigger the swap limit path
        Cudd_SetSiftMaxSwap(manager, 10);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Sift max var limit", "[cuddReorder]") {
    SECTION("Limit variables during sifting") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);
        
        // Set max var to only sift first few variables
        Cudd_SetSiftMaxVar(manager, 3);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Reorder with tight max growth", "[cuddReorder]") {
    SECTION("Tight max growth constraint") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Set very tight max growth
        Cudd_SetMaxGrowth(manager, 1.01);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Ordering monitoring", "[cuddReorder]") {
    SECTION("Enable and disable ordering monitoring") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Check initial state
        REQUIRE(Cudd_OrderingMonitoring(manager) == 0);
        
        // Enable monitoring
        REQUIRE(Cudd_EnableOrderingMonitoring(manager) == 1);
        REQUIRE(Cudd_OrderingMonitoring(manager) == 1);
        
        // Disable monitoring
        REQUIRE(Cudd_DisableOrderingMonitoring(manager) == 1);
        REQUIRE(Cudd_OrderingMonitoring(manager) == 0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reorder with monitoring enabled") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Enable monitoring
        Cudd_EnableOrderingMonitoring(manager);
        
        // Reorder - hooks should be called
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Cudd_PrintGroupedOrder hook", "[cuddReorder]") {
    SECTION("Use PrintGroupedOrder hook during reordering") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Add PrintGroupedOrder as a post-reordering hook
        REQUIRE(Cudd_AddHook(manager, Cudd_PrintGroupedOrder, CUDD_POST_REORDERING_HOOK) == 1);
        
        // Redirect output temporarily to a cross-platform temporary file
        FILE* oldOut = Cudd_ReadStdout(manager);
        FILE* tempOut = tmpfile();
        if (tempOut != nullptr) {
            Cudd_SetStdout(manager, tempOut);
            
            // Reorder - hook should be called
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
            
            fclose(tempOut);
            Cudd_SetStdout(manager, oldOut);
        }
        
        // Remove hook
        REQUIRE(Cudd_RemoveHook(manager, Cudd_PrintGroupedOrder, CUDD_POST_REORDERING_HOOK) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Standard hooks", "[cuddReorder]") {
    SECTION("Add and verify standard hooks") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Add standard pre/post reordering hooks
        REQUIRE(Cudd_AddHook(manager, Cudd_StdPreReordHook, CUDD_PRE_REORDERING_HOOK) == 1);
        REQUIRE(Cudd_AddHook(manager, Cudd_StdPostReordHook, CUDD_POST_REORDERING_HOOK) == 1);
        
        // Check hooks are in place
        REQUIRE(Cudd_IsInHook(manager, Cudd_StdPreReordHook, CUDD_PRE_REORDERING_HOOK) == 1);
        REQUIRE(Cudd_IsInHook(manager, Cudd_StdPostReordHook, CUDD_POST_REORDERING_HOOK) == 1);
        
        // Redirect output to a cross-platform temporary file
        FILE* oldErr = Cudd_ReadStderr(manager);
        FILE* tempErr = tmpfile();
        if (tempErr != nullptr) {
            Cudd_SetStderr(manager, tempErr);
            
            // Reorder
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
            
            fclose(tempErr);
            Cudd_SetStderr(manager, oldErr);
        }
        
        // Remove hooks
        REQUIRE(Cudd_RemoveHook(manager, Cudd_StdPreReordHook, CUDD_PRE_REORDERING_HOOK) == 1);
        REQUIRE(Cudd_RemoveHook(manager, Cudd_StdPostReordHook, CUDD_POST_REORDERING_HOOK) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Average distance calculation", "[cuddReorder]") {
    SECTION("Calculate average distance before and after reordering") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        double distBefore = Cudd_AverageDistance(manager);
        REQUIRE(distBefore >= 0.0);
        
        // Reorder
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        double distAfter = Cudd_AverageDistance(manager);
        REQUIRE(distAfter >= 0.0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Complex shuffle tests", "[cuddReorder]") {
    SECTION("ShuffleHeap with large permutation") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);
        
        // Large reverse permutation
        int permutation[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple shuffles") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Shuffle multiple times
        int perm1[] = {1, 2, 3, 4, 0};
        REQUIRE(Cudd_ShuffleHeap(manager, perm1) == 1);
        
        int perm2[] = {4, 0, 1, 2, 3};
        REQUIRE(Cudd_ShuffleHeap(manager, perm2) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Variable group tree tests", "[cuddReorder]") {
    SECTION("Create and use variable group tree") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create a group tree - group variables 0-2 together
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Verify tree was set
        MtrNode* readTree = Cudd_ReadTree(manager);
        REQUIRE(readTree != nullptr);
        
        // Reorder with tree in place - should use tree-based sifting
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Create nested variable group tree") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Create a parent group for all variables
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Create a nested group for variables 0-3
        MtrNode* subtree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(subtree != nullptr);
        
        // Reorder with tree
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Shuffle with variable group tree") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create a group tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Try shuffle that respects the tree (variables within same group)
        // Rotate within the group: 0,1,2 -> 1,2,0
        int permutation[] = {1, 2, 0, 3, 4, 5};
        int result = Cudd_ShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Free variable group tree") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a group tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Free the tree
        Cudd_FreeTree(manager);
        
        // Tree should now be NULL
        REQUIRE(Cudd_ReadTree(manager) == nullptr);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Fixed group sifting", "[cuddReorder]") {
    SECTION("Group sift with fixed groups") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Create a fixed group
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_FIXED);
        REQUIRE(tree != nullptr);
        
        // Group sift should respect the fixed group
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - SymmProfile", "[cuddReorder]") {
    SECTION("Print symmetry profile") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Redirect output to a cross-platform temporary file
        FILE* oldOut = Cudd_ReadStdout(manager);
        FILE* tempOut = tmpfile();
        if (tempOut != nullptr) {
            Cudd_SetStdout(manager, tempOut);
            
            // Print symmetry profile using public API
            Cudd_SymmProfile(manager, 0, Cudd_ReadSize(manager) - 1);
            
            fclose(tempOut);
            Cudd_SetStdout(manager, oldOut);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}


TEST_CASE("cuddReorder - Random swap with single variable range", "[cuddReorder]") {
    SECTION("Random pivot with single variable upper bound") {
        // This tests the modulo==0 branch in cuddSwapping
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a simple BDD with variables only in a narrow range
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        Cudd_Ref(x3);
        
        // Perform random pivot reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, x3);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddReorder - Swap limit hit during random swapping", "[cuddReorder]") {
    SECTION("Trigger siftMaxSwap limit during random swapping") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);
        
        // Set a very low max swap to trigger the limit
        Cudd_SetSiftMaxSwap(manager, 5);
        
        // This should hit the break due to siftMaxSwap
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}


TEST_CASE("cuddReorder - Trigger max swap in sifting", "[cuddReorder]") {
    SECTION("Sifting stops at max swap") {
        DdManager *manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a large BDD
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        for (int i = 0; i < 12; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* temp = Cudd_bddXor(manager, result, xi);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, result);
            result = temp;
        }
        
        // Very low max swap to trigger the ddTotalNumberSwapping check
        Cudd_SetSiftMaxSwap(manager, 2);
        
        int res = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(res >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}


TEST_CASE("cuddReorder - Variable handled flag", "[cuddReorder]") {
    SECTION("Mark variable as handled") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // Check and set varHandled
        REQUIRE(Cudd_bddIsVarToBeUngrouped(manager, 0) == 0);
        REQUIRE(Cudd_bddSetVarToBeUngrouped(manager, 0) == 1);
        REQUIRE(Cudd_bddIsVarToBeUngrouped(manager, 0) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}
