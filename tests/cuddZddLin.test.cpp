#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - mtr.h must come before cudd.h for tree functions
#include "mtr.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddZddLin.c
 * 
 * This file contains comprehensive tests for the cuddZddLin module,
 * which implements linear sifting reordering for ZDDs. The linear sifting
 * algorithm combines variable swapping with XOR transformations to find
 * better variable orders.
 * 
 * Functions covered:
 * - cuddZddLinearSifting (internal, called via Cudd_zddReduceHeap)
 * - cuddZddLinearInPlace (static)
 * - cuddZddLinearAux (static)
 * - cuddZddLinearUp (static)
 * - cuddZddLinearDown (static)
 * - cuddZddLinearBackward (static)
 * - cuddZddUndoMoves (static)
 */

// Helper function to create a simple ZDD representing a set
static DdNode* createSimpleZdd(DdManager* manager, int numVars) {
    if (numVars < 2) return nullptr;
    
    // Create ZDD variables and build a simple ZDD
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars; i++) {
        DdNode* zvar = Cudd_zddIthVar(manager, i);
        if (zvar == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(zvar);
        
        DdNode* temp = Cudd_zddUnion(manager, result, zvar);
        if (temp == nullptr) {
            Cudd_RecursiveDerefZdd(manager, zvar);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, zvar);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// Helper function to create a ZDD with variable interactions
static DdNode* createInteractingZdd(DdManager* manager, int numVars) {
    if (numVars < 4) return nullptr;
    
    // Create a ZDD where variables interact through products
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    // Create pairs of interacting variables
    for (int i = 0; i < numVars - 1; i += 2) {
        DdNode* var1 = Cudd_zddIthVar(manager, i);
        DdNode* var2 = Cudd_zddIthVar(manager, i + 1);
        if (var1 == nullptr || var2 == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        
        // Create union of the pair
        DdNode* pair = Cudd_zddUnion(manager, var1, var2);
        if (pair == nullptr) {
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(pair);
        
        DdNode* temp = Cudd_zddProduct(manager, result, pair);
        if (temp == nullptr) {
            Cudd_RecursiveDerefZdd(manager, pair);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, pair);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// Helper function to create a complex ZDD with chain structure
static DdNode* createChainZdd(DdManager* manager, int numVars) {
    if (numVars < 3) return nullptr;
    
    // Build a chain of unions
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    for (int i = 0; i < numVars - 1; i++) {
        DdNode* var1 = Cudd_zddIthVar(manager, i);
        DdNode* var2 = Cudd_zddIthVar(manager, i + 1);
        if (var1 == nullptr || var2 == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        
        DdNode* uni = Cudd_zddUnion(manager, var1, var2);
        if (uni == nullptr) {
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(uni);
        
        DdNode* temp = Cudd_zddUnion(manager, result, uni);
        if (temp == nullptr) {
            Cudd_RecursiveDerefZdd(manager, uni);
            Cudd_RecursiveDerefZdd(manager, var1);
            Cudd_RecursiveDerefZdd(manager, var2);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, uni);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// Helper function to create ZDD with spread variables (to test different sifting directions)
static DdNode* createSpreadZdd(DdManager* manager, int numVars) {
    if (numVars < 5) return nullptr;
    
    // Create connections between far variables to force sifting
    DdNode* result = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(result);
    
    // Connect first and last variables
    DdNode* var0 = Cudd_zddIthVar(manager, 0);
    DdNode* varN = Cudd_zddIthVar(manager, numVars - 1);
    if (var0 == nullptr || varN == nullptr) {
        Cudd_RecursiveDerefZdd(manager, result);
        return nullptr;
    }
    Cudd_Ref(var0);
    Cudd_Ref(varN);
    
    DdNode* uni = Cudd_zddUnion(manager, var0, varN);
    if (uni == nullptr) {
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_RecursiveDerefZdd(manager, varN);
        Cudd_RecursiveDerefZdd(manager, result);
        return nullptr;
    }
    Cudd_Ref(uni);
    
    DdNode* prod = Cudd_zddProduct(manager, result, uni);
    if (prod == nullptr) {
        Cudd_RecursiveDerefZdd(manager, uni);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_RecursiveDerefZdd(manager, varN);
        Cudd_RecursiveDerefZdd(manager, result);
        return nullptr;
    }
    Cudd_Ref(prod);
    
    Cudd_RecursiveDerefZdd(manager, uni);
    Cudd_RecursiveDerefZdd(manager, var0);
    Cudd_RecursiveDerefZdd(manager, varN);
    Cudd_RecursiveDerefZdd(manager, result);
    result = prod;
    
    // Add middle variables
    for (int i = 1; i < numVars - 1; i++) {
        DdNode* var = Cudd_zddIthVar(manager, i);
        if (var == nullptr) {
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(var);
        
        DdNode* temp = Cudd_zddUnion(manager, result, var);
        if (temp == nullptr) {
            Cudd_RecursiveDerefZdd(manager, var);
            Cudd_RecursiveDerefZdd(manager, result);
            return nullptr;
        }
        Cudd_Ref(temp);
        
        Cudd_RecursiveDerefZdd(manager, var);
        Cudd_RecursiveDerefZdd(manager, result);
        result = temp;
    }
    
    return result;
}

// ============================================================================
// Tests for cuddZddLinearSifting via Cudd_zddReduceHeap with CUDD_REORDER_LINEAR
// ============================================================================

TEST_CASE("cuddZddLin - Basic linear sifting with CUDD_REORDER_LINEAR", "[cuddZddLin]") {
    SECTION("Simple ZDD with 4 variables") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD with 6 variables") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD with 8 variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddLin - Linear sifting with interacting variables", "[cuddZddLin]") {
    SECTION("Interacting ZDD with 4 variables") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Interacting ZDD with 6 variables") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Interacting ZDD with 8 variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for CUDD_REORDER_LINEAR_CONVERGE (converging linear sifting)
// ============================================================================

TEST_CASE("cuddZddLin - Linear sifting with convergence", "[cuddZddLin]") {
    SECTION("Simple ZDD convergence with 4 variables") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Interacting ZDD convergence with 6 variables") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Chain ZDD convergence with 5 variables") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createChainZdd(manager, 5);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise cuddZddLinearAux different branches
// ============================================================================

TEST_CASE("cuddZddLin - Exercise cuddZddLinearAux branches", "[cuddZddLin]") {
    // The cuddZddLinearAux function has 4 branches:
    // 1. x == xLow: Move down only
    // 2. x == xHigh: Move up only
    // 3. (x - xLow) > (xHigh - x): Go down first (shorter path)
    // 4. else: Go up first
    
    SECTION("Variable at lower boundary (x == xLow)") {
        // Need a variable at the lowest position
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD that will have variable at low position sifted
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        
        DdNode* uni = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(uni);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, uni);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable at upper boundary (x == xHigh)") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD that will have variable at high position sifted
        DdNode* var3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(var3);
        DdNode* var4 = Cudd_zddIthVar(manager, 4);
        Cudd_Ref(var4);
        
        DdNode* uni = Cudd_zddUnion(manager, var3, var4);
        Cudd_Ref(uni);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, uni);
        Cudd_RecursiveDerefZdd(manager, var4);
        Cudd_RecursiveDerefZdd(manager, var3);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable closer to upper boundary (go down first)") {
        DdManager* manager = Cudd_Init(0, 7, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD with variables spread out
        DdNode* zdd = createSpreadZdd(manager, 7);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Variable closer to lower boundary (go up first)") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create chain ZDD to exercise different paths
        DdNode* zdd = createChainZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddLinearInPlace - linear transformation of adjacent variables
// ============================================================================

TEST_CASE("cuddZddLin - Exercise cuddZddLinearInPlace transformations", "[cuddZddLin]") {
    SECTION("Linear transformation with simple structure") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a simple structure that will trigger linear transforms
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var2);
        DdNode* var3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(var3);
        
        // Build: (v0 union v1) product (v2 union v3)
        DdNode* p1 = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddUnion(manager, var2, var3);
        Cudd_Ref(p2);
        DdNode* prod = Cudd_zddProduct(manager, p1, p2);
        Cudd_Ref(prod);
        
        // Multiple reorderings will exercise linear transformations
        for (int i = 0; i < 3; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, var3);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear transformation with complex structure") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create more complex structure
        DdNode* result = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(result);
        
        for (int i = 0; i < 6; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* temp = Cudd_zddUnion(manager, result, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, result);
            Cudd_RecursiveDerefZdd(manager, var);
            result = temp;
        }
        
        // Build products to create interactions
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var5 = Cudd_zddIthVar(manager, 5);
        Cudd_Ref(var5);
        DdNode* prod = Cudd_zddProduct(manager, var0, var5);
        Cudd_Ref(prod);
        
        DdNode* final_result = Cudd_zddUnion(manager, result, prod);
        Cudd_Ref(final_result);
        
        int status = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(status == 1);
        
        Cudd_RecursiveDerefZdd(manager, final_result);
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, var5);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddLinearUp and cuddZddLinearDown
// ============================================================================

TEST_CASE("cuddZddLin - Exercise sifting up and down", "[cuddZddLin]") {
    SECTION("Sifting with 5 variables") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD that requires sifting in both directions
        DdNode* zdd = createChainZdd(manager, 5);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Sifting with 7 variables") {
        DdManager* manager = Cudd_Init(0, 7, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSpreadZdd(manager, 7);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple sifting iterations") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // Multiple iterations to trigger more code paths
        for (int i = 0; i < 5; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddLinearBackward
// ============================================================================

TEST_CASE("cuddZddLin - Exercise backward sifting", "[cuddZddLin]") {
    SECTION("Backward with simple ZDD") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        // Backward is called to restore best position
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Backward with convergence") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createChainZdd(manager, 5);
        REQUIRE(zdd != nullptr);
        
        // Convergence triggers backward multiple times
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddUndoMoves
// ============================================================================

TEST_CASE("cuddZddLin - Exercise undo moves", "[cuddZddLin]") {
    SECTION("Undo with interacting variables") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // UndoMoves is called during sifting when exploring both directions
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Undo with spread variables") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSpreadZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Edge case tests
// ============================================================================

TEST_CASE("cuddZddLin - Edge cases", "[cuddZddLin]") {
    SECTION("Linear reordering with single variable") {
        DdManager* manager = Cudd_Init(0, 1, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        
        // Should handle gracefully
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear reordering with two variables") {
        DdManager* manager = Cudd_Init(0, 2, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        
        DdNode* uni = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(uni);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, uni);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear reordering with three variables") {
        DdManager* manager = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createChainZdd(manager, 3);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear reordering with minsize threshold") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        
        // High minsize should prevent reordering
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 1000000);
        REQUIRE(result == 1);  // Returns 1 even if no reordering happens
        
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for autodyn with linear sifting
// ============================================================================

TEST_CASE("cuddZddLin - Automatic dynamic reordering", "[cuddZddLin]") {
    SECTION("Enable autodyn with LINEAR") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_LINEAR);
        
        // Create ZDDs that might trigger automatic reordering
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Enable autodyn with LINEAR_CONVERGE") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_LINEAR_CONVERGE);
        
        DdNode* zdd = createChainZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests with complex ZDD structures
// ============================================================================

TEST_CASE("cuddZddLin - Complex ZDD structures", "[cuddZddLin]") {
    SECTION("Multiple products and unions") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Build complex structure
        DdNode* vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create pairs
        DdNode* p1 = Cudd_zddUnion(manager, vars[0], vars[1]);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddUnion(manager, vars[2], vars[3]);
        Cudd_Ref(p2);
        DdNode* p3 = Cudd_zddUnion(manager, vars[4], vars[5]);
        Cudd_Ref(p3);
        DdNode* p4 = Cudd_zddUnion(manager, vars[6], vars[7]);
        Cudd_Ref(p4);
        
        // Create products of pairs
        DdNode* prod1 = Cudd_zddProduct(manager, p1, p2);
        Cudd_Ref(prod1);
        DdNode* prod2 = Cudd_zddProduct(manager, p3, p4);
        Cudd_Ref(prod2);
        
        // Final union
        DdNode* final_result = Cudd_zddUnion(manager, prod1, prod2);
        Cudd_Ref(final_result);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Also test convergence
        result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, final_result);
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, prod1);
        Cudd_RecursiveDerefZdd(manager, p4);
        Cudd_RecursiveDerefZdd(manager, p3);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, p1);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
    
    SECTION("Layered ZDD structure") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Build layered structure
        DdNode* layer1[5];
        for (int i = 0; i < 5; i++) {
            DdNode* v1 = Cudd_zddIthVar(manager, i*2);
            Cudd_Ref(v1);
            DdNode* v2 = Cudd_zddIthVar(manager, i*2 + 1);
            Cudd_Ref(v2);
            
            layer1[i] = Cudd_zddUnion(manager, v1, v2);
            Cudd_Ref(layer1[i]);
            
            Cudd_RecursiveDerefZdd(manager, v2);
            Cudd_RecursiveDerefZdd(manager, v1);
        }
        
        // Layer 2
        DdNode* layer2[2];
        DdNode* prod1 = Cudd_zddProduct(manager, layer1[0], layer1[1]);
        Cudd_Ref(prod1);
        layer2[0] = Cudd_zddUnion(manager, prod1, layer1[2]);
        Cudd_Ref(layer2[0]);
        Cudd_RecursiveDerefZdd(manager, prod1);
        
        DdNode* prod2 = Cudd_zddProduct(manager, layer1[3], layer1[4]);
        Cudd_Ref(prod2);
        layer2[1] = prod2;
        
        // Final
        DdNode* final_result = Cudd_zddUnion(manager, layer2[0], layer2[1]);
        Cudd_Ref(final_result);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, final_result);
        Cudd_RecursiveDerefZdd(manager, layer2[1]);
        Cudd_RecursiveDerefZdd(manager, layer2[0]);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDerefZdd(manager, layer1[i]);
        }
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for linear transformation effectiveness
// ============================================================================

TEST_CASE("cuddZddLin - Linear transform effectiveness", "[cuddZddLin]") {
    SECTION("Transform preserves ZDD semantics") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a specific ZDD
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        
        DdNode* uni = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(uni);
        
        // Count paths before reordering
        double countBefore = Cudd_zddCountDouble(manager, uni);
        
        // Apply linear sifting
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Count paths after reordering - should be the same
        double countAfter = Cudd_zddCountDouble(manager, uni);
        REQUIRE(countBefore == countAfter);
        
        Cudd_RecursiveDerefZdd(manager, uni);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple transforms preserve semantics") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createChainZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        double countBefore = Cudd_zddCountDouble(manager, zdd);
        
        // Multiple reorderings
        for (int i = 0; i < 3; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        double countAfter = Cudd_zddCountDouble(manager, zdd);
        REQUIRE(countBefore == countAfter);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for special node handling in cuddZddLinearInPlace
// ============================================================================

TEST_CASE("cuddZddLin - Special node handling", "[cuddZddLin]") {
    SECTION("Nodes with specific structure for special list") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure that exercises special node handling
        // The special list is used for nodes where f1->index == yindex && cuddE(f1) == empty
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        
        // Product creates structure that may hit special case
        DdNode* prod = Cudd_zddProduct(manager, var0, var1);
        Cudd_Ref(prod);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Nodes marked by reference from above") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure with nodes referenced from above
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var2);
        
        DdNode* u1 = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(u1);
        DdNode* p1 = Cudd_zddProduct(manager, u1, var2);
        Cudd_Ref(p1);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for growth limit handling
// ============================================================================

TEST_CASE("cuddZddLin - Growth limit handling", "[cuddZddLin]") {
    SECTION("With default growth limit") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // Default maxGrowth is used to limit sifting
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("With restrictive growth limit") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set very restrictive growth limit
        Cudd_SetMaxGrowth(manager, 1.01);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to exercise different move flag types
// ============================================================================

TEST_CASE("cuddZddLin - Move flag types", "[cuddZddLin]") {
    // The code uses three move types:
    // CUDD_SWAP_MOVE (0)
    // CUDD_LINEAR_TRANSFORM_MOVE (1)
    // CUDD_INVERSE_TRANSFORM_MOVE (2)
    
    SECTION("Exercise swap and transform moves") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure that exercises all move types
        DdNode* zdd = createInteractingZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Multiple iterations to hit different move combinations
        for (int i = 0; i < 4; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Exercise inverse transform moves via undo") {
        DdManager* manager = Cudd_Init(0, 7, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSpreadZdd(manager, 7);
        REQUIRE(zdd != nullptr);
        
        // This should trigger inverse transforms during undo
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for siftMaxVar and siftMaxSwap limits
// ============================================================================

TEST_CASE("cuddZddLin - Sift limits", "[cuddZddLin]") {
    SECTION("With limited siftMaxVar") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Limit variables to sift
        Cudd_SetSiftMaxVar(manager, 3);
        
        DdNode* zdd = createSimpleZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("With limited siftMaxSwap") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Limit swaps
        Cudd_SetSiftMaxSwap(manager, 10);
        
        DdNode* zdd = createChainZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests alternating between LINEAR and other reordering methods
// ============================================================================

TEST_CASE("cuddZddLin - Alternating with other methods", "[cuddZddLin]") {
    SECTION("LINEAR then SIFT") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result1 = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result1 == 1);
        
        int result2 = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result2 == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("SIFT then LINEAR") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createChainZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result1 = Cudd_zddReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result1 == 1);
        
        int result2 = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result2 == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("LINEAR_CONVERGE then SYMM_SIFT") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSpreadZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        int result1 = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result1 == 1);
        
        int result2 = Cudd_zddReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        REQUIRE(result2 >= 1);  // May return >1 for symmetric sift
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional tests for complete coverage
// ============================================================================

TEST_CASE("cuddZddLin - Additional coverage tests", "[cuddZddLin]") {
    SECTION("Large number of variables") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createSimpleZdd(manager, 12);
        REQUIRE(zdd != nullptr);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Mix of products and differences") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var2);
        DdNode* var3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(var3);
        
        DdNode* u1 = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(manager, var2, var3);
        Cudd_Ref(u2);
        
        DdNode* prod = Cudd_zddProduct(manager, u1, u2);
        Cudd_Ref(prod);
        
        DdNode* diff = Cudd_zddDiff(manager, prod, var0);
        Cudd_Ref(diff);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, diff);
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, var3);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Repeated convergence") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zdd = createInteractingZdd(manager, 4);
        REQUIRE(zdd != nullptr);
        
        // Multiple convergence calls
        for (int i = 0; i < 3; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}
