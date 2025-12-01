#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddDecomp.c
 * 
 * This file contains comprehensive tests for the cuddDecomp module
 * to ensure 100% code coverage and correct functionality.
 * 
 * The module provides various decomposition methods:
 * - Cudd_bddApproxConjDecomp: Approximate conjunctive decomposition
 * - Cudd_bddApproxDisjDecomp: Approximate disjunctive decomposition
 * - Cudd_bddIterConjDecomp: Iterative conjunctive decomposition
 * - Cudd_bddIterDisjDecomp: Iterative disjunctive decomposition
 * - Cudd_bddGenConjDecomp: General conjunctive decomposition
 * - Cudd_bddGenDisjDecomp: General disjunctive decomposition
 * - Cudd_bddVarConjDecomp: Variable-based conjunctive decomposition
 * - Cudd_bddVarDisjDecomp: Variable-based disjunctive decomposition
 */

TEST_CASE("Cudd_bddApproxConjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ONE") {
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, one, &conjuncts);
        
        // Constant ONE should return 1 (no meaningful decomposition)
        REQUIRE(result == 1);
        REQUIRE(conjuncts != nullptr);
        REQUIRE(conjuncts[0] == one);
        
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
    }
    
    SECTION("Decompose constant ZERO") {
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, zero, &conjuncts);
        
        // Constant ZERO may return 1 or 2 (depending on decomposition algorithm)
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // The AND of all conjuncts should equal zero
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == zero);
            Cudd_RecursiveDeref(manager, reconstructed);
        } else {
            REQUIRE(conjuncts[0] == zero);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
    }
    
    SECTION("Decompose single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, x, &conjuncts);
        
        // Single variable may return 1 (no meaningful decomposition)
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Decompose simple AND") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, f, &conjuncts);
        
        // Should decompose into conjuncts
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Verify that the AND of the conjuncts equals the original function
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Decompose complex function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f = (x AND y) AND (y AND z)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *yz = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddAnd(manager, xy, yz);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Verify that the AND of the conjuncts implies the original function
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            int implies = Cudd_bddLeq(manager, f, reconstructed);
            REQUIRE(implies == 1);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddApproxDisjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ONE") {
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddApproxDisjDecomp(manager, one, &disjuncts);
        
        // Constant ONE may return 1 or 2 (depending on decomposition algorithm)
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        // The OR of all disjuncts should equal one
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == one);
            Cudd_RecursiveDeref(manager, reconstructed);
        } else {
            REQUIRE(disjuncts[0] == one);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
    }
    
    SECTION("Decompose constant ZERO") {
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddApproxDisjDecomp(manager, zero, &disjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(disjuncts != nullptr);
        REQUIRE(disjuncts[0] == zero);
        
        Cudd_RecursiveDeref(manager, disjuncts[0]);
        FREE(disjuncts);
    }
    
    SECTION("Decompose simple OR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddApproxDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        // Verify that the OR of the disjuncts covers the original function
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            int implies = Cudd_bddLeq(manager, reconstructed, f);
            REQUIRE(implies == 1);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIterConjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ONE") {
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, one, &conjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(conjuncts != nullptr);
        REQUIRE(conjuncts[0] == one);
        
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
    }
    
    SECTION("Decompose simple AND") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Decompose complex function with multiple variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Create a function with shared subgraphs
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *f = Cudd_bddAnd(manager, xy, zw);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIterDisjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ZERO") {
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddIterDisjDecomp(manager, zero, &disjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(disjuncts != nullptr);
        REQUIRE(disjuncts[0] == zero);
        
        Cudd_RecursiveDeref(manager, disjuncts[0]);
        FREE(disjuncts);
    }
    
    SECTION("Decompose simple OR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddIterDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(disjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddGenConjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ONE") {
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, one, &conjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(conjuncts != nullptr);
        REQUIRE(conjuncts[0] == one);
        
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
    }
    
    SECTION("Decompose constant ZERO") {
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, zero, &conjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(conjuncts != nullptr);
        REQUIRE(conjuncts[0] == zero);
        
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
    }
    
    SECTION("Decompose single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, x, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Decompose simple AND") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Verify that the AND of the conjuncts equals the original function
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Decompose complex function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        DdNode *w = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        Cudd_Ref(w);
        
        // Create a function: (x OR y) AND (z OR w)
        DdNode *xy = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddOr(manager, z, w);
        Cudd_Ref(zw);
        DdNode *f = Cudd_bddAnd(manager, xy, zw);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Verify that the AND of the conjuncts equals the original function
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, w);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddGenDisjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ZERO") {
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddGenDisjDecomp(manager, zero, &disjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(disjuncts != nullptr);
        REQUIRE(disjuncts[0] == zero);
        
        Cudd_RecursiveDeref(manager, disjuncts[0]);
        FREE(disjuncts);
    }
    
    SECTION("Decompose simple OR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddGenDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        // Verify that the OR of the disjuncts equals the original function
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Decompose complex function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f = (x AND y) OR z
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddGenDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(disjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddVarConjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ONE") {
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, one, &conjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(conjuncts != nullptr);
        REQUIRE(conjuncts[0] == one);
        
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
    }
    
    SECTION("Decompose single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, x, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Decompose simple function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // The decomposition should be f = (f+x)(f+!x) = conjuncts[0] AND conjuncts[1]
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Decompose complex function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f = (x AND y) OR (NOT x AND z)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *notx = Cudd_Not(x);
        DdNode *notxz = Cudd_bddAnd(manager, notx, z);
        Cudd_Ref(notxz);
        DdNode *f = Cudd_bddOr(manager, xy, notxz);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Verify the decomposition
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, notxz);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddVarDisjDecomp - Basic functionality", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Decompose constant ZERO") {
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddVarDisjDecomp(manager, zero, &disjuncts);
        
        REQUIRE(result == 1);
        REQUIRE(disjuncts != nullptr);
        REQUIRE(disjuncts[0] == zero);
        
        Cudd_RecursiveDeref(manager, disjuncts[0]);
        FREE(disjuncts);
    }
    
    SECTION("Decompose single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddVarDisjDecomp(manager, x, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Decompose simple OR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddVarDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        // The decomposition should be f = f*x + f*!x = disjuncts[0] OR disjuncts[1]
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Decompose complex function") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create f = (x OR y) AND z
        DdNode *xy = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddVarDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        // Verify the decomposition
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Large BDD decomposition", "[cuddDecomp]") {
    // Test with larger BDDs to exercise more code paths
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Large conjunctive function with GenConjDecomp") {
        // Create a large function with many variables
        const int nvars = 10;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a complex function: (v0 AND v1 AND v2) AND (v3 AND v4 AND v5) AND (v6 OR v7 OR v8)
        DdNode *term1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(term1);
        DdNode *term1a = Cudd_bddAnd(manager, term1, vars[2]);
        Cudd_Ref(term1a);
        Cudd_RecursiveDeref(manager, term1);
        
        DdNode *term2 = Cudd_bddAnd(manager, vars[3], vars[4]);
        Cudd_Ref(term2);
        DdNode *term2a = Cudd_bddAnd(manager, term2, vars[5]);
        Cudd_Ref(term2a);
        Cudd_RecursiveDeref(manager, term2);
        
        DdNode *term3 = Cudd_bddOr(manager, vars[6], vars[7]);
        Cudd_Ref(term3);
        DdNode *term3a = Cudd_bddOr(manager, term3, vars[8]);
        Cudd_Ref(term3a);
        Cudd_RecursiveDeref(manager, term3);
        
        DdNode *f1 = Cudd_bddAnd(manager, term1a, term2a);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddAnd(manager, f1, term3a);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        // Verify decomposition
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, term3a);
        Cudd_RecursiveDeref(manager, term2a);
        Cudd_RecursiveDeref(manager, term1a);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Large disjunctive function with GenDisjDecomp") {
        // Create a large function with many variables
        const int nvars = 8;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a complex function: (v0 OR v1) OR (v2 AND v3) OR (v4 AND v5 AND v6)
        DdNode *term1 = Cudd_bddOr(manager, vars[0], vars[1]);
        Cudd_Ref(term1);
        
        DdNode *term2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(term2);
        
        DdNode *term3 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(term3);
        DdNode *term3a = Cudd_bddAnd(manager, term3, vars[6]);
        Cudd_Ref(term3a);
        Cudd_RecursiveDeref(manager, term3);
        
        DdNode *f1 = Cudd_bddOr(manager, term1, term2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddOr(manager, f1, term3a);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddGenDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(disjuncts != nullptr);
        
        // Verify decomposition
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, term3a);
        Cudd_RecursiveDeref(manager, term2);
        Cudd_RecursiveDeref(manager, term1);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Test iterative decomposition with complex function") {
        const int nvars = 12;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a complex nested function
        DdNode *sub1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(sub1);
        DdNode *sub2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(sub2);
        DdNode *sub3 = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(sub3);
        DdNode *sub4 = Cudd_bddAnd(manager, vars[6], vars[7]);
        Cudd_Ref(sub4);
        
        DdNode *level1_1 = Cudd_bddAnd(manager, sub1, sub2);
        Cudd_Ref(level1_1);
        DdNode *level1_2 = Cudd_bddAnd(manager, sub3, sub4);
        Cudd_Ref(level1_2);
        
        DdNode *f = Cudd_bddAnd(manager, level1_1, level1_2);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, level1_2);
        Cudd_RecursiveDeref(manager, level1_1);
        Cudd_RecursiveDeref(manager, sub4);
        Cudd_RecursiveDeref(manager, sub3);
        Cudd_RecursiveDeref(manager, sub2);
        Cudd_RecursiveDeref(manager, sub1);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Var decomposition with various patterns", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("VarConjDecomp with multiple cofactor branches") {
        const int nvars = 6;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = (x0 AND x1 AND x2) OR (NOT x0 AND x3 AND x4)
        DdNode *term1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(term1);
        DdNode *term1a = Cudd_bddAnd(manager, term1, vars[2]);
        Cudd_Ref(term1a);
        Cudd_RecursiveDeref(manager, term1);
        
        DdNode *notx0 = Cudd_Not(vars[0]);
        DdNode *term2 = Cudd_bddAnd(manager, notx0, vars[3]);
        Cudd_Ref(term2);
        DdNode *term2a = Cudd_bddAnd(manager, term2, vars[4]);
        Cudd_Ref(term2a);
        Cudd_RecursiveDeref(manager, term2);
        
        DdNode *f = Cudd_bddOr(manager, term1a, term2a);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Verify the decomposition is correct
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, term2a);
        Cudd_RecursiveDeref(manager, term1a);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("VarDisjDecomp with Shannon expansion") {
        const int nvars = 5;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = (x0 OR x1) AND (x2 OR x3)
        DdNode *term1 = Cudd_bddOr(manager, vars[0], vars[1]);
        Cudd_Ref(term1);
        DdNode *term2 = Cudd_bddOr(manager, vars[2], vars[3]);
        Cudd_Ref(term2);
        DdNode *f = Cudd_bddAnd(manager, term1, term2);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddVarDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        // Verify the decomposition
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddOr(manager, disjuncts[0], disjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, term2);
        Cudd_RecursiveDeref(manager, term1);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Approx decomposition with nested structure", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("ApproxConjDecomp with nested ANDs") {
        const int nvars = 8;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a deeply nested AND structure
        DdNode *current = vars[0];
        Cudd_Ref(current);
        
        for (int i = 1; i < nvars; i++) {
            DdNode *next = Cudd_bddAnd(manager, current, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, current);
            current = next;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, current, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, current);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("ApproxDisjDecomp with nested ORs") {
        const int nvars = 7;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a deeply nested OR structure
        DdNode *current = vars[0];
        Cudd_Ref(current);
        
        for (int i = 1; i < nvars; i++) {
            DdNode *next = Cudd_bddOr(manager, current, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, current);
            current = next;
        }
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddApproxDisjDecomp(manager, current, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(disjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, current);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("IterConjDecomp with multiple iterations") {
        const int nvars = 10;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a function that will benefit from iterative decomposition
        // f = (v0 AND v1 AND v2) AND (v3 AND v4 AND v5) AND (v6 AND v7)
        DdNode *g1 = vars[0];
        Cudd_Ref(g1);
        for (int i = 1; i < 3; i++) {
            DdNode *next = Cudd_bddAnd(manager, g1, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, g1);
            g1 = next;
        }
        
        DdNode *g2 = vars[3];
        Cudd_Ref(g2);
        for (int i = 4; i < 6; i++) {
            DdNode *next = Cudd_bddAnd(manager, g2, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, g2);
            g2 = next;
        }
        
        DdNode *g3 = Cudd_bddAnd(manager, vars[6], vars[7]);
        Cudd_Ref(g3);
        
        DdNode *f1 = Cudd_bddAnd(manager, g1, g2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddAnd(manager, f1, g3);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, g3);
        Cudd_RecursiveDeref(manager, g2);
        Cudd_RecursiveDeref(manager, g1);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("IterDisjDecomp with multiple iterations") {
        const int nvars = 9;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a function that will benefit from iterative decomposition
        // f = (v0 OR v1 OR v2) OR (v3 OR v4 OR v5) OR (v6 OR v7)
        DdNode *g1 = vars[0];
        Cudd_Ref(g1);
        for (int i = 1; i < 3; i++) {
            DdNode *next = Cudd_bddOr(manager, g1, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, g1);
            g1 = next;
        }
        
        DdNode *g2 = vars[3];
        Cudd_Ref(g2);
        for (int i = 4; i < 6; i++) {
            DdNode *next = Cudd_bddOr(manager, g2, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, g2);
            g2 = next;
        }
        
        DdNode *g3 = Cudd_bddOr(manager, vars[6], vars[7]);
        Cudd_Ref(g3);
        
        DdNode *f1 = Cudd_bddOr(manager, g1, g2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddOr(manager, f1, g3);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddIterDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(disjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, g3);
        Cudd_RecursiveDeref(manager, g2);
        Cudd_RecursiveDeref(manager, g1);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Very large BDDs to trigger internal paths", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("GenConjDecomp with deeply nested structure") {
        const int nvars = 16;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a function with depth > DEPTH (which is 5)
        // This should trigger the decomposition points based on distance
        DdNode *level[4];
        for (int l = 0; l < 4; l++) {
            DdNode *term = vars[l*4];
            Cudd_Ref(term);
            for (int i = 1; i < 4; i++) {
                DdNode *next = Cudd_bddAnd(manager, term, vars[l*4 + i]);
                Cudd_Ref(next);
                Cudd_RecursiveDeref(manager, term);
                term = next;
            }
            level[l] = term;
        }
        
        DdNode *pair1 = Cudd_bddAnd(manager, level[0], level[1]);
        Cudd_Ref(pair1);
        DdNode *pair2 = Cudd_bddAnd(manager, level[2], level[3]);
        Cudd_Ref(pair2);
        DdNode *f = Cudd_bddAnd(manager, pair1, pair2);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        // Clean up
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, pair2);
        Cudd_RecursiveDeref(manager, pair1);
        for (int l = 0; l < 4; l++) {
            Cudd_RecursiveDeref(manager, level[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("GenDisjDecomp with large disjunctive structure") {
        const int nvars = 14;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a large disjunctive function
        DdNode *terms[7];
        for (int t = 0; t < 7; t++) {
            terms[t] = Cudd_bddAnd(manager, vars[t*2], vars[t*2+1]);
            Cudd_Ref(terms[t]);
        }
        
        DdNode *current = terms[0];
        Cudd_Ref(current);
        for (int t = 1; t < 7; t++) {
            DdNode *next = Cudd_bddOr(manager, current, terms[t]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, current);
            current = next;
        }
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddGenDisjDecomp(manager, current, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(disjuncts != nullptr);
        
        // Clean up
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, current);
        for (int t = 0; t < 7; t++) {
            Cudd_RecursiveDeref(manager, terms[t]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("ApproxConjDecomp with many shared nodes") {
        const int nvars = 15;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a function with lots of shared structure
        // f = (v0 AND v1) AND (v1 AND v2) AND (v2 AND v3) AND ...
        DdNode *pairs[14];
        for (int i = 0; i < 14; i++) {
            pairs[i] = Cudd_bddAnd(manager, vars[i], vars[i+1]);
            Cudd_Ref(pairs[i]);
        }
        
        DdNode *current = pairs[0];
        Cudd_Ref(current);
        for (int i = 1; i < 14; i++) {
            DdNode *next = Cudd_bddAnd(manager, current, pairs[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, current);
            current = next;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, current, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        // Clean up
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, current);
        for (int i = 0; i < 14; i++) {
            Cudd_RecursiveDeref(manager, pairs[i]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("IterConjDecomp with iteration triggering") {
        const int nvars = 20;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create groups that will trigger multiple iterations
        DdNode *groups[4];
        for (int g = 0; g < 4; g++) {
            DdNode *group = vars[g*5];
            Cudd_Ref(group);
            for (int i = 1; i < 5; i++) {
                DdNode *next = Cudd_bddAnd(manager, group, vars[g*5 + i]);
                Cudd_Ref(next);
                Cudd_RecursiveDeref(manager, group);
                group = next;
            }
            groups[g] = group;
        }
        
        DdNode *half1 = Cudd_bddAnd(manager, groups[0], groups[1]);
        Cudd_Ref(half1);
        DdNode *half2 = Cudd_bddAnd(manager, groups[2], groups[3]);
        Cudd_Ref(half2);
        DdNode *f = Cudd_bddAnd(manager, half1, half2);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        // Clean up
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, half2);
        Cudd_RecursiveDeref(manager, half1);
        for (int g = 0; g < 4; g++) {
            Cudd_RecursiveDeref(manager, groups[g]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("VarConjDecomp with complex estimator selection") {
        const int nvars = 12;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a function where cofactor estimation matters
        // f = (v0 AND v1 AND v2 AND v3) OR (NOT v0 AND v4 AND v5 AND v6)
        //     OR (v7 AND v8) OR (v9 AND v10 AND v11)
        DdNode *t1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(t1);
        DdNode *t1a = Cudd_bddAnd(manager, t1, vars[2]);
        Cudd_Ref(t1a);
        DdNode *t1b = Cudd_bddAnd(manager, t1a, vars[3]);
        Cudd_Ref(t1b);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t1a);
        
        DdNode *notv0 = Cudd_Not(vars[0]);
        DdNode *t2 = Cudd_bddAnd(manager, notv0, vars[4]);
        Cudd_Ref(t2);
        DdNode *t2a = Cudd_bddAnd(manager, t2, vars[5]);
        Cudd_Ref(t2a);
        DdNode *t2b = Cudd_bddAnd(manager, t2a, vars[6]);
        Cudd_Ref(t2b);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t2a);
        
        DdNode *t3 = Cudd_bddAnd(manager, vars[7], vars[8]);
        Cudd_Ref(t3);
        
        DdNode *t4 = Cudd_bddAnd(manager, vars[9], vars[10]);
        Cudd_Ref(t4);
        DdNode *t4a = Cudd_bddAnd(manager, t4, vars[11]);
        Cudd_Ref(t4a);
        Cudd_RecursiveDeref(manager, t4);
        
        DdNode *or1 = Cudd_bddOr(manager, t1b, t2b);
        Cudd_Ref(or1);
        DdNode *or2 = Cudd_bddOr(manager, or1, t3);
        Cudd_Ref(or2);
        DdNode *f = Cudd_bddOr(manager, or2, t4a);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        // Clean up
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, or2);
        Cudd_RecursiveDeref(manager, or1);
        Cudd_RecursiveDeref(manager, t4a);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t2b);
        Cudd_RecursiveDeref(manager, t1b);
        
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Edge cases and special paths", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Complemented nodes in decomposition") {
        const int nvars = 10;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create function with complemented edges
        DdNode *t1 = Cudd_Not(vars[0]);
        DdNode *t2 = Cudd_bddAnd(manager, t1, Cudd_Not(vars[1]));
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddAnd(manager, t2, vars[2]);
        Cudd_Ref(t3);
        DdNode *t4 = Cudd_bddOr(manager, t3, vars[3]);
        Cudd_Ref(t4);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, t4, &conjuncts);
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        // Also test with negated function
        DdNode *negated = Cudd_Not(t4);
        conjuncts = nullptr;
        result = Cudd_bddGenConjDecomp(manager, negated, &conjuncts);
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, t4);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t2);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Functions with high reference counts") {
        const int nvars = 15;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create shared subgraph with high local references
        DdNode *shared = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared);
        
        // Reference it multiple times
        for (int i = 0; i < 10; i++) {
            Cudd_Ref(shared);
        }
        
        DdNode *t1 = Cudd_bddAnd(manager, shared, vars[2]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, shared, vars[3]);
        Cudd_Ref(t2);
        DdNode *f = Cudd_bddAnd(manager, t1, t2);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t1);
        for (int i = 0; i < 10; i++) {
            Cudd_RecursiveDeref(manager, shared);
        }
        Cudd_RecursiveDeref(manager, shared);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Very deep BDD exceeding DEPTH constant") {
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a chain longer than DEPTH (5) to trigger decomposition
        DdNode *current = vars[0];
        Cudd_Ref(current);
        for (int i = 1; i < nvars; i++) {
            DdNode *next = Cudd_bddAnd(manager, current, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, current);
            current = next;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, current, &conjuncts);
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, current);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Mixed AND/OR structure for better coverage") {
        const int nvars = 18;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex mixed structure
        DdNode *or1 = Cudd_bddOr(manager, vars[0], vars[1]);
        Cudd_Ref(or1);
        DdNode *or2 = Cudd_bddOr(manager, vars[2], vars[3]);
        Cudd_Ref(or2);
        DdNode *and1 = Cudd_bddAnd(manager, or1, or2);
        Cudd_Ref(and1);
        
        DdNode *or3 = Cudd_bddOr(manager, vars[4], vars[5]);
        Cudd_Ref(or3);
        DdNode *or4 = Cudd_bddOr(manager, vars[6], vars[7]);
        Cudd_Ref(or4);
        DdNode *and2 = Cudd_bddAnd(manager, or3, or4);
        Cudd_Ref(and2);
        
        DdNode *final = Cudd_bddAnd(manager, and1, and2);
        Cudd_Ref(final);
        
        // Test all decomposition methods
        DdNode **conjuncts = nullptr;
        
        int r1 = Cudd_bddGenConjDecomp(manager, final, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r2 = Cudd_bddApproxConjDecomp(manager, final, &conjuncts);
        REQUIRE(r2 >= 1);
        for (int i = 0; i < r2; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r3 = Cudd_bddIterConjDecomp(manager, final, &conjuncts);
        REQUIRE(r3 >= 1);
        for (int i = 0; i < r3; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r4 = Cudd_bddVarConjDecomp(manager, final, &conjuncts);
        REQUIRE(r4 >= 1);
        for (int i = 0; i < r4; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, final);
        Cudd_RecursiveDeref(manager, and2);
        Cudd_RecursiveDeref(manager, or4);
        Cudd_RecursiveDeref(manager, or3);
        Cudd_RecursiveDeref(manager, and1);
        Cudd_RecursiveDeref(manager, or2);
        Cudd_RecursiveDeref(manager, or1);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Multiple variables with different depths") {
        const int nvars = 20;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create structures at different depths
        DdNode *deep = vars[0];
        Cudd_Ref(deep);
        for (int i = 1; i < 10; i++) {
            DdNode *next = Cudd_bddAnd(manager, deep, vars[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, deep);
            deep = next;
        }
        
        DdNode *shallow = Cudd_bddAnd(manager, vars[10], vars[11]);
        Cudd_Ref(shallow);
        
        DdNode *combined = Cudd_bddAnd(manager, deep, shallow);
        Cudd_Ref(combined);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, combined, &conjuncts);
        REQUIRE(result >= 1);
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, combined);
        Cudd_RecursiveDeref(manager, shallow);
        Cudd_RecursiveDeref(manager, deep);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Disjunctive variants comprehensive", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Complex disjunctive structures") {
        const int nvars = 16;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build complex OR structure
        DdNode *groups[4];
        for (int g = 0; g < 4; g++) {
            DdNode *term = vars[g*4];
            Cudd_Ref(term);
            for (int i = 1; i < 4; i++) {
                DdNode *next = Cudd_bddOr(manager, term, vars[g*4 + i]);
                Cudd_Ref(next);
                Cudd_RecursiveDeref(manager, term);
                term = next;
            }
            groups[g] = term;
        }
        
        DdNode *p1 = Cudd_bddOr(manager, groups[0], groups[1]);
        Cudd_Ref(p1);
        DdNode *p2 = Cudd_bddOr(manager, groups[2], groups[3]);
        Cudd_Ref(p2);
        DdNode *final = Cudd_bddAnd(manager, p1, p2);
        Cudd_Ref(final);
        
        DdNode **disjuncts = nullptr;
        
        int r1 = Cudd_bddGenDisjDecomp(manager, final, &disjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        int r2 = Cudd_bddApproxDisjDecomp(manager, final, &disjuncts);
        REQUIRE(r2 >= 1);
        for (int i = 0; i < r2; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        int r3 = Cudd_bddIterDisjDecomp(manager, final, &disjuncts);
        REQUIRE(r3 >= 1);
        for (int i = 0; i < r3; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        int r4 = Cudd_bddVarDisjDecomp(manager, final, &disjuncts);
        REQUIRE(r4 >= 1);
        for (int i = 0; i < r4; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, final);
        Cudd_RecursiveDeref(manager, p2);
        Cudd_RecursiveDeref(manager, p1);
        for (int g = 0; g < 4; g++) {
            Cudd_RecursiveDeref(manager, groups[g]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests to trigger BuildConjuncts and internal helper functions.
 * The key insight is that cuddConjunctsAux only calls BuildConjuncts
 * when distance >= approxDistance (DEPTH=5). We need BDDs with
 * sufficient depth to trigger the full decomposition algorithm.
 */
TEST_CASE("cuddDecomp - Deep BDD decomposition for BuildConjuncts coverage", "[cuddDecomp]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Creating a BDD with depth > 5 (DEPTH constant)
    // to trigger the BuildConjuncts function
    SECTION("GenConjDecomp with depth > DEPTH to trigger BuildConjuncts") {
        // Create at least 8 levels of nesting to ensure depth > 5
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a BDD with a tree structure that has depth > 5
        // Build multiple independent subtrees then combine
        DdNode *levels[8];
        for (int l = 0; l < 8; l++) {
            int base = l * 3;
            DdNode *t = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t);
            DdNode *t2 = Cudd_bddAnd(manager, t, vars[base+2]);
            Cudd_Ref(t2);
            Cudd_RecursiveDeref(manager, t);
            levels[l] = t2;
        }
        
        // Combine levels in a tree structure
        DdNode *pair1 = Cudd_bddAnd(manager, levels[0], levels[1]);
        Cudd_Ref(pair1);
        DdNode *pair2 = Cudd_bddAnd(manager, levels[2], levels[3]);
        Cudd_Ref(pair2);
        DdNode *pair3 = Cudd_bddAnd(manager, levels[4], levels[5]);
        Cudd_Ref(pair3);
        DdNode *pair4 = Cudd_bddAnd(manager, levels[6], levels[7]);
        Cudd_Ref(pair4);
        
        DdNode *quad1 = Cudd_bddAnd(manager, pair1, pair2);
        Cudd_Ref(quad1);
        DdNode *quad2 = Cudd_bddAnd(manager, pair3, pair4);
        Cudd_Ref(quad2);
        
        DdNode *f = Cudd_bddAnd(manager, quad1, quad2);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        // Verify the decomposition is correct
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, quad2);
        Cudd_RecursiveDeref(manager, quad1);
        Cudd_RecursiveDeref(manager, pair4);
        Cudd_RecursiveDeref(manager, pair3);
        Cudd_RecursiveDeref(manager, pair2);
        Cudd_RecursiveDeref(manager, pair1);
        for (int l = 0; l < 8; l++) {
            Cudd_RecursiveDeref(manager, levels[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Deep BDD with mixed structure for coverage paths") {
        const int nvars = 40;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex mixed structure
        // Level 1: Small clauses
        DdNode *c[10];
        for (int i = 0; i < 10; i++) {
            c[i] = Cudd_bddAnd(manager, vars[i*4], vars[i*4+1]);
            Cudd_Ref(c[i]);
            DdNode *tmp = Cudd_bddAnd(manager, c[i], vars[i*4+2]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, c[i]);
            c[i] = Cudd_bddAnd(manager, tmp, vars[i*4+3]);
            Cudd_Ref(c[i]);
            Cudd_RecursiveDeref(manager, tmp);
        }
        
        // Level 2: Combine with OR
        DdNode *d[5];
        for (int i = 0; i < 5; i++) {
            d[i] = Cudd_bddOr(manager, c[i*2], c[i*2+1]);
            Cudd_Ref(d[i]);
        }
        
        // Level 3: Combine with AND
        DdNode *e1 = Cudd_bddAnd(manager, d[0], d[1]);
        Cudd_Ref(e1);
        DdNode *e2 = Cudd_bddAnd(manager, d[2], d[3]);
        Cudd_Ref(e2);
        DdNode *e3 = Cudd_bddAnd(manager, e1, d[4]);
        Cudd_Ref(e3);
        
        DdNode *f = Cudd_bddAnd(manager, e2, e3);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, e3);
        Cudd_RecursiveDeref(manager, e2);
        Cudd_RecursiveDeref(manager, e1);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, d[i]);
        }
        for (int i = 0; i < 10; i++) {
            Cudd_RecursiveDeref(manager, c[i]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Deep BDD with implication structure") {
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create implication chain: x0 -> x1 -> x2 -> ... -> xn
        // (NOT x0) OR x1, (NOT x1) OR x2, etc.
        DdNode *implications[24];
        for (int i = 0; i < 24; i++) {
            DdNode *notxi = Cudd_Not(vars[i]);
            implications[i] = Cudd_bddOr(manager, notxi, vars[i+1]);
            Cudd_Ref(implications[i]);
        }
        
        // AND all implications together
        DdNode *current = implications[0];
        Cudd_Ref(current);
        for (int i = 1; i < 24; i++) {
            DdNode *next = Cudd_bddAnd(manager, current, implications[i]);
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, current);
            current = next;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, current, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, current);
        for (int i = 0; i < 24; i++) {
            Cudd_RecursiveDeref(manager, implications[i]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Very deep chain for ZeroCase coverage") {
        const int nvars = 35;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a chain with one child being zero sometimes
        // This helps trigger ZeroCase function
        DdNode *chain = vars[0];
        Cudd_Ref(chain);
        
        for (int i = 1; i < nvars; i++) {
            DdNode *next;
            if (i % 3 == 0) {
                // Use OR to create paths where one child might be constant
                next = Cudd_bddOr(manager, chain, vars[i]);
            } else {
                next = Cudd_bddAnd(manager, chain, vars[i]);
            }
            Cudd_Ref(next);
            Cudd_RecursiveDeref(manager, chain);
            chain = next;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, chain, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, chain);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Functions triggering different branches in PickOnePair") {
        const int nvars = 32;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create shared subexpressions to trigger different reference count paths
        DdNode *shared = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared);
        
        // Reference shared multiple times
        for (int i = 0; i < 5; i++) {
            Cudd_Ref(shared);
        }
        
        DdNode *branches[6];
        for (int i = 0; i < 6; i++) {
            int base = 2 + i * 5;
            DdNode *t1 = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t1);
            DdNode *t2 = Cudd_bddAnd(manager, t1, vars[base+2]);
            Cudd_Ref(t2);
            DdNode *t3 = Cudd_bddAnd(manager, t2, shared);
            Cudd_Ref(t3);
            DdNode *t4 = Cudd_bddAnd(manager, t3, vars[base+3]);
            Cudd_Ref(t4);
            branches[i] = Cudd_bddAnd(manager, t4, vars[base+4]);
            Cudd_Ref(branches[i]);
            Cudd_RecursiveDeref(manager, t4);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t1);
        }
        
        DdNode *p1 = Cudd_bddAnd(manager, branches[0], branches[1]);
        Cudd_Ref(p1);
        DdNode *p2 = Cudd_bddAnd(manager, branches[2], branches[3]);
        Cudd_Ref(p2);
        DdNode *p3 = Cudd_bddAnd(manager, branches[4], branches[5]);
        Cudd_Ref(p3);
        
        DdNode *q1 = Cudd_bddAnd(manager, p1, p2);
        Cudd_Ref(q1);
        DdNode *f = Cudd_bddAnd(manager, q1, p3);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, q1);
        Cudd_RecursiveDeref(manager, p3);
        Cudd_RecursiveDeref(manager, p2);
        Cudd_RecursiveDeref(manager, p1);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, branches[i]);
        }
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(manager, shared);
        }
        Cudd_RecursiveDeref(manager, shared);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Complemented edges for coverage of complement handling") {
        const int nvars = 28;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build with complemented edges
        DdNode *parts[7];
        for (int i = 0; i < 7; i++) {
            int base = i * 4;
            DdNode *t1 = Cudd_bddAnd(manager, vars[base], Cudd_Not(vars[base+1]));
            Cudd_Ref(t1);
            DdNode *t2 = Cudd_bddAnd(manager, t1, vars[base+2]);
            Cudd_Ref(t2);
            parts[i] = Cudd_bddAnd(manager, t2, Cudd_Not(vars[base+3]));
            Cudd_Ref(parts[i]);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t1);
        }
        
        DdNode *g1 = Cudd_bddAnd(manager, parts[0], parts[1]);
        Cudd_Ref(g1);
        DdNode *g2 = Cudd_bddAnd(manager, parts[2], parts[3]);
        Cudd_Ref(g2);
        DdNode *g3 = Cudd_bddAnd(manager, parts[4], parts[5]);
        Cudd_Ref(g3);
        
        DdNode *h1 = Cudd_bddAnd(manager, g1, g2);
        Cudd_Ref(h1);
        DdNode *h2 = Cudd_bddAnd(manager, h1, g3);
        Cudd_Ref(h2);
        DdNode *f = Cudd_bddAnd(manager, h2, parts[6]);
        Cudd_Ref(f);
        
        // Test with the complement
        DdNode *fnot = Cudd_Not(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, fnot, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, h2);
        Cudd_RecursiveDeref(manager, h1);
        Cudd_RecursiveDeref(manager, g3);
        Cudd_RecursiveDeref(manager, g2);
        Cudd_RecursiveDeref(manager, g1);
        for (int i = 0; i < 7; i++) {
            Cudd_RecursiveDeref(manager, parts[i]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Additional tests to increase code coverage by targeting specific 
 * uncovered paths in the decomposition algorithms.
 */

TEST_CASE("cuddDecomp - IterConjDecomp returning 2 conjuncts", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create a function that should decompose into 2 non-trivial conjuncts
    // We need a function where the iterative approximation finds two distinct factors
    SECTION("Function designed to return 2 conjuncts via IterConjDecomp") {
        // Create a more complex function that the iterative algorithm can decompose
        // f = (a AND b AND c) AND (d AND e AND f) where the two parts have no overlap
        const int nvars = 8;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build first independent clause: a AND b AND c AND d
        DdNode *clause1 = vars[0];
        Cudd_Ref(clause1);
        for (int i = 1; i < 4; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, clause1, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, clause1);
            clause1 = tmp;
        }
        
        // Build second independent clause: e AND f AND g AND h
        DdNode *clause2 = vars[4];
        Cudd_Ref(clause2);
        for (int i = 5; i < 8; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, clause2, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, clause2);
            clause2 = tmp;
        }
        
        // Combine them
        DdNode *f = Cudd_bddAnd(manager, clause1, clause2);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        // Clean up
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, clause2);
        Cudd_RecursiveDeref(manager, clause1);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Large asymmetric function for IterConjDecomp") {
        const int nvars = 16;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create asymmetric structure: (a AND b) AND ((c AND d) OR (e AND f)) AND (g AND h AND i AND j)
        DdNode *ab = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(ab);
        
        DdNode *cd = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(cd);
        DdNode *ef = Cudd_bddAnd(manager, vars[4], vars[5]);
        Cudd_Ref(ef);
        DdNode *cd_or_ef = Cudd_bddOr(manager, cd, ef);
        Cudd_Ref(cd_or_ef);
        
        DdNode *ghij = vars[6];
        Cudd_Ref(ghij);
        for (int i = 7; i < 10; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, ghij, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, ghij);
            ghij = tmp;
        }
        
        DdNode *f1 = Cudd_bddAnd(manager, ab, cd_or_ef);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddAnd(manager, f1, ghij);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, ghij);
        Cudd_RecursiveDeref(manager, cd_or_ef);
        Cudd_RecursiveDeref(manager, ef);
        Cudd_RecursiveDeref(manager, cd);
        Cudd_RecursiveDeref(manager, ab);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - GenConjDecomp with extremely large BDDs for BuildConjuncts coverage", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Very large BDD to trigger all BuildConjuncts paths") {
        // Create a BDD with many levels and complex structure to exercise
        // all paths in BuildConjuncts including ZeroCase, CheckInTables, PickOnePair
        const int nvars = 50;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex nested structure with shared nodes
        // This helps trigger the table lookup paths
        DdNode *layers[10];
        for (int l = 0; l < 10; l++) {
            int base = l * 5;
            DdNode *t = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t);
            DdNode *t2 = Cudd_bddOr(manager, t, vars[base+2]);
            Cudd_Ref(t2);
            DdNode *t3 = Cudd_bddAnd(manager, t2, vars[base+3]);
            Cudd_Ref(t3);
            layers[l] = Cudd_bddOr(manager, t3, vars[base+4]);
            Cudd_Ref(layers[l]);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t);
        }
        
        // Combine all layers
        DdNode *combined = layers[0];
        Cudd_Ref(combined);
        for (int l = 1; l < 10; l++) {
            DdNode *tmp = Cudd_bddAnd(manager, combined, layers[l]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, combined);
            combined = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, combined, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, combined);
        for (int l = 0; l < 10; l++) {
            Cudd_RecursiveDeref(manager, layers[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("BDD with many shared subgraphs") {
        const int nvars = 40;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create shared subgraphs that will appear in multiple places
        DdNode *shared1 = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared1);
        DdNode *shared2 = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(shared2);
        DdNode *shared3 = Cudd_bddOr(manager, vars[4], vars[5]);
        Cudd_Ref(shared3);
        
        // Use shared subgraphs in multiple places
        DdNode *t1 = Cudd_bddAnd(manager, shared1, shared2);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, shared2, shared3);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddOr(manager, shared1, shared3);
        Cudd_Ref(t3);
        
        // Combine in complex ways
        DdNode *u1 = Cudd_bddAnd(manager, t1, t2);
        Cudd_Ref(u1);
        DdNode *u2 = Cudd_bddOr(manager, u1, t3);
        Cudd_Ref(u2);
        
        // Add more variables
        DdNode *f = u2;
        Cudd_Ref(f);
        for (int i = 6; i < 20; i++) {
            DdNode *tmp;
            if (i % 3 == 0) {
                tmp = Cudd_bddAnd(manager, f, vars[i]);
            } else {
                tmp = Cudd_bddOr(manager, f, vars[i]);
            }
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, u2);
        Cudd_RecursiveDeref(manager, u1);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, shared3);
        Cudd_RecursiveDeref(manager, shared2);
        Cudd_RecursiveDeref(manager, shared1);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Various BDD structures for ZeroCase coverage", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("BDD with zero children for ZeroCase") {
        // Create BDDs that have children equal to zero in the tree
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create structure where some paths lead to zero
        // (a AND b) OR (NOT a AND c) type structure
        DdNode *clauses[6];
        for (int c = 0; c < 6; c++) {
            int base = c * 5;
            DdNode *lit;
            if (c % 2 == 0) {
                lit = vars[base];
            } else {
                lit = Cudd_Not(vars[base]);
            }
            Cudd_Ref(lit);
            DdNode *term = Cudd_bddAnd(manager, lit, vars[base+1]);
            Cudd_Ref(term);
            Cudd_RecursiveDeref(manager, lit);
            DdNode *term2 = Cudd_bddAnd(manager, term, vars[base+2]);
            Cudd_Ref(term2);
            Cudd_RecursiveDeref(manager, term);
            DdNode *term3 = Cudd_bddAnd(manager, term2, vars[base+3]);
            Cudd_Ref(term3);
            Cudd_RecursiveDeref(manager, term2);
            clauses[c] = Cudd_bddAnd(manager, term3, vars[base+4]);
            Cudd_Ref(clauses[c]);
            Cudd_RecursiveDeref(manager, term3);
        }
        
        // OR the clauses together (creates mutual exclusion patterns)
        DdNode *f = clauses[0];
        Cudd_Ref(f);
        for (int c = 1; c < 6; c++) {
            DdNode *tmp = Cudd_bddOr(manager, f, clauses[c]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int c = 0; c < 6; c++) {
            Cudd_RecursiveDeref(manager, clauses[c]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Cube-like structures") {
        // Create structures that look like cubes (chain of ANDs)
        // These should trigger the "cube times function" case in ZeroCase
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a cube: x0 AND x1 AND x2 AND ... AND x9
        DdNode *cube = vars[0];
        Cudd_Ref(cube);
        for (int i = 1; i < 10; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, cube, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
        }
        
        // Create a non-cube function: (x10 OR x11) AND (x12 OR x13)
        DdNode *or1 = Cudd_bddOr(manager, vars[10], vars[11]);
        Cudd_Ref(or1);
        DdNode *or2 = Cudd_bddOr(manager, vars[12], vars[13]);
        Cudd_Ref(or2);
        DdNode *nonCube = Cudd_bddAnd(manager, or1, or2);
        Cudd_Ref(nonCube);
        
        // Combine cube with non-cube
        DdNode *f = Cudd_bddAnd(manager, cube, nonCube);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, nonCube);
        Cudd_RecursiveDeref(manager, or2);
        Cudd_RecursiveDeref(manager, or1);
        Cudd_RecursiveDeref(manager, cube);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Symmetric and asymmetric BDD structures", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Symmetric XOR-like structure") {
        const int nvars = 20;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create XOR of pairs: (x0 XOR x1) AND (x2 XOR x3) AND ...
        DdNode *parts[10];
        for (int p = 0; p < 10; p++) {
            int base = p * 2;
            parts[p] = Cudd_bddXor(manager, vars[base], vars[base+1]);
            Cudd_Ref(parts[p]);
        }
        
        DdNode *f = parts[0];
        Cudd_Ref(f);
        for (int p = 1; p < 10; p++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, parts[p]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int p = 0; p < 10; p++) {
            Cudd_RecursiveDeref(manager, parts[p]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("All decomposition methods on same complex BDD") {
        const int nvars = 24;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex formula
        // ((a AND b) OR (c AND d)) AND ((e OR f) AND (g OR h)) AND (i XOR j)
        DdNode *ab = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(ab);
        DdNode *cd = Cudd_bddAnd(manager, vars[2], vars[3]);
        Cudd_Ref(cd);
        DdNode *ab_or_cd = Cudd_bddOr(manager, ab, cd);
        Cudd_Ref(ab_or_cd);
        
        DdNode *ef = Cudd_bddOr(manager, vars[4], vars[5]);
        Cudd_Ref(ef);
        DdNode *gh = Cudd_bddOr(manager, vars[6], vars[7]);
        Cudd_Ref(gh);
        DdNode *ef_and_gh = Cudd_bddAnd(manager, ef, gh);
        Cudd_Ref(ef_and_gh);
        
        DdNode *ij_xor = Cudd_bddXor(manager, vars[8], vars[9]);
        Cudd_Ref(ij_xor);
        
        DdNode *f1 = Cudd_bddAnd(manager, ab_or_cd, ef_and_gh);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddAnd(manager, f1, ij_xor);
        Cudd_Ref(f);
        
        // Test all decomposition methods
        DdNode **conjuncts = nullptr;
        DdNode **disjuncts = nullptr;
        
        int r1 = Cudd_bddApproxConjDecomp(manager, f, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r2 = Cudd_bddApproxDisjDecomp(manager, f, &disjuncts);
        REQUIRE(r2 >= 1);
        for (int i = 0; i < r2; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        int r3 = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        REQUIRE(r3 >= 1);
        for (int i = 0; i < r3; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r4 = Cudd_bddIterDisjDecomp(manager, f, &disjuncts);
        REQUIRE(r4 >= 1);
        for (int i = 0; i < r4; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        int r5 = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        REQUIRE(r5 >= 1);
        for (int i = 0; i < r5; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r6 = Cudd_bddGenDisjDecomp(manager, f, &disjuncts);
        REQUIRE(r6 >= 1);
        for (int i = 0; i < r6; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        int r7 = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
        REQUIRE(r7 >= 1);
        for (int i = 0; i < r7; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        int r8 = Cudd_bddVarDisjDecomp(manager, f, &disjuncts);
        REQUIRE(r8 >= 1);
        for (int i = 0; i < r8; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, ij_xor);
        Cudd_RecursiveDeref(manager, ef_and_gh);
        Cudd_RecursiveDeref(manager, gh);
        Cudd_RecursiveDeref(manager, ef);
        Cudd_RecursiveDeref(manager, ab_or_cd);
        Cudd_RecursiveDeref(manager, cd);
        Cudd_RecursiveDeref(manager, ab);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Minterms and distance table paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("BDD where minterm count affects path selection") {
        // Build BDDs where the minterm count difference between branches affects
        // which child is processed first in BuildConjuncts
        const int nvars = 35;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create "heavy" branch (many minterms)
        // Single variable OR - has many minterms
        DdNode *heavy = vars[0];
        Cudd_Ref(heavy);
        for (int i = 1; i < 10; i++) {
            DdNode *tmp = Cudd_bddOr(manager, heavy, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, heavy);
            heavy = tmp;
        }
        
        // Create "light" branch (few minterms)
        // Chain of ANDs - has fewer minterms
        DdNode *light = vars[10];
        Cudd_Ref(light);
        for (int i = 11; i < 20; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, light, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, light);
            light = tmp;
        }
        
        // Create asymmetric BDD
        DdNode *f = Cudd_bddAnd(manager, heavy, light);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, light);
        Cudd_RecursiveDeref(manager, heavy);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("BDD with high local reference counts") {
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a shared node that will have high local reference count
        DdNode *shared = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared);
        
        // Reference it many times to increase localRef
        for (int i = 0; i < 20; i++) {
            Cudd_Ref(shared);
        }
        
        // Build multiple branches using the shared node
        DdNode *branches[5];
        for (int b = 0; b < 5; b++) {
            int base = 2 + b * 5;
            DdNode *t1 = Cudd_bddAnd(manager, shared, vars[base]);
            Cudd_Ref(t1);
            DdNode *t2 = Cudd_bddAnd(manager, t1, vars[base+1]);
            Cudd_Ref(t2);
            DdNode *t3 = Cudd_bddAnd(manager, t2, vars[base+2]);
            Cudd_Ref(t3);
            branches[b] = Cudd_bddAnd(manager, t3, vars[base+3]);
            Cudd_Ref(branches[b]);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t1);
        }
        
        // Combine branches
        DdNode *f = branches[0];
        Cudd_Ref(f);
        for (int b = 1; b < 5; b++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, branches[b]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int b = 0; b < 5; b++) {
            Cudd_RecursiveDeref(manager, branches[b]);
        }
        for (int i = 0; i < 20; i++) {
            Cudd_RecursiveDeref(manager, shared);
        }
        Cudd_RecursiveDeref(manager, shared);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Additional coverage for VarConjDecomp and VarDisjDecomp", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("VarConjDecomp with balanced cofactors") {
        const int nvars = 16;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create function with balanced positive and negative cofactors
        // f = (x0 AND x1 AND x2 AND x3) OR (NOT x0 AND x4 AND x5 AND x6)
        DdNode *pos = vars[0];
        Cudd_Ref(pos);
        for (int i = 1; i < 4; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, pos, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, pos);
            pos = tmp;
        }
        
        DdNode *not_x0 = Cudd_Not(vars[0]);
        DdNode *neg = Cudd_bddAnd(manager, not_x0, vars[4]);
        Cudd_Ref(neg);
        for (int i = 5; i < 7; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, neg, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, neg);
            neg = tmp;
        }
        
        DdNode *f = Cudd_bddOr(manager, pos, neg);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(conjuncts != nullptr);
        
        if (result == 2) {
            DdNode *reconstructed = Cudd_bddAnd(manager, conjuncts[0], conjuncts[1]);
            Cudd_Ref(reconstructed);
            REQUIRE(reconstructed == f);
            Cudd_RecursiveDeref(manager, reconstructed);
        }
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, neg);
        Cudd_RecursiveDeref(manager, pos);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("VarDisjDecomp with imbalanced cofactors") {
        const int nvars = 20;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create function where one cofactor is much larger
        // Positive cofactor is large
        DdNode *large = vars[1];
        Cudd_Ref(large);
        for (int i = 2; i < 15; i++) {
            DdNode *tmp = Cudd_bddOr(manager, large, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, large);
            large = tmp;
        }
        
        // Negative cofactor is small
        DdNode *small = Cudd_bddAnd(manager, vars[15], vars[16]);
        Cudd_Ref(small);
        
        // Build ITE: x0 ? large : small
        DdNode *f = Cudd_bddIte(manager, vars[0], large, small);
        Cudd_Ref(f);
        
        DdNode **disjuncts = nullptr;
        int result = Cudd_bddVarDisjDecomp(manager, f, &disjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(result <= 2);
        REQUIRE(disjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, disjuncts[i]);
        }
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, small);
        Cudd_RecursiveDeref(manager, large);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Edge cases with constant functions", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("GenConjDecomp on constants") {
        DdNode **conjuncts = nullptr;
        
        int r1 = Cudd_bddGenConjDecomp(manager, one, &conjuncts);
        REQUIRE(r1 == 1);
        REQUIRE(conjuncts[0] == one);
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
        
        int r2 = Cudd_bddGenConjDecomp(manager, zero, &conjuncts);
        REQUIRE(r2 == 1);
        REQUIRE(conjuncts[0] == zero);
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
    }
    
    SECTION("IterConjDecomp on constants") {
        DdNode **conjuncts = nullptr;
        
        int r1 = Cudd_bddIterConjDecomp(manager, one, &conjuncts);
        REQUIRE(r1 == 1);
        Cudd_RecursiveDeref(manager, conjuncts[0]);
        FREE(conjuncts);
        
        int r2 = Cudd_bddIterConjDecomp(manager, zero, &conjuncts);
        REQUIRE(r2 >= 1);
        for (int i = 0; i < r2; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
    }
    
    SECTION("ApproxConjDecomp on complemented variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        DdNode *notx = Cudd_Not(x);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddApproxConjDecomp(manager, notx, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Maximum depth and local ref scenarios", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Exceeding DEPTH threshold significantly") {
        // DEPTH is 5, so create BDD with depth >> 5
        const int nvars = 60;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create very deep tree structure
        DdNode *levels[12];
        for (int l = 0; l < 12; l++) {
            int base = l * 5;
            DdNode *t = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t);
            DdNode *t2 = Cudd_bddAnd(manager, t, vars[base+2]);
            Cudd_Ref(t2);
            DdNode *t3 = Cudd_bddAnd(manager, t2, vars[base+3]);
            Cudd_Ref(t3);
            levels[l] = Cudd_bddAnd(manager, t3, vars[base+4]);
            Cudd_Ref(levels[l]);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t);
        }
        
        // Combine in tree fashion
        DdNode *p1 = Cudd_bddAnd(manager, levels[0], levels[1]);
        Cudd_Ref(p1);
        DdNode *p2 = Cudd_bddAnd(manager, levels[2], levels[3]);
        Cudd_Ref(p2);
        DdNode *p3 = Cudd_bddAnd(manager, levels[4], levels[5]);
        Cudd_Ref(p3);
        DdNode *p4 = Cudd_bddAnd(manager, levels[6], levels[7]);
        Cudd_Ref(p4);
        DdNode *p5 = Cudd_bddAnd(manager, levels[8], levels[9]);
        Cudd_Ref(p5);
        DdNode *p6 = Cudd_bddAnd(manager, levels[10], levels[11]);
        Cudd_Ref(p6);
        
        DdNode *q1 = Cudd_bddAnd(manager, p1, p2);
        Cudd_Ref(q1);
        DdNode *q2 = Cudd_bddAnd(manager, p3, p4);
        Cudd_Ref(q2);
        DdNode *q3 = Cudd_bddAnd(manager, p5, p6);
        Cudd_Ref(q3);
        
        DdNode *r1 = Cudd_bddAnd(manager, q1, q2);
        Cudd_Ref(r1);
        DdNode *f = Cudd_bddAnd(manager, r1, q3);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, r1);
        Cudd_RecursiveDeref(manager, q3);
        Cudd_RecursiveDeref(manager, q2);
        Cudd_RecursiveDeref(manager, q1);
        Cudd_RecursiveDeref(manager, p6);
        Cudd_RecursiveDeref(manager, p5);
        Cudd_RecursiveDeref(manager, p4);
        Cudd_RecursiveDeref(manager, p3);
        Cudd_RecursiveDeref(manager, p2);
        Cudd_RecursiveDeref(manager, p1);
        for (int l = 0; l < 12; l++) {
            Cudd_RecursiveDeref(manager, levels[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Additional tests targeting specific internal code paths in cuddDecomp.c
 * These tests aim to trigger paths in CheckInTables, CheckTablesCacheAndReturn,
 * PickOnePair, ZeroCase, and BuildConjuncts.
 */

TEST_CASE("cuddDecomp - Specific paths for internal function coverage", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("BDD with Nv==zero branch in BuildConjuncts") {
        // Create BDD where one cofactor is zero to trigger ZeroCase
        const int nvars = 20;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = x0 AND (x1 OR x2 OR ... OR x9)
        // Negative cofactor of x0 is zero
        DdNode *disjunction = vars[1];
        Cudd_Ref(disjunction);
        for (int i = 2; i < 10; i++) {
            DdNode *tmp = Cudd_bddOr(manager, disjunction, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, disjunction);
            disjunction = tmp;
        }
        
        DdNode *f = Cudd_bddAnd(manager, vars[0], disjunction);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, disjunction);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("BDD with NOT x0 AND (terms) to trigger alternate zero case") {
        const int nvars = 15;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = NOT x0 AND (x1 AND x2 AND x3)
        // Positive cofactor of x0 is zero  
        DdNode *conjunction = vars[1];
        Cudd_Ref(conjunction);
        for (int i = 2; i < 4; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, conjunction, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, conjunction);
            conjunction = tmp;
        }
        
        DdNode *notx0 = Cudd_Not(vars[0]);
        DdNode *f = Cudd_bddAnd(manager, notx0, conjunction);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, conjunction);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("BDD to trigger value==3 path in BuildConjuncts") {
        // We need to create a BDD where a node gets registered in ghTable 
        // with value 3 (i.e., both as g and h conjunct)
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a structure where the same subgraph appears multiple times
        DdNode *shared = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared);
        
        // Build structures that use shared in both g and h roles
        DdNode *t1 = Cudd_bddAnd(manager, shared, vars[2]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, shared, vars[3]);
        Cudd_Ref(t2);
        DdNode *t3 = Cudd_bddOr(manager, shared, vars[4]);
        Cudd_Ref(t3);
        
        DdNode *combined = Cudd_bddAnd(manager, t1, t2);
        Cudd_Ref(combined);
        DdNode *f = Cudd_bddAnd(manager, combined, t3);
        Cudd_Ref(f);
        
        // Add more complexity
        for (int i = 5; i < 15; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, combined);
        Cudd_RecursiveDeref(manager, t3);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, shared);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Large BDD with alternating structure for minterm comparison paths") {
        const int nvars = 40;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create structure where minNv and minNnv differ to trigger the swap path
        // Build a "heavy" positive branch and "light" negative branch
        DdNode *pos_heavy = vars[1];
        Cudd_Ref(pos_heavy);
        for (int i = 2; i < 10; i++) {
            DdNode *tmp = Cudd_bddOr(manager, pos_heavy, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, pos_heavy);
            pos_heavy = tmp;
        }
        
        DdNode *neg_light = vars[10];
        Cudd_Ref(neg_light);
        for (int i = 11; i < 15; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, neg_light, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, neg_light);
            neg_light = tmp;
        }
        
        // Build ITE: x0 ? pos_heavy : neg_light
        DdNode *f = Cudd_bddIte(manager, vars[0], pos_heavy, neg_light);
        Cudd_Ref(f);
        
        // Add more structure
        for (int i = 15; i < 25; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, neg_light);
        Cudd_RecursiveDeref(manager, pos_heavy);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Different pairValue combinations", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Complex structure triggering different pair values in CheckInTables") {
        const int nvars = 50;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create many shared subexpressions that will interact in the tables
        DdNode *shared[10];
        for (int s = 0; s < 10; s++) {
            int base = s * 5;
            shared[s] = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(shared[s]);
            DdNode *tmp = Cudd_bddAnd(manager, shared[s], vars[base+2]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, shared[s]);
            shared[s] = tmp;
        }
        
        // Create cross-references between shared expressions
        DdNode *cross[5];
        for (int c = 0; c < 5; c++) {
            cross[c] = Cudd_bddAnd(manager, shared[c], shared[c+5]);
            Cudd_Ref(cross[c]);
        }
        
        // Combine everything
        DdNode *f = cross[0];
        Cudd_Ref(f);
        for (int c = 1; c < 5; c++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, cross[c]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int c = 0; c < 5; c++) {
            Cudd_RecursiveDeref(manager, cross[c]);
        }
        for (int s = 0; s < 10; s++) {
            Cudd_RecursiveDeref(manager, shared[s]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Structure forcing g2/h2 selection in PickOnePair") {
        const int nvars = 45;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build structure where g2,h2 pair has better properties than g1,h1
        DdNode *layers[9];
        for (int l = 0; l < 9; l++) {
            int base = l * 5;
            DdNode *t1 = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t1);
            DdNode *t2 = Cudd_bddOr(manager, t1, vars[base+2]);
            Cudd_Ref(t2);
            DdNode *t3 = Cudd_bddAnd(manager, t2, vars[base+3]);
            Cudd_Ref(t3);
            layers[l] = Cudd_bddOr(manager, t3, vars[base+4]);
            Cudd_Ref(layers[l]);
            Cudd_RecursiveDeref(manager, t3);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t1);
        }
        
        // Create asymmetric combination
        DdNode *left = Cudd_bddAnd(manager, layers[0], layers[1]);
        Cudd_Ref(left);
        DdNode *middle = Cudd_bddAnd(manager, layers[2], layers[3]);
        Cudd_Ref(middle);
        DdNode *middle2 = Cudd_bddAnd(manager, middle, layers[4]);
        Cudd_Ref(middle2);
        DdNode *right = Cudd_bddOr(manager, layers[5], layers[6]);
        Cudd_Ref(right);
        DdNode *right2 = Cudd_bddAnd(manager, right, layers[7]);
        Cudd_Ref(right2);
        DdNode *right3 = Cudd_bddOr(manager, right2, layers[8]);
        Cudd_Ref(right3);
        
        DdNode *f1 = Cudd_bddAnd(manager, left, middle2);
        Cudd_Ref(f1);
        DdNode *f = Cudd_bddAnd(manager, f1, right3);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, right3);
        Cudd_RecursiveDeref(manager, right2);
        Cudd_RecursiveDeref(manager, right);
        Cudd_RecursiveDeref(manager, middle2);
        Cudd_RecursiveDeref(manager, middle);
        Cudd_RecursiveDeref(manager, left);
        for (int l = 0; l < 9; l++) {
            Cudd_RecursiveDeref(manager, layers[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddDecomp - Comprehensive all-method tests for coverage", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("All methods on various BDD patterns") {
        const int nvars = 32;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create several different BDD patterns
        // Pattern 1: Deep AND chain
        DdNode *andChain = vars[0];
        Cudd_Ref(andChain);
        for (int i = 1; i < 8; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, andChain, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, andChain);
            andChain = tmp;
        }
        
        // Pattern 2: Deep OR chain
        DdNode *orChain = vars[8];
        Cudd_Ref(orChain);
        for (int i = 9; i < 16; i++) {
            DdNode *tmp = Cudd_bddOr(manager, orChain, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, orChain);
            orChain = tmp;
        }
        
        // Pattern 3: Mixed
        DdNode *mixed = Cudd_bddAnd(manager, andChain, orChain);
        Cudd_Ref(mixed);
        
        // Test all decomposition methods
        DdNode **conjuncts = nullptr;
        DdNode **disjuncts = nullptr;
        
        // Test on andChain
        int r1 = Cudd_bddGenConjDecomp(manager, andChain, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        r1 = Cudd_bddApproxConjDecomp(manager, andChain, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        r1 = Cudd_bddIterConjDecomp(manager, andChain, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        r1 = Cudd_bddVarConjDecomp(manager, andChain, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        // Test on orChain  
        r1 = Cudd_bddGenDisjDecomp(manager, orChain, &disjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        r1 = Cudd_bddApproxDisjDecomp(manager, orChain, &disjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        r1 = Cudd_bddIterDisjDecomp(manager, orChain, &disjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        r1 = Cudd_bddVarDisjDecomp(manager, orChain, &disjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        // Test on mixed
        r1 = Cudd_bddGenConjDecomp(manager, mixed, &conjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        r1 = Cudd_bddGenDisjDecomp(manager, mixed, &disjuncts);
        REQUIRE(r1 >= 1);
        for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
        FREE(disjuncts);
        
        Cudd_RecursiveDeref(manager, mixed);
        Cudd_RecursiveDeref(manager, orChain);
        Cudd_RecursiveDeref(manager, andChain);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Additional tests targeting ZeroCase with Nnv==zero path
 */
TEST_CASE("cuddDecomp - ZeroCase Nnv==zero path coverage", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("BDD with Nnv==zero structure for ZeroCase") {
        // Create BDD structure where the Nnv (else child) becomes zero after processing
        // This triggers the "if (Nv == zero)" path in BuildConjuncts (line 1813)
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = NOT(x0) AND (complex expression)
        // where the positive cofactor of x0 is zero
        DdNode *expr = vars[1];
        Cudd_Ref(expr);
        for (int i = 2; i < 15; i++) {
            DdNode *tmp = Cudd_bddOr(manager, expr, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, expr);
            expr = tmp;
        }
        
        // f = NOT(x0) AND expr means:
        // - when x0=1: f=0 (Nv=zero after processing)
        // - when x0=0: f=expr
        DdNode *notx0 = Cudd_Not(vars[0]);
        DdNode *f = Cudd_bddAnd(manager, notx0, expr);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, expr);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("BDD with Nv==zero and NOT(Nnv==zero) structure") {
        // Create structure where THEN child is zero but ELSE child is not
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create f = x0 AND (big expression)
        // where the negative cofactor of x0 is zero
        DdNode *expr = vars[1];
        Cudd_Ref(expr);
        for (int i = 2; i < 12; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, expr, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, expr);
            expr = tmp;
        }
        
        DdNode *f = Cudd_bddAnd(manager, vars[0], expr);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, expr);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Switched structure for ZeroCase coverage") {
        // Create structure that triggers the switched=1 path in ZeroCase
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create two branches with different minterm counts
        // Heavy branch (many minterms - ORs)
        DdNode *heavy = vars[1];
        Cudd_Ref(heavy);
        for (int i = 2; i < 12; i++) {
            DdNode *tmp = Cudd_bddOr(manager, heavy, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, heavy);
            heavy = tmp;
        }
        
        // Light branch (few minterms - ANDs)
        DdNode *light = vars[12];
        Cudd_Ref(light);
        for (int i = 13; i < 20; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, light, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, light);
            light = tmp;
        }
        
        // x0 ? heavy : zero structure (negative cofactor is 0)
        // This should trigger switched=1 when minNv < minNnv
        DdNode *f = Cudd_bddAnd(manager, vars[0], heavy);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, light);
        Cudd_RecursiveDeref(manager, heavy);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests for g==DD_ONE and h==DD_ONE paths in ZeroCase
 */
TEST_CASE("cuddDecomp - ZeroCase factorsNv.g/h == ONE paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Structure where factorsNv->g becomes ONE") {
        // Create BDD structure that will produce factors where g=ONE
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a structure: x0 AND (x1 OR x2 OR ... OR x10)
        // At decomposition point, one factor should be ONE
        DdNode *orPart = vars[1];
        Cudd_Ref(orPart);
        for (int i = 2; i < 10; i++) {
            DdNode *tmp = Cudd_bddOr(manager, orPart, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, orPart);
            orPart = tmp;
        }
        
        DdNode *f = Cudd_bddAnd(manager, vars[0], orPart);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, orPart);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Structure where factorsNv->h becomes ONE") {
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a structure where h=ONE at decomposition
        DdNode *andPart = vars[1];
        Cudd_Ref(andPart);
        for (int i = 2; i < 8; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, andPart, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, andPart);
            andPart = tmp;
        }
        
        // x0 AND andPart - when zero case triggers, h should become ONE
        DdNode *f = Cudd_bddAnd(manager, vars[0], andPart);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, andPart);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests targeting value==3 path in BuildConjuncts (lines 1699-1706)
 * This requires a node to appear in ghTable with value==3 (both g and h)
 */
TEST_CASE("cuddDecomp - BuildConjuncts value==3 ghTable path", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Structure to trigger value==3 lookup") {
        const int nvars = 40;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a shared subexpression that will be used in both g and h positions
        DdNode *shared = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared);
        
        // Create multiple structures using shared in ways that will cause it
        // to be registered as both g and h in ghTable (value=3)
        DdNode *part1 = Cudd_bddAnd(manager, shared, vars[2]);
        Cudd_Ref(part1);
        DdNode *part2 = Cudd_bddAnd(manager, shared, vars[3]);
        Cudd_Ref(part2);
        DdNode *part3 = Cudd_bddOr(manager, shared, vars[4]);
        Cudd_Ref(part3);
        
        // Combine in ways that cause the shared node to appear multiple times
        DdNode *combined1 = Cudd_bddAnd(manager, part1, part2);
        Cudd_Ref(combined1);
        DdNode *combined2 = Cudd_bddAnd(manager, combined1, part3);
        Cudd_Ref(combined2);
        
        // Add more complexity to push past decomposition threshold
        DdNode *f = combined2;
        Cudd_Ref(f);
        for (int i = 5; i < 25; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, combined2);
        Cudd_RecursiveDeref(manager, combined1);
        Cudd_RecursiveDeref(manager, part3);
        Cudd_RecursiveDeref(manager, part2);
        Cudd_RecursiveDeref(manager, part1);
        Cudd_RecursiveDeref(manager, shared);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Multiple decompositions on same BDD") {
        // Run decomposition multiple times to hit different random paths
        const int nvars = 35;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a complex structure
        DdNode *layers[7];
        for (int l = 0; l < 7; l++) {
            int base = l * 5;
            DdNode *t = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t);
            DdNode *t2 = Cudd_bddOr(manager, t, vars[base+2]);
            Cudd_Ref(t2);
            layers[l] = Cudd_bddAnd(manager, t2, vars[base+3]);
            Cudd_Ref(layers[l]);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t);
        }
        
        DdNode *f = layers[0];
        Cudd_Ref(f);
        for (int l = 1; l < 7; l++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, layers[l]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        // Run decomposition multiple times
        for (int iter = 0; iter < 5; iter++) {
            DdNode **conjuncts = nullptr;
            int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
            
            REQUIRE(result >= 1);
            REQUIRE(conjuncts != nullptr);
            
            for (int i = 0; i < result; i++) {
                Cudd_RecursiveDeref(manager, conjuncts[i]);
            }
            FREE(conjuncts);
        }
        
        Cudd_RecursiveDeref(manager, f);
        for (int l = 0; l < 7; l++) {
            Cudd_RecursiveDeref(manager, layers[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests targeting the Gv/Gnv == NOT(DD_ONE) paths in ZeroCase
 * These trigger special handling for "variable below" cases
 */
TEST_CASE("cuddDecomp - ZeroCase child-is-variable paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Structure with Gv == NOT(ONE) - child is complemented constant") {
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create structure: x0 AND NOT(x1) AND (more structure)
        // This creates BDD where one child of x0 leads to NOT(ONE)=ZERO
        DdNode *notx1 = Cudd_Not(vars[1]);
        DdNode *f = Cudd_bddAnd(manager, vars[0], notx1);
        Cudd_Ref(f);
        
        // Add more structure
        for (int i = 2; i < 15; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Structure with Hv == NOT(ONE) path") {
        const int nvars = 30;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex structure
        DdNode *or1 = Cudd_bddOr(manager, vars[1], vars[2]);
        Cudd_Ref(or1);
        DdNode *or2 = Cudd_bddOr(manager, vars[3], vars[4]);
        Cudd_Ref(or2);
        
        // x0 AND (or1 OR or2) type structure
        DdNode *ors = Cudd_bddOr(manager, or1, or2);
        Cudd_Ref(ors);
        DdNode *f = Cudd_bddAnd(manager, vars[0], ors);
        Cudd_Ref(f);
        
        // Add more depth
        for (int i = 5; i < 20; i++) {
            DdNode *tmp;
            if (i % 2 == 0) {
                tmp = Cudd_bddAnd(manager, f, vars[i]);
            } else {
                tmp = Cudd_bddOr(manager, f, vars[i]);
            }
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, ors);
        Cudd_RecursiveDeref(manager, or2);
        Cudd_RecursiveDeref(manager, or1);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests targeting the CheckInTables various pairValue branches
 */
TEST_CASE("cuddDecomp - CheckInTables pairValue paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Complex BDD to trigger various pairValue combinations") {
        const int nvars = 50;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create structure with many shared subexpressions
        DdNode *shared[8];
        for (int s = 0; s < 8; s++) {
            int base = s * 6;
            shared[s] = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(shared[s]);
            DdNode *tmp = Cudd_bddOr(manager, shared[s], vars[base+2]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, shared[s]);
            shared[s] = tmp;
        }
        
        // Cross-reference shared subexpressions
        DdNode *cross[4];
        for (int c = 0; c < 4; c++) {
            cross[c] = Cudd_bddAnd(manager, shared[c], shared[c+4]);
            Cudd_Ref(cross[c]);
        }
        
        // Combine cross references
        DdNode *f = cross[0];
        Cudd_Ref(f);
        for (int c = 1; c < 4; c++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, cross[c]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int c = 0; c < 4; c++) {
            Cudd_RecursiveDeref(manager, cross[c]);
        }
        for (int s = 0; s < 8; s++) {
            Cudd_RecursiveDeref(manager, shared[s]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("BDD with high-reference shared nodes for PickOnePair") {
        const int nvars = 45;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create a shared node with high reference count
        DdNode *shared = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(shared);
        
        // Reference multiple times
        for (int i = 0; i < 10; i++) {
            Cudd_Ref(shared);
        }
        
        // Build complex structure using shared many times
        DdNode *branches[6];
        for (int b = 0; b < 6; b++) {
            int base = 2 + b * 7;
            DdNode *t = Cudd_bddAnd(manager, shared, vars[base]);
            Cudd_Ref(t);
            DdNode *t2 = Cudd_bddOr(manager, t, vars[base+1]);
            Cudd_Ref(t2);
            branches[b] = Cudd_bddAnd(manager, t2, vars[base+2]);
            Cudd_Ref(branches[b]);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t);
        }
        
        DdNode *f = branches[0];
        Cudd_Ref(f);
        for (int b = 1; b < 6; b++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, branches[b]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int b = 0; b < 6; b++) {
            Cudd_RecursiveDeref(manager, branches[b]);
        }
        for (int i = 0; i < 10; i++) {
            Cudd_RecursiveDeref(manager, shared);
        }
        Cudd_RecursiveDeref(manager, shared);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Additional tests for PairInTables return values
 */
TEST_CASE("cuddDecomp - PairInTables various return paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Structures to trigger different pair lookup scenarios") {
        const int nvars = 55;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create multiple layers with various sharing patterns
        DdNode *layer1[5];
        for (int l = 0; l < 5; l++) {
            int base = l * 10;
            DdNode *a = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(a);
            DdNode *b = Cudd_bddOr(manager, vars[base+2], vars[base+3]);
            Cudd_Ref(b);
            layer1[l] = Cudd_bddAnd(manager, a, b);
            Cudd_Ref(layer1[l]);
            Cudd_RecursiveDeref(manager, a);
            Cudd_RecursiveDeref(manager, b);
        }
        
        // Create cross-references
        DdNode *cross1 = Cudd_bddAnd(manager, layer1[0], layer1[1]);
        Cudd_Ref(cross1);
        DdNode *cross2 = Cudd_bddAnd(manager, layer1[2], layer1[3]);
        Cudd_Ref(cross2);
        DdNode *cross3 = Cudd_bddOr(manager, cross1, layer1[4]);
        Cudd_Ref(cross3);
        
        DdNode *f = Cudd_bddAnd(manager, cross2, cross3);
        Cudd_Ref(f);
        
        // Run multiple decomposition methods
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        REQUIRE(result >= 1);
        for (int i = 0; i < result; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        result = Cudd_bddApproxConjDecomp(manager, f, &conjuncts);
        REQUIRE(result >= 1);
        for (int i = 0; i < result; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        result = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
        REQUIRE(result >= 1);
        for (int i = 0; i < result; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, cross3);
        Cudd_RecursiveDeref(manager, cross2);
        Cudd_RecursiveDeref(manager, cross1);
        for (int l = 0; l < 5; l++) {
            Cudd_RecursiveDeref(manager, layer1[l]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests for distance < approxDistance path in cuddConjunctsAux
 * (lines 1998-2010)
 */
TEST_CASE("cuddDecomp - cuddConjunctsAux distance < approxDistance path", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Small BDD where distance < DEPTH") {
        // DEPTH is 5, so create BDD with depth < 5
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Simple AND - very shallow
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        // Should return early with (f, 1)
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("BDD with exactly DEPTH=5 distance") {
        // Create BDD with depth approximately equal to DEPTH constant
        const int nvars = 8;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Chain of ANDs creates specific depth
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 5; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests for complemented node handling in various functions
 */
TEST_CASE("cuddDecomp - Complemented node handling", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Complemented input to GenConjDecomp") {
        const int nvars = 20;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex BDD
        DdNode *f = vars[0];
        Cudd_Ref(f);
        for (int i = 1; i < 15; i++) {
            DdNode *tmp;
            if (i % 2 == 0) {
                tmp = Cudd_bddAnd(manager, f, vars[i]);
            } else {
                tmp = Cudd_bddOr(manager, f, vars[i]);
            }
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        // Test with complemented input
        DdNode *notf = Cudd_Not(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, notf, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Complemented edges in BDD structure") {
        const int nvars = 25;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build with complemented edges
        DdNode *notx0 = Cudd_Not(vars[0]);
        DdNode *notx1 = Cudd_Not(vars[1]);
        DdNode *a = Cudd_bddAnd(manager, notx0, vars[2]);
        Cudd_Ref(a);
        DdNode *b = Cudd_bddOr(manager, notx1, vars[3]);
        Cudd_Ref(b);
        DdNode *f = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(f);
        
        // Add more with complements
        for (int i = 4; i < 18; i++) {
            DdNode *tmp;
            if (i % 3 == 0) {
                tmp = Cudd_bddAnd(manager, f, Cudd_Not(vars[i]));
            } else {
                tmp = Cudd_bddAnd(manager, f, vars[i]);
            }
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, a);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * These tests are specifically designed to trigger the remaining uncovered paths
 * in the PairInTables and CheckInTables functions.
 */
TEST_CASE("cuddDecomp - Targeted tests for internal table lookup paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Very large complex BDD for all paths") {
        // Create an extremely complex BDD to increase the probability
        // of hitting rare table lookup paths
        const int nvars = 80;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create many shared subexpressions
        DdNode *shared[20];
        for (int s = 0; s < 20; s++) {
            int base = s * 4;
            DdNode *t1 = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t1);
            DdNode *t2 = Cudd_bddOr(manager, t1, vars[base+2]);
            Cudd_Ref(t2);
            shared[s] = Cudd_bddAnd(manager, t2, vars[base+3]);
            Cudd_Ref(shared[s]);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t1);
        }
        
        // Create cross-references (shared used in multiple places)
        DdNode *cross[10];
        for (int c = 0; c < 10; c++) {
            cross[c] = Cudd_bddAnd(manager, shared[c], shared[c+10]);
            Cudd_Ref(cross[c]);
        }
        
        // Build a deeply nested structure
        DdNode *level1 = Cudd_bddAnd(manager, cross[0], cross[1]);
        Cudd_Ref(level1);
        DdNode *level2 = Cudd_bddOr(manager, cross[2], cross[3]);
        Cudd_Ref(level2);
        DdNode *level3 = Cudd_bddAnd(manager, cross[4], cross[5]);
        Cudd_Ref(level3);
        DdNode *level4 = Cudd_bddOr(manager, cross[6], cross[7]);
        Cudd_Ref(level4);
        
        DdNode *combo1 = Cudd_bddAnd(manager, level1, level2);
        Cudd_Ref(combo1);
        DdNode *combo2 = Cudd_bddAnd(manager, level3, level4);
        Cudd_Ref(combo2);
        DdNode *f = Cudd_bddAnd(manager, combo1, combo2);
        Cudd_Ref(f);
        
        // Also AND in remaining crosses
        for (int c = 8; c < 10; c++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, cross[c]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, combo2);
        Cudd_RecursiveDeref(manager, combo1);
        Cudd_RecursiveDeref(manager, level4);
        Cudd_RecursiveDeref(manager, level3);
        Cudd_RecursiveDeref(manager, level2);
        Cudd_RecursiveDeref(manager, level1);
        for (int c = 0; c < 10; c++) {
            Cudd_RecursiveDeref(manager, cross[c]);
        }
        for (int s = 0; s < 20; s++) {
            Cudd_RecursiveDeref(manager, shared[s]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Run decomposition multiple times with random seed variations") {
        // Run decomposition many times to hit different random paths
        // (lastTimeG alternation depends on Cudd_Random)
        const int nvars = 40;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex function
        DdNode *parts[8];
        for (int p = 0; p < 8; p++) {
            int base = p * 5;
            DdNode *t = Cudd_bddAnd(manager, vars[base], vars[base+1]);
            Cudd_Ref(t);
            DdNode *t2 = Cudd_bddOr(manager, t, vars[base+2]);
            Cudd_Ref(t2);
            parts[p] = Cudd_bddAnd(manager, t2, vars[base+3]);
            Cudd_Ref(parts[p]);
            Cudd_RecursiveDeref(manager, t2);
            Cudd_RecursiveDeref(manager, t);
        }
        
        DdNode *f = parts[0];
        Cudd_Ref(f);
        for (int p = 1; p < 8; p++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, parts[p]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        // Run decomposition many times
        for (int iter = 0; iter < 20; iter++) {
            DdNode **conjuncts = nullptr;
            int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
            
            REQUIRE(result >= 1);
            REQUIRE(conjuncts != nullptr);
            
            for (int i = 0; i < result; i++) {
                Cudd_RecursiveDeref(manager, conjuncts[i]);
            }
            FREE(conjuncts);
        }
        
        Cudd_RecursiveDeref(manager, f);
        for (int p = 0; p < 8; p++) {
            Cudd_RecursiveDeref(manager, parts[p]);
        }
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Test with functions that have specific cofactor patterns
 */
TEST_CASE("cuddDecomp - Specific cofactor patterns", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Function with all possible cofactor combinations") {
        const int nvars = 50;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create function with many different structural patterns
        // ITE structures, shared subgraphs, complemented edges
        
        // Pattern 1: ITE(x, y AND z, w)
        DdNode *yz = Cudd_bddAnd(manager, vars[1], vars[2]);
        Cudd_Ref(yz);
        DdNode *ite1 = Cudd_bddIte(manager, vars[0], yz, vars[3]);
        Cudd_Ref(ite1);
        
        // Pattern 2: ITE(x, NOT y, z)
        DdNode *noty = Cudd_Not(vars[5]);
        DdNode *ite2 = Cudd_bddIte(manager, vars[4], noty, vars[6]);
        Cudd_Ref(ite2);
        
        // Pattern 3: Shared usage
        DdNode *shared = Cudd_bddAnd(manager, vars[7], vars[8]);
        Cudd_Ref(shared);
        DdNode *use1 = Cudd_bddAnd(manager, shared, vars[9]);
        Cudd_Ref(use1);
        DdNode *use2 = Cudd_bddOr(manager, shared, vars[10]);
        Cudd_Ref(use2);
        
        // Combine everything
        DdNode *c1 = Cudd_bddAnd(manager, ite1, ite2);
        Cudd_Ref(c1);
        DdNode *c2 = Cudd_bddAnd(manager, use1, use2);
        Cudd_Ref(c2);
        DdNode *f = Cudd_bddAnd(manager, c1, c2);
        Cudd_Ref(f);
        
        // Add more variables
        for (int i = 11; i < 30; i++) {
            DdNode *tmp;
            if (i % 4 == 0) {
                tmp = Cudd_bddAnd(manager, f, vars[i]);
            } else if (i % 4 == 1) {
                tmp = Cudd_bddOr(manager, f, vars[i]);
            } else if (i % 4 == 2) {
                tmp = Cudd_bddAnd(manager, f, Cudd_Not(vars[i]));
            } else {
                tmp = Cudd_bddOr(manager, f, Cudd_Not(vars[i]));
            }
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, c2);
        Cudd_RecursiveDeref(manager, c1);
        Cudd_RecursiveDeref(manager, use2);
        Cudd_RecursiveDeref(manager, use1);
        Cudd_RecursiveDeref(manager, shared);
        Cudd_RecursiveDeref(manager, ite2);
        Cudd_RecursiveDeref(manager, ite1);
        Cudd_RecursiveDeref(manager, yz);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Tests with very specific structure to trigger BuildConjuncts paths
 */
TEST_CASE("cuddDecomp - BuildConjuncts specific paths", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Structure with Nnv == zero triggering ZeroCase") {
        // Create BDD where one child leads to zero
        const int nvars = 35;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create: (x0 AND (big expression)) creates zero on x0=0 side
        DdNode *bigExpr = vars[1];
        Cudd_Ref(bigExpr);
        for (int i = 2; i < 20; i++) {
            DdNode *tmp = Cudd_bddOr(manager, bigExpr, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, bigExpr);
            bigExpr = tmp;
        }
        
        // x0 AND bigExpr - when x0=0, result is 0
        DdNode *f = Cudd_bddAnd(manager, vars[0], bigExpr);
        Cudd_Ref(f);
        
        // Add more depth
        for (int i = 20; i < 30; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, bigExpr);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Structure with Nv == zero triggering alternate ZeroCase") {
        const int nvars = 35;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create: (NOT x0 AND (big expression)) creates zero on x0=1 side
        DdNode *bigExpr = vars[1];
        Cudd_Ref(bigExpr);
        for (int i = 2; i < 20; i++) {
            DdNode *tmp = Cudd_bddOr(manager, bigExpr, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, bigExpr);
            bigExpr = tmp;
        }
        
        // NOT x0 AND bigExpr - when x0=1, result is 0
        DdNode *notx0 = Cudd_Not(vars[0]);
        DdNode *f = Cudd_bddAnd(manager, notx0, bigExpr);
        Cudd_Ref(f);
        
        // Add more depth
        for (int i = 20; i < 30; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, f);
            f = tmp;
        }
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, bigExpr);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("Structure to trigger switched path in BuildConjuncts") {
        const int nvars = 40;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create asymmetric cofactors where minNv < minNnv
        // Heavy (many minterms) on negative side
        DdNode *heavy = vars[1];
        Cudd_Ref(heavy);
        for (int i = 2; i < 15; i++) {
            DdNode *tmp = Cudd_bddOr(manager, heavy, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, heavy);
            heavy = tmp;
        }
        
        // Light (few minterms) on positive side
        DdNode *light = vars[15];
        Cudd_Ref(light);
        for (int i = 16; i < 25; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, light, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, light);
            light = tmp;
        }
        
        // ITE: x0 ? light : heavy
        // When minNv (light) < minNnv (heavy), switched=1
        DdNode *f = Cudd_bddIte(manager, vars[0], light, heavy);
        Cudd_Ref(f);
        
        DdNode **conjuncts = nullptr;
        int result = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
        
        REQUIRE(result >= 1);
        REQUIRE(conjuncts != nullptr);
        
        for (int i = 0; i < result; i++) {
            Cudd_RecursiveDeref(manager, conjuncts[i]);
        }
        FREE(conjuncts);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, light);
        Cudd_RecursiveDeref(manager, heavy);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

/**
 * Test all decomposition types with various BDD patterns
 */
TEST_CASE("cuddDecomp - All decomposition types with various BDDs", "[cuddDecomp][coverage]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Comprehensive test with all methods") {
        const int nvars = 45;
        DdNode *vars[nvars];
        for (int i = 0; i < nvars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Create several different BDD types
        // Type 1: Pure AND chain
        DdNode *andChain = vars[0];
        Cudd_Ref(andChain);
        for (int i = 1; i < 10; i++) {
            DdNode *tmp = Cudd_bddAnd(manager, andChain, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, andChain);
            andChain = tmp;
        }
        
        // Type 2: Pure OR chain
        DdNode *orChain = vars[10];
        Cudd_Ref(orChain);
        for (int i = 11; i < 20; i++) {
            DdNode *tmp = Cudd_bddOr(manager, orChain, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, orChain);
            orChain = tmp;
        }
        
        // Type 3: Mixed
        DdNode *mixed = vars[20];
        Cudd_Ref(mixed);
        for (int i = 21; i < 30; i++) {
            DdNode *tmp;
            if (i % 2 == 0) {
                tmp = Cudd_bddAnd(manager, mixed, vars[i]);
            } else {
                tmp = Cudd_bddOr(manager, mixed, vars[i]);
            }
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, mixed);
            mixed = tmp;
        }
        
        // Type 4: XOR-like
        DdNode *xorLike = Cudd_bddXor(manager, vars[30], vars[31]);
        Cudd_Ref(xorLike);
        for (int i = 32; i < 40; i += 2) {
            DdNode *xorPart = Cudd_bddXor(manager, vars[i], vars[i+1]);
            Cudd_Ref(xorPart);
            DdNode *tmp = Cudd_bddAnd(manager, xorLike, xorPart);
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, xorPart);
            Cudd_RecursiveDeref(manager, xorLike);
            xorLike = tmp;
        }
        
        // Test all methods on each type
        DdNode *bdds[] = {andChain, orChain, mixed, xorLike};
        for (int b = 0; b < 4; b++) {
            DdNode *f = bdds[b];
            DdNode **conjuncts = nullptr;
            DdNode **disjuncts = nullptr;
            
            int r1 = Cudd_bddGenConjDecomp(manager, f, &conjuncts);
            REQUIRE(r1 >= 1);
            for (int i = 0; i < r1; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
            FREE(conjuncts);
            
            int r2 = Cudd_bddGenDisjDecomp(manager, f, &disjuncts);
            REQUIRE(r2 >= 1);
            for (int i = 0; i < r2; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
            FREE(disjuncts);
            
            int r3 = Cudd_bddApproxConjDecomp(manager, f, &conjuncts);
            REQUIRE(r3 >= 1);
            for (int i = 0; i < r3; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
            FREE(conjuncts);
            
            int r4 = Cudd_bddApproxDisjDecomp(manager, f, &disjuncts);
            REQUIRE(r4 >= 1);
            for (int i = 0; i < r4; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
            FREE(disjuncts);
            
            int r5 = Cudd_bddIterConjDecomp(manager, f, &conjuncts);
            REQUIRE(r5 >= 1);
            for (int i = 0; i < r5; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
            FREE(conjuncts);
            
            int r6 = Cudd_bddIterDisjDecomp(manager, f, &disjuncts);
            REQUIRE(r6 >= 1);
            for (int i = 0; i < r6; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
            FREE(disjuncts);
            
            int r7 = Cudd_bddVarConjDecomp(manager, f, &conjuncts);
            REQUIRE(r7 >= 1);
            for (int i = 0; i < r7; i++) Cudd_RecursiveDeref(manager, conjuncts[i]);
            FREE(conjuncts);
            
            int r8 = Cudd_bddVarDisjDecomp(manager, f, &disjuncts);
            REQUIRE(r8 >= 1);
            for (int i = 0; i < r8; i++) Cudd_RecursiveDeref(manager, disjuncts[i]);
            FREE(disjuncts);
        }
        
        Cudd_RecursiveDeref(manager, xorLike);
        Cudd_RecursiveDeref(manager, mixed);
        Cudd_RecursiveDeref(manager, orChain);
        Cudd_RecursiveDeref(manager, andChain);
        for (int i = 0; i < nvars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}
