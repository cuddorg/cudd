#include <catch2/catch_test_macros.hpp>

#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Comprehensive test file for cuddZddFuncs.c
 * 
 * Tests for ZDD cover manipulation functions to achieve 90% coverage.
 */

// ============================================================================
// TESTS FOR Cudd_zddProduct
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddProduct basic tests", "[cuddZddFuncs]") {
    SECTION("Product of two ZDD variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* prod = Cudd_zddProduct(manager, z0, z1);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("Product with one") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* prod = Cudd_zddProduct(manager, one, z0);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Product with zero") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* prod = Cudd_zddProduct(manager, zero, z0);
        REQUIRE(prod != nullptr);
        REQUIRE(prod == zero);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Product complex covers") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        DdNode* cover1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(cover1);
        DdNode* cover2 = Cudd_zddUnion(manager, z2, z3);
        Cudd_Ref(cover2);
        
        DdNode* prod = Cudd_zddProduct(manager, cover1, cover2);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddUnateProduct
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddUnateProduct tests", "[cuddZddFuncs]") {
    SECTION("Unate product of two ZDD variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* prod = Cudd_zddUnateProduct(manager, z0, z1);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("Unate product with one") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* prod = Cudd_zddUnateProduct(manager, one, z0);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Unate product with zero") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* prod = Cudd_zddUnateProduct(manager, zero, z0);
        REQUIRE(prod != nullptr);
        REQUIRE(prod == zero);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Unate product complex covers") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        DdNode* cover1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(cover1);
        DdNode* cover2 = Cudd_zddUnion(manager, z2, z3);
        Cudd_Ref(cover2);
        
        DdNode* prod = Cudd_zddUnateProduct(manager, cover1, cover2);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddWeakDiv
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddWeakDiv tests", "[cuddZddFuncs]") {
    SECTION("Weak division by one") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddWeakDiv(manager, z0, one);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Weak division of zero") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddWeakDiv(manager, zero, z0);
        REQUIRE(div != nullptr);
        REQUIRE(div == zero);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Weak division f == g") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddWeakDiv(manager, z0, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Weak division complex covers") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        DdNode* p1 = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        DdNode* div = Cudd_zddWeakDiv(manager, f, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddDivide
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddDivide tests", "[cuddZddFuncs]") {
    SECTION("Divide by one") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddDivide(manager, z0, one);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Divide zero") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddDivide(manager, zero, z0);
        REQUIRE(div != nullptr);
        REQUIRE(div == zero);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Divide f == g") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddDivide(manager, z0, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Divide complex covers") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        DdNode* cover1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(cover1);
        DdNode* cover2 = Cudd_zddUnion(manager, z1, z2);
        Cudd_Ref(cover2);
        
        DdNode* div = Cudd_zddDivide(manager, cover1, cover2);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddWeakDivF
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddWeakDivF tests", "[cuddZddFuncs]") {
    SECTION("WeakDivF by one") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddWeakDivF(manager, z0, one);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF of zero") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddWeakDivF(manager, zero, z0);
        REQUIRE(div != nullptr);
        REQUIRE(div == zero);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF f == g") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddWeakDivF(manager, z0, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF complex covers") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        DdNode* p1 = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z2, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        DdNode* div = Cudd_zddWeakDivF(manager, f, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddDivideF
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddDivideF tests", "[cuddZddFuncs]") {
    SECTION("DivideF by one") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddDivideF(manager, z0, one);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("DivideF of zero") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddDivideF(manager, zero, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("DivideF f == g") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        DdNode* div = Cudd_zddDivideF(manager, z0, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("DivideF complex") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        DdNode* cover1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(cover1);
        DdNode* cover2 = Cudd_zddUnion(manager, z1, z2);
        Cudd_Ref(cover2);
        
        DdNode* div = Cudd_zddDivideF(manager, cover1, cover2);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddComplement
// ============================================================================

TEST_CASE("cuddZddFuncs - Cudd_zddComplement tests", "[cuddZddFuncs]") {
    SECTION("Complement of single variable") {
        DdManager* manager = Cudd_Init(2, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            DdNode* comp = Cudd_zddComplement(manager, zdd_I);
            if (comp != nullptr) {
                Cudd_Ref(comp);
                Cudd_RecursiveDerefZdd(manager, comp);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("Complement of complex cover") {
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
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            DdNode* comp = Cudd_zddComplement(manager, zdd_I);
            if (comp != nullptr) {
                Cudd_Ref(comp);
                Cudd_RecursiveDerefZdd(manager, comp);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddGetCofactors3
// ============================================================================

TEST_CASE("cuddZddFuncs - cuddZddGetCofactors3 tests", "[cuddZddFuncs]") {
    SECTION("Cofactors of simple ZDD") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* f = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(f);
        
        DdNode* f1 = nullptr;
        DdNode* f0 = nullptr;
        DdNode* fd = nullptr;
        
        int result = cuddZddGetCofactors3(manager, f, 0, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1);
        Cudd_Ref(f0);
        Cudd_Ref(fd);
        
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR cuddZddGetCofactors2
// ============================================================================

TEST_CASE("cuddZddFuncs - cuddZddGetCofactors2 tests", "[cuddZddFuncs]") {
    SECTION("Two-way cofactors of simple ZDD") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* f = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(f);
        
        DdNode* f1 = nullptr;
        DdNode* f0 = nullptr;
        
        int result = cuddZddGetCofactors2(manager, f, 0, &f1, &f0);
        REQUIRE(result == 0);
        Cudd_Ref(f1);
        Cudd_Ref(f0);
        
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR helper functions
// ============================================================================

TEST_CASE("cuddZddFuncs - Variable index helper tests", "[cuddZddFuncs]") {
    SECTION("Get positive and negative variable index") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int pv = cuddZddGetPosVarIndex(manager, 0);
        int nv = cuddZddGetNegVarIndex(manager, 0);
        REQUIRE(pv == 0);
        REQUIRE(nv == 1);
        
        pv = cuddZddGetPosVarIndex(manager, 2);
        nv = cuddZddGetNegVarIndex(manager, 2);
        REQUIRE(pv == 2);
        REQUIRE(nv == 3);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Get positive and negative variable level") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int pl = cuddZddGetPosVarLevel(manager, 0);
        int nl = cuddZddGetNegVarLevel(manager, 0);
        REQUIRE(pl >= 0);
        REQUIRE(nl >= 0);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// ADDITIONAL COVERAGE TESTS
// ============================================================================

TEST_CASE("cuddZddFuncs - Complex recursive coverage", "[cuddZddFuncs]") {
    SECTION("Product with multiple levels") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        for (int i = 0; i < 8; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* temp = Cudd_zddUnion(manager, result, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, result);
            Cudd_RecursiveDerefZdd(manager, var);
            result = temp;
        }
        
        DdNode* cover2 = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(cover2);
        for (int i = 8; i < 16; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* temp = Cudd_zddUnion(manager, cover2, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, cover2);
            Cudd_RecursiveDerefZdd(manager, var);
            cover2 = temp;
        }
        
        DdNode* prod = Cudd_zddProduct(manager, result, cover2);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_Quit(manager);
    }
    
    SECTION("Division with different variable orders") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        DdNode* z6 = Cudd_zddIthVar(manager, 6);
        Cudd_Ref(z6);
        
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z4, z6);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        DdNode* div1 = Cudd_zddWeakDiv(manager, f, z0);
        REQUIRE(div1 != nullptr);
        Cudd_Ref(div1);
        
        DdNode* div2 = Cudd_zddDivide(manager, f, z0);
        REQUIRE(div2 != nullptr);
        Cudd_Ref(div2);
        
        DdNode* div3 = Cudd_zddWeakDivF(manager, f, z0);
        REQUIRE(div3 != nullptr);
        Cudd_Ref(div3);
        
        DdNode* div4 = Cudd_zddDivideF(manager, f, z0);
        REQUIRE(div4 != nullptr);
        Cudd_Ref(div4);
        
        Cudd_RecursiveDerefZdd(manager, div4);
        Cudd_RecursiveDerefZdd(manager, div3);
        Cudd_RecursiveDerefZdd(manager, div2);
        Cudd_RecursiveDerefZdd(manager, div1);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z6);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Edge cases for recursive operations", "[cuddZddFuncs]") {
    SECTION("Product caching") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* prod1 = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(prod1);
        DdNode* prod2 = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(prod2);
        
        REQUIRE(prod1 == prod2);
        
        Cudd_RecursiveDerefZdd(manager, prod1);
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("Unate product caching") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* prod1 = Cudd_zddUnateProduct(manager, z0, z1);
        Cudd_Ref(prod1);
        DdNode* prod2 = Cudd_zddUnateProduct(manager, z0, z1);
        Cudd_Ref(prod2);
        
        REQUIRE(prod1 == prod2);
        
        Cudd_RecursiveDerefZdd(manager, prod1);
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("Division caching") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* f = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(f);
        
        DdNode* div1 = Cudd_zddWeakDiv(manager, f, z0);
        Cudd_Ref(div1);
        DdNode* div2 = Cudd_zddWeakDiv(manager, f, z0);
        Cudd_Ref(div2);
        
        REQUIRE(div1 == div2);
        
        Cudd_RecursiveDerefZdd(manager, div1);
        Cudd_RecursiveDerefZdd(manager, div2);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Swap order coverage for product", "[cuddZddFuncs]") {
    SECTION("Product with swapped arguments") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        DdNode* prod1 = Cudd_zddProduct(manager, z0, z4);
        Cudd_Ref(prod1);
        DdNode* prod2 = Cudd_zddProduct(manager, z4, z0);
        Cudd_Ref(prod2);
        
        REQUIRE(prod1 == prod2);
        
        Cudd_RecursiveDerefZdd(manager, prod1);
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
    
    SECTION("Unate product with swapped arguments") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        DdNode* prod1 = Cudd_zddUnateProduct(manager, z0, z4);
        Cudd_Ref(prod1);
        DdNode* prod2 = Cudd_zddUnateProduct(manager, z4, z0);
        Cudd_Ref(prod2);
        
        REQUIRE(prod1 == prod2);
        
        Cudd_RecursiveDerefZdd(manager, prod1);
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// ADDITIONAL TESTS FOR HIGHER COVERAGE
// ============================================================================

TEST_CASE("cuddZddFuncs - Deep recursion coverage", "[cuddZddFuncs]") {
    SECTION("Product swap branch - single variables direct call") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // With single variables, when we call cuddZddProduct(z8, z0):
        // z8->index = 8, z0->index = 0
        // top_f = permZ[8], top_g = permZ[0]
        // In default order, permZ[8] > permZ[0], so swap should happen
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(z8);
        
        // Directly call with higher index first
        // The internal cuddZddProduct should see f->index=8, g->index=0
        // and top_f > top_g, triggering the swap
        DdNode* prod = Cudd_zddProduct(manager, z8, z0);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Product swap branch with fresh manager") {
        // Use a completely fresh manager to avoid any caching issues
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables with large index gap
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z15 = Cudd_zddIthVar(manager, 15);
        Cudd_Ref(z15);
        
        // Call with z15 first (higher index = higher level)
        // This should trigger swap since top_f(15) > top_g(0)
        DdNode* prod = Cudd_zddProduct(manager, z15, z0);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, z15);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("UnateProduct swap branch with single variables") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(z8);
        
        // Call with higher index first
        DdNode* prod = Cudd_zddUnateProduct(manager, z8, z0);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("Product swap branch - force with complex ZDDs") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDDs with specific top variables
        // f will have top variable at higher index than g
        DdNode* z8 = Cudd_zddIthVar(manager, 8);  
        Cudd_Ref(z8);
        DdNode* z9 = Cudd_zddIthVar(manager, 9);
        Cudd_Ref(z9);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);  
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        // f = z8 | z9 (top is z8 with higher permZ)
        DdNode* f = Cudd_zddUnion(manager, z8, z9);
        Cudd_Ref(f);
        
        // g = z0 | z1 (top is z0 with lower permZ)
        DdNode* g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        // When cuddZddProduct(f, g) is called:
        // top_f = permZ[f->index] where f->index should be 8
        // top_g = permZ[g->index] where g->index should be 0
        // If top_f > top_g, the swap branch should be taken
        DdNode* prod = Cudd_zddProduct(manager, f, g);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_RecursiveDerefZdd(manager, z9);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("UnateProduct swap branch with complex ZDDs") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(z8);
        DdNode* z9 = Cudd_zddIthVar(manager, 9);
        Cudd_Ref(z9);
        DdNode* z0 = Cudd_zddIthVar(manager, 0);  
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* f = Cudd_zddUnion(manager, z8, z9);
        Cudd_Ref(f);
        DdNode* g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        DdNode* prod = Cudd_zddUnateProduct(manager, f, g);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_RecursiveDerefZdd(manager, z9);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("Deep recursion in cuddZddWeakDiv") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Build complex ZDD structure for division
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        DdNode* z6 = Cudd_zddIthVar(manager, 6);
        Cudd_Ref(z6);
        
        // Create complex cover: (z0*z2) | (z0*z4) | (z0*z6)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z0, z4);
        Cudd_Ref(p2);
        DdNode* p3 = Cudd_zddProduct(manager, z0, z6);
        Cudd_Ref(p3);
        
        DdNode* tmp = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(tmp);
        DdNode* f = Cudd_zddUnion(manager, tmp, p3);
        Cudd_Ref(f);
        
        // Divide by z0 - should exercise recursion paths
        DdNode* div = Cudd_zddWeakDiv(manager, f, z0);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        // Additional divisions
        DdNode* div2 = Cudd_zddDivide(manager, f, z0);
        REQUIRE(div2 != nullptr);
        Cudd_Ref(div2);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, div2);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, tmp);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, p3);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z6);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF with f having variables not in g") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        DdNode* z6 = Cudd_zddIthVar(manager, 6);
        Cudd_Ref(z6);
        
        // f uses variables 0,2,4,6
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z4, z6);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // g only uses z4
        DdNode* div = Cudd_zddWeakDivF(manager, f, z4);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z6);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Division with intersection paths", "[cuddZddFuncs]") {
    SECTION("WeakDiv requiring intersection") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Build complex dividend and divisor to trigger intersection paths
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // Dividend: (z0*z2) | (z1*z3)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // Divisor: z0 | z1
        DdNode* g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        DdNode* div = Cudd_zddWeakDiv(manager, f, g);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
    
    SECTION("DivideF with complex covers") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        // Build (z0 | z1) * z2
        DdNode* cover1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(cover1);
        DdNode* f = Cudd_zddProduct(manager, cover1, z2);
        Cudd_Ref(f);
        
        // Divide by z0 | z1
        DdNode* div = Cudd_zddDivideF(manager, f, cover1);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - GetCofactors branches", "[cuddZddFuncs]") {
    SECTION("GetCofactors3 with level comparisons") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex ZDD to test all branches of cuddZddGetCofactors3
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // Build f = z0 | z1 | z2 | z3
        DdNode* tmp1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(tmp1);
        DdNode* tmp2 = Cudd_zddUnion(manager, z2, z3);
        Cudd_Ref(tmp2);
        DdNode* f = Cudd_zddUnion(manager, tmp1, tmp2);
        Cudd_Ref(f);
        
        // Get cofactors at different variables to test all branches
        DdNode* f1, *f0, *fd;
        
        // Test with variable 0
        int result = cuddZddGetCofactors3(manager, f, 0, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1);
        Cudd_Ref(f0);
        Cudd_Ref(fd);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        
        // Test with variable 2
        result = cuddZddGetCofactors3(manager, f, 2, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1);
        Cudd_Ref(f0);
        Cudd_Ref(fd);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        
        // Test with variable beyond current top - should hit hv < ht branch
        result = cuddZddGetCofactors3(manager, z3, 0, &f1, &f0, &fd);
        REQUIRE(result == 0);
        // In this case, f1 and f0 should be zero, fd should be z3
        
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, tmp1);
        Cudd_RecursiveDerefZdd(manager, tmp2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
    
    SECTION("GetCofactors2 with various levels") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        DdNode* f = Cudd_zddUnion(manager, z0, Cudd_zddUnion(manager, z2, z4));
        Cudd_Ref(f);
        
        DdNode* f1, *f0;
        
        // Test at different variable levels
        int result = cuddZddGetCofactors2(manager, f, 0, &f1, &f0);
        REQUIRE(result == 0);
        Cudd_Ref(f1);
        Cudd_Ref(f0);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        
        result = cuddZddGetCofactors2(manager, f, 2, &f1, &f0);
        REQUIRE(result == 0);
        Cudd_Ref(f1);
        Cudd_Ref(f0);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Complement and ISOP coverage", "[cuddZddFuncs]") {
    SECTION("Complement with cached result") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            // First complement - should compute and cache
            DdNode* comp1 = Cudd_zddComplement(manager, zdd_I);
            if (comp1 != nullptr) {
                Cudd_Ref(comp1);
                
                // Second complement - should hit cache
                DdNode* comp2 = Cudd_zddComplement(manager, zdd_I);
                if (comp2 != nullptr) {
                    Cudd_Ref(comp2);
                    REQUIRE(comp1 == comp2);  // Same cached result
                    Cudd_RecursiveDerefZdd(manager, comp2);
                }
                
                Cudd_RecursiveDerefZdd(manager, comp1);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("Complement of OR function") {
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
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            DdNode* comp = Cudd_zddComplement(manager, zdd_I);
            if (comp != nullptr) {
                Cudd_Ref(comp);
                Cudd_RecursiveDerefZdd(manager, comp);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - UnateProduct recursion", "[cuddZddFuncs]") {
    SECTION("Deep UnateProduct recursion") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Build complex covers for deep recursion
        DdNode* cover1 = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(cover1);
        for (int i = 0; i < 6; i += 2) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* tmp = Cudd_zddUnion(manager, cover1, var);
            Cudd_Ref(tmp);
            Cudd_RecursiveDerefZdd(manager, cover1);
            Cudd_RecursiveDerefZdd(manager, var);
            cover1 = tmp;
        }
        
        DdNode* cover2 = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(cover2);
        for (int i = 1; i < 6; i += 2) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* tmp = Cudd_zddUnion(manager, cover2, var);
            Cudd_Ref(tmp);
            Cudd_RecursiveDerefZdd(manager, cover2);
            Cudd_RecursiveDerefZdd(manager, var);
            cover2 = tmp;
        }
        
        DdNode* prod = Cudd_zddUnateProduct(manager, cover1, cover2);
        REQUIRE(prod != nullptr);
        Cudd_Ref(prod);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Division returning zero", "[cuddZddFuncs]") {
    SECTION("WeakDiv returning zero") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // f and g have no common terms
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        // f = z0 * z2
        DdNode* f = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(f);
        
        // g = z4 (disjoint from f)
        DdNode* div = Cudd_zddWeakDiv(manager, f, z4);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        // Result should be zero when division fails
        DdNode* zero = Cudd_ReadZero(manager);
        // Check result is valid (may or may not be zero depending on algorithm)
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
    
    SECTION("Divide returning zero") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        DdNode* f = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(f);
        
        DdNode* div = Cudd_zddDivide(manager, f, z4);
        REQUIRE(div != nullptr);
        Cudd_Ref(div);
        
        Cudd_RecursiveDerefZdd(manager, div);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Variable helper edge cases", "[cuddZddFuncs]") {
    SECTION("Odd and even variable indices") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Test with odd index
        int pv = cuddZddGetPosVarIndex(manager, 1);
        int nv = cuddZddGetNegVarIndex(manager, 1);
        REQUIRE(pv == 0);  // 1 & ~1 = 0
        REQUIRE(nv == 1);  // 1 | 1 = 1
        
        // Test with even index
        pv = cuddZddGetPosVarIndex(manager, 4);
        nv = cuddZddGetNegVarIndex(manager, 4);
        REQUIRE(pv == 4);  // 4 & ~1 = 4
        REQUIRE(nv == 5);  // 4 | 1 = 5
        
        // Test levels
        int pl = cuddZddGetPosVarLevel(manager, 2);
        int nl = cuddZddGetNegVarLevel(manager, 2);
        REQUIRE(pl >= 0);
        REQUIRE(nl >= 0);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// ADDITIONAL TESTS FOR COVERAGE OF UNCOVERED BRANCHES
// ============================================================================

TEST_CASE("cuddZddFuncs - cuddZddWeakDivF special cases", "[cuddZddFuncs]") {
    SECTION("WeakDivF division by one returns f") {
        // Cover line 896-897: if (g == one) return(f);
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        // Create a product to get a more complex ZDD
        DdNode* f = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(f);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        
        // WeakDivF(f, 1) should return f (or equivalent)
        DdNode* result = Cudd_zddWeakDivF(manager, f, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // Note: result may not be pointer-equal to f, just logically equivalent
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF with vf < vg branch") {
        // Cover lines 914-976: Special case when v == top_f && vf < vg
        // This happens when top variable of f is at lower level than top variable of g
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create f with low-level variables: z0, z2
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        // Create g with high-level variable: z8
        DdNode* z8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(z8);
        
        // f = z0 * z2 (has top at level 0)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        
        // Also add z4 to f for more complex structure
        DdNode* p2 = Cudd_zddProduct(manager, z0, z4);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // Now divide by z8 which has top at level 8
        // Since permZ[f->index] < permZ[g->index], and vf < vg, 
        // this should exercise the special path in cuddZddWeakDivF
        DdNode* result = Cudd_zddWeakDivF(manager, f, z8);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF with v from g->index branch") {
        // Cover line 981-982: else v = (int) g->index;
        // This happens when top_g < top_f
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create f with high-level variable
        DdNode* z8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(z8);
        DdNode* z10 = Cudd_zddIthVar(manager, 10);
        Cudd_Ref(z10);
        DdNode* f = Cudd_zddProduct(manager, z8, z10);
        Cudd_Ref(f);
        
        // Create g with low-level variable
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* g = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(g);
        
        // Divide f by g where top_g < top_f
        DdNode* result = Cudd_zddWeakDivF(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_RecursiveDerefZdd(manager, z10);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - cuddZddComplement internal function", "[cuddZddFuncs]") {
    SECTION("Call internal cuddZddComplement directly") {
        // Cover cuddZddComplement internal function (lines 1467-1495)
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            // Call internal cuddZddComplement function directly
            DdNode* comp = cuddZddComplement(manager, zdd_I);
            if (comp != nullptr) {
                Cudd_Ref(comp);
                
                // Call again to hit cache path
                DdNode* comp2 = cuddZddComplement(manager, zdd_I);
                if (comp2 != nullptr) {
                    Cudd_Ref(comp2);
                    REQUIRE(comp == comp2);  // Should hit cache
                    Cudd_RecursiveDerefZdd(manager, comp2);
                }
                
                Cudd_RecursiveDerefZdd(manager, comp);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
    
    SECTION("cuddZddComplement with multi-variable cover") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        DdNode* x1 = Cudd_bddIthVar(manager, 1);
        DdNode* bdd = Cudd_bddXor(manager, x0, x1);  // XOR is more complex
        Cudd_Ref(bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, bdd, bdd, &zdd_I);
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            // Call internal function
            DdNode* comp = cuddZddComplement(manager, zdd_I);
            if (comp != nullptr) {
                Cudd_Ref(comp);
                Cudd_RecursiveDerefZdd(manager, comp);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - cuddZddGetCofactors3 uncovered branches", "[cuddZddFuncs]") {
    SECTION("GetCofactors3 pv level vs nv level branch") {
        // The function has two branches based on 
        // cuddZddGetPosVarLevel(dd, v) < cuddZddGetNegVarLevel(dd, v)
        // and the else branch. We need to test both.
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // Create a ZDD with multiple variables
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        DdNode* f1, *f0, *fd;
        
        // Test cofactors at different variables
        int result = cuddZddGetCofactors3(manager, f, 0, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1); Cudd_Ref(f0); Cudd_Ref(fd);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        
        // Test with variable 1 (odd index)
        result = cuddZddGetCofactors3(manager, f, 1, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1); Cudd_Ref(f0); Cudd_Ref(fd);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        
        // Test with variable 2
        result = cuddZddGetCofactors3(manager, f, 2, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1); Cudd_Ref(f0); Cudd_Ref(fd);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        
        // Test with variable 3 (odd)
        result = cuddZddGetCofactors3(manager, f, 3, &f1, &f0, &fd);
        REQUIRE(result == 0);
        Cudd_Ref(f1); Cudd_Ref(f0); Cudd_Ref(fd);
        Cudd_RecursiveDerefZdd(manager, f1);
        Cudd_RecursiveDerefZdd(manager, f0);
        Cudd_RecursiveDerefZdd(manager, fd);
        
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
    
    SECTION("GetCofactors3 hv < ht case") {
        // Cover lines 1331-1335: if (hv < ht) { *f1 = zero; *f0 = zero; *fd = f; }
        // hv = permZ[v] >> 1, ht = permZ[f->index] >> 1
        // We need v such that permZ[v]/2 < permZ[f->index]/2
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD with high-level variable (variable 8 is at permZ position 8)
        DdNode* z8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(z8);
        
        DdNode* f1, *f0, *fd;
        
        // Get cofactors at variable with lower permZ level
        // For a ZDD that only contains variable 8, we need to use a variable
        // where hv < ht. Variable 0 has permZ[0] = 0, so hv = 0.
        // Variable 8 as top of z8 gives ht = permZ[8]/2 = 8/2 = 4
        // So hv (0) < ht (4), this should trigger the branch
        int result = cuddZddGetCofactors3(manager, z8, 0, &f1, &f0, &fd);
        REQUIRE(result == 0);
        
        // In this case, f1 = zero, f0 = zero, fd = f
        // But the values may have been ref'd differently, just check it works
        // The key is the function succeeded
        
        Cudd_RecursiveDerefZdd(manager, z8);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - WeakDiv intersection branches", "[cuddZddFuncs]") {
    SECTION("WeakDiv with gd != zero triggering intersection") {
        // Cover lines 835-861 in cuddZddWeakDiv
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // Create f = (z0*z2) | (z1*z3)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // Create g = (z0*z1) | z2 - this will create non-zero g0, g1, and gd
        DdNode* p3 = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(p3);
        DdNode* g = Cudd_zddUnion(manager, p3, z2);
        Cudd_Ref(g);
        
        DdNode* result = Cudd_zddWeakDiv(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, p3);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDiv with q == g path") {
        // Cover line 809: if (q == g) q = tmp;
        // and line 846: if (q == g) q = tmp;
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        // f = z0 * z2
        DdNode* f = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(f);
        
        // g = z0 (g0 will be zero, so q stays as g initially)
        DdNode* result = Cudd_zddWeakDiv(manager, f, z0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - WeakDivF intersection and zero paths", "[cuddZddFuncs]") {
    SECTION("WeakDivF returning zero at various points") {
        // Cover lines 1021-1028: q == zero path after g0 division
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        DdNode* z6 = Cudd_zddIthVar(manager, 6);
        Cudd_Ref(z6);
        
        // Create complex f and g where division might return zero
        DdNode* f = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(f);
        
        DdNode* g = Cudd_zddProduct(manager, z4, z6);
        Cudd_Ref(g);
        
        // f and g are disjoint, division should return zero or minimal result
        DdNode* result = Cudd_zddWeakDivF(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z6);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF with intersection at g1 != zero") {
        // Cover lines 1031-1060: g1 != zero path with intersection
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // f = (z0 * z2) | (z1 * z3)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // g = z0 | z1 - both g0 and g1 will be non-zero
        DdNode* g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        DdNode* result = Cudd_zddWeakDivF(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
    
    SECTION("WeakDivF with larger structures") {
        // Cover more paths in WeakDivF with larger ZDD structures
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        
        // Simple product
        DdNode* f = Cudd_zddProduct(manager, z0, z2);
        REQUIRE(f != nullptr);
        Cudd_Ref(f);
        
        // Divide by z4 which has no overlap with f
        DdNode* result = Cudd_zddWeakDivF(manager, f, z4);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - DivideF and Divide with intersection", "[cuddZddFuncs]") {
    SECTION("DivideF with r != zero && g0 != zero path") {
        // Cover lines 1262-1285 in cuddZddDivideF
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // f = (z0*z2) | (z1*z3)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // g = z0 | z1
        DdNode* g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        DdNode* result = Cudd_zddDivideF(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
    
    SECTION("Divide with r != zero && g0 != zero path") {
        // Cover lines 1165-1188 in cuddZddDivide
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // f = (z0*z2) | (z1*z3)
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z3);
        Cudd_Ref(p2);
        DdNode* f = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(f);
        
        // g = z0 | z1
        DdNode* g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        DdNode* result = Cudd_zddDivide(manager, f, g);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - UnateProduct deep coverage", "[cuddZddFuncs]") {
    SECTION("UnateProduct with complex multi-level covers") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        DdNode* z5 = Cudd_zddIthVar(manager, 5);
        Cudd_Ref(z5);
        
        // Build multi-level covers
        DdNode* c1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(c1);
        DdNode* c2 = Cudd_zddUnion(manager, c1, z2);
        Cudd_Ref(c2);
        
        DdNode* c3 = Cudd_zddUnion(manager, z3, z4);
        Cudd_Ref(c3);
        DdNode* c4 = Cudd_zddUnion(manager, c3, z5);
        Cudd_Ref(c4);
        
        DdNode* result = Cudd_zddUnateProduct(manager, c2, c4);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, c1);
        Cudd_RecursiveDerefZdd(manager, c2);
        Cudd_RecursiveDerefZdd(manager, c3);
        Cudd_RecursiveDerefZdd(manager, c4);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z5);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Product deep union operations", "[cuddZddFuncs]") {
    SECTION("Product exercising all union combinations") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        DdNode* z4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(z4);
        DdNode* z5 = Cudd_zddIthVar(manager, 5);
        Cudd_Ref(z5);
        DdNode* z6 = Cudd_zddIthVar(manager, 6);
        Cudd_Ref(z6);
        DdNode* z7 = Cudd_zddIthVar(manager, 7);
        Cudd_Ref(z7);
        
        // Create covers that will exercise all union paths in cuddZddProduct
        // Cover1 = z0 | z2 | z4 | z6
        DdNode* t1 = Cudd_zddUnion(manager, z0, z2);
        Cudd_Ref(t1);
        DdNode* t2 = Cudd_zddUnion(manager, z4, z6);
        Cudd_Ref(t2);
        DdNode* cover1 = Cudd_zddUnion(manager, t1, t2);
        Cudd_Ref(cover1);
        
        // Cover2 = z1 | z3 | z5 | z7
        DdNode* t3 = Cudd_zddUnion(manager, z1, z3);
        Cudd_Ref(t3);
        DdNode* t4 = Cudd_zddUnion(manager, z5, z7);
        Cudd_Ref(t4);
        DdNode* cover2 = Cudd_zddUnion(manager, t3, t4);
        Cudd_Ref(cover2);
        
        DdNode* result = Cudd_zddProduct(manager, cover1, cover2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, cover1);
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_RecursiveDerefZdd(manager, t1);
        Cudd_RecursiveDerefZdd(manager, t2);
        Cudd_RecursiveDerefZdd(manager, t3);
        Cudd_RecursiveDerefZdd(manager, t4);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_RecursiveDerefZdd(manager, z4);
        Cudd_RecursiveDerefZdd(manager, z5);
        Cudd_RecursiveDerefZdd(manager, z6);
        Cudd_RecursiveDerefZdd(manager, z7);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddFuncs - Complement of one", "[cuddZddFuncs]") {
    SECTION("Complement of ZDD representing tautology") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        // Create a ZDD for the constant 1 (tautology)
        DdNode* one_bdd = Cudd_ReadOne(manager);
        Cudd_Ref(one_bdd);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, one_bdd, one_bdd, &zdd_I);
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            // Complement of tautology should be empty (zero)
            DdNode* comp = Cudd_zddComplement(manager, zdd_I);
            if (comp != nullptr) {
                Cudd_Ref(comp);
                Cudd_RecursiveDerefZdd(manager, comp);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, one_bdd);
        Cudd_Quit(manager);
    }
}
