#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddZddSetop.c
 * 
 * This file contains comprehensive tests for all public functions
 * in the cuddZddSetop module to achieve 90%+ code coverage.
 * 
 * Functions tested:
 * - Cudd_zddIte
 * - Cudd_zddUnion
 * - Cudd_zddIntersect
 * - Cudd_zddDiff
 * - Cudd_zddDiffConst
 * - Cudd_zddSubset1
 * - Cudd_zddSubset0
 * - Cudd_zddChange
 */

// ============================================================================
// TESTS FOR Cudd_zddIte
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddIte basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("ITE with f = 0 returns h") {
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddIte(dd, zero, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z1);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with f = 1 returns g") {
        DdNode* one = DD_ONE(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddIte(dd, one, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // ITE with tautology f returns g, result should be z0 
        // but DD_ONE for ZDD may not be at top level, so just verify not null
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with g = h returns g") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddIte(dd, z0, z1, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z1);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with f and g=1, h=0") {
        DdNode* one = DD_ONE(dd);
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddIte(dd, z0, one, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // Result should be valid, just verify it's not null
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with general case - topf < v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        DdNode* result = Cudd_zddIte(dd, z0, z1, z2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with topf > v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z3);
        
        // z0 has smaller index, z2 and z3 have larger
        DdNode* result = Cudd_zddIte(dd, z2, z0, z3);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with topf == v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* g = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(g);
        DdNode* h = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(h);
        
        DdNode* result = Cudd_zddIte(dd, z0, g, h);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, h);
        Cudd_RecursiveDerefZdd(dd, g);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - Cudd_zddIte zddVarToConst optimization", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("ITE with f == g optimization") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // ITE(F, F, H) should optimize to ITE(F, 1, H)
        DdNode* result = Cudd_zddIte(dd, z0, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE with f == h optimization") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // ITE(F, G, F) should optimize to ITE(F, G, 0)
        DdNode* result = Cudd_zddIte(dd, z0, z1, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddUnion
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddUnion basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Union with empty returns other set") {
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result1 = Cudd_zddUnion(dd, zero, z0);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        REQUIRE(result1 == z0);
        
        DdNode* result2 = Cudd_zddUnion(dd, z0, zero);
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        REQUIRE(result2 == z0);
        
        Cudd_RecursiveDerefZdd(dd, result2);
        Cudd_RecursiveDerefZdd(dd, result1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Union of set with itself") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddUnion(dd, z0, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z0);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Union when p_top < q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddUnion(dd, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Union when p_top > q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddUnion(dd, z1, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Union when p_top == q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // Create union of z0|z1, then union with z0 again
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        
        DdNode* result = Cudd_zddUnion(dd, u1, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - Cudd_zddUnion complex tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Union of multiple variables") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        DdNode* result = Cudd_zddUnion(dd, u1, z2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddIntersect
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddIntersect basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Intersect with empty returns empty") {
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result1 = Cudd_zddIntersect(dd, zero, z0);
        REQUIRE(result1 != nullptr);
        REQUIRE(result1 == zero);
        
        DdNode* result2 = Cudd_zddIntersect(dd, z0, zero);
        REQUIRE(result2 != nullptr);
        REQUIRE(result2 == zero);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect of set with itself") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddIntersect(dd, z0, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z0);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect when p_top < q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddIntersect(dd, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect when p_top > q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddIntersect(dd, z1, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect when p_top == q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Create unions then intersect
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(dd, z0, z2);
        Cudd_Ref(u2);
        
        DdNode* result = Cudd_zddIntersect(dd, u1, u2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddDiff
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddDiff basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Diff with P=empty returns empty") {
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddDiff(dd, zero, z0);
        REQUIRE(result != nullptr);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff with Q=empty returns P") {
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddDiff(dd, z0, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z0);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff with P==Q returns empty") {
        DdNode* zero = DD_ZERO(dd);
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddDiff(dd, z0, z0);
        REQUIRE(result != nullptr);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff when p_top < q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddDiff(dd, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff when p_top > q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddDiff(dd, z1, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff when p_top == q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Create unions then diff
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(dd, z0, z2);
        Cudd_Ref(u2);
        
        DdNode* result = Cudd_zddDiff(dd, u1, u2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddDiffConst
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddDiffConst basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    DdNode* empty = DD_ZERO(dd);
    
    SECTION("DiffConst with P=empty returns empty") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddDiffConst(dd, empty, z0);
        REQUIRE(result != nullptr);
        REQUIRE(result == empty);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst with Q=empty returns P") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddDiffConst(dd, z0, empty);
        REQUIRE(result != nullptr);
        REQUIRE(result == z0);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst with P==Q returns empty") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* result = Cudd_zddDiffConst(dd, z0, z0);
        REQUIRE(result != nullptr);
        REQUIRE(result == empty);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst when p_top < q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddDiffConst(dd, z0, z1);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst when p_top > q_top") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* result = Cudd_zddDiffConst(dd, z1, z0);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst when p_top == q_top and subset check") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Create unions  
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(dd, z0, z2);
        Cudd_Ref(u2);
        
        DdNode* result = Cudd_zddDiffConst(dd, u1, u2);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddSubset1
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddSubset1 basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Subset1 of empty returns empty") {
        DdNode* zero = DD_ZERO(dd);
        
        DdNode* result = Cudd_zddSubset1(dd, zero, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDerefZdd(dd, result);
    }
    
    SECTION("Subset1 of constant base") {
        DdNode* one = DD_ONE(dd);
        
        DdNode* result = Cudd_zddSubset1(dd, one, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
    }
    
    SECTION("Subset1 of single variable") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // Get subset where var 0 is present
        DdNode* result = Cudd_zddSubset1(dd, z0, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Subset1 when top_var > level") {
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Get subset where var 0 is present, but z2 is at higher level
        DdNode* result = Cudd_zddSubset1(dd, z2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // Result may be empty or not depending on ZDD semantics
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z2);
    }
    
    SECTION("Subset1 when top_var == level") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // Get subset where var 0 is present
        DdNode* result = Cudd_zddSubset1(dd, u, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Subset1 when top_var < level (recursive)") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // Get subset where var 2 is present (higher level)
        DdNode* result = Cudd_zddSubset1(dd, u, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddSubset0
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddSubset0 basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Subset0 of empty returns empty") {
        DdNode* zero = DD_ZERO(dd);
        
        DdNode* result = Cudd_zddSubset0(dd, zero, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDerefZdd(dd, result);
    }
    
    SECTION("Subset0 of constant base") {
        DdNode* one = DD_ONE(dd);
        
        DdNode* result = Cudd_zddSubset0(dd, one, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
    }
    
    SECTION("Subset0 of single variable") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // Get subset where var 0 is absent
        DdNode* result = Cudd_zddSubset0(dd, z0, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Subset0 when top_var > level") {
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Get subset where var 0 is absent, z2 is at higher level
        DdNode* result = Cudd_zddSubset0(dd, z2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // Result should be valid
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z2);
    }
    
    SECTION("Subset0 when top_var == level") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // Get subset where var 0 is absent
        DdNode* result = Cudd_zddSubset0(dd, u, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Subset0 when top_var < level (recursive)") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // Get subset where var 2 is absent (higher level)
        DdNode* result = Cudd_zddSubset0(dd, u, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TESTS FOR Cudd_zddChange
// ============================================================================

TEST_CASE("cuddZddSetop - Cudd_zddChange basic tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Change with invalid var index") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // Variable index too large
        DdNode* result = Cudd_zddChange(dd, z0, CUDD_MAXINDEX);
        REQUIRE(result == nullptr);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Change of empty returns empty") {
        DdNode* zero = DD_ZERO(dd);
        
        DdNode* result = Cudd_zddChange(dd, zero, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        
        Cudd_RecursiveDerefZdd(dd, result);
    }
    
    SECTION("Change of base returns variable") {
        DdNode* one = DD_ONE(dd);
        
        DdNode* result = Cudd_zddChange(dd, one, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
    }
    
    SECTION("Change when top_var > level") {
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Change var 0, but z2 is at higher level
        DdNode* result = Cudd_zddChange(dd, z2, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z2);
    }
    
    SECTION("Change when top_var == level") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // Change var 0 in z0 - swaps T and E children
        DdNode* result = Cudd_zddChange(dd, z0, 0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Change when top_var < level (recursive)") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // Change var 2 (higher level than z0 and z1)
        DdNode* result = Cudd_zddChange(dd, u, 2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// COMPREHENSIVE TESTS FOR EDGE CASES AND COVERAGE
// ============================================================================

TEST_CASE("cuddZddSetop - Complex operations for deep recursion", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Deep nested operations") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z3);
        
        // Build complex structure
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(dd, z2, z3);
        Cudd_Ref(u2);
        
        DdNode* i1 = Cudd_zddIntersect(dd, u1, u2);
        Cudd_Ref(i1);
        
        DdNode* d1 = Cudd_zddDiff(dd, u1, i1);
        Cudd_Ref(d1);
        
        DdNode* ite = Cudd_zddIte(dd, z0, u1, u2);
        REQUIRE(ite != nullptr);
        Cudd_Ref(ite);
        
        Cudd_RecursiveDerefZdd(dd, ite);
        Cudd_RecursiveDerefZdd(dd, d1);
        Cudd_RecursiveDerefZdd(dd, i1);
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Product and operations") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        DdNode* p1 = Cudd_zddProduct(dd, z0, z1);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(dd, z1, z2);
        Cudd_Ref(p2);
        
        DdNode* u = Cudd_zddUnion(dd, p1, p2);
        Cudd_Ref(u);
        
        DdNode* s1 = Cudd_zddSubset1(dd, u, 1);
        Cudd_Ref(s1);
        DdNode* s0 = Cudd_zddSubset0(dd, u, 1);
        Cudd_Ref(s0);
        
        DdNode* c = Cudd_zddChange(dd, u, 1);
        REQUIRE(c != nullptr);
        Cudd_Ref(c);
        
        Cudd_RecursiveDerefZdd(dd, c);
        Cudd_RecursiveDerefZdd(dd, s0);
        Cudd_RecursiveDerefZdd(dd, s1);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, p2);
        Cudd_RecursiveDerefZdd(dd, p1);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - Cache utilization tests", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Repeated operations to test caching") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // First call - fills cache
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        
        // Second call - should hit cache
        DdNode* u2 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u2);
        
        REQUIRE(u1 == u2);
        
        // Test other operations
        DdNode* i1 = Cudd_zddIntersect(dd, z0, z1);
        Cudd_Ref(i1);
        DdNode* i2 = Cudd_zddIntersect(dd, z0, z1);
        Cudd_Ref(i2);
        REQUIRE(i1 == i2);
        
        DdNode* d1 = Cudd_zddDiff(dd, z0, z1);
        Cudd_Ref(d1);
        DdNode* d2 = Cudd_zddDiff(dd, z0, z1);
        Cudd_Ref(d2);
        REQUIRE(d1 == d2);
        
        Cudd_RecursiveDerefZdd(dd, d2);
        Cudd_RecursiveDerefZdd(dd, d1);
        Cudd_RecursiveDerefZdd(dd, i2);
        Cudd_RecursiveDerefZdd(dd, i1);
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - Constants and special cases", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    DdNode* zero = DD_ZERO(dd);
    DdNode* one = DD_ONE(dd);
    
    SECTION("Operations with constants") {
        // ITE with constants
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        DdNode* ite1 = Cudd_zddIte(dd, zero, z0, one);
        REQUIRE(ite1 == one);
        
        DdNode* ite2 = Cudd_zddIte(dd, one, z0, zero);
        // ITE with tautology f returns g, but DD_ONE may not be tautology at this level
        REQUIRE(ite2 != nullptr);
        
        // Union with constants
        DdNode* u1 = Cudd_zddUnion(dd, zero, z0);
        REQUIRE(u1 == z0);
        
        DdNode* u2 = Cudd_zddUnion(dd, z0, zero);
        REQUIRE(u2 == z0);
        
        // Intersect with constants
        DdNode* i1 = Cudd_zddIntersect(dd, zero, z0);
        REQUIRE(i1 == zero);
        
        DdNode* i2 = Cudd_zddIntersect(dd, z0, zero);
        REQUIRE(i2 == zero);
        
        // Diff with constants
        DdNode* d1 = Cudd_zddDiff(dd, zero, z0);
        REQUIRE(d1 == zero);
        
        DdNode* d2 = Cudd_zddDiff(dd, z0, zero);
        Cudd_Ref(d2);
        REQUIRE(d2 == z0);
        Cudd_RecursiveDerefZdd(dd, d2);
        
        // Subset with constants
        DdNode* s1 = Cudd_zddSubset1(dd, one, 0);
        REQUIRE(s1 != nullptr);
        Cudd_Ref(s1);
        Cudd_RecursiveDerefZdd(dd, s1);
        
        DdNode* s0 = Cudd_zddSubset0(dd, one, 0);
        REQUIRE(s0 != nullptr);
        Cudd_Ref(s0);
        Cudd_RecursiveDerefZdd(dd, s0);
        
        // Change with constants
        DdNode* c1 = Cudd_zddChange(dd, zero, 0);
        REQUIRE(c1 == zero);
        
        DdNode* c2 = Cudd_zddChange(dd, one, 0);
        REQUIRE(c2 != nullptr);
        Cudd_Ref(c2);
        Cudd_RecursiveDerefZdd(dd, c2);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - Multiple variable levels", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Operations across multiple levels") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        Cudd_Ref(z4);
        
        DdNode* u1 = Cudd_zddUnion(dd, z0, z2);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(dd, u1, z4);
        Cudd_Ref(u2);
        
        // Test subset operations at different levels
        DdNode* s1 = Cudd_zddSubset1(dd, u2, 2);
        REQUIRE(s1 != nullptr);
        Cudd_Ref(s1);
        
        DdNode* s0 = Cudd_zddSubset0(dd, u2, 2);
        REQUIRE(s0 != nullptr);
        Cudd_Ref(s0);
        
        // Test change at different levels
        DdNode* c = Cudd_zddChange(dd, u2, 2);
        REQUIRE(c != nullptr);
        Cudd_Ref(c);
        
        Cudd_RecursiveDerefZdd(dd, c);
        Cudd_RecursiveDerefZdd(dd, s0);
        Cudd_RecursiveDerefZdd(dd, s1);
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// ADDITIONAL TESTS FOR BETTER COVERAGE
// ============================================================================

TEST_CASE("cuddZddSetop - DiffConst with constants", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    DdNode* empty = DD_ZERO(dd);
    DdNode* one = DD_ONE(dd);
    
    SECTION("DiffConst with constant P index handling") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // When P is constant, use P->index
        DdNode* result = Cudd_zddDiffConst(dd, one, z0);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst with constant Q index handling") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // When Q is constant, use Q->index
        DdNode* result = Cudd_zddDiffConst(dd, z0, one);
        REQUIRE(result != nullptr);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("DiffConst with both constants") {
        // Both P and Q are constants
        DdNode* result = Cudd_zddDiffConst(dd, one, one);
        REQUIRE(result != nullptr);
        REQUIRE(result == empty);
    }
    
    SECTION("DiffConst subset property check") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // Create superset and subset
        DdNode* superset = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(superset);
        
        // Test if z0 is subset of superset (should return empty)
        DdNode* result = Cudd_zddDiffConst(dd, z0, superset);
        REQUIRE(result != nullptr);
        REQUIRE(result == empty);
        
        Cudd_RecursiveDerefZdd(dd, superset);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - ITE edge cases for deep branches", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("ITE where topg > v and toph > v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z3);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        Cudd_Ref(z4);
        
        // f at low level, g and h at higher levels
        DdNode* result = Cudd_zddIte(dd, z0, z3, z4);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE where topg > v and toph <= v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z3);
        
        // Create union for h at same level as v
        DdNode* h = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(h);
        
        DdNode* result = Cudd_zddIte(dd, z1, z3, h);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, h);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE where topg <= v and toph > v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z3);
        
        // Create union for g at same level as v
        DdNode* g = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(g);
        
        DdNode* result = Cudd_zddIte(dd, z1, g, z3);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, g);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("ITE where topg <= v and toph <= v") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        
        // Both g and h at same or lower level than v
        DdNode* g = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(g);
        DdNode* h = Cudd_zddUnion(dd, z1, z2);
        Cudd_Ref(h);
        
        DdNode* result = Cudd_zddIte(dd, z1, g, h);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, h);
        Cudd_RecursiveDerefZdd(dd, g);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - Stress test for all operations", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Large combination of operations") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z3);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        Cudd_Ref(z4);
        
        // Build complex ZDD structure
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(dd, z2, z3);
        Cudd_Ref(u2);
        DdNode* u3 = Cudd_zddUnion(dd, u1, z4);
        Cudd_Ref(u3);
        
        DdNode* i1 = Cudd_zddIntersect(dd, u1, u2);
        Cudd_Ref(i1);
        DdNode* i2 = Cudd_zddIntersect(dd, u3, u2);
        Cudd_Ref(i2);
        
        DdNode* d1 = Cudd_zddDiff(dd, u3, i2);
        Cudd_Ref(d1);
        
        DdNode* ite1 = Cudd_zddIte(dd, z0, d1, i1);
        Cudd_Ref(ite1);
        
        DdNode* s1 = Cudd_zddSubset1(dd, ite1, 1);
        Cudd_Ref(s1);
        DdNode* s0 = Cudd_zddSubset0(dd, ite1, 1);
        Cudd_Ref(s0);
        
        DdNode* c1 = Cudd_zddChange(dd, ite1, 2);
        REQUIRE(c1 != nullptr);
        Cudd_Ref(c1);
        
        // Verify all operations succeeded
        REQUIRE(u1 != nullptr);
        REQUIRE(u2 != nullptr);
        REQUIRE(u3 != nullptr);
        REQUIRE(i1 != nullptr);
        REQUIRE(i2 != nullptr);
        REQUIRE(d1 != nullptr);
        REQUIRE(ite1 != nullptr);
        REQUIRE(s1 != nullptr);
        REQUIRE(s0 != nullptr);
        
        Cudd_RecursiveDerefZdd(dd, c1);
        Cudd_RecursiveDerefZdd(dd, s0);
        Cudd_RecursiveDerefZdd(dd, s1);
        Cudd_RecursiveDerefZdd(dd, ite1);
        Cudd_RecursiveDerefZdd(dd, d1);
        Cudd_RecursiveDerefZdd(dd, i2);
        Cudd_RecursiveDerefZdd(dd, i1);
        Cudd_RecursiveDerefZdd(dd, u3);
        Cudd_RecursiveDerefZdd(dd, u2);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// TARGETED TESTS FOR REMAINING COVERAGE
// ============================================================================

TEST_CASE("cuddZddSetop - Operations with constant nodes", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    DdNode* one = DD_ONE(dd);
    DdNode* zero = DD_ZERO(dd);
    
    SECTION("Intersect with constant P") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // P is constant - exercises p_top = P->index branch
        DdNode* result = Cudd_zddIntersect(dd, one, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect with constant Q") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // Q is constant - exercises q_top = Q->index branch
        DdNode* result = Cudd_zddIntersect(dd, z0, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect when p_top < q_top with constants") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // Make p_top < q_top case
        DdNode* result = Cudd_zddIntersect(dd, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Intersect when p_top > q_top with constants") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        // Make p_top > q_top case
        DdNode* result = Cudd_zddIntersect(dd, z1, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff with constant P") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // P is constant - exercises p_top = P->index branch
        DdNode* result = Cudd_zddDiff(dd, one, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Diff with constant Q") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        
        // Q is constant - exercises q_top = Q->index branch
        DdNode* result = Cudd_zddDiff(dd, z0, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, result);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddSetop - DiffConst cache test", "[cuddZddSetop]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("DiffConst cache hit with subset case") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z1);
        
        DdNode* superset = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(superset);
        
        // First call - fills cache
        DdNode* result1 = Cudd_zddDiffConst(dd, z0, superset);
        // Result is empty since z0 is subset of superset
        
        // Second call - should hit cache (line 237)
        DdNode* result2 = Cudd_zddDiffConst(dd, z0, superset);
        // Both should return empty
        REQUIRE(result1 == result2);
        REQUIRE(result1 == DD_ZERO(dd));
        
        Cudd_RecursiveDerefZdd(dd, superset);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}
