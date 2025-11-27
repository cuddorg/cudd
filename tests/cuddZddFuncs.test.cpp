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
