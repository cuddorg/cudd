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
