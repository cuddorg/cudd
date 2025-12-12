#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtrInt.h gives full MtrNode structure
#include "mtrInt.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddGroup.c
 * 
 * This file contains comprehensive tests for the cuddGroup module
 * to achieve 90% code coverage. Tests cover:
 * - Cudd_MakeTreeNode function
 * - cuddTreeSifting (via CUDD_REORDER_GROUP_SIFT)
 * - Group sifting algorithms
 * - Variable grouping and tree operations
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

// Helper function to create a larger BDD for reordering tests
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

// Helper to create BDD with extended symmetry pattern
static DdNode* createExtSymmBdd(DdManager* manager, int numVars) {
    if (numVars < 4) return nullptr;
    
    // f = (x0 XOR x1) AND (x2 XOR x3) - creates extended symmetry conditions
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* x2 = Cudd_bddIthVar(manager, 2);
    DdNode* x3 = Cudd_bddIthVar(manager, 3);
    
    DdNode* xor1 = Cudd_bddXor(manager, x0, x1);
    Cudd_Ref(xor1);
    
    DdNode* xor2 = Cudd_bddXor(manager, x2, x3);
    Cudd_Ref(xor2);
    
    DdNode* result = Cudd_bddAnd(manager, xor1, xor2);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, xor1);
    Cudd_RecursiveDeref(manager, xor2);
    
    return result;
}

// ============================================================================
// Tests for Cudd_MakeTreeNode
// ============================================================================

TEST_CASE("cuddGroup - Cudd_MakeTreeNode basic tests", "[cuddGroup]") {
    SECTION("Create tree node for existing variables") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a group tree node for variables 0-2
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Verify tree was set
        MtrNode* readTree = Cudd_ReadTree(manager);
        REQUIRE(readTree != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create tree node with MTR_FIXED") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a fixed group
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_FIXED);
        REQUIRE(tree != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create tree node when tree doesn't exist") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Tree should be NULL initially
        REQUIRE(Cudd_ReadTree(manager) == nullptr);
        
        // Create tree node - should create the tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Now tree should exist
        REQUIRE(Cudd_ReadTree(manager) != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create tree node for non-existing variables") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create group for variables that don't exist yet (low >= size)
        // The position is assumed to be the same as the index
        MtrNode* tree = Cudd_MakeTreeNode(manager, 5, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create nested tree nodes") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create outer group
        MtrNode* outer = Cudd_MakeTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(outer != nullptr);
        
        // Create inner group
        MtrNode* inner = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(inner != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create tree node for empty manager") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create group for manager with no variables
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Extend tree upper bound") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // First create a small tree
        MtrNode* tree1 = Cudd_MakeTreeNode(manager, 0, 2, MTR_DEFAULT);
        REQUIRE(tree1 != nullptr);
        
        // Then create a larger tree that extends beyond existing size
        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 2, 4, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddTreeSifting via GROUP_SIFT reordering
// ============================================================================

TEST_CASE("cuddGroup - cuddTreeSifting via GROUP_SIFT", "[cuddGroup]") {
    SECTION("Group sift with no tree") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // GROUP_SIFT without tree - creates temporary tree
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sift with existing tree") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create a group tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Group sift with existing tree
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sift with nested groups") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Create parent group
        MtrNode* parent = Cudd_MakeTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create child groups
        MtrNode* child1 = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Cudd_MakeTreeNode(manager, 4, 4, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Group sift with nested tree
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sift with fixed groups") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Create a fixed group - should not be reordered internally
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_FIXED);
        REQUIRE(tree != nullptr);
        
        // Create a non-fixed group
        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 4, 4, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);
        
        // Group sift
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for GROUP_SIFT_CONV (convergent group sifting)
// ============================================================================

TEST_CASE("cuddGroup - GROUP_SIFT_CONV tests", "[cuddGroup]") {
    SECTION("Convergent group sift basic") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Convergent group sift with tree") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for LAZY_SIFT reordering (using GROUP_SIFT which exercises similar paths)
// ============================================================================

TEST_CASE("cuddGroup - LAZY_SIFT tests", "[cuddGroup]") {
    SECTION("Group sift basic test") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Use GROUP_SIFT which exercises cuddTreeSifting path
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for groupcheck settings affecting group sifting
// ============================================================================

TEST_CASE("cuddGroup - Groupcheck settings", "[cuddGroup]") {
    SECTION("Group sift with CUDD_NO_CHECK") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        Cudd_SetGroupcheck(manager, CUDD_NO_CHECK);
        REQUIRE(Cudd_ReadGroupcheck(manager) == CUDD_NO_CHECK);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sift with CUDD_GROUP_CHECK5") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createExtSymmBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK5);
        REQUIRE(Cudd_ReadGroupcheck(manager) == CUDD_GROUP_CHECK5);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sift with CUDD_GROUP_CHECK7") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createExtSymmBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK7);
        REQUIRE(Cudd_ReadGroupcheck(manager) == CUDD_GROUP_CHECK7);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddReorderChildren via different reorder methods
// ============================================================================

TEST_CASE("cuddGroup - ddReorderChildren via tree sifting", "[cuddGroup]") {
    SECTION("Tree sifting with SIFT method") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create group tree - will use SIFT on children
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Tree sifting with SIFT_CONVERGE method") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Tree sifting with RANDOM method") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Tree sifting with WINDOW methods") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Tree sifting with SYMM_SIFT method") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createExtSymmBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Tree sifting with LINEAR method") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Tree sifting with EXACT method") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddFindNodeHiLo edge cases
// ============================================================================

TEST_CASE("cuddGroup - ddFindNodeHiLo edge cases", "[cuddGroup]") {
    SECTION("Group with existing variables") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create group for all existing variables
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Group sift should handle this case
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Group with subset of variables") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create group for subset of existing variables
        MtrNode* tree = Cudd_MakeTreeNode(manager, 2, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for group move operations
// ============================================================================

TEST_CASE("cuddGroup - Group move operations", "[cuddGroup]") {
    SECTION("Group sift triggers group moves") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD with many interactions
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 9; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xi1 = Cudd_bddIthVar(manager, i + 1);
            DdNode* clause = Cudd_bddAnd(manager, xi, xi1);
            Cudd_Ref(clause);
            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);
            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }
        
        // Create groups to force group moves
        MtrNode* tree1 = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree1 != nullptr);
        
        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 3, 3, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);
        
        // Enable group check to trigger aggregation
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK7);
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for second difference check (ddSecDiffCheck)
// ============================================================================

TEST_CASE("cuddGroup - Second difference check", "[cuddGroup]") {
    SECTION("Group sift with second difference check") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD structure that might trigger second difference aggregation
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 8; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* temp = Cudd_bddAnd(manager, result, xi);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, result);
            result = temp;
        }
        
        // Set recomb threshold for second difference check
        Cudd_SetRecomb(manager, 0);
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK7);
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for extended symmetry check (ddExtSymmCheck)
// ============================================================================

TEST_CASE("cuddGroup - Extended symmetry check", "[cuddGroup]") {
    SECTION("Group sift with extended symmetry check") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD with extended symmetry pattern
        DdNode* f = createExtSymmBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Set symmetry violation threshold
        Cudd_SetSymmviolation(manager, 10);
        Cudd_SetArcviolation(manager, 10);
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK5);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for variable handled flags (group sifting helpers)
// ============================================================================

TEST_CASE("cuddGroup - Variable handled flags", "[cuddGroup]") {
    SECTION("Group sift covers variable handling") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // These flags are set/checked internally during group sifting
        // Test through group sifting
        DdNode* f = createComplexBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for merge and dissolve group operations
// ============================================================================

TEST_CASE("cuddGroup - Merge and dissolve groups", "[cuddGroup]") {
    SECTION("Group merging during sifting") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Create multiple groups
        MtrNode* tree1 = Cudd_MakeTreeNode(manager, 0, 2, MTR_DEFAULT);
        REQUIRE(tree1 != nullptr);
        
        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 2, 2, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);
        
        // Enable group check to potentially trigger merging
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK7);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for sifting direction (up vs down)
// ============================================================================

TEST_CASE("cuddGroup - Sifting direction tests", "[cuddGroup]") {
    SECTION("Variable at bottom sifts up") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD where bottom variable has many keys
        DdNode* x5 = Cudd_bddIthVar(manager, 5);
        DdNode* result = Cudd_bddAnd(manager, x5, Cudd_bddIthVar(manager, 0));
        Cudd_Ref(result);
        
        for (int i = 1; i < 5; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* temp = Cudd_bddOr(manager, result, xi);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, result);
            result = temp;
        }
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable at top sifts down") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD where top variable has many keys
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* result = Cudd_bddAnd(manager, x0, Cudd_bddIthVar(manager, 5));
        Cudd_Ref(result);
        
        for (int i = 1; i < 5; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* temp = Cudd_bddOr(manager, result, xi);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, result);
            result = temp;
        }
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable in middle chooses shorter path") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for group sifting backward (restoring best position)
// ============================================================================

TEST_CASE("cuddGroup - Group sifting backward", "[cuddGroup]") {
    SECTION("Backward sifting restores best position") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Create groups for group backward moves
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int nodesBefore = Cudd_DagSize(f);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        // BDD should still be valid
        int nodesAfter = Cudd_DagSize(f);
        REQUIRE(nodesAfter > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for max growth limits during sifting
// ============================================================================

TEST_CASE("cuddGroup - Max growth limits", "[cuddGroup]") {
    SECTION("Tight max growth limit") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        // Set very tight max growth
        Cudd_SetMaxGrowth(manager, 1.01);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Max growth with groups") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        Cudd_SetMaxGrowth(manager, 1.1);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for max swap limits during sifting
// ============================================================================

TEST_CASE("cuddGroup - Max swap limits", "[cuddGroup]") {
    SECTION("Low max swap limit") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);
        
        // Set very low max swap
        Cudd_SetSiftMaxSwap(manager, 5);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for max var limits during sifting
// ============================================================================

TEST_CASE("cuddGroup - Max var limits", "[cuddGroup]") {
    SECTION("Low max var limit") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);
        
        // Set low max var
        Cudd_SetSiftMaxVar(manager, 3);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for bound variables during group sifting
// ============================================================================

TEST_CASE("cuddGroup - Bound variables", "[cuddGroup]") {
    SECTION("Group sift with bound variable") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Bind variable
        Cudd_bddBindVar(manager, 2);
        REQUIRE(Cudd_bddVarIsBound(manager, 2) == 1);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for free tree operation
// ============================================================================

TEST_CASE("cuddGroup - Free tree operation", "[cuddGroup]") {
    SECTION("Free tree after group sift") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        REQUIRE(Cudd_ReadTree(manager) != nullptr);
        
        Cudd_FreeTree(manager);
        REQUIRE(Cudd_ReadTree(manager) == nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for Cudd_SetTree
// ============================================================================

TEST_CASE("cuddGroup - Set tree operation", "[cuddGroup]") {
    SECTION("Set custom tree") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create tree externally
        MtrNode* tree = Mtr_InitGroupTree(0, 6);
        REQUIRE(tree != nullptr);
        tree->index = 0;
        
        Cudd_SetTree(manager, tree);
        REQUIRE(Cudd_ReadTree(manager) == tree);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for complex BDD structures
// ============================================================================

TEST_CASE("cuddGroup - Complex BDD structures", "[cuddGroup]") {
    SECTION("Dense BDD with many interactions") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create dense BDD
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 10; i++) {
            for (int j = i + 1; j < 10; j += 2) {
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
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for single variable groups
// ============================================================================

TEST_CASE("cuddGroup - Single variable handling", "[cuddGroup]") {
    SECTION("Group sift with singleton") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create single-variable groups
        MtrNode* tree1 = Cudd_MakeTreeNode(manager, 0, 1, MTR_DEFAULT);
        REQUIRE(tree1 != nullptr);
        
        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 1, 1, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddVarGroupCheck in group sifting
// ============================================================================

TEST_CASE("cuddGroup - ddVarGroupCheck", "[cuddGroup]") {
    SECTION("Group sift basic check") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        // Create a group tree to exercise ddVarGroupCheck-related paths
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Set a different groupcheck - triggers different code paths
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK7);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for multiple reorderings
// ============================================================================

TEST_CASE("cuddGroup - Multiple reorderings", "[cuddGroup]") {
    SECTION("Sequential group sifts") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // Multiple reorderings
        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for early termination conditions
// ============================================================================

TEST_CASE("cuddGroup - Early termination", "[cuddGroup]") {
    SECTION("Return early with minimal BDD") {
        DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create minimal BDD - just one variable
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        // Group sift with minimal BDD should still work
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for interaction with other reordering methods
// ============================================================================

TEST_CASE("cuddGroup - Interaction with window reordering", "[cuddGroup]") {
    SECTION("GROUP_SIFT_CONV uses window reordering") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        // GROUP_SIFT_CONV internally uses window reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for allVars check in ddGroupSiftingDown
// ============================================================================

TEST_CASE("cuddGroup - All variables check", "[cuddGroup]") {
    SECTION("Group with only projection functions") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Just use projection functions (variables)
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for aggregation check during sifting
// ============================================================================

TEST_CASE("cuddGroup - Aggregation during sifting", "[cuddGroup]") {
    SECTION("GROUP_CHECK7 triggers aggregation check") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD structure that might trigger aggregation
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 9; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xi1 = Cudd_bddIthVar(manager, i + 1);
            DdNode* clause = Cudd_bddXor(manager, xi, xi1);
            Cudd_Ref(clause);
            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);
            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }
        
        Cudd_SetGroupcheck(manager, CUDD_GROUP_CHECK7);
        Cudd_SetRecomb(manager, 0);
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for lower bound computation
// ============================================================================

TEST_CASE("cuddGroup - Lower bound computation", "[cuddGroup]") {
    SECTION("Sifting with isolated variables") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that leaves some variables isolated
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x2);
        Cudd_Ref(t1);
        
        DdNode* result = Cudd_bddOr(manager, t1, x4);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, t1);
        
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for Cudd_MakeTreeNode size overflow check
// ============================================================================

TEST_CASE("cuddGroup - MakeTreeNode edge cases", "[cuddGroup]") {
    SECTION("Large size parameter") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create large group
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 1000, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        Cudd_Quit(manager);
    }
}
