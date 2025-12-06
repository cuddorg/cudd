#include <catch2/catch_test_macros.hpp>
#include <cmath>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddAddWalsh.c
 * 
 * This file contains comprehensive tests for the Cudd_addWalsh and
 * Cudd_addResidue functions to achieve >90% code coverage.
 */

// ==================== Tests for Cudd_addWalsh ====================

TEST_CASE("Cudd_addWalsh - n=0 base case", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // n=0 should return the constant one
    DdNode *result = Cudd_addWalsh(manager, nullptr, nullptr, 0);
    REQUIRE(result != nullptr);
    REQUIRE(result == Cudd_ReadOne(manager));
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - n=1 simple case", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variable arrays for n=1
    DdNode *x[1], *y[1];
    x[0] = Cudd_addIthVar(manager, 0);
    y[0] = Cudd_addIthVar(manager, 1);
    Cudd_Ref(x[0]);
    Cudd_Ref(y[0]);
    
    // Build Walsh matrix for n=1 (2x2 matrix)
    DdNode *result = Cudd_addWalsh(manager, x, y, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result is not null and has proper structure
    // Walsh(1) is: [1, 1; 1, -1]
    // For n=1, the result should produce entries of 1 and -1
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x[0]);
    Cudd_RecursiveDeref(manager, y[0]);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - n=2 case with loop", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variable arrays for n=2
    DdNode *x[2], *y[2];
    x[0] = Cudd_addIthVar(manager, 0);
    x[1] = Cudd_addIthVar(manager, 1);
    y[0] = Cudd_addIthVar(manager, 2);
    y[1] = Cudd_addIthVar(manager, 3);
    Cudd_Ref(x[0]);
    Cudd_Ref(x[1]);
    Cudd_Ref(y[0]);
    Cudd_Ref(y[1]);
    
    // Build Walsh matrix for n=2 (4x4 matrix)
    DdNode *result = Cudd_addWalsh(manager, x, y, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result is not null and has proper structure
    // Walsh(2) is a 4x4 matrix with entries +1 and -1
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x[0]);
    Cudd_RecursiveDeref(manager, x[1]);
    Cudd_RecursiveDeref(manager, y[0]);
    Cudd_RecursiveDeref(manager, y[1]);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - n=3 larger case", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variable arrays for n=3
    DdNode *x[3], *y[3];
    for (int i = 0; i < 3; i++) {
        x[i] = Cudd_addIthVar(manager, i);
        y[i] = Cudd_addIthVar(manager, i + 3);
        Cudd_Ref(x[i]);
        Cudd_Ref(y[i]);
    }
    
    // Build Walsh matrix for n=3 (8x8 matrix)
    DdNode *result = Cudd_addWalsh(manager, x, y, 3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result is not null
    // This exercises the loop with multiple iterations (i>0 case)
    
    Cudd_RecursiveDeref(manager, result);
    for (int i = 0; i < 3; i++) {
        Cudd_RecursiveDeref(manager, x[i]);
        Cudd_RecursiveDeref(manager, y[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - n=4 exercising all loop iterations", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variable arrays for n=4
    DdNode *x[4], *y[4];
    for (int i = 0; i < 4; i++) {
        x[i] = Cudd_addIthVar(manager, i);
        y[i] = Cudd_addIthVar(manager, i + 4);
        Cudd_Ref(x[i]);
        Cudd_Ref(y[i]);
    }
    
    // Build Walsh matrix for n=4 (16x16 matrix)
    DdNode *result = Cudd_addWalsh(manager, x, y, 4);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // This exercises the full loop with multiple iterations
    // Covers the i>0 branch fully
    
    Cudd_RecursiveDeref(manager, result);
    for (int i = 0; i < 4; i++) {
        Cudd_RecursiveDeref(manager, x[i]);
        Cudd_RecursiveDeref(manager, y[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - Verify Walsh properties", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variable arrays for n=2
    DdNode *x[2], *y[2];
    x[0] = Cudd_addIthVar(manager, 0);
    x[1] = Cudd_addIthVar(manager, 1);
    y[0] = Cudd_addIthVar(manager, 2);
    y[1] = Cudd_addIthVar(manager, 3);
    Cudd_Ref(x[0]);
    Cudd_Ref(x[1]);
    Cudd_Ref(y[0]);
    Cudd_Ref(y[1]);
    
    DdNode *result = Cudd_addWalsh(manager, x, y, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the ADD has the expected number of terminal nodes
    // Walsh matrix has only +1 and -1 entries
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 2); // Only +1 and -1 as terminals
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x[0]);
    Cudd_RecursiveDeref(manager, x[1]);
    Cudd_RecursiveDeref(manager, y[0]);
    Cudd_RecursiveDeref(manager, y[1]);
    
    Cudd_Quit(manager);
}

// ==================== Tests for Cudd_addResidue ====================

TEST_CASE("Cudd_addResidue - Invalid parameters", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Both n < 1 AND m < 2 should return NULL
    DdNode *result = Cudd_addResidue(manager, 0, 1, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result == nullptr);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - LSB on top, unsigned (default)", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 3-bit number mod 3, LSB on top (default)
    DdNode *result = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result represents residues mod 3
    // Values 0..7 mod 3 should give residues 0, 1, 2, 0, 1, 2, 0, 1
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 3); // Residues 0, 1, 2
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - MSB on top", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 3-bit number mod 3, MSB on top
    DdNode *result = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_MSB, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result represents residues mod 3
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 3); // Residues 0, 1, 2
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Two's complement", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 3-bit two's complement number mod 3
    DdNode *result = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_TC, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves <= 3); // Residues 0, 1, 2
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - MSB and two's complement combined", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 3-bit two's complement with MSB on top
    DdNode *result = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_MSB | CUDD_RESIDUE_TC, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves <= 3); // Residues 0, 1, 2
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Different moduli", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Test with modulus 2 (binary)
    DdNode *result2 = Cudd_addResidue(manager, 4, 2, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result2 != nullptr);
    Cudd_Ref(result2);
    int leaves2 = Cudd_CountLeaves(result2);
    REQUIRE(leaves2 == 2); // Residues 0, 1
    Cudd_RecursiveDeref(manager, result2);
    
    // Test with modulus 5
    DdNode *result5 = Cudd_addResidue(manager, 4, 5, CUDD_RESIDUE_DEFAULT, 4);
    REQUIRE(result5 != nullptr);
    Cudd_Ref(result5);
    int leaves5 = Cudd_CountLeaves(result5);
    REQUIRE(leaves5 == 5); // Residues 0, 1, 2, 3, 4
    Cudd_RecursiveDeref(manager, result5);
    
    // Test with modulus 7
    DdNode *result7 = Cudd_addResidue(manager, 4, 7, CUDD_RESIDUE_DEFAULT, 8);
    REQUIRE(result7 != nullptr);
    Cudd_Ref(result7);
    int leaves7 = Cudd_CountLeaves(result7);
    REQUIRE(leaves7 == 7); // Residues 0-6
    Cudd_RecursiveDeref(manager, result7);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Single bit n=1", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Single bit mod 2
    DdNode *result = Cudd_addResidue(manager, 1, 2, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // For n=1, k will only be 0, so thisOne=0, previous=1
    // This tests the base case of the loop
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - n=1 with two's complement", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Single bit two's complement mod 2
    // When k == n-1 and tc is true, the adjustment happens
    DdNode *result = Cudd_addResidue(manager, 1, 2, CUDD_RESIDUE_TC, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Multiple iterations with even/odd k", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 4 bits mod 3 - this will have k=0,1,2,3
    // which gives thisOne=0,1,0,1 and previous=1,0,1,0
    DdNode *result = Cudd_addResidue(manager, 4, 3, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Larger modulus", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 5 bits mod 10
    DdNode *result = Cudd_addResidue(manager, 5, 10, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 10); // Residues 0-9
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Different top variable", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Use a non-zero top variable index
    DdNode *result = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_DEFAULT, 10);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    
    // Also test with MSB option and non-zero top
    DdNode *result2 = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_MSB, 20);
    REQUIRE(result2 != nullptr);
    Cudd_Ref(result2);
    
    Cudd_RecursiveDeref(manager, result2);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Verify residue correctness for mod 2", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 3-bit number mod 2 should just be the LSB
    DdNode *result = Cudd_addResidue(manager, 3, 2, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // The ADD should have exactly 2 terminal nodes (0 and 1)
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 2);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Verify residue correctness for mod 4", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 4-bit number mod 4 should just be the lower 2 bits
    DdNode *result = Cudd_addResidue(manager, 4, 4, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // The ADD should have exactly 4 terminal nodes (0, 1, 2, 3)
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 4);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Large n value", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // 8-bit number mod 5
    DdNode *result = Cudd_addResidue(manager, 8, 5, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 5); // Residues 0-4
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Two's complement adjustments", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Test various combinations that exercise the tc adjustment at k == n-1
    for (int n = 2; n <= 4; n++) {
        for (int m = 3; m <= 5; m++) {
            DdNode *result = Cudd_addResidue(manager, n, m, CUDD_RESIDUE_TC, 0);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            
            int leaves = Cudd_CountLeaves(result);
            REQUIRE(leaves <= m);
            
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Exercise final array cleanup", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Larger m to test the final loop that dereferences array[(n-1)&1][i] for i=1..m-1
    DdNode *result = Cudd_addResidue(manager, 3, 8, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 8); // Residues 0-7
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - n=2 even case", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // n=2, (n-1)&1 = 1, so we use array[1]
    DdNode *result = Cudd_addResidue(manager, 2, 3, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - n=3 odd case", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // n=3, (n-1)&1 = 0, so we use array[0]
    DdNode *result = Cudd_addResidue(manager, 3, 3, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

// ==================== Integration tests ====================

TEST_CASE("Cudd_addWalsh and Cudd_addResidue - Combined usage", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create Walsh matrix
    DdNode *x[2], *y[2];
    x[0] = Cudd_addIthVar(manager, 0);
    x[1] = Cudd_addIthVar(manager, 1);
    y[0] = Cudd_addIthVar(manager, 2);
    y[1] = Cudd_addIthVar(manager, 3);
    Cudd_Ref(x[0]);
    Cudd_Ref(x[1]);
    Cudd_Ref(y[0]);
    Cudd_Ref(y[1]);
    
    DdNode *walsh = Cudd_addWalsh(manager, x, y, 2);
    REQUIRE(walsh != nullptr);
    Cudd_Ref(walsh);
    
    // Create residue
    DdNode *residue = Cudd_addResidue(manager, 4, 3, CUDD_RESIDUE_DEFAULT, 4);
    REQUIRE(residue != nullptr);
    Cudd_Ref(residue);
    
    // Both should exist independently
    REQUIRE(Cudd_DagSize(walsh) > 0);
    REQUIRE(Cudd_DagSize(residue) > 0);
    
    Cudd_RecursiveDeref(manager, walsh);
    Cudd_RecursiveDeref(manager, residue);
    Cudd_RecursiveDeref(manager, x[0]);
    Cudd_RecursiveDeref(manager, x[1]);
    Cudd_RecursiveDeref(manager, y[0]);
    Cudd_RecursiveDeref(manager, y[1]);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - Check memory is properly managed", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Run multiple times to ensure no memory leaks
    for (int iter = 0; iter < 10; iter++) {
        DdNode *x[2], *y[2];
        x[0] = Cudd_addIthVar(manager, 0);
        x[1] = Cudd_addIthVar(manager, 1);
        y[0] = Cudd_addIthVar(manager, 2);
        y[1] = Cudd_addIthVar(manager, 3);
        Cudd_Ref(x[0]);
        Cudd_Ref(x[1]);
        Cudd_Ref(y[0]);
        Cudd_Ref(y[1]);
        
        DdNode *result = Cudd_addWalsh(manager, x, y, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, x[0]);
        Cudd_RecursiveDeref(manager, x[1]);
        Cudd_RecursiveDeref(manager, y[0]);
        Cudd_RecursiveDeref(manager, y[1]);
    }
    
    // Check for memory cleanup
    REQUIRE(Cudd_CheckZeroRef(manager) == 0);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Check memory is properly managed", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Run multiple times to ensure no memory leaks
    for (int iter = 0; iter < 10; iter++) {
        DdNode *result = Cudd_addResidue(manager, 4, 5, CUDD_RESIDUE_DEFAULT, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    // Check for memory cleanup
    REQUIRE(Cudd_CheckZeroRef(manager) == 0);
    
    Cudd_Quit(manager);
}

// ==================== Tests attempting to trigger error paths ====================

// Global variable to track if timeout handler was called
static int walshTimeoutCalled = 0;

static void walshTimeoutHandler(DdManager *dd, void *arg) {
    (void)dd;
    (void)arg;
    walshTimeoutCalled = 1;
}

TEST_CASE("Cudd_addWalsh - With timeout handler registered", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Register a timeout handler
    Cudd_RegisterTimeoutHandler(manager, walshTimeoutHandler, nullptr);
    
    // Normal operation should not trigger timeout
    walshTimeoutCalled = 0;
    
    DdNode *x[2], *y[2];
    x[0] = Cudd_addIthVar(manager, 0);
    x[1] = Cudd_addIthVar(manager, 1);
    y[0] = Cudd_addIthVar(manager, 2);
    y[1] = Cudd_addIthVar(manager, 3);
    Cudd_Ref(x[0]);
    Cudd_Ref(x[1]);
    Cudd_Ref(y[0]);
    Cudd_Ref(y[1]);
    
    DdNode *result = Cudd_addWalsh(manager, x, y, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Cleanup
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x[0]);
    Cudd_RecursiveDeref(manager, x[1]);
    Cudd_RecursiveDeref(manager, y[0]);
    Cudd_RecursiveDeref(manager, y[1]);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - With memory limit", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Set a reasonable memory limit and try to compute
    // This tests that the function works under memory pressure
    Cudd_SetMaxMemory(manager, 10 * 1024 * 1024);  // 10 MB limit
    
    DdNode *result = Cudd_addResidue(manager, 4, 7, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 7);
    
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addWalsh - Large n for stress test", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variable arrays for n=5 (32x32 matrix)
    const int n = 5;
    DdNode *x[5], *y[5];
    for (int i = 0; i < n; i++) {
        x[i] = Cudd_addIthVar(manager, i);
        y[i] = Cudd_addIthVar(manager, i + n);
        Cudd_Ref(x[i]);
        Cudd_Ref(y[i]);
    }
    
    DdNode *result = Cudd_addWalsh(manager, x, y, n);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Walsh matrix should still only have 2 terminal values
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 2);
    
    Cudd_RecursiveDeref(manager, result);
    for (int i = 0; i < n; i++) {
        Cudd_RecursiveDeref(manager, x[i]);
        Cudd_RecursiveDeref(manager, y[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Large values for stress test", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Larger n and m values to exercise more iterations
    // 8-bit number mod 13
    DdNode *result = Cudd_addResidue(manager, 8, 13, CUDD_RESIDUE_DEFAULT, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    int leaves = Cudd_CountLeaves(result);
    REQUIRE(leaves == 13);
    
    Cudd_RecursiveDeref(manager, result);
    
    // Also test with two's complement and MSB
    DdNode *result2 = Cudd_addResidue(manager, 8, 13, CUDD_RESIDUE_MSB | CUDD_RESIDUE_TC, 8);
    REQUIRE(result2 != nullptr);
    Cudd_Ref(result2);
    Cudd_RecursiveDeref(manager, result2);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - All option combinations", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Test all four option combinations
    int options[] = {
        CUDD_RESIDUE_DEFAULT,           // 0: LSB, unsigned
        CUDD_RESIDUE_MSB,               // 1: MSB, unsigned
        CUDD_RESIDUE_TC,                // 2: LSB, two's complement
        CUDD_RESIDUE_MSB | CUDD_RESIDUE_TC  // 3: MSB, two's complement
    };
    
    for (int i = 0; i < 4; i++) {
        DdNode *result = Cudd_addResidue(manager, 4, 5, options[i], 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        int leaves = Cudd_CountLeaves(result);
        REQUIRE(leaves == 5);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addResidue - Prime modulus", "[cuddAddWalsh]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Test with prime moduli (common case in practice)
    int primes[] = {2, 3, 5, 7, 11, 13};
    
    for (int p : primes) {
        DdNode *result = Cudd_addResidue(manager, 6, p, CUDD_RESIDUE_DEFAULT, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        int leaves = Cudd_CountLeaves(result);
        REQUIRE(leaves == p);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_Quit(manager);
}
