#include <catch2/catch_test_macros.hpp>

#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Comprehensive test file for cuddZddIsop.c
 * 
 * Tests for ISOP (Irredundant Sum of Products) functions to achieve 90% coverage.
 */

// ============================================================================
// TESTS FOR Cudd_zddIsop
// ============================================================================

TEST_CASE("cuddZddIsop - Cudd_zddIsop basic tests", "[cuddZddIsop]") {
    SECTION("ISOP with L = zero returns zero") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(zero);
        DdNode* one = Cudd_ReadOne(manager);
        Cudd_Ref(one);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, zero, one, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        // L = zero should result in zero
        REQUIRE(isop == zero);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, zero);
        Cudd_RecursiveDeref(manager, one);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with U = one returns one") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(zero);
        DdNode* one = Cudd_ReadOne(manager);
        Cudd_Ref(one);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, zero, one, &zdd_I);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, zero);
        Cudd_RecursiveDeref(manager, one);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with single variable L = U = x") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        // Result should be equivalent to x0
        REQUIRE(isop == x0);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with two variables AND") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        REQUIRE(isop == bdd);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with two variables OR") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with complemented variable") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* notx0 = Cudd_Not(x0);
        Cudd_Ref(notx0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, notx0, notx0, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, notx0);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with interval L != U") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // L = x0 AND x1, U = x0 OR x1
        DdNode* L = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(L);
        DdNode* U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, L, U, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        // isop should be between L and U
        int check1 = Cudd_bddLeq(manager, L, isop);
        int check2 = Cudd_bddLeq(manager, isop, U);
        REQUIRE(check1 == 1);
        REQUIRE(check2 == 1);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - Complex ISOP tests", "[cuddZddIsop]") {
    SECTION("ISOP with multiple variables") {
        DdManager* manager = Cudd_Init(6, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        // (x0 AND x1) OR x2
        DdNode* temp = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(temp);
        DdNode* bdd = Cudd_bddOr(manager, temp, x2);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP cache hit test") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        // First call
        DdNode* zdd_I1 = nullptr;
        DdNode* isop1 = Cudd_zddIsop(manager, x0, x0, &zdd_I1);
        REQUIRE(isop1 != nullptr);
        Cudd_Ref(isop1);
        Cudd_Ref(zdd_I1);
        
        // Second call should hit cache
        DdNode* zdd_I2 = nullptr;
        DdNode* isop2 = Cudd_zddIsop(manager, x0, x0, &zdd_I2);
        REQUIRE(isop2 != nullptr);
        Cudd_Ref(isop2);
        Cudd_Ref(zdd_I2);
        
        REQUIRE(isop1 == isop2);
        REQUIRE(zdd_I1 == zdd_I2);
        
        Cudd_RecursiveDeref(manager, isop1);
        Cudd_RecursiveDeref(manager, isop2);
        Cudd_RecursiveDerefZdd(manager, zdd_I1);
        Cudd_RecursiveDerefZdd(manager, zdd_I2);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with XOR function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_bddIsop
// ============================================================================

TEST_CASE("cuddZddIsop - Cudd_bddIsop basic tests", "[cuddZddIsop]") {
    SECTION("bddIsop with L = zero") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(zero);
        DdNode* one = Cudd_ReadOne(manager);
        Cudd_Ref(one);
        
        DdNode* isop = Cudd_bddIsop(manager, zero, one);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        REQUIRE(isop == zero);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, zero);
        Cudd_RecursiveDeref(manager, one);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with U = one") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        DdNode* one = Cudd_ReadOne(manager);
        Cudd_Ref(one);
        
        DdNode* isop = Cudd_bddIsop(manager, x0, one);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_RecursiveDeref(manager, one);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with L = U = single variable") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* isop = Cudd_bddIsop(manager, x0, x0);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        REQUIRE(isop == x0);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with AND function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* isop = Cudd_bddIsop(manager, bdd, bdd);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        REQUIRE(isop == bdd);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with OR function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* isop = Cudd_bddIsop(manager, bdd, bdd);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with complemented inputs") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* notx0 = Cudd_Not(x0);
        Cudd_Ref(notx0);
        
        DdNode* isop = Cudd_bddIsop(manager, notx0, notx0);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, notx0);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with interval") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode* L = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(L);
        DdNode* U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        DdNode* isop = Cudd_bddIsop(manager, L, U);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop cache hit test") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        // First call
        DdNode* isop1 = Cudd_bddIsop(manager, x0, x0);
        REQUIRE(isop1 != nullptr);
        Cudd_Ref(isop1);
        
        // Second call should hit cache
        DdNode* isop2 = Cudd_bddIsop(manager, x0, x0);
        REQUIRE(isop2 != nullptr);
        Cudd_Ref(isop2);
        
        REQUIRE(isop1 == isop2);
        
        Cudd_RecursiveDeref(manager, isop1);
        Cudd_RecursiveDeref(manager, isop2);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - Cudd_bddIsop complex tests", "[cuddZddIsop]") {
    SECTION("bddIsop with multiple variables") {
        DdManager* manager = Cudd_Init(6, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        DdNode* temp = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(temp);
        DdNode* bdd = Cudd_bddOr(manager, temp, x2);
        Cudd_Ref(bdd);
        
        DdNode* isop = Cudd_bddIsop(manager, bdd, bdd);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with XOR") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* isop = Cudd_bddIsop(manager, bdd, bdd);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop top_l != top_u branch") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // L depends on x0, U depends on x1 (different top variables)
        DdNode* L = x0;
        Cudd_Ref(L);
        DdNode* U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        DdNode* isop = Cudd_bddIsop(manager, L, U);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_MakeBddFromZddCover
// ============================================================================

TEST_CASE("cuddZddIsop - Cudd_MakeBddFromZddCover basic tests", "[cuddZddIsop]") {
    SECTION("MakeBddFromZddCover with ZDD one") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* zddOne = DD_ONE(manager);
        Cudd_Ref(zddOne);
        
        DdNode* bdd = Cudd_MakeBddFromZddCover(manager, zddOne);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // ZDD one represents the tautology
        REQUIRE(bdd == Cudd_ReadOne(manager));
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDerefZdd(manager, zddOne);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover with ZDD zero") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* zddZero = DD_ZERO(manager);
        Cudd_Ref(zddZero);
        
        DdNode* bdd = Cudd_MakeBddFromZddCover(manager, zddZero);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // ZDD zero represents the empty cover (contradiction)
        REQUIRE(bdd == Cudd_ReadLogicZero(manager));
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDerefZdd(manager, zddZero);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover roundtrip") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        // Convert ZDD cover back to BDD
        DdNode* bdd = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Should get back the same BDD
        REQUIRE(bdd == isop);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover with AND function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        REQUIRE(recovered == bdd);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover with OR function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover cache hit test") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        // First call
        DdNode* bdd1 = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(bdd1 != nullptr);
        Cudd_Ref(bdd1);
        
        // Second call should hit cache
        DdNode* bdd2 = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(bdd2 != nullptr);
        Cudd_Ref(bdd2);
        
        REQUIRE(bdd1 == bdd2);
        
        Cudd_RecursiveDeref(manager, bdd1);
        Cudd_RecursiveDeref(manager, bdd2);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - MakeBddFromZddCover fd != zero branch", "[cuddZddIsop]") {
    SECTION("MakeBddFromZddCover with XOR (fd != zero)") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover with complex function") {
        DdManager* manager = Cudd_Init(6, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        
        // (x0 AND x1) OR (NOT x0 AND x2)
        DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t1);
        DdNode* notx0 = Cudd_Not(x0);
        DdNode* t2 = Cudd_bddAnd(manager, notx0, x2);
        Cudd_Ref(t2);
        DdNode* bdd = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover Cudd_IsComplement(T) branch") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        // Use a function that creates complemented T
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* notx0 = Cudd_Not(x0);
        Cudd_Ref(notx0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, notx0, notx0, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, notx0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// ADDITIONAL COVERAGE TESTS
// ============================================================================

TEST_CASE("cuddZddIsop - Variable ordering branches", "[cuddZddIsop]") {
    SECTION("top_l < top_u case") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // L depends only on x0, U depends on both
        DdNode* L = x0;
        Cudd_Ref(L);
        DdNode* U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, L, U, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
    
    SECTION("top_l > top_u case") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // L depends on x1, U depends only on x0
        DdNode* L = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(L);
        DdNode* U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, L, U, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - zdd_Isub0 and zdd_Isub1 zero branches", "[cuddZddIsop]") {
    SECTION("zdd_Isub0 = zero branch") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        // Function that triggers zdd_Isub0 = zero
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("zdd_Isub1 = zero branch") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        // Function that triggers zdd_Isub1 = zero (NOT x0)
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* notx0 = Cudd_Not(x0);
        Cudd_Ref(notx0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, notx0, notx0, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, notx0);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - Deep recursion tests", "[cuddZddIsop]") {
    SECTION("Deep recursion with multiple variables") {
        DdManager* manager = Cudd_Init(8, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        // Build complex function
        DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(t2);
        DdNode* bdd = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        REQUIRE(recovered == bdd);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - Edge cases for cuddBddIsop", "[cuddZddIsop]") {
    SECTION("cuddBddIsop with complemented L and U") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode* L = Cudd_Not(Cudd_bddOr(manager, x0, x1));
        Cudd_Ref(L);
        DdNode* U = Cudd_Not(Cudd_bddAnd(manager, x0, x1));
        Cudd_Ref(U);
        
        DdNode* isop = Cudd_bddIsop(manager, L, U);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// ADDITIONAL TESTS FOR HIGHER COVERAGE
// ============================================================================

TEST_CASE("cuddZddIsop - top_l != v branch coverage", "[cuddZddIsop]") {
    SECTION("L is constant, U is variable (top_l > top_u)") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(zero);
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        // L = zero (constant), U = x0 (variable) - triggers top_l > top_u
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, zero, x0, &zdd_I);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, zero);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("L depends on x1, U depends on x0 (top_l > top_u)") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // L = x1 (higher index), U = x0 OR x1 (lower index at top)
        DdNode* L = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(L);
        DdNode* one = Cudd_ReadOne(manager);
        Cudd_Ref(one);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, L, one, &zdd_I);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, one);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - cuddBddIsop top_l != v branch", "[cuddZddIsop]") {
    SECTION("bddIsop L constant, U variable") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadLogicZero(manager);
        Cudd_Ref(zero);
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* isop = Cudd_bddIsop(manager, zero, x0);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, zero);
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("bddIsop with different variable levels in L and U") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // L depends only on x1 (not x0)
        DdNode* L = x1;
        Cudd_Ref(L);
        // U depends on both x0 and x1
        DdNode* U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        DdNode* isop = Cudd_bddIsop(manager, L, U);
        REQUIRE(isop != nullptr);
        Cudd_Ref(isop);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - More complex function tests", "[cuddZddIsop]") {
    SECTION("ISOP with NAND function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddNand(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with NOR function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddNor(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with XNOR function") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddXnor(manager, x0, x1);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP with ITE function") {
        DdManager* manager = Cudd_Init(6, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* bdd = Cudd_bddIte(manager, x0, x1, x2);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - Wide interval tests", "[cuddZddIsop]") {
    SECTION("Wide interval with multiple solutions") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        
        // Very narrow L (just x0 AND x1)
        DdNode* L = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(L);
        
        // Very wide U (tautology)
        DdNode* U = Cudd_ReadOne(manager);
        Cudd_Ref(U);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, L, U, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        // Check isop is in interval
        int check1 = Cudd_bddLeq(manager, L, isop);
        int check2 = Cudd_bddLeq(manager, isop, U);
        REQUIRE(check1 == 1);
        REQUIRE(check2 == 1);
        
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddIsop - Four variable tests", "[cuddZddIsop]") {
    SECTION("Complex 4-variable function") {
        DdManager* manager = Cudd_Init(8, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* x2 = Cudd_bddIthVar(manager, 2);
        DdNode* x3 = Cudd_bddIthVar(manager, 3);
        
        // f = (x0 AND x1) OR (x2 AND x3) OR (x0 AND x2)
        DdNode* t1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(t2);
        DdNode* t3 = Cudd_bddAnd(manager, x0, x2);
        Cudd_Ref(t3);
        DdNode* temp = Cudd_bddOr(manager, t1, t2);
        Cudd_Ref(temp);
        DdNode* bdd = Cudd_bddOr(manager, temp, t3);
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        REQUIRE(isop != nullptr);
        REQUIRE(zdd_I != nullptr);
        Cudd_Ref(isop);
        Cudd_Ref(zdd_I);
        
        DdNode* recovered = Cudd_MakeBddFromZddCover(manager, zdd_I);
        REQUIRE(recovered != nullptr);
        Cudd_Ref(recovered);
        REQUIRE(recovered == bdd);
        
        Cudd_RecursiveDeref(manager, recovered);
        Cudd_RecursiveDeref(manager, isop);
        Cudd_RecursiveDerefZdd(manager, zdd_I);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, temp);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_Quit(manager);
    }
}
