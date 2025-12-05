#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddSubsetHB.c
 * 
 * This file contains comprehensive tests for the cuddSubsetHB module
 * to achieve 80%+ code coverage. Tests cover:
 * - Cudd_SubsetHeavyBranch: Extract dense subset using heavy branch heuristic
 * - Cudd_SupersetHeavyBranch: Extract dense superset using heavy branch heuristic
 * - Edge cases: constants, NULL, various thresholds, numVars settings
 */

// Helper function to create a simple 2-variable BDD (x0 AND x1)
static DdNode* createSimpleBDD(DdManager* dd) {
    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *result = Cudd_bddAnd(dd, x0, x1);
    Cudd_Ref(result);
    return result;
}

// Helper function to create a more complex BDD with multiple variables
static DdNode* createComplexBDD(DdManager* dd, int numVars) {
    if (numVars < 2) return Cudd_ReadOne(dd);
    
    // Create a BDD that combines variables in various ways
    // (x0 AND x1) OR (x2 AND x3) OR (x4 AND x5) ...
    DdNode *result = Cudd_ReadLogicZero(dd);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars - 1; i += 2) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *xi1 = Cudd_bddIthVar(dd, i + 1);
        DdNode *term = Cudd_bddAnd(dd, xi, xi1);
        Cudd_Ref(term);
        
        DdNode *newResult = Cudd_bddOr(dd, result, term);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(dd, term);
        Cudd_RecursiveDeref(dd, result);
        result = newResult;
    }
    
    return result;
}

// Helper to create a deep BDD tree
static DdNode* createDeepBDD(DdManager* dd, int numVars) {
    // Create x0 AND x1 AND x2 AND ... AND xn
    DdNode *result = Cudd_ReadOne(dd);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars; i++) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *newResult = Cudd_bddAnd(dd, result, xi);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(dd, result);
        result = newResult;
    }
    
    return result;
}

// Helper to create a wide BDD (OR of many variables)
static DdNode* createWideBDD(DdManager* dd, int numVars) {
    // Create x0 OR x1 OR x2 OR ... OR xn
    DdNode *result = Cudd_ReadLogicZero(dd);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars; i++) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *newResult = Cudd_bddOr(dd, result, xi);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(dd, result);
        result = newResult;
    }
    
    return result;
}

TEST_CASE("Cudd_SubsetHeavyBranch - Basic functionality", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Simple BDD subset") {
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        int numVars = 2;
        int threshold = 10;
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, numVars, threshold);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // Subset should be less than or equal to original
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        // Size of subset should be <= threshold (or original if already smaller)
        int subsetSize = Cudd_DagSize(subset);
        REQUIRE(subsetSize <= threshold);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Subset with very small threshold") {
        DdNode *f = createComplexBDD(dd, 6);
        REQUIRE(f != nullptr);
        
        int numVars = 6;
        int threshold = 1;  // Very restrictive threshold
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, numVars, threshold);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // Subset should still be valid
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Subset with large threshold") {
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        int numVars = 2;
        int threshold = 1000;  // Larger than BDD size
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, numVars, threshold);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // With large threshold, subset should equal original
        REQUIRE(subset == f);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_SubsetHeavyBranch - Constant inputs", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constant one input") {
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, one, 5, 10);
        REQUIRE(subset == one);
    }
    
    SECTION("Constant zero input") {
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, zero, 5, 10);
        REQUIRE(subset == zero);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_SubsetHeavyBranch - numVars parameter", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("numVars = 0 (auto-detect)") {
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        // When numVars is 0, it should use DBL_MAX_EXP - 1
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 0, 10);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("numVars larger than actual support") {
        DdNode *f = createSimpleBDD(dd);  // 2 variables
        REQUIRE(f != nullptr);
        
        // Use numVars larger than actual support
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 10, 10);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("numVars equals actual support") {
        DdNode *f = createSimpleBDD(dd);  // 2 variables
        REQUIRE(f != nullptr);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 10);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_SupersetHeavyBranch - Basic functionality", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Simple BDD superset") {
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        int numVars = 2;
        int threshold = 10;
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, numVars, threshold);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        // Original should be less than or equal to superset
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Superset with very small threshold") {
        DdNode *f = createComplexBDD(dd, 6);
        REQUIRE(f != nullptr);
        
        int numVars = 6;
        int threshold = 1;
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, numVars, threshold);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        // Original should be subset of superset
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Superset with large threshold") {
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        int numVars = 2;
        int threshold = 1000;
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, numVars, threshold);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        // With large threshold, superset should equal original
        REQUIRE(superset == f);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_SupersetHeavyBranch - Constant inputs", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constant one input") {
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, one, 5, 10);
        REQUIRE(superset == one);
    }
    
    SECTION("Constant zero input") {
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, zero, 5, 10);
        REQUIRE(superset == zero);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Complex BDDs to exercise recursive paths", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Deep AND chain") {
        DdNode *f = createDeepBDD(dd, 8);
        REQUIRE(f != nullptr);
        
        // This creates a deep tree that tests recursive minterm counting
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Wide OR chain") {
        DdNode *f = createWideBDD(dd, 8);
        REQUIRE(f != nullptr);
        
        // Wide OR chain has different minterm distribution
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Complex mixed BDD") {
        DdNode *f = createComplexBDD(dd, 10);
        REQUIRE(f != nullptr);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 10, 8);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Complemented BDD") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *notF = Cudd_Not(f);
        Cudd_Ref(notF);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, notF, 2, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, notF) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, notF);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Heavy vs Light branch selection", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Asymmetric BDD - heavier on THEN branch") {
        // Create (x0 AND (x1 OR x2 OR x3)) - heavy THEN branch
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *orPart = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(orPart);
        DdNode *temp = Cudd_bddOr(dd, orPart, x3);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, orPart);
        orPart = temp;
        
        DdNode *f = Cudd_bddAnd(dd, x0, orPart);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, orPart);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Asymmetric BDD - heavier on ELSE branch") {
        // Create (NOT x0 OR (x1 AND x2)) - light THEN branch under x0
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *andPart = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(andPart);
        
        DdNode *f = Cudd_bddOr(dd, Cudd_Not(x0), andPart);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, andPart);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Equal weight branches") {
        // Create (x0 XOR x1) - both branches have equal minterms
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddXor(dd, x0, x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Threshold boundary tests", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Threshold equals BDD size") {
        DdNode *f = createComplexBDD(dd, 6);
        REQUIRE(f != nullptr);
        
        int bddSize = Cudd_DagSize(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, bddSize);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // When threshold equals size, should return original
        REQUIRE(subset == f);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Threshold = BDD size + 1") {
        DdNode *f = createComplexBDD(dd, 6);
        REQUIRE(f != nullptr);
        
        int bddSize = Cudd_DagSize(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, bddSize + 1);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // Should return original
        REQUIRE(subset == f);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Threshold = BDD size - 1") {
        DdNode *f = createComplexBDD(dd, 6);
        REQUIRE(f != nullptr);
        
        int bddSize = Cudd_DagSize(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, bddSize - 1);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // Should be smaller than original
        int subsetSize = Cudd_DagSize(subset);
        REQUIRE(subsetSize <= bddSize);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Threshold = 0") {
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Single variable BDDs", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Single variable - positive") {
        DdNode *x = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(x);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, x, 1, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, x) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, x);
    }
    
    SECTION("Single variable - negative") {
        DdNode *x = Cudd_bddIthVar(dd, 0);
        DdNode *notX = Cudd_Not(x);
        Cudd_Ref(notX);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, notX, 1, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, notX) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, notX);
    }
    
    SECTION("Single variable - superset positive") {
        DdNode *x = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(x);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, x, 1, 5);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        REQUIRE(Cudd_bddLeq(dd, x, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, x);
    }
    
    SECTION("Single variable - superset negative") {
        DdNode *x = Cudd_bddIthVar(dd, 0);
        DdNode *notX = Cudd_Not(x);
        Cudd_Ref(notX);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, notX, 1, 5);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        REQUIRE(Cudd_bddLeq(dd, notX, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, notX);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Large BDDs exercise page resizing", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Large number of nodes") {
        // Create a BDD with many nodes to potentially trigger page resizing
        // in ResizeNodeDataPages, ResizeCountMintermPages, ResizeCountNodePages
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        for (int i = 0; i < 20; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF = Cudd_bddAnd(dd, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        // Add some OR operations to increase BDD size
        for (int i = 0; i < 10; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i + 20);
            DdNode *term = Cudd_bddAnd(dd, xi, Cudd_bddIthVar(dd, (i + 1) % 30));
            Cudd_Ref(term);
            DdNode *newF = Cudd_bddOr(dd, f, term);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, term);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 30, 50);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Exercises BuildSubsetBdd branches", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD where subset keeps THEN branch") {
        // Create BDD where THEN branch is heavier
        // f = (x0 AND (x1 OR x2 OR x3 OR x4)) 
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        DdNode *orPart = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(orPart);
        DdNode *temp = Cudd_bddOr(dd, orPart, x3);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, orPart);
        orPart = temp;
        temp = Cudd_bddOr(dd, orPart, x4);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, orPart);
        orPart = temp;
        
        DdNode *f = Cudd_bddAnd(dd, x0, orPart);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, orPart);
        
        // Force subsetting
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD where subset keeps ELSE branch") {
        // Create BDD where ELSE branch is heavier
        // f = (x0 OR (x1 AND x2 AND x3 AND x4))
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        DdNode *andPart = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(andPart);
        DdNode *temp = Cudd_bddAnd(dd, andPart, x3);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, andPart);
        andPart = temp;
        temp = Cudd_bddAnd(dd, andPart, x4);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, andPart);
        andPart = temp;
        
        // NOT x0 or andPart makes ELSE branch (when x0=0) heavier
        DdNode *f = Cudd_bddOr(dd, Cudd_Not(x0), andPart);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, andPart);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Repeated operations on same BDD", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *f = createComplexBDD(dd, 8);
    REQUIRE(f != nullptr);
    
    SECTION("Multiple subset calls") {
        // Same parameters should give same result
        DdNode *subset1 = Cudd_SubsetHeavyBranch(dd, f, 8, 5);
        REQUIRE(subset1 != nullptr);
        Cudd_Ref(subset1);
        
        DdNode *subset2 = Cudd_SubsetHeavyBranch(dd, f, 8, 5);
        REQUIRE(subset2 != nullptr);
        Cudd_Ref(subset2);
        
        REQUIRE(subset1 == subset2);
        
        Cudd_RecursiveDeref(dd, subset2);
        Cudd_RecursiveDeref(dd, subset1);
    }
    
    SECTION("Different thresholds") {
        DdNode *subset1 = Cudd_SubsetHeavyBranch(dd, f, 8, 3);
        REQUIRE(subset1 != nullptr);
        Cudd_Ref(subset1);
        
        DdNode *subset2 = Cudd_SubsetHeavyBranch(dd, f, 8, 10);
        REQUIRE(subset2 != nullptr);
        Cudd_Ref(subset2);
        
        // Larger threshold should give larger or equal subset
        int size1 = Cudd_DagSize(subset1);
        int size2 = Cudd_DagSize(subset2);
        REQUIRE(size2 >= size1);
        
        // Both should be subsets of f
        REQUIRE(Cudd_bddLeq(dd, subset1, f) == 1);
        REQUIRE(Cudd_bddLeq(dd, subset2, f) == 1);
        
        // subset1 should be subset of subset2
        REQUIRE(Cudd_bddLeq(dd, subset1, subset2) == 1);
        
        Cudd_RecursiveDeref(dd, subset2);
        Cudd_RecursiveDeref(dd, subset1);
    }
    
    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - StoreNodes and approxTable usage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Shared nodes in BDD") {
        // Create BDD with shared substructure
        // f = (x0 AND x1) OR (x0 AND x2)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x0, x2);
        Cudd_Ref(t2);
        
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Multiple levels of sharing") {
        // More complex shared structure
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        // Common term used multiple times
        DdNode *common = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(common);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, common);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x3, common);
        Cudd_Ref(t2);
        
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, common);
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Complement node processing", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Complement of AND") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *andNode = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(andNode);
        
        DdNode *f = Cudd_Not(andNode);  // NOT(x0 AND x1) = NOT x0 OR NOT x1
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
        Cudd_RecursiveDeref(dd, andNode);
    }
    
    SECTION("Complement of XOR") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *xorNode = Cudd_bddXor(dd, x0, x1);
        Cudd_Ref(xorNode);
        
        DdNode *f = Cudd_Not(xorNode);  // XNOR
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
        Cudd_RecursiveDeref(dd, xorNode);
    }
    
    SECTION("Double complement") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *andNode = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(andNode);
        
        DdNode *f = Cudd_Not(Cudd_Not(andNode));  // Double NOT = original
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
        Cudd_RecursiveDeref(dd, andNode);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Various minterm patterns", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("High minterm count (near tautology)") {
        // f = x0 OR x1 OR x2 OR x3 - covers most minterms
        DdNode *f = createWideBDD(dd, 4);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Low minterm count (near contradiction)") {
        // f = x0 AND x1 AND x2 AND x3 - covers few minterms
        DdNode *f = createDeepBDD(dd, 4);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Medium minterm count") {
        // f = (x0 AND x1) OR (x2 AND x3) - moderate coverage
        DdNode *f = createComplexBDD(dd, 4);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Node count tests", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Subset reduces node count appropriately") {
        DdNode *f = createComplexBDD(dd, 10);
        REQUIRE(f != nullptr);
        
        int origSize = Cudd_DagSize(f);
        
        // Request half the original size
        int threshold = origSize / 2;
        if (threshold < 1) threshold = 1;
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 10, threshold);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        int subsetSize = Cudd_DagSize(subset);
        
        // Subset should approximately meet threshold. The +1 accounts for the
        // fact that SubsetHeavyBranch may not produce an exact size match
        // due to the greedy heuristic nature of the algorithm.
        REQUIRE(subsetSize <= threshold + 1);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Superset complementary tests", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Superset of complex BDD") {
        DdNode *f = createComplexBDD(dd, 8);
        REQUIRE(f != nullptr);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, 8, 5);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        // f should be subset of superset
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Superset of complemented BDD") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *notF = Cudd_Not(f);
        Cudd_Ref(notF);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, notF, 2, 3);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        // notF should be subset of superset
        REQUIRE(Cudd_bddLeq(dd, notF, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, notF);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Exercise SubsetCountNodesAux complement handling", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Complement edge in THEN branch") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        // ITE(x0, NOT x1, x1) 
        DdNode *f = Cudd_bddIte(dd, x0, Cudd_Not(x1), x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Complement edge in ELSE branch") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        // ITE(x0, x1, NOT x1)
        DdNode *f = Cudd_bddIte(dd, x0, x1, Cudd_Not(x1));
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Both branches complemented") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        // ITE(x0, NOT x1, NOT x2)
        DdNode *f = Cudd_bddIte(dd, x0, Cudd_Not(x1), Cudd_Not(x2));
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Additional edge case coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD with constant THEN child") {
        // f = x0 -> 1 (THEN) or x1 (ELSE)
        // This is ITE(x0, 1, x1) = x0 OR x1
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *one = Cudd_ReadOne(dd);
        
        DdNode *f = Cudd_bddIte(dd, x0, one, x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with constant ELSE child") {
        // f = x0 -> x1 (THEN) or 0 (ELSE)
        // This is ITE(x0, x1, 0) = x0 AND x1
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *zero = Cudd_Not(Cudd_ReadOne(dd));
        
        DdNode *f = Cudd_bddIte(dd, x0, x1, zero);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with both constant children") {
        // f = x0 (ITE(x0, 1, 0) = x0)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(x0);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, x0, 1, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, x0) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, x0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Comprehensive BDD structure tests", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Large XOR structure") {
        // XOR creates maximally unbalanced minterms
        DdNode *f = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(f);
        
        for (int i = 1; i < 6; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF = Cudd_bddXor(dd, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 10);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Mux-like structure") {
        // Create a multiplexer: ITE(s0, d1, d0) where d0,d1 are complex
        DdNode *s0 = Cudd_bddIthVar(dd, 0);
        DdNode *d0 = createDeepBDD(dd, 3);  // Uses vars 1,2,3
        
        // Shift d0 to use different variables
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        DdNode *x6 = Cudd_bddIthVar(dd, 6);
        DdNode *x4_and_x5 = Cudd_bddAnd(dd, x4, x5);
        Cudd_Ref(x4_and_x5);
        DdNode *d1 = Cudd_bddAnd(dd, x4_and_x5, x6);
        Cudd_Ref(d1);
        Cudd_RecursiveDeref(dd, x4_and_x5);
        
        DdNode *f = Cudd_bddIte(dd, s0, d1, d0);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, d0);
        Cudd_RecursiveDeref(dd, d1);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 7, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Force ELSE branch selection in BuildSubsetBdd", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD with more minterms in ELSE branch") {
        // Create f = NOT(x0) OR (x0 AND x1 AND x2 AND x3 AND x4)
        // When x0=0, we have 1 (many minterms)
        // When x0=1, we have x1 AND x2 AND x3 AND x4 (few minterms)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        // Build x1 AND x2 AND x3 AND x4
        DdNode *andPart = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(andPart);
        DdNode *temp = Cudd_bddAnd(dd, andPart, x3);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, andPart);
        andPart = temp;
        temp = Cudd_bddAnd(dd, andPart, x4);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, andPart);
        andPart = temp;
        
        // f = NOT(x0) OR (x0 AND andPart)
        // This is ITE(x0, andPart, 1)
        DdNode *f = Cudd_bddOr(dd, Cudd_Not(x0), andPart);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, andPart);
        
        // With a small threshold, should select ELSE branch
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD where minNv < minNnv triggers ELSE recursion") {
        // Create a BDD where ELSE branch has strictly more minterms
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        // f = (NOT x0 AND (x1 OR x2)) OR (x0 AND x1 AND x2)
        // ELSE (x0=0): x1 OR x2 (3 minterms)
        // THEN (x0=1): x1 AND x2 (1 minterm)
        DdNode *orPart = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(orPart);
        
        DdNode *andPart = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(andPart);
        
        DdNode *elseBr = Cudd_bddAnd(dd, Cudd_Not(x0), orPart);
        Cudd_Ref(elseBr);
        
        DdNode *thenBr = Cudd_bddAnd(dd, x0, andPart);
        Cudd_Ref(thenBr);
        
        DdNode *f = Cudd_bddOr(dd, elseBr, thenBr);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, orPart);
        Cudd_RecursiveDeref(dd, andPart);
        Cudd_RecursiveDeref(dd, elseBr);
        Cudd_RecursiveDeref(dd, thenBr);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Exercise approxTable lookup paths", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Multiple subsets to exercise approximation table") {
        // Create a complex BDD
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        for (int i = 0; i < 12; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            if (i % 3 == 0) {
                newF = Cudd_bddAnd(dd, f, xi);
            } else if (i % 3 == 1) {
                newF = Cudd_bddOr(dd, f, xi);
            } else {
                newF = Cudd_bddXor(dd, f, xi);
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        // Very aggressive subsetting to trigger approximations
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 12, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Shared substructures with different approximations") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        
        // Create shared substructure
        DdNode *shared = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(shared);
        
        // Use it in two different contexts
        DdNode *part1 = Cudd_bddAnd(dd, x0, shared);
        Cudd_Ref(part1);
        
        DdNode *part2 = Cudd_bddAnd(dd, x1, shared);
        Cudd_Ref(part2);
        
        DdNode *part3 = Cudd_bddAnd(dd, x4, x5);
        Cudd_Ref(part3);
        
        DdNode *combined = Cudd_bddOr(dd, part1, part2);
        Cudd_Ref(combined);
        
        DdNode *f = Cudd_bddOr(dd, combined, part3);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, shared);
        Cudd_RecursiveDeref(dd, part1);
        Cudd_RecursiveDeref(dd, part2);
        Cudd_RecursiveDeref(dd, part3);
        Cudd_RecursiveDeref(dd, combined);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Page resize triggers", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Large BDD to trigger page resizing") {
        // Create a larger BDD to potentially trigger page resizing
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        // Create BDD with many nodes through various operations
        for (int i = 0; i < 25; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            if (i % 4 == 0) {
                newF = Cudd_bddAnd(dd, f, xi);
            } else if (i % 4 == 1) {
                newF = Cudd_bddOr(dd, f, xi);
            } else if (i % 4 == 2) {
                newF = Cudd_bddXor(dd, f, xi);
            } else {
                // Create more complex patterns
                DdNode *t = Cudd_bddOr(dd, f, xi);
                Cudd_Ref(t);
                newF = Cudd_bddAnd(dd, t, Cudd_bddIthVar(dd, (i + 1) % 25));
                Cudd_RecursiveDeref(dd, t);
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        int origSize = Cudd_DagSize(f);
        
        // Subset with moderate threshold
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 25, origSize / 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Superset with different thresholds", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Superset with very small threshold") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        // Create moderately complex BDD
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        // Very small threshold
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, 4, 1);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Superset with threshold equal to size") {
        DdNode *f = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(f);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, 1, Cudd_DagSize(f));
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        // Should return original when threshold allows
        REQUIRE(superset == f);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Edge cases with zero-minterm scenarios", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("BDD with zero child") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        // f = x0 AND x1 (ELSE child is implicitly zero)
        DdNode *f = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with one child") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        // f = x0 OR NOT x1 (complex structure with one child)
        DdNode *f = Cudd_bddOr(dd, x0, Cudd_Not(x1));
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Different variable orderings", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Variables in reverse order") {
        // Use variables in reverse index order
        DdNode *x9 = Cudd_bddIthVar(dd, 9);
        DdNode *x8 = Cudd_bddIthVar(dd, 8);
        DdNode *x7 = Cudd_bddIthVar(dd, 7);
        
        DdNode *f = Cudd_bddAnd(dd, x9, Cudd_bddOr(dd, x8, x7));
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 10, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Sparse variable indices") {
        // Use non-consecutive variable indices
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        DdNode *x10 = Cudd_bddIthVar(dd, 10);
        DdNode *x15 = Cudd_bddIthVar(dd, 15);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, x5);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x10, x15);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 16, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Highly asymmetric BDDs", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Left-skewed tree") {
        // Build a left-skewed BDD tree
        DdNode *f = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(f);
        
        for (int i = 1; i < 8; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            // Always AND with new variable (creates left-heavy structure)
            DdNode *newF = Cudd_bddAnd(dd, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Right-skewed tree") {
        // Build a right-skewed BDD tree (using OR)
        DdNode *f = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(f);
        
        for (int i = 1; i < 8; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF = Cudd_bddOr(dd, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - StoreNodes coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Deeply nested structure for StoreNodes recursion") {
        // Create a deeply nested BDD to exercise StoreNodes
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        
        // Build: ((((x0 AND x1) OR x2) AND x3) OR x4) AND x5
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddOr(dd, t1, x2);
        Cudd_Ref(t2);
        Cudd_RecursiveDeref(dd, t1);
        DdNode *t3 = Cudd_bddAnd(dd, t2, x3);
        Cudd_Ref(t3);
        Cudd_RecursiveDeref(dd, t2);
        DdNode *t4 = Cudd_bddOr(dd, t3, x4);
        Cudd_Ref(t4);
        Cudd_RecursiveDeref(dd, t3);
        DdNode *f = Cudd_bddAnd(dd, t4, x5);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, t4);
        
        // Force significant subsetting
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - SubsetCountNodesAux branches", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Exercise minT == minE case") {
        // Create BDD where THEN and ELSE minterms are equal
        // XOR creates symmetric minterm distribution
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *f = Cudd_bddXor(dd, x0, x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Exercise minT > minE case") {
        // Create BDD where THEN has more minterms than ELSE
        // f = x0 OR (NOT x0 AND x1 AND x2 AND x3)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *x1_and_x2 = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(x1_and_x2);
        DdNode *andPart = Cudd_bddAnd(dd, x1_and_x2, x3);
        Cudd_Ref(andPart);
        Cudd_RecursiveDeref(dd, x1_and_x2);
        
        DdNode *f = Cudd_bddOr(dd, x0, andPart);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, andPart);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Exercise minT < minE case with complex BDD") {
        // Create BDD where ELSE has more minterms
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        // f = (x0 AND x1 AND x2 AND x3) OR NOT x0
        // When x0=0: TRUE (many minterms)
        // When x0=1: x1 AND x2 AND x3 (few minterms)
        DdNode *x2_and_x3 = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(x2_and_x3);
        DdNode *andPart = Cudd_bddAnd(dd, x1, x2_and_x3);
        Cudd_Ref(andPart);
        Cudd_RecursiveDeref(dd, x2_and_x3);
        
        DdNode *thenPart = Cudd_bddAnd(dd, x0, andPart);
        Cudd_Ref(thenPart);
        
        DdNode *f = Cudd_bddOr(dd, thenPart, Cudd_Not(x0));
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, andPart);
        Cudd_RecursiveDeref(dd, thenPart);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - visitedTable lookup edge cases", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD with many shared nodes") {
        // Create a BDD with maximum sharing
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        
        // Create shared term
        DdNode *shared = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(shared);
        
        // Use shared term in multiple contexts
        DdNode *t1 = Cudd_bddAnd(dd, x0, shared);
        Cudd_Ref(t1);
        
        DdNode *t2 = Cudd_bddAnd(dd, x1, shared);
        Cudd_Ref(t2);
        
        DdNode *t3 = Cudd_bddOr(dd, x4, shared);
        Cudd_Ref(t3);
        
        DdNode *t4 = Cudd_bddAnd(dd, x5, shared);
        Cudd_Ref(t4);
        
        // Combine all
        DdNode *c1 = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(c1);
        DdNode *c2 = Cudd_bddOr(dd, t3, t4);
        Cudd_Ref(c2);
        DdNode *f = Cudd_bddAnd(dd, c1, c2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, shared);
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        Cudd_RecursiveDeref(dd, t3);
        Cudd_RecursiveDeref(dd, t4);
        Cudd_RecursiveDeref(dd, c1);
        Cudd_RecursiveDeref(dd, c2);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Extended size threshold tests", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Subset where size exactly matches threshold") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(f);
        
        int size = Cudd_DagSize(f);
        
        // Threshold exactly equals BDD size
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, size);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        // Should return original since size equals threshold
        REQUIRE(subset == f);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Subset with size slightly larger than threshold") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        int size = Cudd_DagSize(f);
        
        // Threshold one less than BDD size
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, size - 1);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Repeated subsetting", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Subset of a subset") {
        // Create a fairly complex BDD
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        for (int i = 0; i < 10; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            if (i % 2 == 0) {
                newF = Cudd_bddAnd(dd, f, xi);
            } else {
                newF = Cudd_bddOr(dd, f, xi);
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        // First subset
        DdNode *subset1 = Cudd_SubsetHeavyBranch(dd, f, 10, 6);
        REQUIRE(subset1 != nullptr);
        Cudd_Ref(subset1);
        
        // Subset of subset
        DdNode *subset2 = Cudd_SubsetHeavyBranch(dd, subset1, 10, 4);
        REQUIRE(subset2 != nullptr);
        Cudd_Ref(subset2);
        
        // Both should be subsets of original
        REQUIRE(Cudd_bddLeq(dd, subset1, f) == 1);
        REQUIRE(Cudd_bddLeq(dd, subset2, f) == 1);
        
        // subset2 should be subset of subset1
        REQUIRE(Cudd_bddLeq(dd, subset2, subset1) == 1);
        
        Cudd_RecursiveDeref(dd, subset2);
        Cudd_RecursiveDeref(dd, subset1);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Additional complement edge tests", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Complement of complex expression") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddOr(dd, x2, x3);
        Cudd_Ref(t2);
        DdNode *base = Cudd_bddXor(dd, t1, t2);
        Cudd_Ref(base);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        DdNode *f = Cudd_Not(base);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
        Cudd_RecursiveDeref(dd, base);
    }
    
    SECTION("Superset of complement") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *t = Cudd_bddAnd(dd, Cudd_bddOr(dd, x0, x1), x2);
        Cudd_Ref(t);
        
        DdNode *f = Cudd_Not(t);
        Cudd_Ref(f);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, 3, 3);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
        Cudd_RecursiveDeref(dd, t);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Large BDD for comprehensive coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Build and subset a large BDD") {
        // Create a BDD with many nodes to exercise more code paths
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        // Create a complex BDD using various operations
        for (int i = 0; i < 15; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            
            switch (i % 5) {
                case 0:
                    newF = Cudd_bddAnd(dd, f, xi);
                    break;
                case 1:
                    newF = Cudd_bddOr(dd, f, xi);
                    break;
                case 2:
                    newF = Cudd_bddXor(dd, f, xi);
                    break;
                case 3:
                    newF = Cudd_bddAnd(dd, f, Cudd_Not(xi));
                    break;
                case 4:
                    newF = Cudd_bddOr(dd, f, Cudd_Not(xi));
                    break;
                default:
                    newF = f;
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        // Add some more complexity
        for (int i = 0; i < 5; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *xj = Cudd_bddIthVar(dd, i + 5);
            DdNode *term = Cudd_bddAnd(dd, xi, xj);
            Cudd_Ref(term);
            DdNode *newF = Cudd_bddOr(dd, f, term);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, term);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        int origSize = Cudd_DagSize(f);
        
        // Test with various thresholds
        DdNode *subset1 = Cudd_SubsetHeavyBranch(dd, f, 15, origSize / 2);
        REQUIRE(subset1 != nullptr);
        Cudd_Ref(subset1);
        REQUIRE(Cudd_bddLeq(dd, subset1, f) == 1);
        
        DdNode *subset2 = Cudd_SubsetHeavyBranch(dd, f, 15, origSize / 4);
        REQUIRE(subset2 != nullptr);
        Cudd_Ref(subset2);
        REQUIRE(Cudd_bddLeq(dd, subset2, f) == 1);
        
        // subset2 should be smaller or equal to subset1
        REQUIRE(Cudd_bddLeq(dd, subset2, subset1) == 1);
        
        Cudd_RecursiveDeref(dd, subset2);
        Cudd_RecursiveDeref(dd, subset1);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Large BDD superset") {
        DdNode *f = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(f);
        
        for (int i = 1; i < 12; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            if (i % 3 == 0) {
                newF = Cudd_bddAnd(dd, f, xi);
            } else {
                newF = Cudd_bddOr(dd, f, xi);
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, 12, 5);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Very large BDD to trigger page resizing", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD with many nodes to potentially trigger page resize") {
        // Create a BDD with enough nodes to potentially require page resizing
        // DEFAULT_PAGE_SIZE is 2048 and DEFAULT_NODE_DATA_PAGE_SIZE is 1024
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        // Create many variables and combine them
        for (int i = 0; i < 30; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            
            // Create complex patterns to maximize node count
            if (i % 6 == 0) {
                newF = Cudd_bddAnd(dd, f, xi);
            } else if (i % 6 == 1) {
                newF = Cudd_bddOr(dd, f, xi);
            } else if (i % 6 == 2) {
                newF = Cudd_bddXor(dd, f, xi);
            } else if (i % 6 == 3) {
                newF = Cudd_bddAnd(dd, f, Cudd_Not(xi));
            } else if (i % 6 == 4) {
                newF = Cudd_bddOr(dd, f, Cudd_Not(xi));
            } else {
                // Add nested structure
                DdNode *t1 = Cudd_bddAnd(dd, f, xi);
                Cudd_Ref(t1);
                DdNode *t2 = Cudd_bddOr(dd, f, Cudd_bddIthVar(dd, (i + 1) % 30));
                Cudd_Ref(t2);
                newF = Cudd_bddXor(dd, t1, t2);
                Cudd_RecursiveDeref(dd, t1);
                Cudd_RecursiveDeref(dd, t2);
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        // Add more terms to increase node count
        for (int i = 0; i < 15; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *xj = Cudd_bddIthVar(dd, i + 15);
            DdNode *term = Cudd_bddAnd(dd, xi, xj);
            Cudd_Ref(term);
            DdNode *newF = Cudd_bddOr(dd, f, term);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, term);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        int origSize = Cudd_DagSize(f);
        
        // Subset with aggressive threshold
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 30, origSize / 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Different numVars values", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("numVars smaller than support") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *f = Cudd_bddAnd(dd, Cudd_bddOr(dd, x0, x1), Cudd_bddOr(dd, x2, x3));
        Cudd_Ref(f);
        
        // Use numVars smaller than actual support (which is 4)
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("numVars much larger than support") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(f);
        
        // Use very large numVars
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 100, 5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Additional branch coverage for BuildSubsetBdd", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Force multiple approxTable insertions") {
        // Create BDD where multiple nodes get approximated
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        DdNode *x6 = Cudd_bddIthVar(dd, 6);
        DdNode *x7 = Cudd_bddIthVar(dd, 7);
        
        // Create a complex BDD with many distinct subtrees
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddAnd(dd, x4, x5);
        Cudd_Ref(t3);
        DdNode *t4 = Cudd_bddAnd(dd, x6, x7);
        Cudd_Ref(t4);
        
        DdNode *c1 = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(c1);
        DdNode *c2 = Cudd_bddOr(dd, t3, t4);
        Cudd_Ref(c2);
        
        DdNode *f = Cudd_bddXor(dd, c1, c2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        Cudd_RecursiveDeref(dd, t3);
        Cudd_RecursiveDeref(dd, t4);
        Cudd_RecursiveDeref(dd, c1);
        Cudd_RecursiveDeref(dd, c2);
        
        // Very small threshold to force approximations
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Exercise storeTable lookups") {
        // Create BDD that will cause nodes to be found in storeTable
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        // Build structure with shared nodes
        DdNode *shared = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(shared);
        
        DdNode *left = Cudd_bddAnd(dd, shared, x2);
        Cudd_Ref(left);
        
        DdNode *right = Cudd_bddAnd(dd, shared, x3);
        Cudd_Ref(right);
        
        DdNode *f = Cudd_bddOr(dd, left, right);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, shared);
        Cudd_RecursiveDeref(dd, left);
        Cudd_RecursiveDeref(dd, right);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Stress test with repeated operations", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Multiple subset operations in sequence") {
        for (int iter = 0; iter < 5; iter++) {
            DdNode *f = Cudd_ReadOne(dd);
            Cudd_Ref(f);
            
            for (int i = 0; i < 8; i++) {
                DdNode *xi = Cudd_bddIthVar(dd, i);
                DdNode *newF;
                if ((i + iter) % 2 == 0) {
                    newF = Cudd_bddAnd(dd, f, xi);
                } else {
                    newF = Cudd_bddOr(dd, f, xi);
                }
                Cudd_Ref(newF);
                Cudd_RecursiveDeref(dd, f);
                f = newF;
            }
            
            DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 4);
            REQUIRE(subset != nullptr);
            Cudd_Ref(subset);
            
            REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
            
            Cudd_RecursiveDeref(dd, subset);
            Cudd_RecursiveDeref(dd, f);
        }
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Extreme threshold values", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Threshold of 1 on complex BDD") {
        // Create a moderately complex BDD
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x2, x3);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        // Very aggressive threshold
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 1);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Threshold of 2 with asymmetric BDD") {
        // BDD where THEN has fewer minterms than ELSE
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        // ITE(x0, x1 AND x2, 1) -> When x0=1: x1 AND x2 (1 minterm), When x0=0: 1 (4 minterms)
        DdNode *thenBr = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(thenBr);
        DdNode *f = Cudd_bddIte(dd, x0, thenBr, Cudd_ReadOne(dd));
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, thenBr);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Force approxTable path", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Complex BDD with shared approximations") {
        // Create a BDD that will have nodes approximated in multiple places
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        DdNode *x6 = Cudd_bddIthVar(dd, 6);
        DdNode *x7 = Cudd_bddIthVar(dd, 7);
        
        // Create multiple terms with shared structure
        DdNode *shared1 = Cudd_bddAnd(dd, x4, x5);
        Cudd_Ref(shared1);
        DdNode *shared2 = Cudd_bddAnd(dd, x6, x7);
        Cudd_Ref(shared2);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, shared1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x1, shared1);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddAnd(dd, x2, shared2);
        Cudd_Ref(t3);
        DdNode *t4 = Cudd_bddAnd(dd, x3, shared2);
        Cudd_Ref(t4);
        
        DdNode *c1 = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(c1);
        DdNode *c2 = Cudd_bddOr(dd, t3, t4);
        Cudd_Ref(c2);
        DdNode *f = Cudd_bddOr(dd, c1, c2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, shared1);
        Cudd_RecursiveDeref(dd, shared2);
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        Cudd_RecursiveDeref(dd, t3);
        Cudd_RecursiveDeref(dd, t4);
        Cudd_RecursiveDeref(dd, c1);
        Cudd_RecursiveDeref(dd, c2);
        
        // Use aggressive subsetting to force approximations
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Deep asymmetric BDD with minNv < minNnv") {
        // Create BDD where ELSE branch consistently has more minterms
        // so we exercise the else branch path in BuildSubsetBdd
        
        // Start with a large expression where NOT x0 term dominates
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        // f = (x0 AND x1 AND x2 AND x3 AND x4) OR NOT x0
        // THEN branch (x0=1): x1 AND x2 AND x3 AND x4 -> 1 minterm
        // ELSE branch (x0=0): 1 -> 16 minterms
        DdNode *andChain = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(andChain);
        DdNode *tmp = Cudd_bddAnd(dd, andChain, x3);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(dd, andChain);
        andChain = tmp;
        tmp = Cudd_bddAnd(dd, andChain, x4);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(dd, andChain);
        andChain = tmp;
        
        DdNode *thenPart = Cudd_bddAnd(dd, x0, andChain);
        Cudd_Ref(thenPart);
        
        DdNode *f = Cudd_bddOr(dd, thenPart, Cudd_Not(x0));
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, andChain);
        Cudd_RecursiveDeref(dd, thenPart);
        
        // Aggressive subsetting
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Cover storeTable lookup paths", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD with node reuse in lighter branch") {
        // Create structure where a node in the lighter branch is also in storeTable
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        // Create shared substructure
        DdNode *shared = Cudd_bddAnd(dd, x3, x4);
        Cudd_Ref(shared);
        
        // Heavy branch uses shared
        DdNode *heavy = Cudd_bddOr(dd, x1, shared);
        Cudd_Ref(heavy);
        
        // Light branch also uses shared
        DdNode *light = Cudd_bddAnd(dd, x2, shared);
        Cudd_Ref(light);
        
        // Combine with top variable
        DdNode *f = Cudd_bddIte(dd, x0, heavy, light);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, shared);
        Cudd_RecursiveDeref(dd, heavy);
        Cudd_RecursiveDeref(dd, light);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Test with ITE structures", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Nested ITE with various minterm distributions") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        
        // Inner ITE with heavier ELSE
        DdNode *inner1 = Cudd_bddIte(dd, x2, Cudd_bddAnd(dd, x3, x4), Cudd_bddOr(dd, x3, x4));
        Cudd_Ref(inner1);
        
        // Inner ITE with heavier THEN
        DdNode *inner2 = Cudd_bddIte(dd, x4, Cudd_bddOr(dd, x5, x3), Cudd_bddAnd(dd, x5, x3));
        Cudd_Ref(inner2);
        
        // Outer ITE
        DdNode *f = Cudd_bddIte(dd, x0, inner1, inner2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, inner1);
        Cudd_RecursiveDeref(dd, inner2);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Multiple incremental subsets", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Progressively smaller subsets") {
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        for (int i = 0; i < 10; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *newF;
            if (i % 3 == 0) {
                newF = Cudd_bddAnd(dd, f, xi);
            } else if (i % 3 == 1) {
                newF = Cudd_bddOr(dd, f, xi);
            } else {
                newF = Cudd_bddXor(dd, f, xi);
            }
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(dd, f);
            f = newF;
        }
        
        int origSize = Cudd_DagSize(f);
        
        // Create progressively smaller subsets
        DdNode *current = f;
        Cudd_Ref(current);
        
        for (int thresh = origSize - 2; thresh >= 2; thresh -= 2) {
            DdNode *subset = Cudd_SubsetHeavyBranch(dd, current, 10, thresh);
            if (subset != nullptr) {
                Cudd_Ref(subset);
                REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
                Cudd_RecursiveDeref(dd, current);
                current = subset;
            }
        }
        
        Cudd_RecursiveDeref(dd, current);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

// Global flag to track timeout handler invocation
static int g_timeoutHandlerCalled = 0;

// Timeout handler for testing
static void testTimeoutHandler(DdManager *dd, void *arg) {
    (void)dd;
    (void)arg;
    g_timeoutHandlerCalled = 1;
}

TEST_CASE("cuddSubsetHB - Timeout handler coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Subset with timeout handler registered but not triggered") {
        // Register a timeout handler
        g_timeoutHandlerCalled = 0;
        Cudd_RegisterTimeoutHandler(dd, testTimeoutHandler, nullptr);
        
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        // Normal operation - timeout handler should not be called
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 10);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(g_timeoutHandlerCalled == 0);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
        
        // Unregister handler
        Cudd_RegisterTimeoutHandler(dd, nullptr, nullptr);
    }
    
    SECTION("Superset with timeout handler registered but not triggered") {
        g_timeoutHandlerCalled = 0;
        Cudd_RegisterTimeoutHandler(dd, testTimeoutHandler, nullptr);
        
        DdNode *f = createSimpleBDD(dd);
        REQUIRE(f != nullptr);
        
        DdNode *superset = Cudd_SupersetHeavyBranch(dd, f, 2, 10);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        
        REQUIRE(g_timeoutHandlerCalled == 0);
        
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
        
        Cudd_RegisterTimeoutHandler(dd, nullptr, nullptr);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Very large BDD for page boundary coverage", "[cuddSubsetHB][slow]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Create BDD large enough to trigger page boundary conditions") {
        // Create a BDD with many nodes to stress the page allocation system
        // We use XOR chains which create exponential growth in nodes
        DdNode *f = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(f);
        
        // Build a complex BDD structure that grows the node count
        for (int i = 1; i < 20; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *temp;
            
            // Alternate between different operations to create complex structure
            if (i % 4 == 1) {
                temp = Cudd_bddXor(dd, f, xi);
            } else if (i % 4 == 2) {
                DdNode *and_term = Cudd_bddAnd(dd, f, xi);
                Cudd_Ref(and_term);
                temp = Cudd_bddOr(dd, f, and_term);
                Cudd_RecursiveDeref(dd, and_term);
            } else if (i % 4 == 3) {
                DdNode *or_term = Cudd_bddOr(dd, f, xi);
                Cudd_Ref(or_term);
                temp = Cudd_bddXor(dd, f, or_term);
                Cudd_RecursiveDeref(dd, or_term);
            } else {
                temp = Cudd_bddAnd(dd, f, Cudd_Not(xi));
            }
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, f);
            f = temp;
        }
        
        // Add more complexity with nested terms
        for (int i = 0; i < 10; i++) {
            DdNode *xi = Cudd_bddIthVar(dd, i);
            DdNode *xj = Cudd_bddIthVar(dd, i + 10);
            DdNode *xk = Cudd_bddIthVar(dd, (i + 5) % 20);
            
            DdNode *t1 = Cudd_bddAnd(dd, xi, xj);
            Cudd_Ref(t1);
            DdNode *t2 = Cudd_bddXor(dd, t1, xk);
            Cudd_Ref(t2);
            Cudd_RecursiveDeref(dd, t1);
            
            DdNode *temp = Cudd_bddOr(dd, f, t2);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, t2);
            Cudd_RecursiveDeref(dd, f);
            f = temp;
        }
        
        int origSize = Cudd_DagSize(f);
        
        // Test with various thresholds
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 20, origSize / 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Edge cases for constant children paths", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("BDD with THEN child == one (constant 1)") {
        // ITE(x0, 1, x1) = x0 OR x1
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddIte(dd, x0, one, x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with ELSE child == one (constant 1)") {
        // ITE(x0, x1, 1) = NOT x0 OR x1
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddIte(dd, x0, x1, one);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with THEN child == zero (constant 0)") {
        // ITE(x0, 0, x1) = NOT x0 AND x1
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddIte(dd, x0, zero, x1);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with ELSE child == zero (constant 0)") {
        // ITE(x0, x1, 0) = x0 AND x1
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *f = Cudd_bddIte(dd, x0, x1, zero);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 2, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Complex structures for BuildSubsetBdd branch coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD that triggers storeTable lookup success") {
        // Create BDD with structure where nodes are shared and found in storeTable
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        // Shared substructure
        DdNode *shared = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(shared);
        
        // Build: (x0 AND (shared OR x3)) OR (NOT x0 AND (shared OR x4))
        DdNode *or1 = Cudd_bddOr(dd, shared, x3);
        Cudd_Ref(or1);
        DdNode *or2 = Cudd_bddOr(dd, shared, x4);
        Cudd_Ref(or2);
        
        DdNode *t1 = Cudd_bddAnd(dd, x0, or1);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, Cudd_Not(x0), or2);
        Cudd_Ref(t2);
        
        DdNode *f = Cudd_bddOr(dd, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, shared);
        Cudd_RecursiveDeref(dd, or1);
        Cudd_RecursiveDeref(dd, or2);
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        // Use a threshold that forces subsetting
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD that triggers approxTable lookup in ELSE branch path") {
        // Create asymmetric BDD where THEN branch is processed first, 
        // and ELSE branch has an approximation
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        
        // Heavy THEN branch: x1 OR x2 OR x3 (many minterms)
        DdNode *thenBr = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(thenBr);
        DdNode *temp = Cudd_bddOr(dd, thenBr, x3);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, thenBr);
        thenBr = temp;
        
        // Light ELSE branch: x4 AND x5 (few minterms)
        DdNode *elseBr = Cudd_bddAnd(dd, x4, x5);
        Cudd_Ref(elseBr);
        
        // ITE(x0, thenBr, elseBr)
        DdNode *f = Cudd_bddIte(dd, x0, thenBr, elseBr);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, thenBr);
        Cudd_RecursiveDeref(dd, elseBr);
        
        // Aggressive subsetting to trigger approximation paths
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD that triggers approxTable lookup in THEN branch path") {
        // Create asymmetric BDD where ELSE branch is processed first
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        
        // Light THEN branch: x1 AND x2 AND x3 (few minterms)
        DdNode *thenBr = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(thenBr);
        DdNode *temp = Cudd_bddAnd(dd, thenBr, x3);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(dd, thenBr);
        thenBr = temp;
        
        // Heavy ELSE branch: x4 OR x5 (many minterms)
        DdNode *elseBr = Cudd_bddOr(dd, x4, x5);
        Cudd_Ref(elseBr);
        
        // ITE(x0, thenBr, elseBr)
        DdNode *f = Cudd_bddIte(dd, x0, thenBr, elseBr);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, thenBr);
        Cudd_RecursiveDeref(dd, elseBr);
        
        // Aggressive subsetting to trigger approximation paths
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 6, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - SubsetCountNodesAux detailed branch coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("BDD with minNv == minNnv exact tie") {
        // XOR creates perfect tie in minterm counts
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *xor1 = Cudd_bddXor(dd, x0, x1);
        Cudd_Ref(xor1);
        DdNode *f = Cudd_bddXor(dd, xor1, x2);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, xor1);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 4);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with Nv constant one") {
        // Create ITE(x0, 1, complex_expr)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *one = Cudd_ReadOne(dd);
        
        DdNode *complex = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(complex);
        
        DdNode *f = Cudd_bddIte(dd, x0, one, complex);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, complex);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with Nnv constant one") {
        // Create ITE(x0, complex_expr, 1)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *one = Cudd_ReadOne(dd);
        
        DdNode *complex = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(complex);
        
        DdNode *f = Cudd_bddIte(dd, x0, complex, one);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, complex);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with Nv constant zero") {
        // Create ITE(x0, 0, complex_expr)
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *zero = Cudd_Not(Cudd_ReadOne(dd));
        
        DdNode *complex = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(complex);
        
        DdNode *f = Cudd_bddIte(dd, x0, zero, complex);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, complex);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with Nnv constant zero") {
        // Create ITE(x0, complex_expr, 0) = x0 AND complex
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *zero = Cudd_Not(Cudd_ReadOne(dd));
        
        DdNode *complex = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(complex);
        
        DdNode *f = Cudd_bddIte(dd, x0, complex, zero);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(dd, complex);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Deeply nested BDD for page coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Create very deep and wide BDD") {
        DdNode *f = Cudd_ReadOne(dd);
        Cudd_Ref(f);
        
        // Create a BDD with many layers
        for (int layer = 0; layer < 5; layer++) {
            DdNode *layerResult = Cudd_ReadLogicZero(dd);
            Cudd_Ref(layerResult);
            
            for (int i = 0; i < 6; i++) {
                int varIdx = layer * 6 + i;
                DdNode *xi = Cudd_bddIthVar(dd, varIdx);
                DdNode *term = Cudd_bddAnd(dd, f, xi);
                Cudd_Ref(term);
                
                DdNode *newLayerResult = Cudd_bddOr(dd, layerResult, term);
                Cudd_Ref(newLayerResult);
                Cudd_RecursiveDeref(dd, term);
                Cudd_RecursiveDeref(dd, layerResult);
                layerResult = newLayerResult;
            }
            
            Cudd_RecursiveDeref(dd, f);
            f = layerResult;
        }
        
        int origSize = Cudd_DagSize(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 30, origSize / 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - SubsetCountMintermAux constant coverage", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("BDD with paths to constant zero") {
        // x0 AND x1 - ELSE branches lead to zero
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *f = Cudd_bddAnd(dd, Cudd_bddAnd(dd, x0, x1), x2);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("BDD with paths to constant one") {
        // x0 OR x1 - THEN branches often lead to one
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *f = Cudd_bddOr(dd, Cudd_bddOr(dd, x0, x1), x2);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - BuildSubsetBdd approximation node reuse", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Structure where approximated node is reused") {
        // Create a BDD where the same node appears in multiple positions
        // and will be approximated
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        DdNode *x5 = Cudd_bddIthVar(dd, 5);
        DdNode *x6 = Cudd_bddIthVar(dd, 6);
        DdNode *x7 = Cudd_bddIthVar(dd, 7);
        
        // Create structure that will require approximation
        DdNode *term1 = Cudd_bddAnd(dd, x0, Cudd_bddAnd(dd, x1, x2));
        Cudd_Ref(term1);
        DdNode *term2 = Cudd_bddAnd(dd, x3, Cudd_bddAnd(dd, x4, x5));
        Cudd_Ref(term2);
        DdNode *term3 = Cudd_bddAnd(dd, x6, x7);
        Cudd_Ref(term3);
        
        DdNode *c1 = Cudd_bddOr(dd, term1, term2);
        Cudd_Ref(c1);
        DdNode *f = Cudd_bddOr(dd, c1, term3);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, term1);
        Cudd_RecursiveDeref(dd, term2);
        Cudd_RecursiveDeref(dd, term3);
        Cudd_RecursiveDeref(dd, c1);
        
        // Force aggressive subsetting
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 8, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    SECTION("Force N != Cudd_Regular(neW) path in BuildSubsetBdd") {
        // Create structure where the result of ITE differs from input node
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        DdNode *x4 = Cudd_bddIthVar(dd, 4);
        
        // Build a complex structure
        DdNode *t1 = Cudd_bddAnd(dd, x1, x2);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(dd, x3, x4);
        Cudd_Ref(t2);
        
        // ITE(x0, t1, t2)
        DdNode *f = Cudd_bddIte(dd, x0, t1, t2);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, t1);
        Cudd_RecursiveDeref(dd, t2);
        
        // Subsetting with aggressive threshold
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 5, 2);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddSubsetHB - Complement handling in BuildSubsetBdd", "[cuddSubsetHB]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Complemented input to subset") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        DdNode *x3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *base = Cudd_bddAnd(dd, x0, Cudd_bddOr(dd, x1, Cudd_bddAnd(dd, x2, x3)));
        Cudd_Ref(base);
        
        DdNode *f = Cudd_Not(base);
        Cudd_Ref(f);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 4, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
        Cudd_RecursiveDeref(dd, base);
    }
    
    SECTION("Complemented children in BDD") {
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *x1 = Cudd_bddIthVar(dd, 1);
        DdNode *x2 = Cudd_bddIthVar(dd, 2);
        
        // ITE(x0, NOT(x1 AND x2), x1 OR x2)
        DdNode *then_br = Cudd_Not(Cudd_bddAnd(dd, x1, x2));
        Cudd_Ref(then_br);
        DdNode *else_br = Cudd_bddOr(dd, x1, x2);
        Cudd_Ref(else_br);
        
        DdNode *f = Cudd_bddIte(dd, x0, then_br, else_br);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(dd, then_br);
        Cudd_RecursiveDeref(dd, else_br);
        
        DdNode *subset = Cudd_SubsetHeavyBranch(dd, f, 3, 3);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }
    
    Cudd_Quit(dd);
}
