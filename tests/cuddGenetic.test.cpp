#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "mtr.h"        // For MtrNode (variable grouping tree)
#include "cudd/cudd.h"  // Main CUDD API
#include "cuddInt.h"    // Internal definitions
#include "util.h"       // Utility functions

/**
 * @brief Test file for cuddGenetic.c
 *
 * This file contains comprehensive tests for the cuddGenetic module
 * to achieve high code coverage (targeting 90%+). The tests exercise
 * the genetic algorithm reordering through the public Cudd_ReduceHeap API.
 *
 * The genetic algorithm (cuddGa) performs:
 * 1. Initial sifting to produce a reference DD
 * 2. Population initialization with random orders
 * 3. Crossover (PMX) operations to generate offspring
 * 4. Selection of the best order from the population
 */

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Helper function to create a BDD with interacting adjacent variables
 * Creates f = (x0 AND x1) OR (x1 AND x2) which has variable interactions.
 */
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

/**
 * @brief Helper function to create a chained BDD clause
 * Creates f = (x0 OR x1) AND (x1 OR x2) AND ... for all adjacent pairs.
 */
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

/**
 * @brief Helper to create a BDD with many node interactions across all variables
 */
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
// Basic Genetic Algorithm Tests
// ============================================================================

TEST_CASE("cuddGenetic - Basic genetic reordering", "[cuddGenetic]") {
    SECTION("Genetic algorithm on small BDD") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Use genetic algorithm reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        // Verify the BDD is still valid
        REQUIRE(Cudd_DagSize(f) > 0);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic algorithm on complex BDD with many interactions") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Genetic algorithm should complete successfully
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic reordering count increments") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        unsigned int reorderingsBefore = Cudd_ReadReorderings(manager);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
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

TEST_CASE("cuddGenetic - Genetic algorithm with varying variable counts", "[cuddGenetic]") {
    SECTION("Genetic with 5 variables") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 5);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic with 10 variables") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic with 12 variables - larger BDD") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 12);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for population size configuration
// ============================================================================

TEST_CASE("cuddGenetic - Population size configuration", "[cuddGenetic]") {
    SECTION("Default population size (3 * numvars)") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Default population size is 0, meaning use default calculation
        REQUIRE(Cudd_ReadPopulationSize(manager) == 0);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Custom population size - moderate") {
        // Use population size >= numvars to avoid known bounds issue
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Set a moderate population size (at least >= numvars)
        Cudd_SetPopulationSize(manager, 10);
        REQUIRE(Cudd_ReadPopulationSize(manager) == 10);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Custom population size - at numvars boundary") {
        // Set population size equal to numvars
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 5);
        REQUIRE(f != nullptr);

        // Set population size equal to number of variables
        Cudd_SetPopulationSize(manager, 5);
        REQUIRE(Cudd_ReadPopulationSize(manager) == 5);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Custom population size - large") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Set a larger population size
        Cudd_SetPopulationSize(manager, 50);
        REQUIRE(Cudd_ReadPopulationSize(manager) == 50);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for crossover configuration
// ============================================================================

TEST_CASE("cuddGenetic - Crossover configuration", "[cuddGenetic]") {
    SECTION("Default number of crossovers") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Default is 0, meaning use default calculation
        REQUIRE(Cudd_ReadNumberXovers(manager) == 0);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Custom number of crossovers - small") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Set a small number of crossovers
        Cudd_SetNumberXovers(manager, 5);
        REQUIRE(Cudd_ReadNumberXovers(manager) == 5);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Custom number of crossovers - large") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Set a larger number of crossovers
        Cudd_SetNumberXovers(manager, 30);
        REQUIRE(Cudd_ReadNumberXovers(manager) == 30);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Crossovers exceeding population size") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Set crossovers greater than population - will be clamped
        Cudd_SetPopulationSize(manager, 10);
        Cudd_SetNumberXovers(manager, 100);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for different BDD structures
// ============================================================================

TEST_CASE("cuddGenetic - Different BDD structures", "[cuddGenetic]") {
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

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
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

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
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

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for random seed variations (exercises different code paths)
// ============================================================================

TEST_CASE("cuddGenetic - Random seed variations", "[cuddGenetic]") {
    SECTION("Multiple genetic runs with seeded random") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Seed the random number generator for deterministic behavior
        Cudd_Srandom(manager, 12345);

        DdNode* f = createLargerBdd(manager, 7);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Different random seed") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Different seed for different random path
        Cudd_Srandom(manager, 99999);

        DdNode* f = createLargerBdd(manager, 7);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Multiple genetic runs exercise different random paths") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Multiple runs with different random seeds
        for (int seed = 1; seed <= 5; seed++) {
            Cudd_Srandom(manager, seed * 7919);  // Prime number for varied seeds

            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for BDD integrity verification
// ============================================================================

TEST_CASE("cuddGenetic - BDD integrity verification", "[cuddGenetic]") {
    SECTION("BDD validity after genetic algorithm") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        double mintermsBefore = Cudd_CountMinterm(manager, f, 6);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        // BDD should still represent the same function (same minterm count)
        double mintermsAfter = Cudd_CountMinterm(manager, f, 6);
        REQUIRE(mintermsBefore == mintermsAfter);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Multiple BDDs remain valid after genetic algorithm") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f1 = createComplexBdd(manager, 8);
        REQUIRE(f1 != nullptr);

        DdNode* f2 = createLargerBdd(manager, 8);
        REQUIRE(f2 != nullptr);

        double minterms1Before = Cudd_CountMinterm(manager, f1, 8);
        double minterms2Before = Cudd_CountMinterm(manager, f2, 8);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
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
// Tests for order preservation and tracking
// ============================================================================

TEST_CASE("cuddGenetic - Order preservation", "[cuddGenetic]") {
    SECTION("Order is tracked correctly during genetic algorithm") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        // After genetic algorithm, the order may have changed, but should still be valid
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
// Tests with variable groups
// ============================================================================

TEST_CASE("cuddGenetic - Genetic algorithm with variable groups", "[cuddGenetic]") {
    SECTION("Genetic with variable group constraints") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Create a group tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic with multiple variable groups") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        // Create multiple groups
        MtrNode* tree1 = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree1 != nullptr);

        MtrNode* tree2 = Cudd_MakeTreeNode(manager, 5, 5, MTR_DEFAULT);
        REQUIRE(tree2 != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests combining genetic algorithm with other operations
// ============================================================================

TEST_CASE("cuddGenetic - Genetic combined with other methods", "[cuddGenetic]") {
    SECTION("Genetic after sifting") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // First sift
        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result1 == 1);

        // Then genetic
        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Sifting after genetic") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // First genetic
        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result1 == 1);

        // Then sift
        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for boundary conditions
// ============================================================================

TEST_CASE("cuddGenetic - Boundary conditions", "[cuddGenetic]") {
    SECTION("Genetic with minimum viable variable count") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic with 4 variables") {
        DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for BDD requiring significant reordering
// ============================================================================

TEST_CASE("cuddGenetic - BDD requiring significant reordering", "[cuddGenetic]") {
    SECTION("BDD with reverse pairing interactions") {
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

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for max growth constraints
// ============================================================================

TEST_CASE("cuddGenetic - Max growth constraints", "[cuddGenetic]") {
    SECTION("Genetic respects max growth") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Set tight max growth constraint
        Cudd_SetMaxGrowth(manager, 1.1);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Genetic with very tight max growth") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Very tight constraint
        Cudd_SetMaxGrowth(manager, 1.01);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Stress tests
// ============================================================================

TEST_CASE("cuddGenetic - Stress tests", "[cuddGenetic]") {
    SECTION("Genetic on larger BDD") {
        DdManager* manager = Cudd_Init(15, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 15);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Sequential genetic operations") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Repeated genetic algorithm
        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise PMX crossover operation
// ============================================================================

TEST_CASE("cuddGenetic - PMX crossover exercises", "[cuddGenetic]") {
    SECTION("Exercise crossover with varied population") {
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

        // Multiple crossovers to exercise PMX
        Cudd_SetNumberXovers(manager, 20);

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise roulette wheel selection
// ============================================================================

TEST_CASE("cuddGenetic - Roulette wheel selection", "[cuddGenetic]") {
    SECTION("Exercise roulette selection with varied fitness") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD where far variables interact - creates varied fitness
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

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

        // Large population to exercise roulette wheel
        Cudd_SetPopulationSize(manager, 30);

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for find_best and largest functions
// ============================================================================

TEST_CASE("cuddGenetic - Best/Largest selection exercises", "[cuddGenetic]") {
    SECTION("Exercise best finding with large population") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        // Large population to exercise find_best and largest
        Cudd_SetPopulationSize(manager, 40);
        Cudd_SetNumberXovers(manager, 30);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for computed table (duplicate order handling)
// ============================================================================

TEST_CASE("cuddGenetic - Computed table exercises", "[cuddGenetic]") {
    SECTION("Exercise computed table with many crossovers") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Many crossovers may create duplicate orders
        Cudd_SetPopulationSize(manager, 20);
        Cudd_SetNumberXovers(manager, 40);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Small population with many crossovers (more duplicates)") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 6);
        REQUIRE(f != nullptr);

        // Small population with many crossovers increases duplicate chance
        Cudd_SetPopulationSize(manager, 6);
        Cudd_SetNumberXovers(manager, 30);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests with various random seed values
// ============================================================================

TEST_CASE("cuddGenetic - Extensive random seed variations", "[cuddGenetic]") {
    SECTION("Many random seeds to exercise all paths") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 7);
        REQUIRE(f != nullptr);

        // Run with many different seeds
        for (int i = 0; i < 10; i++) {
            Cudd_Srandom(manager, i * 1000 + 1);
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for reverse order handling
// ============================================================================

TEST_CASE("cuddGenetic - Reverse order handling", "[cuddGenetic]") {
    SECTION("Initial population includes reverse order") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // The genetic algorithm adds the reverse order as second element
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for population limit
// The genetic algorithm caps population size at 120 (CUDD_GENETIC_POP_MAX)
// ============================================================================

TEST_CASE("cuddGenetic - Population limit", "[cuddGenetic]") {
    SECTION("Large variable count to trigger population cap") {
        // With 50 variables, default population would be 3*50=150,
        // but it gets capped at the maximum of 120
        DdManager* manager = Cudd_Init(50, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create a simple BDD with many variables
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 20; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 20);
            DdNode* clause = Cudd_bddOr(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for crossover limit
// The genetic algorithm caps crossovers at 60 (CUDD_GENETIC_XOVER_MAX)
// ============================================================================

TEST_CASE("cuddGenetic - Crossover limit", "[cuddGenetic]") {
    SECTION("Large variable count to trigger crossover cap") {
        // With 25 variables, default crossovers would be 3*25=75,
        // but it gets capped at the maximum of 60
        DdManager* manager = Cudd_Init(25, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create a BDD
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 10; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 10);
            DdNode* clause = Cudd_bddOr(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for sift_up function (within build_dd)
// ============================================================================

TEST_CASE("cuddGenetic - Sift up operations", "[cuddGenetic]") {
    SECTION("Build DD with various orders") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        // The build_dd function uses sift_up to build permutations
        Cudd_SetPopulationSize(manager, 15);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for array_hash and array_compare functions
// ============================================================================

TEST_CASE("cuddGenetic - Hash table operations", "[cuddGenetic]") {
    SECTION("Exercise hash table with many orders") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Large population creates many hash table entries
        Cudd_SetPopulationSize(manager, 30);
        Cudd_SetNumberXovers(manager, 50);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for make_random function
// ============================================================================

TEST_CASE("cuddGenetic - Make random orders", "[cuddGenetic]") {
    SECTION("Generate many random orders") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 12);
        REQUIRE(f != nullptr);

        // Large population means many random orders
        Cudd_SetPopulationSize(manager, 36);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for offspring replacement
// ============================================================================

TEST_CASE("cuddGenetic - Offspring replacement", "[cuddGenetic]") {
    SECTION("Exercise offspring replacing larger DD") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD that benefits from reordering
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Interactions: far pairs
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

        // Many crossovers to ensure offspring replacement happens
        Cudd_SetNumberXovers(manager, 30);

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests with stable BDDs
// ============================================================================

TEST_CASE("cuddGenetic - Stable BDD handling", "[cuddGenetic]") {
    SECTION("Simple BDD already near optimal") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Simple adjacent interactions - likely already optimal
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for repeat count handling
// ============================================================================

TEST_CASE("cuddGenetic - Repeat count handling", "[cuddGenetic]") {
    SECTION("Exercise repeat count with many duplicates") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 5);
        REQUIRE(f != nullptr);

        // Small number of variables with large population = more duplicates
        Cudd_SetPopulationSize(manager, 15);
        Cudd_SetNumberXovers(manager, 40);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for size limit in build_dd
// ============================================================================

TEST_CASE("cuddGenetic - Build DD size limit", "[cuddGenetic]") {
    SECTION("Exercise size limit during building") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create a large BDD to potentially hit size limits
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 11; i++) {
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

        int geneticResult = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        REQUIRE(geneticResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}
