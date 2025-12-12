#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"

/**
 * @brief Comprehensive test file for cuddSplit.c
 * 
 * This file contains comprehensive tests for the Cudd_SplitSet function
 * and related internal functions to achieve >90% code coverage.
 */

// ============================================================================
// Trivial Cases Tests
// ============================================================================

TEST_CASE("Cudd_SplitSet - m=0 returns zero", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    // Create a variable
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    // Create xVars array
    DdNode *xVars[1] = {x};
    
    // m=0 should always return zero
    DdNode *result = Cudd_SplitSet(manager, x, xVars, 1, 0.0);
    REQUIRE(result == zero);
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - S=zero returns NULL", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    // Create a variable
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    // Create xVars array
    DdNode *xVars[1] = {x};
    
    // S=zero should return NULL
    DdNode *result = Cudd_SplitSet(manager, zero, xVars, 1, 1.0);
    REQUIRE(result == nullptr);
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - m > max returns NULL", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create a variable
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    // Create xVars array
    DdNode *xVars[1] = {x};
    
    // n=1 means max=2, so m=3 > max should return NULL
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 1, 3.0);
    REQUIRE(result == nullptr);
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

// ============================================================================
// S=one Cases (selectMintermsFromUniverse)
// ============================================================================

TEST_CASE("Cudd_SplitSet - S=one m=max returns S", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // n=2 means max=4, m=4 equals max
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 2, 4.0);
    REQUIRE(result == one);
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - S=one m<max selectMintermsFromUniverse", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // n=2 means max=4, m=2 < max triggers selectMintermsFromUniverse
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 2, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result has exactly 2 minterms
    double mintermCount = Cudd_CountMinterm(manager, result, 2);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - S=one m=1 selectMintermsFromUniverse", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // n=3 means max=8, m=1 extracts single minterm
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result has exactly 1 minterm
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// ============================================================================
// m = num (returns S when requested minterms equals actual minterms)
// ============================================================================

TEST_CASE("Cudd_SplitSet - m equals num returns S", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    // Create S = x AND y (has 1 minterm)
    DdNode *S = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // n=2 means max=4, S has 1 minterm
    // Requesting m=1 (exact match) should return S
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 2, 1.0);
    REQUIRE(result == S);
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

// ============================================================================
// cuddSplitSetRecur - Various Code Paths
// ============================================================================

TEST_CASE("Cudd_SplitSet - Both children constants", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    // S = x (children are 1 and 0, both constants)
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // n=2 means max=4, x has 2 minterms
    // Requesting m=1 triggers constant children path
    DdNode *result = Cudd_SplitSet(manager, x, xVars, 2, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify the result has exactly 1 minterm
    double mintermCount = Cudd_CountMinterm(manager, result, 2);
    REQUIRE(mintermCount == 1.0);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, x) == 1);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - numT equals n (perfect match THEN)", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    // Create S = x AND (y OR z)
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *S = Cudd_bddAnd(manager, x, yz);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // S has 3 minterms in x's THEN branch
    // Request m that equals numT to trigger perfect match path
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 3.0);
    REQUIRE(result == S);
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - numE equals n (perfect match ELSE)", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables - control variable order
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S = !x AND (y OR z) - has minterms in ELSE branch
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *S = Cudd_bddAnd(manager, Cudd_Not(x), yz);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // S has 3 minterms total
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 3.0);
    REQUIRE(result == S);
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - numT < n (extract from ELSE)", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S = x OR y (has minterms in both branches)
    DdNode *S = Cudd_bddOr(manager, x, y);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // max = 8, S has 6 minterms
    // Request m=5 which is more than numT (4) but less than total (6)
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 5.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    // Verify minterm count
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 5.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - numE < n (extract from THEN)", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    
    // Create S = !x OR y
    // At variable x: T(x)=1, E(x)=y
    // numT = 2, numE = 1 in 2-variable space
    DdNode *S = Cudd_bddOr(manager, Cudd_Not(x), y);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // max = 4, S has 3 minterms
    // Request m=2 to trigger numE < n path
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 2, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    // Verify minterm count
    double mintermCount = Cudd_CountMinterm(manager, result, 2);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Nv constant, Nnv not constant", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables in order
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S where THEN child is constant, ELSE is not
    // S = x OR (!x AND (y OR z))
    // For x: T = 1 (constant), E = y OR z (not constant)
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *S = Cudd_bddOr(manager, x, Cudd_bddAnd(manager, Cudd_Not(x), yz));
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // Request m where n < numT to trigger constant branch path
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    // Verify minterm count
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Nv not constant, Nnv constant", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables in order
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S where THEN child is not constant, ELSE is constant
    // S = x AND (y OR z)
    // For x: T = y OR z (not constant), E = 0 (constant)
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *S = Cudd_bddAnd(manager, x, yz);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // S has 3 minterms, request 1 to trigger code path
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    // Verify minterm count
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Both children non-constant numT < numE", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables in order
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S where both children are non-constant and numT < numE
    // S = (x AND y) OR (!x AND (y OR z))
    DdNode *xy = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(xy);
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *notx_yz = Cudd_bddAnd(manager, Cudd_Not(x), yz);
    Cudd_Ref(notx_yz);
    DdNode *S = Cudd_bddOr(manager, xy, notx_yz);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // S has several minterms, request small number to take branch with fewer minterms
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    // Verify minterm count
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, notx_yz);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_RecursiveDeref(manager, xy);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Both children non-constant numT >= numE", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables in order
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S where both children are non-constant and numT >= numE
    // S = (x AND (y OR z)) OR (!x AND y)
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *x_yz = Cudd_bddAnd(manager, x, yz);
    Cudd_Ref(x_yz);
    DdNode *notx_y = Cudd_bddAnd(manager, Cudd_Not(x), y);
    Cudd_Ref(notx_y);
    DdNode *S = Cudd_bddOr(manager, x_yz, notx_y);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // S has several minterms, request 1 to take branch
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    // Verify minterm count
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, notx_y);
    Cudd_RecursiveDeref(manager, x_yz);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

// ============================================================================
// mintermsFromUniverse Coverage - Various n values
// ============================================================================

TEST_CASE("Cudd_SplitSet - mintermsFromUniverse n=max", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    // Create xVars array
    DdNode *xVars[1] = {x};
    
    // n=1 means max=2, request m=2 (all minterms)
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 1, 2.0);
    REQUIRE(result == one);
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - mintermsFromUniverse n=max2", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // n=2 means max=4, max2=2, request m=2 to return single variable
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 2, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 2);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - mintermsFromUniverse n > max2", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // n=2 means max=4, max2=2, request m=3 > max2
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 2, 3.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 2);
    REQUIRE(mintermCount == 3.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - mintermsFromUniverse n < max2", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    // n=3 means max=8, max2=4, request m=1 < max2
    DdNode *result = Cudd_SplitSet(manager, one, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// ============================================================================
// Complemented Node Coverage
// ============================================================================

TEST_CASE("Cudd_SplitSet - Complemented BDD input", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    
    // Create S = NOT(x AND y) = !x OR !y
    DdNode *xy = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(xy);
    DdNode *S = Cudd_Not(xy);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[2] = {x, y};
    
    // S has 3 minterms
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 2, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 2);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, xy);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Complex complemented BDD", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create a complex complemented BDD
    // S = NOT((x AND y) OR z)
    DdNode *xy = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(xy);
    DdNode *xyz = Cudd_bddOr(manager, xy, z);
    Cudd_Ref(xyz);
    DdNode *S = Cudd_Not(xyz);
    Cudd_Ref(S);
    
    // Create xVars array
    DdNode *xVars[3] = {x, y, z};
    
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Verify result is subset of S
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, xyz);
    Cudd_RecursiveDeref(manager, xy);
    Cudd_Quit(manager);
}

// ============================================================================
// Deep BDD Coverage - Exercise recursion
// ============================================================================

TEST_CASE("Cudd_SplitSet - Deep BDD chain", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    const int numVars = 8;
    DdNode *vars[numVars];
    
    // Create variables
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create S = x0 AND x1 AND x2 AND ... AND x7 (deep AND chain)
    DdNode *S = Cudd_ReadOne(manager);
    Cudd_Ref(S);
    for (int i = 0; i < numVars; i++) {
        DdNode *temp = Cudd_bddAnd(manager, S, vars[i]);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(manager, S);
        S = temp;
    }
    
    // S has exactly 1 minterm
    DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, 1.0);
    REQUIRE(result == S);
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Wide OR chain", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    const int numVars = 6;
    DdNode *vars[numVars];
    
    // Create variables
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create S = x0 OR x1 OR x2 OR ... OR x5 (wide OR chain)
    DdNode *S = Cudd_ReadLogicZero(manager);
    Cudd_Ref(S);
    for (int i = 0; i < numVars; i++) {
        DdNode *temp = Cudd_bddOr(manager, S, vars[i]);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(manager, S);
        S = temp;
    }
    
    // max = 2^6 = 64, S has 64-1=63 minterms (all except x0=x1=...=x5=0)
    // Request various values
    DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, 32.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, numVars);
    REQUIRE(mintermCount == 32.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - XOR chain (complex structure)", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    const int numVars = 4;
    DdNode *vars[numVars];
    
    // Create variables
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create S = x0 XOR x1 XOR x2 XOR x3 (complex parity function)
    DdNode *S = vars[0];
    Cudd_Ref(S);
    for (int i = 1; i < numVars; i++) {
        DdNode *temp = Cudd_bddXor(manager, S, vars[i]);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(manager, S);
        S = temp;
    }
    
    // XOR of 4 variables has exactly 8 minterms (half of 16)
    DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, 4.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, numVars);
    REQUIRE(mintermCount == 4.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_CASE("Cudd_SplitSet - Single variable BDD", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    DdNode *xVars[1] = {x};
    
    // x has 1 minterm in 1-variable space
    // Request m=1 (exact match)
    DdNode *result = Cudd_SplitSet(manager, x, xVars, 1, 1.0);
    REQUIRE(result == x);
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Many variables small request", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    const int numVars = 10;
    DdNode *vars[numVars];
    
    // Create variables
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create S = first variable (many unused variables)
    DdNode *S = vars[0];
    Cudd_Ref(S);
    
    // Request 1 minterm from large space
    DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, numVars);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - ITE structured BDD", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S using ITE: if x then y else z
    DdNode *S = Cudd_bddIte(manager, x, y, z);
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // S has various minterms depending on structure
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

// ============================================================================
// bddAnnotateMintermCount Coverage - Through complex structures
// ============================================================================

TEST_CASE("Cudd_SplitSet - Shared subgraph (annotation table)", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create shared subgraph
    DdNode *shared = Cudd_bddAnd(manager, y, z);
    Cudd_Ref(shared);
    
    // S = (x AND shared) OR (!x AND shared) = shared
    // But build it explicitly to create shared structure
    DdNode *t1 = Cudd_bddAnd(manager, x, shared);
    Cudd_Ref(t1);
    DdNode *t2 = Cudd_bddAnd(manager, Cudd_Not(x), shared);
    Cudd_Ref(t2);
    DdNode *S = Cudd_bddOr(manager, t1, t2);
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // This exercises the memoization in bddAnnotateMintermCount
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, t2);
    Cudd_RecursiveDeref(manager, t1);
    Cudd_RecursiveDeref(manager, shared);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Multiple requests same BDD", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    DdNode *S = Cudd_bddOr(manager, x, Cudd_bddOr(manager, y, z));
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // Multiple requests with different m values
    for (int m = 1; m <= 7; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 3);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional Edge Cases for Full Coverage
// ============================================================================

TEST_CASE("Cudd_SplitSet - Constant p triggers selectMintermsFromUniverse in recur", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    DdNode *w = Cudd_bddIthVar(manager, 3);
    
    // Create S that will reach a constant during recursion
    // S = x (this has constant children 1 and 0)
    DdNode *S = x;
    Cudd_Ref(S);
    
    DdNode *xVars[4] = {x, y, z, w};
    
    // Request minterms to exercise various paths
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 4, 4.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 4);
    REQUIRE(mintermCount == 4.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - n=0 edge case", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // n=0 means max=1
    // S=one with m=1 should return one
    DdNode *result = Cudd_SplitSet(manager, one, nullptr, 0, 1.0);
    REQUIRE(result == one);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Large BDD stress test", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    const int numVars = 12;
    DdNode *vars[numVars];
    
    // Create variables
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create complex BDD: (x0 AND x1) OR (x2 AND x3) OR ... OR (x10 AND x11)
    DdNode *S = Cudd_ReadLogicZero(manager);
    Cudd_Ref(S);
    for (int i = 0; i < numVars; i += 2) {
        DdNode *pair = Cudd_bddAnd(manager, vars[i], vars[i+1]);
        Cudd_Ref(pair);
        DdNode *temp = Cudd_bddOr(manager, S, pair);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(manager, pair);
        Cudd_RecursiveDeref(manager, S);
        S = temp;
    }
    
    // Request various sizes
    for (int m = 1; m <= 6; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, numVars);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional Coverage Tests - Targeting Specific Code Paths
// ============================================================================

TEST_CASE("Cudd_SplitSet - Nv non-constant Nnv constant with n < both", "[cuddSplit]") {
    // This test targets lines 399-416:
    // !Cudd_IsConstantInt(Nv) && Cudd_IsConstantInt(Nnv)
    // where n < numT and n < numE
    
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables in order
    DdNode *x = Cudd_bddIthVar(manager, 0);  // top variable
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S = !x OR (y AND z)
    // At x: T = y AND z (not constant), E = 1 (constant)
    // In 3-variable space: numT = 1, numE = 4
    // Requesting n=1 would give numE (4) > n, so we need to be careful
    DdNode *yz = Cudd_bddAnd(manager, y, z);
    Cudd_Ref(yz);
    DdNode *S = Cudd_bddOr(manager, Cudd_Not(x), yz);
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // S has 5 minterms: 4 in ELSE (when x=0) and 1 in THEN (x=1, y=1, z=1)
    // Request 1 minterm - this should trigger the path for extracting from constant branch
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 1.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - ELSE branch constant one extraction", "[cuddSplit]") {
    // Target the path where ELSE is constant 1 and we extract from it
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    DdNode *w = Cudd_bddIthVar(manager, 3);
    
    // Create S = !x OR (x AND y AND z)
    // At x: T = y AND z, E = 1
    // numT < numE since T has only 1 minterm and E has all the rest
    DdNode *xyz = Cudd_bddAnd(manager, x, Cudd_bddAnd(manager, y, z));
    Cudd_Ref(xyz);
    DdNode *S = Cudd_bddOr(manager, Cudd_Not(x), xyz);
    Cudd_Ref(S);
    
    DdNode *xVars[4] = {x, y, z, w};
    
    // S has 9 minterms (8 when x=0, 1 when x=1,y=1,z=1)
    // Request 2 minterms to trigger various paths
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 4, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 4);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, xyz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - numT zero case", "[cuddSplit]") {
    // Target line 292: numT = 0 when Nv = 0
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    
    // Create S = !x AND y (THEN = 0 for x, ELSE = y)
    // At x: T = 0 (constant 0), E = y
    // This gives numT = 0
    DdNode *S = Cudd_bddAnd(manager, Cudd_Not(x), y);
    Cudd_Ref(S);
    
    DdNode *xVars[2] = {x, y};
    
    // S has 1 minterm (x=0, y=1)
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 2, 1.0);
    REQUIRE(result == S);
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Deep recursion with constant at bottom", "[cuddSplit]") {
    // Target lines 248-249: when p becomes constant during recursion
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create a BDD that will have constant at deep level
    // S = x OR y OR z (OR chain)
    DdNode *S = Cudd_bddOr(manager, x, Cudd_bddOr(manager, y, z));
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // Request multiple minterms to exercise various paths
    for (int m = 1; m <= 7; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 3);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - mintermsFromUniverse full coverage", "[cuddSplit]") {
    // Exercise all branches in mintermsFromUniverse
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create many variables to exercise recursive paths
    const int numVars = 5;
    DdNode *vars[numVars];
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // max = 32, test various m values
    // m = 0 -> return zero
    // m = 16 (max2) -> return single variable
    // m = 20 (> max2) -> triggers n > max2 path
    // m = 8 (< max2) -> triggers n < max2 path
    // m = 32 (max) -> return one
    
    double testValues[] = {1.0, 8.0, 16.0, 20.0, 24.0, 31.0};
    for (int i = 0; i < 6; i++) {
        DdNode *result = Cudd_SplitSet(manager, one, vars, numVars, testValues[i]);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        double mintermCount = Cudd_CountMinterm(manager, result, numVars);
        REQUIRE(mintermCount == testValues[i]);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - selectMintermsFromUniverse with few vars seen", "[cuddSplit]") {
    // Test selectMintermsFromUniverse when few variables have been seen
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create many variables
    const int numVars = 6;
    DdNode *vars[numVars];
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create a simple BDD that only touches first two variables
    // S = x0 AND x1
    DdNode *S = Cudd_bddAnd(manager, vars[0], vars[1]);
    Cudd_Ref(S);
    
    // x0 AND x1 leaves x2-x5 as "unseen" variables
    // S has 16 minterms in 6-variable space (2^4 for the 4 unseen variables)
    
    DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, 8.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, numVars);
    REQUIRE(mintermCount == 8.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Extreme cases for recursion depth", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create many variables
    const int numVars = 10;
    DdNode *vars[numVars];
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create a BDD that exercises deep recursion paths
    // S = (x0 AND x1 AND x2) OR (x3 AND x4 AND x5) OR (x6 AND x7 AND x8 AND x9)
    DdNode *term1 = Cudd_bddAnd(manager, vars[0], Cudd_bddAnd(manager, vars[1], vars[2]));
    Cudd_Ref(term1);
    DdNode *term2 = Cudd_bddAnd(manager, vars[3], Cudd_bddAnd(manager, vars[4], vars[5]));
    Cudd_Ref(term2);
    DdNode *term3 = Cudd_bddAnd(manager, vars[6], 
                    Cudd_bddAnd(manager, vars[7], 
                    Cudd_bddAnd(manager, vars[8], vars[9])));
    Cudd_Ref(term3);
    
    DdNode *S = Cudd_bddOr(manager, term1, Cudd_bddOr(manager, term2, term3));
    Cudd_Ref(S);
    
    // Test various extraction sizes
    for (int m = 1; m <= 3; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, numVars);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, term3);
    Cudd_RecursiveDeref(manager, term2);
    Cudd_RecursiveDeref(manager, term1);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Mixed constant and non-constant children", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    DdNode *w = Cudd_bddIthVar(manager, 3);
    
    // Create BDD where different levels have different constant/non-constant mix
    // S = x AND (y OR z OR w)
    // At x: T = (y OR z OR w), E = 0
    DdNode *yzw = Cudd_bddOr(manager, y, Cudd_bddOr(manager, z, w));
    Cudd_Ref(yzw);
    DdNode *S = Cudd_bddAnd(manager, x, yzw);
    Cudd_Ref(S);
    
    DdNode *xVars[4] = {x, y, z, w};
    
    // S has 7 minterms (2^3 - 1 when x=1)
    for (int m = 1; m <= 7; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, xVars, 4, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 4);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yzw);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Perfect match on numE at deeper level", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create BDD: (x AND y) OR (!x AND z)
    // This has minterms split between branches
    DdNode *xy = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(xy);
    DdNode *notxz = Cudd_bddAnd(manager, Cudd_Not(x), z);
    Cudd_Ref(notxz);
    DdNode *S = Cudd_bddOr(manager, xy, notxz);
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // S has 4 minterms: 2 in THEN (x=1,y=1), 2 in ELSE (x=0,z=1)
    // Request 2 minterms to trigger numE == n path
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, notxz);
    Cudd_RecursiveDeref(manager, xy);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - bddAnnotateMintermCount complemented nodes", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create complemented BDD to exercise annotation paths
    // S = !((x AND y) OR z) = (!x OR !y) AND !z
    DdNode *xy = Cudd_bddAnd(manager, x, y);
    Cudd_Ref(xy);
    DdNode *xyor_z = Cudd_bddOr(manager, xy, z);
    Cudd_Ref(xyor_z);
    DdNode *S = Cudd_Not(xyor_z);
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // S has 3 minterms (complement of 5)
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, xyor_z);
    Cudd_RecursiveDeref(manager, xy);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional Targeted Coverage Tests
// ============================================================================

TEST_CASE("Cudd_SplitSet - Target Nnv constant branch (lines 399-416)", "[cuddSplit]") {
    // To hit lines 399-416, we need:
    // 1. Nv is NOT constant
    // 2. Nnv IS constant (must be 1 for numE > 0)
    // 3. n < numT AND n < numE
    // This happens in the "None of the above cases" section
    
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create variables in specific order
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    DdNode *w = Cudd_bddIthVar(manager, 3);
    
    // Create S = (x AND (y OR z)) OR !x
    // At x: T = y OR z (not constant), E = 1 (constant)
    // In 4-variable space:
    // - numT = 6 (x=1, y OR z has 3, times 2 for w) 
    // - numE = 8 (x=0, all combinations of y,z,w)
    // If we request n=1, we have n < numT (6) and n < numE (8)
    // Both conditions met, Nv is not constant, Nnv is constant 1
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *x_yz = Cudd_bddAnd(manager, x, yz);
    Cudd_Ref(x_yz);
    DdNode *S = Cudd_bddOr(manager, x_yz, Cudd_Not(x));
    Cudd_Ref(S);
    
    DdNode *xVars[4] = {x, y, z, w};
    
    // Request small number of minterms
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 4, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 4);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, x_yz);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Target numT=0 case (line 292)", "[cuddSplit]") {
    // Line 292 is hit when Nv is constant 0
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    
    // Create S = !x AND (y OR z)
    // At x: T = 0 (constant), E = y OR z
    // This will give numT = 0
    DdNode *yz = Cudd_bddOr(manager, y, z);
    Cudd_Ref(yz);
    DdNode *S = Cudd_bddAnd(manager, Cudd_Not(x), yz);
    Cudd_Ref(S);
    
    DdNode *xVars[3] = {x, y, z};
    
    // S has 3 minterms (x=0, y OR z)
    DdNode *result = Cudd_SplitSet(manager, S, xVars, 3, 2.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
    
    double mintermCount = Cudd_CountMinterm(manager, result, 3);
    REQUIRE(mintermCount == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yz);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Deeper recursion for constant p path", "[cuddSplit]") {
    // Target lines 248-249: when p becomes constant during recursion
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    DdNode *w = Cudd_bddIthVar(manager, 3);
    
    // Create a BDD that will have constant 1 at a deep level
    // S = x OR (y AND (z OR w))
    DdNode *zw = Cudd_bddOr(manager, z, w);
    Cudd_Ref(zw);
    DdNode *yzw = Cudd_bddAnd(manager, y, zw);
    Cudd_Ref(yzw);
    DdNode *S = Cudd_bddOr(manager, x, yzw);
    Cudd_Ref(S);
    
    DdNode *xVars[4] = {x, y, z, w};
    
    // Exercise various extraction paths
    for (int m = 1; m <= 11; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, xVars, 4, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 4);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, yzw);
    Cudd_RecursiveDeref(manager, zw);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Exercise various mintermsFromUniverse paths", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Use varying numbers of variables to exercise different recursive depths
    for (int numVars = 1; numVars <= 6; numVars++) {
        DdNode *vars[6];
        for (int i = 0; i < numVars; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        double max = static_cast<double>(1 << numVars);
        
        // Test various intermediate values
        for (double m = 1.0; m < max; m = m * 2) {
            DdNode *result = Cudd_SplitSet(manager, one, vars, numVars, m);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            
            double mintermCount = Cudd_CountMinterm(manager, result, numVars);
            REQUIRE(mintermCount == m);
            
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Complex BDD triggering multiple paths", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create 5 variables
    DdNode *vars[5];
    for (int i = 0; i < 5; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create a complex BDD: (x0 AND x1) OR (!x0 AND x2 AND (x3 OR x4))
    // This has various constant/non-constant children at different levels
    DdNode *x01 = Cudd_bddAnd(manager, vars[0], vars[1]);
    Cudd_Ref(x01);
    DdNode *x34 = Cudd_bddOr(manager, vars[3], vars[4]);
    Cudd_Ref(x34);
    DdNode *x234 = Cudd_bddAnd(manager, vars[2], x34);
    Cudd_Ref(x234);
    DdNode *notx0_x234 = Cudd_bddAnd(manager, Cudd_Not(vars[0]), x234);
    Cudd_Ref(notx0_x234);
    DdNode *S = Cudd_bddOr(manager, x01, notx0_x234);
    Cudd_Ref(S);
    
    // Test extraction of various sizes
    for (int m = 1; m <= 10; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, vars, 5, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 5);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, notx0_x234);
    Cudd_RecursiveDeref(manager, x234);
    Cudd_RecursiveDeref(manager, x34);
    Cudd_RecursiveDeref(manager, x01);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - OR of single variables with extraction", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *y = Cudd_bddIthVar(manager, 1);
    DdNode *z = Cudd_bddIthVar(manager, 2);
    DdNode *w = Cudd_bddIthVar(manager, 3);
    DdNode *v = Cudd_bddIthVar(manager, 4);
    
    // Create S = x OR y OR z OR w OR v
    DdNode *S = Cudd_bddOr(manager, x, 
                Cudd_bddOr(manager, y,
                Cudd_bddOr(manager, z,
                Cudd_bddOr(manager, w, v))));
    Cudd_Ref(S);
    
    DdNode *xVars[5] = {x, y, z, w, v};
    
    // S has 31 minterms (2^5 - 1)
    // Test various extraction sizes
    double testValues[] = {1.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0};
    for (int i = 0; i < 7; i++) {
        DdNode *result = Cudd_SplitSet(manager, S, xVars, 5, testValues[i]);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 5);
        REQUIRE(mintermCount == testValues[i]);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Deeply nested BDD for recursion paths", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create 6 variables
    DdNode *vars[6];
    for (int i = 0; i < 6; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create a deeply nested ITE structure
    // S = ITE(x0, ITE(x1, ITE(x2, x3, x4), x5), ITE(x1, x3, ITE(x2, x4, x5)))
    DdNode *inner1 = Cudd_bddIte(manager, vars[2], vars[3], vars[4]);
    Cudd_Ref(inner1);
    DdNode *inner2 = Cudd_bddIte(manager, vars[1], inner1, vars[5]);
    Cudd_Ref(inner2);
    DdNode *inner3 = Cudd_bddIte(manager, vars[2], vars[4], vars[5]);
    Cudd_Ref(inner3);
    DdNode *inner4 = Cudd_bddIte(manager, vars[1], vars[3], inner3);
    Cudd_Ref(inner4);
    DdNode *S = Cudd_bddIte(manager, vars[0], inner2, inner4);
    Cudd_Ref(S);
    
    // Test various extraction sizes
    for (int m = 1; m <= 20; m += 3) {
        DdNode *result = Cudd_SplitSet(manager, S, vars, 6, (double)m);
        if (result != nullptr) {
            Cudd_Ref(result);
            REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
            double mintermCount = Cudd_CountMinterm(manager, result, 6);
            REQUIRE(mintermCount == (double)m);
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, inner4);
    Cudd_RecursiveDeref(manager, inner3);
    Cudd_RecursiveDeref(manager, inner2);
    Cudd_RecursiveDeref(manager, inner1);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional tests targeting specific edge cases for better coverage
// ============================================================================

TEST_CASE("Cudd_SplitSet - mintermsFromUniverse recursion base cases", "[cuddSplit]") {
    // This test specifically targets hitting base cases in mintermsFromUniverse
    // when recursion reduces numVars to small values
    
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Use a small number of variables to ensure base cases are reached
    DdNode *vars[2];
    vars[0] = Cudd_bddIthVar(manager, 0);
    vars[1] = Cudd_bddIthVar(manager, 1);
    
    // With 2 vars, max=4. Test all possible values
    // This should exercise mintermsFromUniverse with:
    // - n=1: goes recursive, eventually hits n=0 base case on one branch
    // - n=2: hits n==max2 case
    // - n=3: goes recursive, eventually hits n=max base case on one branch
    
    for (int m = 1; m <= 3; m++) {
        DdNode *result = Cudd_SplitSet(manager, one, vars, 2, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 2);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    // Test with 1 variable (max=2)
    DdNode *singleVar[1] = {vars[0]};
    
    // m=1 should return the variable itself
    DdNode *result = Cudd_SplitSet(manager, one, singleVar, 1, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    double mintermCount = Cudd_CountMinterm(manager, result, 1);
    REQUIRE(mintermCount == 1.0);
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - selectMintermsFromUniverse with unseen vars", "[cuddSplit]") {
    // Test when there are unseen variables that need to be extracted from
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create many variables but only use a few in the BDD
    const int numVars = 5;
    DdNode *vars[numVars];
    for (int i = 0; i < numVars; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create S = x0 (only uses first variable)
    // This means x1, x2, x3, x4 are "unseen" and will be used by selectMintermsFromUniverse
    DdNode *S = vars[0];
    Cudd_Ref(S);
    
    // S has 16 minterms in 5-variable space (x0=1, any values for x1-x4)
    // Test various extraction sizes
    double testValues[] = {1.0, 4.0, 8.0, 12.0, 15.0};
    for (int i = 0; i < 5; i++) {
        DdNode *result = Cudd_SplitSet(manager, S, vars, numVars, testValues[i]);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, numVars);
        REQUIRE(mintermCount == testValues[i]);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Very small BDD edge cases", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    // Test with a single variable
    DdNode *x = Cudd_bddIthVar(manager, 0);
    DdNode *xVars[1] = {x};
    
    // x has 1 minterm, !x has 1 minterm, one has 2 minterms
    
    // Test extracting from x (1 minterm)
    DdNode *result = Cudd_SplitSet(manager, x, xVars, 1, 1.0);
    REQUIRE(result == x);
    
    // Test extracting from !x (1 minterm)
    DdNode *notx = Cudd_Not(x);
    Cudd_Ref(notx);
    result = Cudd_SplitSet(manager, notx, xVars, 1, 1.0);
    REQUIRE(result == notx);
    Cudd_RecursiveDeref(manager, notx);
    
    // Test extracting 1 from one (2 minterms)
    result = Cudd_SplitSet(manager, one, xVars, 1, 1.0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    double mintermCount = Cudd_CountMinterm(manager, result, 1);
    REQUIRE(mintermCount == 1.0);
    Cudd_RecursiveDeref(manager, result);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SplitSet - Alternating structure BDD", "[cuddSplit]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *vars[4];
    for (int i = 0; i < 4; i++) {
        vars[i] = Cudd_bddIthVar(manager, i);
    }
    
    // Create S = (x0 XOR x1) AND (x2 XOR x3)
    // This creates a BDD with interesting structure where each level has
    // both constant and non-constant children
    DdNode *xor01 = Cudd_bddXor(manager, vars[0], vars[1]);
    Cudd_Ref(xor01);
    DdNode *xor23 = Cudd_bddXor(manager, vars[2], vars[3]);
    Cudd_Ref(xor23);
    DdNode *S = Cudd_bddAnd(manager, xor01, xor23);
    Cudd_Ref(S);
    
    // S has 4 minterms
    for (int m = 1; m <= 4; m++) {
        DdNode *result = Cudd_SplitSet(manager, S, vars, 4, (double)m);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        REQUIRE(Cudd_bddLeq(manager, result, S) == 1);
        
        double mintermCount = Cudd_CountMinterm(manager, result, 4);
        REQUIRE(mintermCount == (double)m);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, S);
    Cudd_RecursiveDeref(manager, xor23);
    Cudd_RecursiveDeref(manager, xor01);
    Cudd_Quit(manager);
}
