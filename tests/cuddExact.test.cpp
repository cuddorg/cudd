#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - cuddInt.h provides access to internal functions
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddExact.c
 * 
 * This file contains comprehensive tests for the cuddExact module
 * to achieve 90% code coverage. The cuddExact module implements
 * exact variable reordering for BDDs.
 */

// Helper function to create a simple BDD with interacting variables
static DdNode* createSimpleBdd(DdManager* manager, int numVars) {
    if (manager == nullptr || numVars < 2) return nullptr;
    
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    
    DdNode* result = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(result);
    
    return result;
}

// Helper function to create a complex BDD with multiple variable interactions
static DdNode* createComplexBdd(DdManager* manager, int numVars) {
    if (manager == nullptr || numVars < 3) return nullptr;
    
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

// Helper function to create a chain-like BDD with linear interactions
static DdNode* createChainBdd(DdManager* manager, int numVars) {
    if (manager == nullptr || numVars < 2) return nullptr;
    
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

// Helper function to create a BDD with XOR pattern for multiple nodes
static DdNode* createXorBdd(DdManager* manager, int numVars) {
    if (manager == nullptr || numVars < 2) return nullptr;
    
    DdNode* result = Cudd_bddIthVar(manager, 0);
    Cudd_Ref(result);
    
    for (int i = 1; i < numVars; i++) {
        DdNode* xi = Cudd_bddIthVar(manager, i);
        DdNode* newResult = Cudd_bddXor(manager, result, xi);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(manager, result);
        result = newResult;
    }
    
    return result;
}

TEST_CASE("cuddExact - Basic Module Test", "[cuddExact]") {
    // Basic test to verify the module compiles and links
    REQUIRE(true);
}

TEST_CASE("cuddExact - Basic exact reordering with small BDD", "[cuddExact]") {
    SECTION("Simple two-variable BDD") {
        DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createSimpleBdd(manager, 2);
        REQUIRE(f != nullptr);
        
        // Exact reordering on small BDD - exercises cuddExact main path
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        // BDD should still be valid
        REQUIRE(Cudd_DagSize(f) > 0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Three-variable BDD with interactions") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Exact reordering with varying variable counts", "[cuddExact]") {
    SECTION("Four variables - tests multiple subset iterations") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createChainBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Five variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createChainBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Six variables - larger subset space") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createChainBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Trivial case with lower == upper", "[cuddExact]") {
    SECTION("Single variable used - trivial problem returns 1") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Only use middle variable so lower can equal upper after exclusion
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(x1);
        
        // The exact reordering should return 1 for trivial problem
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, x1);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Test with unused variables", "[cuddExact]") {
    SECTION("BDD with some unused middle variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Only use variables 0 and 4, leaving 1, 2, 3 unused
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        DdNode* f = Cudd_bddAnd(manager, x0, x4);
        Cudd_Ref(f);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Skip unused variables in inner loop") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that uses variables 0, 1, 4, 5 (skipping 2, 3)
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        DdNode* x5 = Cudd_bddIthVar(manager, 5);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x4, x5);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - XOR BDD for root counting tests", "[cuddExact]") {
    SECTION("XOR BDD creates many roots") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createXorBdd(manager, 4);
        REQUIRE(f != nullptr);
        
        // XOR creates different topology, tests ddCountRoots
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("XOR with 5 variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createXorBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - BDD with complemented edges", "[cuddExact]") {
    SECTION("BDD with NOT operations creates complemented edges") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        // Create f = (NOT x0) AND (x1 XOR x2) - uses complemented edges
        DdNode* notX0 = Cudd_Not(x0);
        DdNode* xorX1X2 = Cudd_bddXor(manager, x1, x2);
        Cudd_Ref(xorX1X2);
        
        DdNode* f = Cudd_bddAnd(manager, notX0, xorX1X2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, xorX1X2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple complemented edges") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        // Create f = (NOT x0 AND x1) OR (NOT x2 AND x3)
        DdNode* t1 = Cudd_bddAnd(manager, Cudd_Not(x0), x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, Cudd_Not(x2), x3);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for symmetry detection paths", "[cuddExact]") {
    SECTION("Symmetric variables pattern") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a function with symmetric variables
        // f = x0 XOR x1 has x0 and x1 symmetric
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        DdNode* xorX0X1 = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(xorX0X1);
        
        DdNode* f = Cudd_bddAnd(manager, xorX0X1, x2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, xorX0X1);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple symmetry groups") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        
        // Create (x0 XOR x1) AND (x2 XOR x3) AND x4
        DdNode* xor1 = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(xor1);
        DdNode* xor2 = Cudd_bddXor(manager, x2, x3);
        Cudd_Ref(xor2);
        
        DdNode* t = Cudd_bddAnd(manager, xor1, xor2);
        Cudd_Ref(t);
        DdNode* f = Cudd_bddAnd(manager, t, x4);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, xor1);
        Cudd_RecursiveDeref(manager, xor2);
        Cudd_RecursiveDeref(manager, t);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for lower bound pruning", "[cuddExact]") {
    SECTION("BDD where lower bound may exceed upper bound") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a relatively optimal BDD structure
        DdNode* f = createChainBdd(manager, 5);
        REQUIRE(f != nullptr);
        
        // The exact algorithm should compute LB and compare with UB
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("BDD with multiple root nodes") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create multiple BDDs to have multiple roots
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        DdNode* f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode* f2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(f2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for updateEntry and subset matching", "[cuddExact]") {
    SECTION("Multiple subsets with same variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Complex BDD that creates multiple subset orderings
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        
        // (x0 OR x1) AND (x1 OR x2) AND (x2 OR x3) AND (x3 OR x4)
        DdNode* f = Cudd_ReadOne(manager);
        Cudd_Ref(f);
        
        DdNode* c1 = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(c1);
        DdNode* t1 = Cudd_bddAnd(manager, f, c1);
        Cudd_Ref(t1);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, c1);
        f = t1;
        
        DdNode* c2 = Cudd_bddOr(manager, x1, x2);
        Cudd_Ref(c2);
        DdNode* t2 = Cudd_bddAnd(manager, f, c2);
        Cudd_Ref(t2);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, c2);
        f = t2;
        
        DdNode* c3 = Cudd_bddOr(manager, x2, x3);
        Cudd_Ref(c3);
        DdNode* t3 = Cudd_bddAnd(manager, f, c3);
        Cudd_Ref(t3);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, c3);
        f = t3;
        
        DdNode* c4 = Cudd_bddOr(manager, x3, x4);
        Cudd_Ref(c4);
        DdNode* t4 = Cudd_bddAnd(manager, f, c4);
        Cudd_Ref(t4);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, c4);
        f = t4;
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for updateUB path", "[cuddExact]") {
    SECTION("Find better order during reordering") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that benefits from reordering
        // Using suboptimal initial variable order
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        // f = (x0 AND x3) OR (x1 AND x2) - benefits from reordering
        DdNode* t1 = Cudd_bddAnd(manager, x0, x3);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x1, x2);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        
        int sizeBefore = Cudd_DagSize(f);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        // Size should be the same or better
        int sizeAfter = Cudd_DagSize(f);
        REQUIRE(sizeAfter <= sizeBefore);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for ddShuffle and ddSiftUp", "[cuddExact]") {
    SECTION("Shuffle with variables at different positions") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that requires moving variables
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x4);
        Cudd_Ref(t1);
        DdNode* f = Cudd_bddOr(manager, t1, x2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for ddCountRoots and ddClearGlobal", "[cuddExact]") {
    SECTION("BDD with nodes at multiple levels") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD with nodes at all levels
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(t2);
        DdNode* t3 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(t3);
        DdNode* f = Cudd_bddAnd(manager, t3, x4);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("BDD with complemented edges at different levels") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        // Create BDD with complemented edges: (NOT x0 OR x1) AND (x2 NAND x3)
        DdNode* t1 = Cudd_bddOr(manager, Cudd_Not(x0), x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddNand(manager, x2, x3);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddAnd(manager, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for computeLB branches", "[cuddExact]") {
    SECTION("Lower bound with levels below lower") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a multi-level BDD
        DdNode* f = createChainBdd(manager, 6);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Test lb2 > lb1 path") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD where lb2 might be larger
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        
        // Many nodes at lower levels
        DdNode* t1 = Cudd_bddXor(manager, x3, x4);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddXor(manager, x2, t1);
        Cudd_Ref(t2);
        DdNode* t3 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t3);
        DdNode* f = Cudd_bddOr(manager, t3, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for pushDown function", "[cuddExact]") {
    SECTION("BDD requiring order push down during exact reordering") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a BDD structure that requires pushDown
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        DdNode* t1 = Cudd_bddOr(manager, x0, x2);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddOr(manager, x1, x3);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddAnd(manager, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for checkSymmInfo return 0", "[cuddExact]") {
    SECTION("Symmetry check returns 0 - skip pushDown") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a symmetric function to test checkSymmInfo
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        // XOR creates symmetry
        DdNode* xor01 = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(xor01);
        DdNode* f = Cudd_bddAnd(manager, xor01, x2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, xor01);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Dense BDD for thorough path coverage", "[cuddExact]") {
    SECTION("Dense BDD with many nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a dense BDD by combining many operations
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        
        // Create complex function: ((x0 XOR x1) AND (x2 OR x3)) XOR x4
        DdNode* xor01 = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(xor01);
        DdNode* or23 = Cudd_bddOr(manager, x2, x3);
        Cudd_Ref(or23);
        DdNode* and12 = Cudd_bddAnd(manager, xor01, or23);
        Cudd_Ref(and12);
        DdNode* f = Cudd_bddXor(manager, and12, x4);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, xor01);
        Cudd_RecursiveDeref(manager, or23);
        Cudd_RecursiveDeref(manager, and12);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Edge cases for getMaxBinomial", "[cuddExact]") {
    // getMaxBinomial is called with size - unused
    // We test various BDD configurations that produce different size values
    
    SECTION("Size produces getMaxBinomial with n=2") {
        DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createSimpleBdd(manager, 2);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Size produces getMaxBinomial with n=3") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Size produces larger getMaxBinomial values") {
        DdManager *manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createChainBdd(manager, 7);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Multiple BDDs and roots", "[cuddExact]") {
    SECTION("Multiple independent BDDs") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        DdNode* x5 = Cudd_bddIthVar(manager, 5);
        
        DdNode* f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode* f2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(f2);
        DdNode* f3 = Cudd_bddOr(manager, x4, x5);
        Cudd_Ref(f3);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f3);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for getLevelKeys", "[cuddExact]") {
    SECTION("Level with isolated projection function") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD that leaves some variables as projection functions
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* f = Cudd_bddAnd(manager, x0, x3);
        Cudd_Ref(f);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Level with non-isolated projection function") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        // Use all variables so none are isolated
        DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Tests for constant handling in ddCountRoots", "[cuddExact]") {
    SECTION("BDD with constant children") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // AND creates a BDD where some paths lead to constants
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("BDD with constant one") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* one = Cudd_ReadOne(manager);
        
        // OR with one gives a tautology path
        DdNode* t = Cudd_bddOr(manager, x0, one);
        Cudd_Ref(t);
        
        // The result is one, but we want a non-trivial BDD
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Additional coverage for initSymmInfo and checkSymmInfo", "[cuddExact]") {
    SECTION("Symmetry with circular list") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create XOR to establish symmetry between variables
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        // (x0 XOR x1) AND (x2 XOR x3)
        DdNode* xor01 = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(xor01);
        DdNode* xor23 = Cudd_bddXor(manager, x2, x3);
        Cudd_Ref(xor23);
        DdNode* f = Cudd_bddAnd(manager, xor01, xor23);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, xor01);
        Cudd_RecursiveDeref(manager, xor23);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Larger variable count for more iterations", "[cuddExact]") {
    SECTION("Eight variables for extensive subset iteration") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* f = createChainBdd(manager, 8);
        REQUIRE(f != nullptr);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddExact - Test upper bound update with better order", "[cuddExact]") {
    SECTION("BDD that improves during reordering") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a BDD in a suboptimal order
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        // f = (x0 AND x4) OR (x1 AND x3) OR x2
        DdNode* t1 = Cudd_bddAnd(manager, x0, x4);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x1, x3);
        Cudd_Ref(t2);
        DdNode* t3 = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(t3);
        DdNode* f = Cudd_bddOr(manager, t3, x2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_EXACT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}
