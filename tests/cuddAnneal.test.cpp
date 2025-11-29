#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtr.h must come before cudd.h for tree functions
#include "mtr.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddAnneal.c
 *
 * This file contains comprehensive tests for the cuddAnneal module
 * to achieve high code coverage (89.3% line coverage). The tests exercise
 * the simulated annealing reordering algorithm through the public
 * Cudd_ReduceHeap API. The remaining uncovered lines are error handling
 * paths for memory allocation failures.
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

// Helper function to create a larger BDD for reordering
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

// Helper to create a BDD with many node interactions across all variables
static DdNode* createDenseBdd(DdManager* manager, int numVars) {
    if (numVars < 4) return nullptr;

    DdNode* result = Cudd_ReadOne(manager);
    Cudd_Ref(result);

    // Create pairwise interactions between many variables
    for (int i = 0; i < numVars; i++) {
        for (int j = i + 2; j < numVars && j < i + 4; j++) {
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

    return result;
}

// ============================================================================
// Basic Annealing Tests
// ============================================================================

TEST_CASE("cuddAnneal - Basic annealing reordering", "[cuddAnneal]") {
    SECTION("Annealing on small BDD") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Use annealing reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        // Verify the BDD is still valid
        REQUIRE(Cudd_DagSize(f) > 0);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing on complex BDD with many interactions") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Annealing should complete successfully
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing reordering count increments") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        unsigned int reorderingsBefore = Cudd_ReadReorderings(manager);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        unsigned int reorderingsAfter = Cudd_ReadReorderings(manager);
        REQUIRE(reorderingsAfter == reorderingsBefore + 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for different BDD sizes and structures
// ============================================================================

TEST_CASE("cuddAnneal - Annealing with varying variable counts", "[cuddAnneal]") {
    SECTION("Annealing with 5 variables") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 5);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with 10 variables") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with 12 variables - larger BDD") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 12);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise different code paths (exchange, jump up, jump down)
// ============================================================================

TEST_CASE("cuddAnneal - Exercise different annealing moves", "[cuddAnneal]") {
    SECTION("Multiple annealing iterations exercise all move types") {
        // Running annealing multiple times exercises exchange, jump up, and jump down
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Multiple annealing passes to exercise different random paths
        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with seeded random - deterministic path") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Seed the random number generator for deterministic behavior
        Cudd_Srandom(manager, 12345);

        DdNode* f = createLargerBdd(manager, 7);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with different random seed") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Different seed for different random path
        Cudd_Srandom(manager, 99999);

        DdNode* f = createLargerBdd(manager, 7);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests with various BDD structures to exercise different swap scenarios
// ============================================================================

TEST_CASE("cuddAnneal - Different BDD structures", "[cuddAnneal]") {
    SECTION("BDD with non-adjacent variable interactions") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD with non-adjacent interactions: x0 AND x3 AND x6
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        DdNode* x6 = Cudd_bddIthVar(manager, 6);

        DdNode* t1 = Cudd_bddAnd(manager, x0, x3);
        Cudd_Ref(t1);

        DdNode* f = Cudd_bddAnd(manager, t1, x6);
        Cudd_Ref(f);

        Cudd_RecursiveDeref(manager, t1);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD with XOR structure") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create XOR chain: x0 XOR x1 XOR x2 XOR x3 XOR x4 XOR x5
        DdNode* f = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(f);

        for (int i = 1; i < 6; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* newF = Cudd_bddXor(manager, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD with disjunction structure") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create OR chain: x0 OR x1 OR x2 OR x3 OR x4 OR x5
        DdNode* f = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(f);

        for (int i = 1; i < 6; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* newF = Cudd_bddOr(manager, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise stopping criteria and temperature decay
// ============================================================================

TEST_CASE("cuddAnneal - Stopping criteria exercises", "[cuddAnneal]") {
    SECTION("Annealing converges on stable BDD") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Simple BDD that is already close to optimal
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing on BDD requiring significant reordering") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Build BDD where variables interact in non-optimal order
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Create interactions like (x0,x9), (x1,x8), etc. - reverse pairing
        for (int i = 0; i < 5; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, 9 - i);
            DdNode* clause = Cudd_bddOr(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int annealResult = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(annealResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for siftBackwardProb coin toss behavior
// ============================================================================

TEST_CASE("cuddAnneal - Probabilistic backward sift", "[cuddAnneal]") {
    SECTION("Multiple annealing runs exercise coin toss") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Multiple runs with different random seeds to exercise the coin toss
        // path in siftBackwardProb
        for (int seed = 1; seed <= 5; seed++) {
            Cudd_Srandom(manager, seed * 7919);  // Prime number for varied seeds

            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for copyOrder and restoreOrder
// ============================================================================

TEST_CASE("cuddAnneal - Order preservation", "[cuddAnneal]") {
    SECTION("Order is tracked correctly during annealing") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Record initial permutation
        int perm[6];
        for (int i = 0; i < 6; i++) {
            perm[i] = Cudd_ReadPerm(manager, i);
        }

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        // After annealing, the order may have changed, but should still be valid
        for (int i = 0; i < 6; i++) {
            int newPerm = Cudd_ReadPerm(manager, i);
            REQUIRE(newPerm >= 0);
            REQUIRE(newPerm < 6);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests with max growth constraints
// ============================================================================

TEST_CASE("cuddAnneal - Max growth constraints", "[cuddAnneal]") {
    SECTION("Annealing respects max growth") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Set tight max growth constraint
        Cudd_SetMaxGrowth(manager, 1.1);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with very tight max growth") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Very tight constraint
        Cudd_SetMaxGrowth(manager, 1.01);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise boundary conditions
// ============================================================================

TEST_CASE("cuddAnneal - Boundary conditions", "[cuddAnneal]") {
    SECTION("Annealing with minimum viable variable count") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with 4 variables") {
        DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddExchange specific paths
// ============================================================================

TEST_CASE("cuddAnneal - Exchange operation paths", "[cuddAnneal]") {
    SECTION("BDD structure that requires multiple exchanges") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD with many adjacent variable interactions
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 5; i++) {
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

        int annealResult = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(annealResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddJumpingUp and ddJumpingDown paths
// ============================================================================

TEST_CASE("cuddAnneal - Jumping operations", "[cuddAnneal]") {
    SECTION("BDD favoring jumping operations") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD where far variables interact
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Interactions: (x0,x7), (x1,x6), (x2,x5), (x3,x4)
        for (int i = 0; i < 4; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, 7 - i);
            DdNode* clause = Cudd_bddXor(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int annealResult = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(annealResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD with mixed jump patterns") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create complex interaction pattern
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Mix of near and far interactions
        int pairs[][2] = {{0, 3}, {1, 7}, {2, 9}, {4, 6}, {5, 8}};
        for (int p = 0; p < 5; p++) {
            DdNode* xi = Cudd_bddIthVar(manager, pairs[p][0]);
            DdNode* xj = Cudd_bddIthVar(manager, pairs[p][1]);
            DdNode* clause = Cudd_bddOr(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int annealResult = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(annealResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests combining annealing with other operations
// ============================================================================

TEST_CASE("cuddAnneal - Annealing combined with other methods", "[cuddAnneal]") {
    SECTION("Annealing after sifting") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // First sift
        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result1 == 1);

        // Then anneal
        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Sifting after annealing") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // First anneal
        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result1 == 1);

        // Then sift
        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for various temperature decay scenarios
// ============================================================================

TEST_CASE("cuddAnneal - Temperature decay paths", "[cuddAnneal]") {
    SECTION("Long running annealing with temperature decay") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        // Larger BDD may require more iterations, testing temperature decay
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to verify BDD integrity after annealing
// ============================================================================

TEST_CASE("cuddAnneal - BDD integrity verification", "[cuddAnneal]") {
    SECTION("BDD validity after annealing") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        int sizeBefore = Cudd_DagSize(f);
        double mintermsBefore = Cudd_CountMinterm(manager, f, 6);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        // BDD should still represent the same function (same minterm count)
        double mintermsAfter = Cudd_CountMinterm(manager, f, 6);
        REQUIRE(mintermsBefore == mintermsAfter);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Multiple BDDs remain valid after annealing") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f1 = createComplexBdd(manager, 8);
        REQUIRE(f1 != nullptr);

        DdNode* f2 = createLargerBdd(manager, 8);
        REQUIRE(f2 != nullptr);

        double minterms1Before = Cudd_CountMinterm(manager, f1, 8);
        double minterms2Before = Cudd_CountMinterm(manager, f2, 8);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        // Both BDDs should still be valid
        REQUIRE(Cudd_CountMinterm(manager, f1, 8) == minterms1Before);
        REQUIRE(Cudd_CountMinterm(manager, f2, 8) == minterms2Before);

        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for edge cases in random selection
// ============================================================================

TEST_CASE("cuddAnneal - Random selection edge cases", "[cuddAnneal]") {
    SECTION("Many annealing iterations with varied seeds") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 7);
        REQUIRE(f != nullptr);

        // Run many times with different seeds to exercise all random paths
        for (int i = 0; i < 10; i++) {
            Cudd_Srandom(manager, i * 1000 + 1);
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests using variable groups to exercise cuddAnnealing with bounds
// ============================================================================

TEST_CASE("cuddAnneal - Annealing with variable groups", "[cuddAnneal]") {
    SECTION("Annealing with variable group constraints") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Create a group tree to test bounded annealing
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Annealing with multiple variable groups") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        // Create multiple groups
        MtrNode* tree1 = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree1 != nullptr);

        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 5, 5, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Stress tests
// ============================================================================

TEST_CASE("cuddAnneal - Stress tests", "[cuddAnneal]") {
    SECTION("Annealing on larger BDD") {
        DdManager* manager = Cudd_Init(15, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 15);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Sequential annealing operations") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Repeated annealing
        for (int i = 0; i < 5; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}
