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
