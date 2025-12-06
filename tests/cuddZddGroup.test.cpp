#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtrInt.h gives full MtrNode structure
#include "mtrInt.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Comprehensive test file for cuddZddGroup.c
 * 
 * This file contains comprehensive tests for the cuddZddGroup module
 * to achieve 90% code coverage. Tests cover:
 * - Cudd_MakeZddTreeNode function
 * - cuddZddTreeSifting (via various reordering methods)
 * - Group sifting algorithms
 * - Variable grouping and tree operations for ZDDs
 */

// Helper function to create a simple ZDD with multiple variables
static DdNode* createSimpleZdd(DdManager* manager, int numVars) {
    if (numVars < 2) return nullptr;
    
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars; i++) {
        DdNode* zvar = Cudd_zddIthVar(manager, i);
        if (zvar == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(zvar);
        
        DdNode* temp = Cudd_zddUnion(manager, result, zvar);
        if (temp == nullptr) {
            Cudd_RecursiveDerefZdd(manager, zvar);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, zvar);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// Helper function to create a ZDD with variable interactions
static DdNode* createComplexZdd(DdManager* manager, int numVars) {
    if (numVars < 3) return nullptr;
    
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars - 1; i++) {
        DdNode* var1 = Cudd_zddIthVar(manager, i);
        DdNode* var2 = Cudd_zddIthVar(manager, i + 1);
        if (var1 == nullptr || var2 == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        
        DdNode* prod = Cudd_zddProduct(manager, var1, var2);
        if (prod == nullptr) {
            prod = Cudd_zddUnion(manager, var1, var2);
        }
        if (prod == nullptr) {
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(prod);
        
        DdNode* temp = Cudd_zddUnion(manager, result, prod);
        if (temp == nullptr) {
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// Helper to create a larger ZDD with many nodes for testing reordering
static DdNode* createLargeZdd(DdManager* manager, int numVars) {
    if (numVars < 4) return nullptr;
    
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    // Create a ZDD with multiple combinations
    for (int i = 0; i < numVars - 2; i++) {
        DdNode* var1 = Cudd_zddIthVar(manager, i);
        DdNode* var2 = Cudd_zddIthVar(manager, i + 1);
        DdNode* var3 = Cudd_zddIthVar(manager, i + 2);
        
        if (var1 == nullptr || var2 == nullptr || var3 == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        Cudd_Ref(var3);
        
        // Create product of var1 and var2
        DdNode* prod12 = Cudd_zddProduct(manager, var1, var2);
        if (prod12 == nullptr) {
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, var3);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(prod12);
        
        // Create product of var2 and var3
        DdNode* prod23 = Cudd_zddProduct(manager, var2, var3);
        if (prod23 == nullptr) {
            Cudd_RecursiveDerefZdd(manager, prod12);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, var3);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(prod23);
        
        // Union all combinations
        DdNode* union1 = Cudd_zddUnion(manager, result, var1);
        Cudd_Ref(union1);
        DdNode* union2 = Cudd_zddUnion(manager, union1, prod12);
        Cudd_Ref(union2);
        DdNode* union3 = Cudd_zddUnion(manager, union2, prod23);
        Cudd_Ref(union3);
        
        Cudd_RecursiveDerefZdd(manager, prod12);
        Cudd_RecursiveDerefZdd(manager, prod23);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, var3);
        Cudd_RecursiveDerefZdd(manager, union1);
        Cudd_RecursiveDerefZdd(manager, union2);
        Cudd_RecursiveDerefZdd(manager, result);
        result = union3;
    }
    
    return result;
}

// ============================================================================
// Tests for Cudd_MakeZddTreeNode
// ============================================================================

TEST_CASE("cuddZddGroup - Cudd_MakeZddTreeNode basic tests", "[cuddZddGroup]") {
    SECTION("Create tree node for existing ZDD variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some ZDD variables first
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        REQUIRE(z2 != nullptr);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        // Create a group of 3 variables starting at index 0
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        REQUIRE(group->size == 3);
        REQUIRE(group->index == 0);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Create tree node with MTR_FIXED type") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        for (int i = 0; i < 4; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        // Create a fixed group
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_FIXED);
        REQUIRE(group != nullptr);
        REQUIRE(group->flags == MTR_FIXED);
        REQUIRE(group->size == 4);
        
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Create multiple groups") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        for (int i = 0; i < 12; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        // Create first group
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        
        // Create second group
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        REQUIRE(group2 != nullptr);
        
        // Create third group
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 8, 4, MTR_DEFAULT);
        REQUIRE(group3 != nullptr);
        
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Create nested groups") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        for (int i = 0; i < 12; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        // Create parent group
        MtrNode* parent = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create child groups within parent
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Cudd_MakeZddTreeNode edge cases", "[cuddZddGroup]") {
    SECTION("Create group before variables exist") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create group before creating variables
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Now create the variables
        for (int i = 0; i < 4; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Create single variable group") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        REQUIRE(z0 != nullptr);
        Cudd_Ref(z0);
        
        // Create a group with just one variable
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 1, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        REQUIRE(group->size == 1);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test tree initialization") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Tree should be NULL initially
        REQUIRE(manager->treeZ == nullptr);
        
        // Create variables
        for (int i = 0; i < 4; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        // Create a group - this should initialize the tree
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        REQUIRE(manager->treeZ != nullptr);
        
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddTreeSifting and group sifting
// ============================================================================

TEST_CASE("cuddZddGroup - Group sifting with CUDD_REORDER_GROUP_SIFT", "[cuddZddGroup]") {
    SECTION("Basic group sifting") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex ZDD
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        REQUIRE(group2 != nullptr);
        
        // Perform group sifting
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sifting with larger ZDD") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create multiple groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 3, 3, MTR_DEFAULT);
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 6, 3, MTR_DEFAULT);
        MtrNode* group4 = Cudd_MakeZddTreeNode(manager, 9, 3, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        REQUIRE(group3 != nullptr);
        REQUIRE(group4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sifting with nested groups") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create parent group
        MtrNode* parent = Cudd_MakeZddTreeNode(manager, 0, 12, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create child groups
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        MtrNode* child3 = Cudd_MakeZddTreeNode(manager, 8, 4, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        REQUIRE(child2 != nullptr);
        REQUIRE(child3 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sifting with fixed groups") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Create a fixed group and a regular group
        MtrNode* fixedGroup = Cudd_MakeZddTreeNode(manager, 0, 5, MTR_FIXED);
        MtrNode* normalGroup = Cudd_MakeZddTreeNode(manager, 5, 5, MTR_DEFAULT);
        REQUIRE(fixedGroup != nullptr);
        REQUIRE(normalGroup != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Group sifting convergence", "[cuddZddGroup]") {
    SECTION("CUDD_REORDER_GROUP_SIFT_CONV") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Create groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 5, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 5, 5, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        // Test group sifting with convergence
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple iterations of group sifting convergence") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create groups
        for (int i = 0; i < 3; i++) {
            MtrNode* group = Cudd_MakeZddTreeNode(manager, i * 4, 4, MTR_DEFAULT);
            REQUIRE(group != nullptr);
        }
        
        // Run convergence multiple times
        for (int i = 0; i < 3; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Tree sifting with different methods", "[cuddZddGroup]") {
    SECTION("CUDD_REORDER_SIFT with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create groups for tree sifting
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_SIFT_CONVERGE with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_SYMM_SIFT with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_SYMM_SIFT_CONV with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_LINEAR with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_LINEAR_CONVERGE with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_RANDOM with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_RANDOM_PIVOT with tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Tree sifting without predefined tree", "[cuddZddGroup]") {
    SECTION("Tree sifting creates temporary tree") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Don't create a tree - tree sifting should create one temporarily
        REQUIRE(manager->treeZ == nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Tree should still be NULL after reordering (temporary tree freed)
        REQUIRE(manager->treeZ == nullptr);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple tree siftings without tree") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Run multiple reorderings without predefined tree
        for (int i = 0; i < 3; i++) {
            REQUIRE(manager->treeZ == nullptr);
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
            REQUIRE(result >= 1);
            REQUIRE(manager->treeZ == nullptr);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Complex group configurations", "[cuddZddGroup]") {
    SECTION("Groups with varying sizes") {
        DdManager* manager = Cudd_Init(0, 15, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 15);
        REQUIRE(zdd != nullptr);
        
        // Create groups of different sizes
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 2, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 2, 5, MTR_DEFAULT);
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 7, 3, MTR_DEFAULT);
        MtrNode* group4 = Cudd_MakeZddTreeNode(manager, 10, 5, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        REQUIRE(group3 != nullptr);
        REQUIRE(group4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Deep nested group hierarchy") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 16);
        REQUIRE(zdd != nullptr);
        
        // Create a 3-level hierarchy
        MtrNode* root = Cudd_MakeZddTreeNode(manager, 0, 16, MTR_DEFAULT);
        REQUIRE(root != nullptr);
        
        // Level 2
        MtrNode* l2_1 = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        MtrNode* l2_2 = Cudd_MakeZddTreeNode(manager, 8, 8, MTR_DEFAULT);
        REQUIRE(l2_1 != nullptr);
        REQUIRE(l2_2 != nullptr);
        
        // Level 3
        MtrNode* l3_1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* l3_2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        MtrNode* l3_3 = Cudd_MakeZddTreeNode(manager, 8, 4, MTR_DEFAULT);
        MtrNode* l3_4 = Cudd_MakeZddTreeNode(manager, 12, 4, MTR_DEFAULT);
        REQUIRE(l3_1 != nullptr);
        REQUIRE(l3_2 != nullptr);
        REQUIRE(l3_3 != nullptr);
        REQUIRE(l3_4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Mix of fixed and non-fixed groups") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create alternating fixed and non-fixed groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_FIXED);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 3, 3, MTR_DEFAULT);
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 6, 3, MTR_FIXED);
        MtrNode* group4 = Cudd_MakeZddTreeNode(manager, 9, 3, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        REQUIRE(group3 != nullptr);
        REQUIRE(group4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Edge cases and boundary conditions", "[cuddZddGroup]") {
    SECTION("Single group covering all variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Many small groups") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create 6 groups of 2 variables each
        for (int i = 0; i < 6; i++) {
            MtrNode* group = Cudd_MakeZddTreeNode(manager, i * 2, 2, MTR_DEFAULT);
            REQUIRE(group != nullptr);
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sifting with small ZDD") {
        DdManager* manager = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 3);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Reordering with partially existing groups") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create only some of the variables
        for (int i = 0; i < 5; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createSimpleZdd(manager, 5);
        REQUIRE(zdd != nullptr);
        
        // Create a group that extends beyond existing variables
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 10, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Group with nested partial groups") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create only half the variables
        for (int i = 0; i < 8; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createLargeZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create parent group that extends beyond existing variables
        MtrNode* parent = Cudd_MakeZddTreeNode(manager, 0, 16, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create child groups, some of which extend beyond existing variables
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        MtrNode* child3 = Cudd_MakeZddTreeNode(manager, 8, 4, MTR_DEFAULT);
        MtrNode* child4 = Cudd_MakeZddTreeNode(manager, 12, 4, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        REQUIRE(child2 != nullptr);
        REQUIRE(child3 != nullptr);
        REQUIRE(child4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test with max swap limit") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Create groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 5, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 5, 5, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        // Set a very low swap limit to test early termination
        manager->siftMaxSwap = 10;
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 0); // May return 0 or 1 depending on limit
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test with variable limit") {
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 20);
        REQUIRE(zdd != nullptr);
        
        // Create many groups
        for (int i = 0; i < 10; i++) {
            MtrNode* group = Cudd_MakeZddTreeNode(manager, i * 2, 2, MTR_DEFAULT);
            REQUIRE(group != nullptr);
        }
        
        // Limit the number of variables to sift
        manager->siftMaxVar = 3;
        manager->siftMaxSwap = 1000;
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Additional reordering scenarios", "[cuddZddGroup]") {
    SECTION("Group sifting up scenario") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a ZDD with variables in specific order
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create groups that will test upward sifting
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 2, 3, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 5, 3, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Group sifting down scenario") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create groups that will test downward sifting
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 2, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 2, 6, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple reordering passes") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Create groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 5, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 5, 5, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        // Multiple passes of different reordering methods
        int result1 = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result1 >= 1);
        
        int result2 = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result2 >= 1);
        
        int result3 = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result3 >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Single variable groups") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create all single-variable groups
        for (int i = 0; i < 8; i++) {
            MtrNode* group = Cudd_MakeZddTreeNode(manager, i, 1, MTR_DEFAULT);
            REQUIRE(group != nullptr);
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Large number of variables with groups") {
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 20);
        REQUIRE(zdd != nullptr);
        
        // Create 5 groups of 4 variables
        for (int i = 0; i < 5; i++) {
            MtrNode* group = Cudd_MakeZddTreeNode(manager, i * 4, 4, MTR_DEFAULT);
            REQUIRE(group != nullptr);
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test group sifting backward") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 8, 4, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        REQUIRE(group3 != nullptr);
        
        // This should exercise the backward sifting path
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGroup - Coverage for special edge cases", "[cuddZddGroup]") {
    SECTION("Group beyond existing ZDD variables") {
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create only 5 variables
        for (int i = 0; i < 5; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createSimpleZdd(manager, 5);
        REQUIRE(zdd != nullptr);
        
        // Create a group starting beyond existing variables
        // This tests the case where treenode->low >= table->sizeZ
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 10, 5, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // This should trigger the early return in zddFindNodeHiLo
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Parent group with partially existing child groups") {
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create only 10 variables
        for (int i = 0; i < 10; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Create parent group that extends beyond existing variables
        MtrNode* parent = Cudd_MakeZddTreeNode(manager, 0, 20, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create child groups where some straddle the sizeZ boundary
        // This tests the case in zddFindNodeHiLo with auxnode != NULL
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 5, MTR_DEFAULT);
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 5, 5, MTR_DEFAULT);
        MtrNode* child3 = Cudd_MakeZddTreeNode(manager, 10, 5, MTR_DEFAULT);  // Beyond sizeZ
        MtrNode* child4 = Cudd_MakeZddTreeNode(manager, 15, 5, MTR_DEFAULT);  // Beyond sizeZ
        REQUIRE(child1 != nullptr);
        REQUIRE(child2 != nullptr);
        REQUIRE(child3 != nullptr);
        REQUIRE(child4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Parent group with child straddling sizeZ") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create 6 variables
        for (int i = 0; i < 6; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // Create parent
        MtrNode* parent = Cudd_MakeZddTreeNode(manager, 0, 12, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create a child that starts below sizeZ but extends beyond it
        // This tests: thisUpper >= table->sizeZ && thisLower < table->sizeZ
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 4, 8, MTR_DEFAULT);  // Starts at 4, ends at 11
        REQUIRE(child1 != nullptr);
        REQUIRE(child2 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple children with one straddling - iterate through while loop") {
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create only 8 variables
        for (int i = 0; i < 8; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create parent that extends beyond sizeZ
        MtrNode* parent = Cudd_MakeZddTreeNode(manager, 0, 20, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create multiple children where the while loop needs to iterate
        // This will test the auxnode = auxnode->younger line (537)
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);  // Fully inside
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 4, 6, MTR_DEFAULT);  // Straddles: starts at 4, ends at 9
        MtrNode* child3 = Cudd_MakeZddTreeNode(manager, 10, 5, MTR_DEFAULT); // Beyond sizeZ
        MtrNode* child4 = Cudd_MakeZddTreeNode(manager, 15, 5, MTR_DEFAULT); // Beyond sizeZ
        REQUIRE(child1 != nullptr);
        REQUIRE(child2 != nullptr);
        REQUIRE(child3 != nullptr);
        REQUIRE(child4 != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Terminal node in partially existing group") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create only 6 variables
        for (int i = 0; i < 6; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // Create a terminal node (no children) that extends beyond sizeZ
        // This tests the auxnode == NULL case in the partially existing group code
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 12, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        // Don't create any child groups - this makes it terminal
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Convergence scenarios for sift converge") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a smaller ZDD that will converge quickly
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        DdNode* zdd = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(zdd);
        DdNode* temp = Cudd_zddUnion(manager, zdd, z2);
        Cudd_RecursiveDerefZdd(manager, zdd);
        zdd = temp;
        Cudd_Ref(zdd);
        
        // Create groups
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // SIFT_CONVERGE should stop when size doesn't improve
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear converge scenario") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // LINEAR_CONVERGE should test convergence loop
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test with all reordering methods on same ZDD") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Try each reordering method to maximize coverage
        Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT_CONV, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Very large ZDD for convergence testing") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a larger ZDD using the helper
        DdNode* result = createLargeZdd(manager, 16);
        REQUIRE(result != nullptr);
        
        // Create groups
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 8, 8, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        
        // Test convergence
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(res >= 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Maximum size group to test boundaries") {
        DdManager* manager = Cudd_Init(0, 100, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many variables
        for (int i = 0; i < 50; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createLargeZdd(manager, 50);
        REQUIRE(zdd != nullptr);
        
        // Create a large group
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 50, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Limit iterations to keep test fast
        manager->siftMaxVar = 5;
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 0);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test different paths in group sifting aux") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Create groups with specific configurations to exercise different code paths
        // This will test the various branches in zddGroupSiftingAux
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_DEFAULT);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 3, 3, MTR_DEFAULT);
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 6, 3, MTR_DEFAULT);
        MtrNode* group4 = Cudd_MakeZddTreeNode(manager, 9, 3, MTR_DEFAULT);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        REQUIRE(group3 != nullptr);
        REQUIRE(group4 != nullptr);
        
        // Multiple passes to exercise different sifting directions
        for (int i = 0; i < 3; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test reordering with all fixed groups") {
        DdManager* manager = Cudd_Init(0, 9, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 9);
        REQUIRE(zdd != nullptr);
        
        // Create all fixed groups - this tests the fixed group handling path
        MtrNode* group1 = Cudd_MakeZddTreeNode(manager, 0, 3, MTR_FIXED);
        MtrNode* group2 = Cudd_MakeZddTreeNode(manager, 3, 3, MTR_FIXED);
        MtrNode* group3 = Cudd_MakeZddTreeNode(manager, 6, 3, MTR_FIXED);
        REQUIRE(group1 != nullptr);
        REQUIRE(group2 != nullptr);
        REQUIRE(group3 != nullptr);
        
        // With all fixed groups, reordering should still work but not reorder within groups
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Asymmetric groups for testing different sifting directions") {
        DdManager* manager = Cudd_Init(0, 15, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 15);
        REQUIRE(zdd != nullptr);
        
        // Create asymmetric groups to exercise the "up first vs down first" logic
        MtrNode* small1 = Cudd_MakeZddTreeNode(manager, 0, 2, MTR_DEFAULT);
        MtrNode* large = Cudd_MakeZddTreeNode(manager, 2, 8, MTR_DEFAULT);
        MtrNode* small2 = Cudd_MakeZddTreeNode(manager, 10, 2, MTR_DEFAULT);
        MtrNode* medium = Cudd_MakeZddTreeNode(manager, 12, 3, MTR_DEFAULT);
        REQUIRE(small1 != nullptr);
        REQUIRE(large != nullptr);
        REQUIRE(small2 != nullptr);
        REQUIRE(medium != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test with very simple ZDD to reach convergence early") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create minimal ZDD - just two variables
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        DdNode* zdd = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(zdd);
        
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 2, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // With such a simple ZDD, convergence methods should exit early
        int result1 = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(result1 >= 1);
        
        int result2 = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result2 >= 1);
        
        int result3 = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result3 >= 1);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Groups at boundaries of manager size") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables up to the limit
        for (int i = 0; i < 8; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Create a group that goes right up to the boundary
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // This tests boundary conditions in the reordering code
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Extensive testing of all code paths with multiple reordering passes") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex ZDD structure
        DdNode* zdd = createLargeZdd(manager, 16);
        REQUIRE(zdd != nullptr);
        
        // Create a hierarchical group structure
        MtrNode* root = Cudd_MakeZddTreeNode(manager, 0, 16, MTR_DEFAULT);
        REQUIRE(root != nullptr);
        
        MtrNode* g1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* g2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        MtrNode* g3 = Cudd_MakeZddTreeNode(manager, 8, 4, MTR_DEFAULT);
        MtrNode* g4 = Cudd_MakeZddTreeNode(manager, 12, 4, MTR_DEFAULT);
        REQUIRE(g1 != nullptr);
        REQUIRE(g2 != nullptr);
        REQUIRE(g3 != nullptr);
        REQUIRE(g4 != nullptr);
        
        // Sub-groups
        MtrNode* sg1 = Cudd_MakeZddTreeNode(manager, 0, 2, MTR_DEFAULT);
        MtrNode* sg2 = Cudd_MakeZddTreeNode(manager, 2, 2, MTR_DEFAULT);
        MtrNode* sg3 = Cudd_MakeZddTreeNode(manager, 4, 2, MTR_DEFAULT);
        MtrNode* sg4 = Cudd_MakeZddTreeNode(manager, 6, 2, MTR_DEFAULT);
        REQUIRE(sg1 != nullptr);
        REQUIRE(sg2 != nullptr);
        REQUIRE(sg3 != nullptr);
        REQUIRE(sg4 != nullptr);
        
        // Run multiple reordering methods to maximize coverage
        Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        
        // Try convergence methods
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test with single element in reordering range") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        for (int i = 0; i < 4; i++) {
            DdNode* z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
            Cudd_Ref(z);
            Cudd_RecursiveDerefZdd(manager, z);
        }
        
        DdNode* zdd = createSimpleZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        // Create a group with just one element - tests edge case
        MtrNode* group = Cudd_MakeZddTreeNode(manager, 1, 1, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // This should test the x == xHigh return path (line 755)
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_FreeZddTree(manager);
        Cudd_Quit(manager);
    }
}
