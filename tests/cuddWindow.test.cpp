#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtr.h must come before cudd.h for tree functions
#include "mtr.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddWindow.c
 *
 * This file contains comprehensive tests for the cuddWindow module
 * to achieve 90% code coverage. The tests exercise the window-based
 * reordering algorithms including:
 * - cuddWindowReorder (main entry point)
 * - ddWindow2, ddWindowConv2 (window-2 reordering)
 * - ddWindow3, ddWindowConv3, ddPermuteWindow3 (window-3 reordering)
 * - ddWindow4, ddWindowConv4, ddPermuteWindow4 (window-4 reordering)
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

// Helper to create BDD with XOR structure (good for exercise various permutations)
static DdNode* createXorChainBdd(DdManager* manager, int numVars) {
    if (numVars < 2) return nullptr;

    DdNode* f = Cudd_bddIthVar(manager, 0);
    Cudd_Ref(f);

    for (int i = 1; i < numVars; i++) {
        DdNode* xi = Cudd_bddIthVar(manager, i);
        DdNode* newF = Cudd_bddXor(manager, f, xi);
        Cudd_Ref(newF);
        Cudd_RecursiveDeref(manager, f);
        f = newF;
    }

    return f;
}

// Helper to create BDD with non-adjacent interactions (exercises different permutation paths)
static DdNode* createNonAdjacentBdd(DdManager* manager, int numVars) {
    if (numVars < 4) return nullptr;

    DdNode* result = Cudd_ReadOne(manager);
    Cudd_Ref(result);

    // Create interactions between distant variables
    for (int i = 0; i < numVars / 2; i++) {
        DdNode* xi = Cudd_bddIthVar(manager, i);
        DdNode* xj = Cudd_bddIthVar(manager, numVars - 1 - i);
        DdNode* clause = Cudd_bddOr(manager, xi, xj);
        Cudd_Ref(clause);

        DdNode* newResult = Cudd_bddAnd(manager, result, clause);
        Cudd_Ref(newResult);

        Cudd_RecursiveDeref(manager, clause);
        Cudd_RecursiveDeref(manager, result);
        result = newResult;
    }

    return result;
}

// ============================================================================
// Tests for cuddWindowReorder entry point
// ============================================================================

TEST_CASE("cuddWindow - cuddWindowReorder with WINDOW2", "[cuddWindow]") {
    SECTION("Window2 on BDD with sufficient variables") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        // BDD should still be valid
        REQUIRE(Cudd_DagSize(f) > 0);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window2 on small BDD") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - cuddWindowReorder with WINDOW3", "[cuddWindow]") {
    SECTION("Window3 on BDD with sufficient variables") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 falls back to Window2 for small range") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);

        // With only 3 variables, Window3 should fall back to Window2
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - cuddWindowReorder with WINDOW4", "[cuddWindow]") {
    SECTION("Window4 on BDD with sufficient variables") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 falls back to Window3 for small range") {
        DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);

        // With only 4 variables, Window4 should fall back to Window3
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - cuddWindowReorder with WINDOW2_CONV", "[cuddWindow]") {
    SECTION("Window2 convergence on BDD") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window2 convergence with XOR structure") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createXorChainBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - cuddWindowReorder with WINDOW3_CONV", "[cuddWindow]") {
    SECTION("Window3 convergence on BDD") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 convergence falls back to Window2 for small range") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);

        // With only 3 variables, should fall back to Window2Conv
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - cuddWindowReorder with WINDOW4_CONV", "[cuddWindow]") {
    SECTION("Window4 convergence on BDD") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 convergence falls back to Window3 for small range") {
        DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);

        // With only 4 variables, should fall back to Window3Conv
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddWindow2 function behavior
// ============================================================================

TEST_CASE("cuddWindow - ddWindow2 edge cases", "[cuddWindow]") {
    SECTION("Window2 with improvement during swap") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD where reordering can help
        DdNode* f = createNonAdjacentBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window2 with no improvement (undo permutation)") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create a simple BDD that's already optimally ordered
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddWindowConv2 function behavior
// ============================================================================

TEST_CASE("cuddWindow - ddWindowConv2 iterations", "[cuddWindow]") {
    SECTION("Window2Conv with events triggering multiple iterations") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD that may require multiple convergence iterations
        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window2Conv exercises event propagation") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddPermuteWindow3 - all 6 permutations
// ============================================================================

TEST_CASE("cuddWindow - ddPermuteWindow3 permutation paths", "[cuddWindow]") {
    SECTION("Window3 exercises multiple permutations on complex BDD") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD that likely exercises different permutation branches
        DdNode* f = createXorChainBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 on non-adjacent interaction BDD") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createNonAdjacentBdd(manager, 7);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 with dense BDD for more paths") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddWindowConv3 - all cases in switch statement
// ============================================================================

TEST_CASE("cuddWindow - ddWindowConv3 event handling", "[cuddWindow]") {
    SECTION("Window3Conv exercises different result cases") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3Conv with XOR chain") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createXorChainBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3Conv with non-adjacent interactions") {
        DdManager* manager = Cudd_Init(9, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createNonAdjacentBdd(manager, 9);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddPermuteWindow4 - all 24 permutations
// ============================================================================

TEST_CASE("cuddWindow - ddPermuteWindow4 permutation paths", "[cuddWindow]") {
    SECTION("Window4 exercises multiple permutations") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 on XOR chain BDD") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createXorChainBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 on non-adjacent BDD") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createNonAdjacentBdd(manager, 12);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 on larger BDD for more coverage") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddWindowConv4 - all cases in switch statement
// ============================================================================

TEST_CASE("cuddWindow - ddWindowConv4 event handling", "[cuddWindow]") {
    SECTION("Window4Conv exercises different result cases") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4Conv with XOR chain") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createXorChainBdd(manager, 12);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4Conv with non-adjacent interactions") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createNonAdjacentBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for BDD integrity after window reordering
// ============================================================================

TEST_CASE("cuddWindow - BDD integrity after reordering", "[cuddWindow]") {
    SECTION("Window2 preserves BDD semantics") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        double mintermsBefore = Cudd_CountMinterm(manager, f, 6);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        double mintermsAfter = Cudd_CountMinterm(manager, f, 6);
        REQUIRE(mintermsBefore == mintermsAfter);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 preserves BDD semantics") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 6);
        REQUIRE(f != nullptr);

        double mintermsBefore = Cudd_CountMinterm(manager, f, 6);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        double mintermsAfter = Cudd_CountMinterm(manager, f, 6);
        REQUIRE(mintermsBefore == mintermsAfter);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 preserves BDD semantics") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);

        double mintermsBefore = Cudd_CountMinterm(manager, f, 8);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        double mintermsAfter = Cudd_CountMinterm(manager, f, 8);
        REQUIRE(mintermsBefore == mintermsAfter);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for multiple reorderings
// ============================================================================

TEST_CASE("cuddWindow - Multiple sequential reorderings", "[cuddWindow]") {
    SECTION("Multiple Window2 reorderings") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 6);
        REQUIRE(f != nullptr);

        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Multiple Window3 reorderings") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Multiple Window4 reorderings") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        for (int i = 0; i < 3; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for combining window methods with other reordering
// ============================================================================

TEST_CASE("cuddWindow - Window methods combined with sifting", "[cuddWindow]") {
    SECTION("Sift then Window2") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result1 == 1);

        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 then Sift") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result1 == 1);

        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 then Window2") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result1 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result1 == 1);

        int result2 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result2 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for variable groups with window reordering
// ============================================================================

TEST_CASE("cuddWindow - Window reordering with variable groups", "[cuddWindow]") {
    SECTION("Window2 with variable group tree") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);

        // Create a group tree
        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 with variable group tree") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 8);
        REQUIRE(f != nullptr);

        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 4, MTR_DEFAULT);
        REQUIRE(tree != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 with variable group tree") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 10);
        REQUIRE(f != nullptr);

        MtrNode* tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for special BDD structures
// ============================================================================

TEST_CASE("cuddWindow - Special BDD structures", "[cuddWindow]") {
    SECTION("Window reordering on AND chain") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create AND chain: x0 AND x1 AND x2 AND x3 AND x4 AND x5
        DdNode* f = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(f);

        for (int i = 1; i < 6; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* newF = Cudd_bddAnd(manager, f, xi);
            Cudd_Ref(newF);
            Cudd_RecursiveDeref(manager, f);
            f = newF;
        }

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window reordering on OR chain") {
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

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window reordering on mixed structure") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create mixed structure with ANDs, ORs and XORs
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 7; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xi1 = Cudd_bddIthVar(manager, i + 1);
            DdNode* clause;
            if (i % 3 == 0) {
                clause = Cudd_bddAnd(manager, xi, xi1);
            } else if (i % 3 == 1) {
                clause = Cudd_bddOr(manager, xi, xi1);
            } else {
                clause = Cudd_bddXor(manager, xi, xi1);
            }
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for boundary conditions
// ============================================================================

TEST_CASE("cuddWindow - Boundary conditions", "[cuddWindow]") {
    SECTION("Window2 with minimum variables (2)") {
        DdManager* manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 with minimum variables (3)") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 3);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 with minimum variables (4)") {
        DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createComplexBdd(manager, 4);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 with 5 variables") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 5);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for larger BDDs
// ============================================================================

TEST_CASE("cuddWindow - Larger BDDs", "[cuddWindow]") {
    SECTION("Window2 on larger BDD") {
        DdManager* manager = Cudd_Init(15, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 15);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3 on larger BDD") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 12);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 on larger BDD") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 12);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for different BDD patterns to exercise all switch cases
// ============================================================================

TEST_CASE("cuddWindow - Exercise switch cases in ddWindowConv3", "[cuddWindow]") {
    SECTION("BDD pattern for ABC case (no change)") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Simple BDD that may not change during permutation
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern that exercises BAC case") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // BDD with specific structure for BAC permutation
        DdNode* f = createXorChainBdd(manager, 6);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for BCA/CBA/CAB cases") {
        DdManager* manager = Cudd_Init(9, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createNonAdjacentBdd(manager, 9);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for ACB case") {
        DdManager* manager = Cudd_Init(7, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 7);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise ddWindowConv4 switch cases
// ============================================================================

TEST_CASE("cuddWindow - Exercise switch cases in ddWindowConv4", "[cuddWindow]") {
    SECTION("BDD pattern for ABCD case") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for BACD case") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createXorChainBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for BADC case") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createNonAdjacentBdd(manager, 10);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for ABDC case") {
        DdManager* manager = Cudd_Init(9, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 9);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for complex case group (DACB, DABC, etc)") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create complex BDD that may trigger many different permutation cases
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 12; i++) {
            for (int j = i + 2; j < 12 && j < i + 5; j++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause = Cudd_bddXor(manager, xi, xj);
                Cudd_Ref(clause);

                DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for BCAD/CBAD/CABD cases") {
        DdManager* manager = Cudd_Init(11, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 11);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD pattern for ACBD case") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 8);
        REQUIRE(f != nullptr);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Stress tests
// ============================================================================

TEST_CASE("cuddWindow - Stress tests", "[cuddWindow]") {
    SECTION("Many sequential Window2 convergence iterations") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        for (int i = 0; i < 5; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Many sequential Window3 convergence iterations") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        for (int i = 0; i < 5; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Many sequential Window4 convergence iterations") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        for (int i = 0; i < 5; i++) {
            int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(result == 1);
        }

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional tests to exercise more permutation paths in ddPermuteWindow3/4
// ============================================================================

TEST_CASE("cuddWindow - Additional permutation pattern tests", "[cuddWindow]") {
    SECTION("BDD structure favoring various 3-var permutations") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create a complex BDD with specific interaction patterns
        // to trigger different permutation outcomes
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Create interlocked variable pattern
        for (int i = 0; i < 8; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, (i + 3) % 10);
            DdNode* xk = Cudd_bddIthVar(manager, (i + 5) % 10);
            
            DdNode* t1 = Cudd_bddAnd(manager, xi, xj);
            Cudd_Ref(t1);
            DdNode* t2 = Cudd_bddOr(manager, t1, xk);
            Cudd_Ref(t2);
            
            DdNode* newResult = Cudd_bddAnd(manager, result, t2);
            Cudd_Ref(newResult);
            
            Cudd_RecursiveDeref(manager, t1);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD structure favoring various 4-var permutations") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create a complex BDD with specific interaction patterns
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Create interlocked variable pattern across 4 variables
        for (int i = 0; i < 9; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, (i + 1) % 12);
            DdNode* xk = Cudd_bddIthVar(manager, (i + 2) % 12);
            DdNode* xl = Cudd_bddIthVar(manager, (i + 3) % 12);
            
            DdNode* t1 = Cudd_bddXor(manager, xi, xj);
            Cudd_Ref(t1);
            DdNode* t2 = Cudd_bddXor(manager, xk, xl);
            Cudd_Ref(t2);
            DdNode* t3 = Cudd_bddOr(manager, t1, t2);
            Cudd_Ref(t3);
            
            DdNode* newResult = Cudd_bddAnd(manager, result, t3);
            Cudd_Ref(newResult);
            
            Cudd_RecursiveDeref(manager, t1);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("Window4Conv on highly connected BDD") {
        DdManager* manager = Cudd_Init(14, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create highly interconnected BDD
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 14; i++) {
            for (int j = i + 1; j < 14 && j < i + 6; j++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause;
                
                if ((i + j) % 3 == 0) {
                    clause = Cudd_bddAnd(manager, xi, xj);
                } else if ((i + j) % 3 == 1) {
                    clause = Cudd_bddOr(manager, xi, xj);
                } else {
                    clause = Cudd_bddXor(manager, xi, xj);
                }
                Cudd_Ref(clause);

                DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("Window3Conv on zigzag pattern BDD") {
        DdManager* manager = Cudd_Init(11, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create zigzag pattern: x0-x10, x1-x9, x2-x8, etc.
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i <= 5; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, 10 - i);
            DdNode* clause = Cudd_bddXor(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for specific event propagation in convergence algorithms
// ============================================================================

TEST_CASE("cuddWindow - Event propagation tests", "[cuddWindow]") {
    SECTION("Window2Conv event at boundary x=0") {
        DdManager* manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD where first variables benefit from reordering
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x5 = Cudd_bddIthVar(manager, 5);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x5);
        Cudd_Ref(t1);
        DdNode* f = Cudd_bddOr(manager, t1, x1);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, t1);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window3Conv event at boundary x=nwin-1") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD where last variables benefit from reordering
        DdNode* x6 = Cudd_bddIthVar(manager, 6);
        DdNode* x7 = Cudd_bddIthVar(manager, 7);
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        
        DdNode* t1 = Cudd_bddAnd(manager, x6, x0);
        Cudd_Ref(t1);
        DdNode* f = Cudd_bddOr(manager, t1, x7);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, t1);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4Conv event propagation") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD with structure to trigger specific event patterns
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Distant interactions to force event propagation
        for (int i = 0; i < 5; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 5);
            DdNode* clause = Cudd_bddAnd(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for various BDD sizes to exercise window sliding
// ============================================================================

TEST_CASE("cuddWindow - Window sliding tests", "[cuddWindow]") {
    SECTION("Window3 exactly 3 variables") {
        DdManager* manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x2);
        Cudd_Ref(t1);
        DdNode* f = Cudd_bddOr(manager, t1, x1);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, t1);

        // Window3 with exactly 3 vars - single window
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 exactly 4 variables") {
        DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        DdNode* t1 = Cudd_bddAnd(manager, x0, x3);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x1, x2);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);

        // Window4 with exactly 4 vars - single window
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("Window4 exactly 5 variables") {
        DdManager* manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createLargerBdd(manager, 5);
        REQUIRE(f != nullptr);

        // Window4 with 5 vars - two overlapping windows
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise cuddWindowReorder directly
// ============================================================================

TEST_CASE("cuddWindow - cuddWindowReorder internal API", "[cuddWindow]") {
    SECTION("Direct call via ReduceHeap exercises all window types") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* f = createDenseBdd(manager, 10);
        REQUIRE(f != nullptr);

        // Test all window methods in sequence
        int r1 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        REQUIRE(r1 == 1);

        int r2 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3, 0);
        REQUIRE(r2 == 1);

        int r3 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(r3 == 1);

        int r4 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2_CONV, 0);
        REQUIRE(r4 == 1);

        int r5 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(r5 == 1);

        int r6 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(r6 == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Targeted tests to exercise specific permutation paths
// ============================================================================

TEST_CASE("cuddWindow - Targeted permutation tests for Window4", "[cuddWindow]") {
    SECTION("BDD structure favoring BACD permutation result") {
        // Try to create a BDD where swapping first two variables is best
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create specific pattern to favor BACD outcome
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x4 = Cudd_bddIthVar(manager, 4);
        DdNode* x5 = Cudd_bddIthVar(manager, 5);
        
        // Pattern: (x1 AND x4) XOR (x0 AND x5)
        DdNode* t1 = Cudd_bddAnd(manager, x1, x4);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x0, x5);
        Cudd_Ref(t2);
        DdNode* f = Cudd_bddXor(manager, t1, t2);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);

        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(result == 1);

        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }

    SECTION("BDD structure with many 4-var windows") {
        DdManager* manager = Cudd_Init(16, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create very complex BDD to trigger more permutation paths
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 13; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 1);
            DdNode* xk = Cudd_bddIthVar(manager, i + 2);
            DdNode* xl = Cudd_bddIthVar(manager, i + 3);
            
            // Create complex 4-variable interaction
            DdNode* t1 = Cudd_bddAnd(manager, xi, xl);
            Cudd_Ref(t1);
            DdNode* t2 = Cudd_bddAnd(manager, xj, xk);
            Cudd_Ref(t2);
            DdNode* t3 = Cudd_bddXor(manager, t1, t2);
            Cudd_Ref(t3);
            
            DdNode* newResult = Cudd_bddAnd(manager, result, t3);
            Cudd_Ref(newResult);
            
            Cudd_RecursiveDeref(manager, t1);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("Repeated Window4Conv with different seeds") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create BDD with many interactions
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                if (i != j && abs(i - j) > 2) {
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
        }

        // Multiple reordering passes to exercise different paths
        for (int i = 0; i < 3; i++) {
            int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(reorderResult == 1);
        }

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - Targeted permutation tests for Window3", "[cuddWindow]") {
    SECTION("BDD with specific 3-var interaction pattern") {
        DdManager* manager = Cudd_Init(9, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create pattern that exercises different 3-var permutations
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 7; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 1);
            DdNode* xk = Cudd_bddIthVar(manager, i + 2);
            
            // Pattern: (xi XOR xk) AND NOT(xj)
            DdNode* t1 = Cudd_bddXor(manager, xi, xk);
            Cudd_Ref(t1);
            DdNode* notXj = Cudd_Not(xj);
            DdNode* t2 = Cudd_bddAnd(manager, t1, notXj);
            Cudd_Ref(t2);
            
            DdNode* newResult = Cudd_bddOr(manager, result, t2);
            Cudd_Ref(newResult);
            
            Cudd_RecursiveDeref(manager, t1);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddWindow - Large BDD for extensive permutation testing", "[cuddWindow]") {
    SECTION("Very large BDD with complex interactions") {
        DdManager* manager = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create very complex BDD
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 20; i++) {
            for (int j = i + 3; j < 20 && j < i + 8; j++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause = Cudd_bddXor(manager, xi, xj);
                Cudd_Ref(clause);

                DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }

        // Test Window4 on large BDD
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional targeted tests for rare permutation outcomes
// ============================================================================

TEST_CASE("cuddWindow - Rare permutation outcome tests", "[cuddWindow]") {
    SECTION("BDD with reverse-order interactions") {
        // Create BDD where variables interact in reverse order
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Pattern: x11-x0, x10-x1, x9-x2, etc.
        for (int i = 0; i < 6; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, 11 - i);
            DdNode* clause = Cudd_bddAnd(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD with skip-2 interactions") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Pattern: x0-x2, x1-x3, x2-x4, etc.
        for (int i = 0; i < 8; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 2);
            DdNode* clause = Cudd_bddAnd(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        // Additional complexity
        for (int i = 0; i < 5; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 5);
            DdNode* clause = Cudd_bddXor(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD with modular interaction pattern") {
        DdManager* manager = Cudd_Init(16, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Create modular pattern
        for (int i = 0; i < 16; i++) {
            int j = (i + 5) % 16;
            if (i != j) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause = Cudd_bddAnd(manager, xi, Cudd_Not(xj));
                Cudd_Ref(clause);

                DdNode* newResult = Cudd_bddOr(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD alternating AND/XOR pattern") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 10; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 2);
            DdNode* clause;
            
            if (i % 2 == 0) {
                clause = Cudd_bddAnd(manager, xi, xj);
            } else {
                clause = Cudd_bddXor(manager, xi, xj);
            }
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD with dense center interactions") {
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Dense interactions in center variables (3,4,5,6)
        for (int i = 3; i <= 6; i++) {
            for (int j = i + 1; j <= 6; j++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause = Cudd_bddAnd(manager, xi, xj);
                Cudd_Ref(clause);

                DdNode* newResult = Cudd_bddOr(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }

        // Edge interactions with center
        for (int i = 0; i < 3; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xc = Cudd_bddIthVar(manager, 5);
            DdNode* clause = Cudd_bddXor(manager, xi, xc);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }
        for (int i = 7; i < 10; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xc = Cudd_bddIthVar(manager, 4);
            DdNode* clause = Cudd_bddXor(manager, xi, xc);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("Multiple convergence passes on asymmetric BDD") {
        DdManager* manager = Cudd_Init(14, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Asymmetric interaction pattern
        for (int i = 0; i < 7; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, i + 7);
            DdNode* xk = Cudd_bddIthVar(manager, (i + 3) % 14);
            
            DdNode* t1 = Cudd_bddAnd(manager, xi, xj);
            Cudd_Ref(t1);
            DdNode* t2 = Cudd_bddOr(manager, t1, xk);
            Cudd_Ref(t2);
            
            DdNode* newResult = Cudd_bddAnd(manager, result, t2);
            Cudd_Ref(newResult);
            
            Cudd_RecursiveDeref(manager, t1);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        // Multiple passes to exercise convergence
        for (int pass = 0; pass < 5; pass++) {
            int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(reorderResult == 1);
        }

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests targeting specific switch cases in ddWindowConv4
// ============================================================================

TEST_CASE("cuddWindow - ddWindowConv4 specific cases", "[cuddWindow]") {
    SECTION("BDD favoring BACD result in Window4 convergence") {
        // Try to trigger BACD case (code 7) in ddWindowConv4
        DdManager* manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Create pattern where swapping first pair helps
        for (int i = 0; i < 8; i++) {
            DdNode* x0 = Cudd_bddIthVar(manager, 1);  // Note: using x1, not x0
            DdNode* xi = Cudd_bddIthVar(manager, i + 2);
            DdNode* clause = Cudd_bddAnd(manager, x0, xi);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD favoring BADC result in Window4 convergence") {
        // Try to trigger BADC case (code 13)
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Create pattern where both first pair and last pair swaps help
        for (int i = 0; i < 10; i++) {
            DdNode* x1 = Cudd_bddIthVar(manager, 1);
            DdNode* x3 = Cudd_bddIthVar(manager, 3);
            DdNode* xi = Cudd_bddIthVar(manager, i + 4);
            
            DdNode* t1 = Cudd_bddAnd(manager, x1, xi);
            Cudd_Ref(t1);
            DdNode* t2 = Cudd_bddXor(manager, t1, x3);
            Cudd_Ref(t2);
            
            DdNode* newResult = Cudd_bddOr(manager, result, t2);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, t1);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("Multiple Window4Conv passes with various BDD sizes") {
        for (int numVars = 8; numVars <= 18; numVars += 2) {
            DdManager* manager = Cudd_Init(numVars, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
            REQUIRE(manager != nullptr);

            DdNode* result = Cudd_ReadOne(manager);
            Cudd_Ref(result);

            for (int i = 0; i < numVars - 3; i++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, (i + 2) % numVars);
                DdNode* xk = Cudd_bddIthVar(manager, (i + 4) % numVars);
                
                DdNode* t1 = Cudd_bddOr(manager, xi, xj);
                Cudd_Ref(t1);
                DdNode* clause = Cudd_bddAnd(manager, t1, xk);
                Cudd_Ref(clause);
                Cudd_RecursiveDeref(manager, t1);

                DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }

            int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(reorderResult == 1);

            Cudd_RecursiveDeref(manager, result);
            Cudd_Quit(manager);
        }
    }
}

// ============================================================================
// Tests targeting BDAC, DCBA, DBCA permutation outcomes
// ============================================================================

TEST_CASE("cuddWindow - Rare permutation specific tests", "[cuddWindow]") {
    SECTION("BDD targeting BDAC permutation (code 19)") {
        DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Try to create structure where B-D pair swap is beneficial
        // but not A-B or C-D swaps
        for (int group = 0; group < 3; group++) {
            int base = group * 4;
            DdNode* xb = Cudd_bddIthVar(manager, base + 1); // B
            DdNode* xd = Cudd_bddIthVar(manager, base + 3); // D
            DdNode* xe = Cudd_bddIthVar(manager, (base + 5) % 12);
            
            DdNode* t1 = Cudd_bddAnd(manager, xb, xd);
            Cudd_Ref(t1);
            DdNode* clause = Cudd_bddXor(manager, t1, xe);
            Cudd_Ref(clause);
            Cudd_RecursiveDeref(manager, t1);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD targeting DCBA permutation (code 22)") {
        DdManager* manager = Cudd_Init(14, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Pattern: strong D-A interactions
        for (int i = 0; i < 11; i++) {
            DdNode* xd = Cudd_bddIthVar(manager, i + 3); // D position
            DdNode* xa = Cudd_bddIthVar(manager, i);      // A position
            DdNode* xf = Cudd_bddIthVar(manager, (i + 7) % 14);
            
            DdNode* t1 = Cudd_bddAnd(manager, xd, xa);
            Cudd_Ref(t1);
            DdNode* clause = Cudd_bddOr(manager, t1, xf);
            Cudd_Ref(clause);
            Cudd_RecursiveDeref(manager, t1);

            DdNode* newResult = Cudd_bddAnd(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD with all pairwise interactions in 4-var window") {
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // All pairwise interactions within 4-var window
        for (int w = 0; w <= 4; w++) {
            for (int i = 0; i < 4; i++) {
                for (int j = i + 1; j < 4; j++) {
                    DdNode* xi = Cudd_bddIthVar(manager, w + i);
                    DdNode* xj = Cudd_bddIthVar(manager, w + j);
                    DdNode* clause = Cudd_bddXor(manager, xi, xj);
                    Cudd_Ref(clause);

                    DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                    Cudd_Ref(newResult);

                    Cudd_RecursiveDeref(manager, clause);
                    Cudd_RecursiveDeref(manager, result);
                    result = newResult;
                }
            }
        }

        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Exhaustive tests to try to trigger BACD and BADC permutation outcomes
// ============================================================================

TEST_CASE("cuddWindow - BACD/BADC triggering tests", "[cuddWindow]") {
    SECTION("Many random-like BDD structures for BACD") {
        // Try many different BDD structures to trigger BACD outcome
        for (int seed = 0; seed < 20; seed++) {
            DdManager* manager = Cudd_Init(12, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
            REQUIRE(manager != nullptr);

            DdNode* result = Cudd_ReadOne(manager);
            Cudd_Ref(result);

            // Create various interaction patterns based on seed
            for (int i = 0; i < 10; i++) {
                int v1 = (i + seed) % 12;
                int v2 = (i * 2 + seed * 3) % 12;
                if (v1 != v2) {
                    DdNode* xi = Cudd_bddIthVar(manager, v1);
                    DdNode* xj = Cudd_bddIthVar(manager, v2);
                    DdNode* clause;
                    
                    if ((i + seed) % 3 == 0) {
                        clause = Cudd_bddAnd(manager, xi, xj);
                    } else if ((i + seed) % 3 == 1) {
                        clause = Cudd_bddOr(manager, xi, xj);
                    } else {
                        clause = Cudd_bddXor(manager, xi, xj);
                    }
                    Cudd_Ref(clause);

                    DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                    Cudd_Ref(newResult);

                    Cudd_RecursiveDeref(manager, clause);
                    Cudd_RecursiveDeref(manager, result);
                    result = newResult;
                }
            }

            int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(reorderResult == 1);

            Cudd_RecursiveDeref(manager, result);
            Cudd_Quit(manager);
        }
    }

    SECTION("Stress convergence with many iterations") {
        DdManager* manager = Cudd_Init(16, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        // Create complex BDD
        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 16; i++) {
            for (int j = i + 1; j < 16 && j < i + 5; j++) {
                DdNode* xi = Cudd_bddIthVar(manager, i);
                DdNode* xj = Cudd_bddIthVar(manager, j);
                DdNode* clause = Cudd_bddXor(manager, xi, xj);
                Cudd_Ref(clause);

                DdNode* newResult = Cudd_bddAnd(manager, result, clause);
                Cudd_Ref(newResult);

                Cudd_RecursiveDeref(manager, clause);
                Cudd_RecursiveDeref(manager, result);
                result = newResult;
            }
        }

        // Many convergence iterations
        for (int pass = 0; pass < 10; pass++) {
            int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(reorderResult == 1);
        }

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("Interleaved Window3Conv and Window4Conv") {
        DdManager* manager = Cudd_Init(14, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        for (int i = 0; i < 12; i++) {
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* xj = Cudd_bddIthVar(manager, (i + 4) % 14);
            DdNode* clause = Cudd_bddAnd(manager, xi, xj);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        // Interleave different methods
        for (int pass = 0; pass < 5; pass++) {
            int r1 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW3_CONV, 0);
            REQUIRE(r1 == 1);
            int r2 = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
            REQUIRE(r2 == 1);
        }

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }

    SECTION("BDD with strong first-pair interaction") {
        // Create BDD where swapping first pair is clearly beneficial
        DdManager* manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);

        DdNode* result = Cudd_ReadOne(manager);
        Cudd_Ref(result);

        // Strong interaction between x1 and all other vars
        for (int i = 2; i < 8; i++) {
            DdNode* x1 = Cudd_bddIthVar(manager, 1);
            DdNode* xi = Cudd_bddIthVar(manager, i);
            DdNode* clause = Cudd_bddAnd(manager, x1, xi);
            Cudd_Ref(clause);

            DdNode* newResult = Cudd_bddOr(manager, result, clause);
            Cudd_Ref(newResult);

            Cudd_RecursiveDeref(manager, clause);
            Cudd_RecursiveDeref(manager, result);
            result = newResult;
        }

        // No interaction with x0
        int reorderResult = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW4_CONV, 0);
        REQUIRE(reorderResult == 1);

        Cudd_RecursiveDeref(manager, result);
        Cudd_Quit(manager);
    }
}
