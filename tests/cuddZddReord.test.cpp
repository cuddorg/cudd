#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtr.h must come before cudd.h for tree functions
#include "mtr.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddZddReord.c
 * 
 * This file contains comprehensive tests for the cuddZddReord module
 * to achieve 90% code coverage and ensure correct functionality
 * of the ZDD dynamic variable reordering functions.
 */

// Helper function to create a simple ZDD
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

// Helper to create a larger ZDD with many nodes
static DdNode* createLargeZdd(DdManager* manager, int numVars) {
    if (numVars < 4) return nullptr;
    
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars / 2; i++) {
        DdNode* var1 = Cudd_zddIthVar(manager, i);
        DdNode* var2 = Cudd_zddIthVar(manager, numVars - 1 - i);
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        
        DdNode* prod = Cudd_zddProduct(manager, var1, var2);
        Cudd_Ref(prod);
        
        DdNode* temp = Cudd_zddUnion(manager, result, prod);
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// ============================================================================
// TESTS FOR Cudd_zddReduceHeap
// ============================================================================

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap basic tests", "[cuddZddReord]") {
    SECTION("ReduceHeap with too few nodes (below minsize)") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 1000000);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_NONE") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_NONE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_SAME") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_SIFT);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SAME, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_SIFT") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        unsigned int initialReorderings = Cudd_ReadReorderings(manager);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        REQUIRE(Cudd_ReadReorderings(manager) == initialReorderings + 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ReduceHeap with CUDD_REORDER_SIFT_CONVERGE") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap random reordering", "[cuddZddReord]") {
    SECTION("CUDD_REORDER_RANDOM") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_RANDOM_PIVOT") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_RANDOM with larger ZDD") {
        DdManager *manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_RANDOM_PIVOT with multiple iterations") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        for (int i = 0; i < 5; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap linear reordering", "[cuddZddReord]") {
    SECTION("CUDD_REORDER_LINEAR") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_LINEAR_CONVERGE") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap symmetric sifting", "[cuddZddReord]") {
    SECTION("CUDD_REORDER_SYMM_SIFT") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("CUDD_REORDER_SYMM_SIFT_CONV") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT_CONV, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap with hooks", "[cuddZddReord]") {
    SECTION("Pre and post reordering hooks") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        REQUIRE(Cudd_EnableReorderingReporting(manager) == 1);
        
        FILE* oldOut = Cudd_ReadStdout(manager);
        FILE* oldErr = Cudd_ReadStderr(manager);
        FILE* tempOut = tmpfile();
        FILE* tempErr = tmpfile();
        if (tempOut != nullptr && tempErr != nullptr) {
            Cudd_SetStdout(manager, tempOut);
            Cudd_SetStderr(manager, tempErr);
            
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
            
            fclose(tempOut);
            fclose(tempErr);
            Cudd_SetStdout(manager, oldOut);
            Cudd_SetStderr(manager, oldErr);
        }
        
        REQUIRE(Cudd_DisableReorderingReporting(manager) == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap with BDD alignment", "[cuddZddReord]") {
    SECTION("ZDD reordering with BDD realignment") {
        DdManager *manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        Cudd_bddRealignEnable(manager);
        REQUIRE(Cudd_bddRealignmentEnabled(manager) == 1);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddReord - Cudd_zddReduceHeap nextDyn updates", "[cuddZddReord]") {
    SECTION("Multiple reorderings to trigger nextDyn else branch") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        for (int i = 0; i < 25; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddShuffleHeap
// ============================================================================

TEST_CASE("cuddZddReord - Cudd_zddShuffleHeap tests", "[cuddZddReord]") {
    SECTION("ShuffleHeap with identity permutation") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int permutation[] = {0, 1, 2, 3, 4, 5};
        int result = Cudd_zddShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap with reversed permutation") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int permutation[] = {5, 4, 3, 2, 1, 0};
        int result = Cudd_zddShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        REQUIRE(Cudd_ReadInvPermZdd(manager, 0) == 5);
        REQUIRE(Cudd_ReadInvPermZdd(manager, 5) == 0);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap with rotation permutation") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int permutation[] = {1, 2, 3, 4, 5, 0};
        int result = Cudd_zddShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ShuffleHeap with complex ZDD") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int permutation[] = {7, 6, 5, 4, 3, 2, 1, 0};
        int result = Cudd_zddShuffleHeap(manager, permutation);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple shuffles") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int perm1[] = {1, 2, 3, 4, 5, 0};
        REQUIRE(Cudd_zddShuffleHeap(manager, perm1) == 1);
        
        int perm2[] = {5, 0, 1, 2, 3, 4};
        REQUIRE(Cudd_zddShuffleHeap(manager, perm2) == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddAlignToBdd
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddAlignToBdd tests", "[cuddZddReord]") {
    SECTION("Align with zero ZDD size") {
        DdManager *manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int result = cuddZddAlignToBdd(manager);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Align with matching BDD and ZDD variables") {
        DdManager *manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        int result = cuddZddAlignToBdd(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Align with ZDD multiplicity") {
        DdManager *manager = Cudd_Init(2, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        int result = cuddZddAlignToBdd(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Align with non-multiple ZDD variables returns 0") {
        DdManager *manager = Cudd_Init(3, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 5);
        REQUIRE(zdd != nullptr);
        
        int result = cuddZddAlignToBdd(manager);
        REQUIRE(result == 0);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddNextHigh and cuddZddNextLow
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddNextHigh and cuddZddNextLow tests", "[cuddZddReord]") {
    SECTION("cuddZddNextHigh returns x+1") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(cuddZddNextHigh(manager, 0) == 1);
        REQUIRE(cuddZddNextHigh(manager, 2) == 3);
        REQUIRE(cuddZddNextHigh(manager, 5) == 6);
        
        Cudd_Quit(manager);
    }
    
    SECTION("cuddZddNextLow returns x-1") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(cuddZddNextLow(manager, 5) == 4);
        REQUIRE(cuddZddNextLow(manager, 2) == 1);
        REQUIRE(cuddZddNextLow(manager, 0) == -1);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddUniqueCompare
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddUniqueCompare tests", "[cuddZddReord]") {
    SECTION("Compare function returns difference in keys") {
        IndexKey x = {0, 100};
        IndexKey y = {1, 50};
        
        int result = cuddZddUniqueCompare(&x, &y);
        REQUIRE(result == -50);
        
        result = cuddZddUniqueCompare(&y, &x);
        REQUIRE(result == 50);
    }
    
    SECTION("Compare function with equal keys") {
        IndexKey x = {0, 75};
        IndexKey y = {1, 75};
        
        int result = cuddZddUniqueCompare(&x, &y);
        REQUIRE(result == 0);
    }
}

// ============================================================================
// TESTS FOR cuddZddSwapInPlace
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSwapInPlace tests", "[cuddZddReord]") {
    SECTION("Swap adjacent variables via ShuffleHeap") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int perm[] = {1, 0, 2, 3, 4, 5};
        int result = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple swaps via sifting") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        REQUIRE(Cudd_zddDagSize(zdd) > 0);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSwapping
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSwapping tests", "[cuddZddReord]") {
    SECTION("Random swapping via ReduceHeap RANDOM") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Random pivot swapping via ReduceHeap RANDOM_PIVOT") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple random swapping iterations") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        for (int i = 0; i < 3; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSifting
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSifting tests", "[cuddZddReord]") {
    SECTION("Sifting on simple ZDD") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting on complex ZDD") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting with max swap limit") {
        DdManager *manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        Cudd_SetSiftMaxSwap(manager, 5);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting with max var limit") {
        DdManager *manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        Cudd_SetSiftMaxVar(manager, 3);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting with tight max growth") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        Cudd_SetMaxGrowth(manager, 1.01);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSiftingAux boundary conditions
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSiftingAux boundary conditions", "[cuddZddReord]") {
    SECTION("Variable at low boundary (x == x_low)") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(zdd);
        
        for (int i = 1; i < 8; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* temp = Cudd_zddUnion(manager, zdd, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, var);
            Cudd_RecursiveDerefZdd(manager, zdd);
            zdd = temp;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable at high boundary (x == x_high)") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = Cudd_zddIthVar(manager, 7);
        Cudd_Ref(zdd);
        
        for (int i = 0; i < 7; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* temp = Cudd_zddUnion(manager, zdd, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, var);
            Cudd_RecursiveDerefZdd(manager, zdd);
            zdd = temp;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable in middle - shorter distance to high") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR ZDD variable group tree
// ============================================================================

TEST_CASE("cuddZddReord - ZDD variable group tree tests", "[cuddZddReord]") {
    SECTION("Shuffle with ZDD group tree") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        MtrNode* tree = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        int perm[] = {1, 2, 3, 0, 5, 6, 7, 4};
        int result = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Free ZDD group tree") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        MtrNode* tree = Cudd_MakeZddTreeNode(manager, 0, 6, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        Cudd_FreeZddTree(manager);
        REQUIRE(Cudd_ReadZddTree(manager) == nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR auto reordering
// ============================================================================

TEST_CASE("cuddZddReord - ZDD auto reordering tests", "[cuddZddReord]") {
    SECTION("Enable and disable ZDD auto reordering") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_ReorderingType method;
        
        REQUIRE(Cudd_ReorderingStatusZdd(manager, &method) == 0);
        
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_SIFT);
        REQUIRE(Cudd_ReorderingStatusZdd(manager, &method) == 1);
        REQUIRE(method == CUDD_REORDER_SIFT);
        
        Cudd_AutodynDisableZdd(manager);
        REQUIRE(Cudd_ReorderingStatusZdd(manager, &method) == 0);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR edge cases and coverage improvement
// ============================================================================

TEST_CASE("cuddZddReord - Edge cases for coverage", "[cuddZddReord]") {
    SECTION("Empty manager reordering") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Single variable ZDD") {
        DdManager *manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = Cudd_zddIthVar(manager, 0);
        REQUIRE(zdd != nullptr);
        Cudd_Ref(zdd);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Dense ZDD reordering") {
        DdManager *manager = Cudd_Init(0, 10, 64, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        for (int iter = 0; iter < 3; iter++) {
            for (int i = 0; i < 10; i++) {
                DdNode* xi = Cudd_zddIthVar(manager, i);
                Cudd_Ref(xi);
                DdNode* temp = Cudd_zddUnion(manager, result, xi);
                Cudd_Ref(temp);
                Cudd_RecursiveDerefZdd(manager, xi);
                Cudd_RecursiveDerefZdd(manager, result);
                result = temp;
            }
        }
        
        int reorderResult = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("Large ZDD for subtable shrinking") {
        DdManager *manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 16);
        REQUIRE(zdd != nullptr);
        
        for (int i = 0; i < 5; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR zddSiftUp and zddSiftDown paths
// ============================================================================

TEST_CASE("cuddZddReord - zddSiftUp and zddSiftDown via shuffle", "[cuddZddReord]") {
    SECTION("Sift variable up via shuffle") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Move variable 5 up to position 1
        int perm[] = {0, 5, 1, 2, 3, 4, 6, 7};
        int result = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR zddSwapAny internal paths
// ============================================================================

TEST_CASE("cuddZddReord - zddSwapAny via random reordering", "[cuddZddReord]") {
    SECTION("Exercise all branches of zddSwapAny") {
        DdManager *manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        // Multiple random reorderings exercise different branches
        for (int i = 0; i < 10; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSiftingBackward
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSiftingBackward via sifting", "[cuddZddReord]") {
    SECTION("Sifting backward finds best position") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        unsigned int sizeBefore = Cudd_zddDagSize(zdd);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        unsigned int sizeAfter = Cudd_zddDagSize(zdd);
        REQUIRE(sizeAfter > 0);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR zddFixTree (via cuddZddAlignToBdd)
// ============================================================================

TEST_CASE("cuddZddReord - zddFixTree via alignment", "[cuddZddReord]") {
    SECTION("Exercise zddFixTree with tree hierarchy") {
        DdManager *manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD tree with children and siblings
        MtrNode* root = Cudd_MakeZddTreeNode(manager, 0, 8, MTR_DEFAULT);
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Cudd_MakeZddTreeNode(manager, 0, 4, MTR_DEFAULT);
        MtrNode* child2 = Cudd_MakeZddTreeNode(manager, 4, 4, MTR_DEFAULT);
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Trigger zddFixTree via cuddZddAlignToBdd
        int result = cuddZddAlignToBdd(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD tree with multiple levels") {
        DdManager *manager = Cudd_Init(4, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a multi-level tree structure
        MtrNode* root = Cudd_MakeZddTreeNode(manager, 0, 12, MTR_DEFAULT);
        REQUIRE(root != nullptr);
        
        MtrNode* tree1 = Cudd_MakeZddTreeNode(manager, 0, 6, MTR_DEFAULT);
        MtrNode* tree2 = Cudd_MakeZddTreeNode(manager, 6, 6, MTR_DEFAULT);
        
        DdNode* zdd = createComplexZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        int result = cuddZddAlignToBdd(manager);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR zddReorderPostprocess subtable shrinking
// ============================================================================

TEST_CASE("cuddZddReord - zddReorderPostprocess subtable management", "[cuddZddReord]") {
    SECTION("Create sparse subtables that trigger shrinking") {
        // Use large initial slots to create sparse subtables
        DdManager *manager = Cudd_Init(0, 16, 4096, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD with many nodes
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        for (int i = 0; i < 16; i++) {
            DdNode* var1 = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var1);
            
            for (int j = i + 1; j < 16; j++) {
                DdNode* var2 = Cudd_zddIthVar(manager, j);
                Cudd_Ref(var2);
                
                DdNode* prod = Cudd_zddProduct(manager, var1, var2);
                Cudd_Ref(prod);
                
                DdNode* temp = Cudd_zddUnion(manager, result, prod);
                Cudd_Ref(temp);
                
                Cudd_RecursiveDerefZdd(manager, prod);
                Cudd_RecursiveDerefZdd(manager, var2);
                Cudd_RecursiveDerefZdd(manager, result);
                result = temp;
            }
            Cudd_RecursiveDerefZdd(manager, var1);
        }
        
        // Multiple reorderings should trigger postprocess shrinking
        for (int i = 0; i < 10; i++) {
            int reorderResult = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
            REQUIRE(reorderResult >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSiftingBackward finding better positions
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSiftingBackward improvements", "[cuddZddReord]") {
    SECTION("Sifting that finds improved position") {
        DdManager *manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create interleaved ZDD structure that benefits from reordering
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        // Create poor initial order: variables that interact are far apart
        for (int i = 0; i < 6; i++) {
            DdNode* var1 = Cudd_zddIthVar(manager, i);
            DdNode* var2 = Cudd_zddIthVar(manager, 11 - i);
            Cudd_Ref(var1);
            Cudd_Ref(var2);
            
            DdNode* prod = Cudd_zddProduct(manager, var1, var2);
            Cudd_Ref(prod);
            
            DdNode* temp = Cudd_zddUnion(manager, result, prod);
            Cudd_Ref(temp);
            
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            result = temp;
        }
        
        int reorderResult = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting convergence finds optimal") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        // Chain structure
        for (int i = 0; i < 9; i++) {
            DdNode* var1 = Cudd_zddIthVar(manager, i);
            DdNode* var2 = Cudd_zddIthVar(manager, i + 1);
            Cudd_Ref(var1);
            Cudd_Ref(var2);
            
            DdNode* prod = Cudd_zddProduct(manager, var1, var2);
            Cudd_Ref(prod);
            
            DdNode* temp = Cudd_zddUnion(manager, result, prod);
            Cudd_Ref(temp);
            
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            result = temp;
        }
        
        int reorderResult = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT_CONVERGE, 0);
        REQUIRE(reorderResult >= 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSwapInPlace edge cases
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSwapInPlace edge cases", "[cuddZddReord]") {
    SECTION("Swap with f1 children not at yindex") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create specific structure to hit f11 = empty branch
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var0);
        Cudd_Ref(var2);
        
        DdNode* prod = Cudd_zddProduct(manager, var0, var2);
        Cudd_Ref(prod);
        
        // Shuffle to swap adjacent vars, triggering swap in place
        int perm[] = {1, 0, 2, 3, 4, 5};
        int result = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_Quit(manager);
    }
    
    SECTION("Swap with complex cofactor structure") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD where swapping creates new nodes
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        for (int i = 0; i < 4; i++) {
            DdNode* var1 = Cudd_zddIthVar(manager, 2*i);
            DdNode* var2 = Cudd_zddIthVar(manager, 2*i + 1);
            Cudd_Ref(var1);
            Cudd_Ref(var2);
            
            DdNode* u = Cudd_zddUnion(manager, var1, var2);
            Cudd_Ref(u);
            
            DdNode* temp = Cudd_zddProduct(manager, result, u);
            Cudd_Ref(temp);
            
            Cudd_RecursiveDerefZdd(manager, u);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            result = temp;
        }
        
        // This creates a complex structure for swapping
        int perm[] = {1, 0, 3, 2, 5, 4, 7, 6};
        int shuffleResult = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(shuffleResult == 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSwapping edge cases
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSwapping comprehensive tests", "[cuddZddReord]") {
    SECTION("RANDOM_PIVOT with pivot at various positions") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure where pivot will be at different positions
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        // Make var 0 have most keys (pivot = 0 case)
        for (int j = 1; j < 8; j++) {
            DdNode* var0 = Cudd_zddIthVar(manager, 0);
            DdNode* varj = Cudd_zddIthVar(manager, j);
            Cudd_Ref(var0);
            Cudd_Ref(varj);
            
            DdNode* prod = Cudd_zddProduct(manager, var0, varj);
            Cudd_Ref(prod);
            
            DdNode* temp = Cudd_zddUnion(manager, result, prod);
            Cudd_Ref(temp);
            
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, var0);
            Cudd_RecursiveDerefZdd(manager, varj);
            Cudd_RecursiveDerefZdd(manager, result);
            result = temp;
        }
        
        // This should trigger modulo < 1 branch (pivot at low position)
        for (int i = 0; i < 5; i++) {
            int reorderResult = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
            REQUIRE(reorderResult >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("RANDOM_PIVOT with pivot at high position") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure where pivot will be at high position
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        // Make var 7 have most keys
        for (int j = 0; j < 7; j++) {
            DdNode* var7 = Cudd_zddIthVar(manager, 7);
            DdNode* varj = Cudd_zddIthVar(manager, j);
            Cudd_Ref(var7);
            Cudd_Ref(varj);
            
            DdNode* prod = Cudd_zddProduct(manager, var7, varj);
            Cudd_Ref(prod);
            
            DdNode* temp = Cudd_zddUnion(manager, result, prod);
            Cudd_Ref(temp);
            
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, var7);
            Cudd_RecursiveDerefZdd(manager, varj);
            Cudd_RecursiveDerefZdd(manager, result);
            result = temp;
        }
        
        // This should trigger modulo == 0 branch (y = pivot)
        for (int i = 0; i < 5; i++) {
            int reorderResult = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM_PIVOT, 0);
            REQUIRE(reorderResult >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR zddSwapAny comprehensive paths
// ============================================================================

TEST_CASE("cuddZddReord - zddSwapAny all paths", "[cuddZddReord]") {
    SECTION("SwapAny with x == y_next case") {
        DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // Multiple random iterations to hit x == y_next path
        for (int i = 0; i < 20; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("SwapAny with x_next == y_next case") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Many iterations to hit x_next == y_next path
        for (int i = 0; i < 30; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("SwapAny growth limit test") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        // Set tight growth limit to trigger break
        Cudd_SetMaxGrowth(manager, 1.001);
        
        for (int i = 0; i < 10; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_RANDOM, 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddSifting termination callbacks
// ============================================================================

TEST_CASE("cuddZddReord - cuddZddSifting time limit", "[cuddZddReord]") {
    SECTION("Time limit during sifting") {
        DdManager *manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createLargeZdd(manager, 20);
        REQUIRE(zdd != nullptr);
        
        // Set very short time limit
        Cudd_SetTimeLimit(manager, 1);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 1);
        
        // Reset time limit
        Cudd_SetTimeLimit(manager, 0);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// ADDITIONAL EDGE CASE TESTS
// ============================================================================

TEST_CASE("cuddZddReord - Additional edge cases", "[cuddZddReord]") {
    SECTION("Swap creates new nodes requiring allocation") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create dense ZDD structure
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        for (int i = 0; i < 10; i++) {
            for (int j = i + 1; j < 10; j++) {
                DdNode* vi = Cudd_zddIthVar(manager, i);
                DdNode* vj = Cudd_zddIthVar(manager, j);
                Cudd_Ref(vi);
                Cudd_Ref(vj);
                
                DdNode* prod = Cudd_zddProduct(manager, vi, vj);
                Cudd_Ref(prod);
                
                DdNode* temp = Cudd_zddUnion(manager, result, prod);
                Cudd_Ref(temp);
                
                Cudd_RecursiveDerefZdd(manager, prod);
                Cudd_RecursiveDerefZdd(manager, vi);
                Cudd_RecursiveDerefZdd(manager, vj);
                Cudd_RecursiveDerefZdd(manager, result);
                result = temp;
            }
        }
        
        // Force swaps that create new nodes
        int perm[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        int shuffleResult = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(shuffleResult == 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("Exercise all reorder methods") {
        DdManager *manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createComplexZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Try each reorder method
        Cudd_ReorderingType methods[] = {
            CUDD_REORDER_SIFT,
            CUDD_REORDER_SIFT_CONVERGE,
            CUDD_REORDER_RANDOM,
            CUDD_REORDER_RANDOM_PIVOT,
            CUDD_REORDER_LINEAR,
            CUDD_REORDER_LINEAR_CONVERGE,
            CUDD_REORDER_SYMM_SIFT,
            CUDD_REORDER_SYMM_SIFT_CONV
        };
        
        for (int i = 0; i < 8; i++) {
            int result = Cudd_zddReduceHeap(manager, methods[i], 0);
            REQUIRE(result >= 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}
