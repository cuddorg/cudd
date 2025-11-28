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

// ============================================================================
// Tests for timeout and termination callback handling
// ============================================================================

// Termination callback for testing
static int terminationFlag = 0;
static int terminationCallback(const void* /* arg */) {
    return terminationFlag;
}

TEST_CASE("cuddZddLin - Timeout and termination handling", "[cuddZddLin]") {
    SECTION("Reordering with time limit") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a ZDD
        DdNode* zdd = createInteractingZdd(manager, 8);
        REQUIRE(zdd != nullptr);
        
        // Set a time limit (very short to potentially trigger timeout path)
        Cudd_SetTimeLimit(manager, 1);
        Cudd_ResetStartTime(manager);
        
        // This might or might not timeout, but it exercises the time limit checking code
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        // Result should be 1 (success) regardless of whether timeout was hit
        REQUIRE(result == 1);
        
        // Clear time limit
        Cudd_UnsetTimeLimit(manager);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
    
    SECTION("Reordering with termination callback") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a ZDD
        DdNode* zdd = createSimpleZdd(manager, 6);
        REQUIRE(zdd != nullptr);
        
        // Register termination callback
        terminationFlag = 0;
        Cudd_RegisterTerminationCallback(manager, terminationCallback, nullptr);
        
        // First call should complete normally
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Now set the flag but the reordering should still return 1
        terminationFlag = 1;
        result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        // Clear callback
        Cudd_UnregisterTerminationCallback(manager);
        terminationFlag = 0;
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for specific cuddZddLinearAux branches
// ============================================================================

TEST_CASE("cuddZddLin - cuddZddLinearAux specific branches", "[cuddZddLin]") {
    SECTION("x == xLow branch with many variables") {
        // Create a structure where the first variable (at xLow) needs to be sifted down
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDDs with first variable heavily used
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        
        DdNode* accum = var0;
        for (int i = 1; i < 6; i++) {
            DdNode* vari = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vari);
            DdNode* prod = Cudd_zddProduct(manager, accum, vari);
            Cudd_Ref(prod);
            if (accum != var0) Cudd_RecursiveDerefZdd(manager, accum);
            Cudd_RecursiveDerefZdd(manager, vari);
            accum = prod;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, accum);
        Cudd_Quit(manager);
    }
    
    SECTION("x == xHigh branch with many variables") {
        // Create a structure where the last variable (at xHigh) needs to be sifted up
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDDs with last variable heavily used
        DdNode* var5 = Cudd_zddIthVar(manager, 5);
        Cudd_Ref(var5);
        
        DdNode* accum = var5;
        for (int i = 4; i >= 0; i--) {
            DdNode* vari = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vari);
            DdNode* prod = Cudd_zddProduct(manager, accum, vari);
            Cudd_Ref(prod);
            if (accum != var5) Cudd_RecursiveDerefZdd(manager, accum);
            Cudd_RecursiveDerefZdd(manager, vari);
            accum = prod;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, accum);
        Cudd_Quit(manager);
    }
    
    SECTION("(x - xLow) > (xHigh - x) branch") {
        // Create a structure where middle variable is closer to xHigh
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD with many nodes at position 7 (closer to xHigh=9)
        DdNode* var7 = Cudd_zddIthVar(manager, 7);
        Cudd_Ref(var7);
        DdNode* var8 = Cudd_zddIthVar(manager, 8);
        Cudd_Ref(var8);
        DdNode* var9 = Cudd_zddIthVar(manager, 9);
        Cudd_Ref(var9);
        
        DdNode* u1 = Cudd_zddUnion(manager, var7, var8);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(manager, u1, var9);
        Cudd_Ref(u2);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, var9);
        Cudd_RecursiveDerefZdd(manager, var8);
        Cudd_RecursiveDerefZdd(manager, var7);
        Cudd_Quit(manager);
    }
    
    SECTION("else branch - (x - xLow) <= (xHigh - x)") {
        // Create a structure where middle variable is closer to xLow
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD with many nodes at position 2 (closer to xLow=0)
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var2);
        
        DdNode* u1 = Cudd_zddUnion(manager, var0, var1);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(manager, u1, var2);
        Cudd_Ref(u2);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddLinearInPlace internal paths
// ============================================================================

TEST_CASE("cuddZddLin - cuddZddLinearInPlace paths", "[cuddZddLin]") {
    SECTION("Linear transform with nodes at both levels") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure where adjacent variables have nodes at both levels
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var2);
        DdNode* var3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(var3);
        
        // Create multiple products to ensure nodes at both levels
        DdNode* p1 = Cudd_zddProduct(manager, var0, var1);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, var1, var2);
        Cudd_Ref(p2);
        DdNode* p3 = Cudd_zddProduct(manager, var2, var3);
        Cudd_Ref(p3);
        
        DdNode* u1 = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(manager, u1, p3);
        Cudd_Ref(u2);
        
        // Multiple reorderings
        for (int i = 0; i < 5; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, p3);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, var3);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear transform with dense subtable") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a dense structure
        DdNode* vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create all pairwise products
        DdNode* products[15];
        int idx = 0;
        for (int i = 0; i < 6; i++) {
            for (int j = i + 1; j < 6; j++) {
                products[idx] = Cudd_zddProduct(manager, vars[i], vars[j]);
                Cudd_Ref(products[idx]);
                idx++;
            }
        }
        
        // Union all products
        DdNode* result_zdd = products[0];
        Cudd_Ref(result_zdd);
        for (int i = 1; i < 15; i++) {
            DdNode* temp = Cudd_zddUnion(manager, result_zdd, products[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, result_zdd);
            result_zdd = temp;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, result_zdd);
        for (int i = 0; i < 15; i++) {
            Cudd_RecursiveDerefZdd(manager, products[i]);
        }
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for special list handling in cuddZddLinearInPlace
// ============================================================================

TEST_CASE("cuddZddLin - Special list handling", "[cuddZddLin]") {
    SECTION("Nodes where f1->index == yindex and cuddE(f1) == empty") {
        DdManager* manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure that creates special list scenario
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode* var1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(var1);
        
        // Product of var0 and var1 creates a node where:
        // - The node at level 0 has then-child at level 1
        // - The else-child is empty
        DdNode* prod = Cudd_zddProduct(manager, var0, var1);
        Cudd_Ref(prod);
        
        // Add more structure
        DdNode* var2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(var2);
        DdNode* prod2 = Cudd_zddProduct(manager, prod, var2);
        Cudd_Ref(prod2);
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, var1);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("Next pointer handling in special list") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create multiple products to populate special list
        DdNode* vars[4];
        for (int i = 0; i < 4; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create chains of products
        DdNode* p01 = Cudd_zddProduct(manager, vars[0], vars[1]);
        Cudd_Ref(p01);
        DdNode* p12 = Cudd_zddProduct(manager, vars[1], vars[2]);
        Cudd_Ref(p12);
        DdNode* p23 = Cudd_zddProduct(manager, vars[2], vars[3]);
        Cudd_Ref(p23);
        DdNode* p012 = Cudd_zddProduct(manager, p01, vars[2]);
        Cudd_Ref(p012);
        
        DdNode* u1 = Cudd_zddUnion(manager, p01, p12);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(manager, u1, p23);
        Cudd_Ref(u2);
        DdNode* u3 = Cudd_zddUnion(manager, u2, p012);
        Cudd_Ref(u3);
        
        // Multiple reorderings to exercise special list
        for (int i = 0; i < 4; i++) {
            int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(result == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, u3);
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, p012);
        Cudd_RecursiveDerefZdd(manager, p23);
        Cudd_RecursiveDerefZdd(manager, p12);
        Cudd_RecursiveDerefZdd(manager, p01);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddLinearBackward with INVERSE_TRANSFORM_MOVE
// ============================================================================

TEST_CASE("cuddZddLin - Backward with inverse transforms", "[cuddZddLin]") {
    SECTION("Backward pass undoing linear transforms") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create structure that will require linear transforms during sifting
        DdNode* vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create pairs and their products
        DdNode* pairs[4];
        for (int i = 0; i < 4; i++) {
            pairs[i] = Cudd_zddUnion(manager, vars[i*2], vars[i*2+1]);
            Cudd_Ref(pairs[i]);
        }
        
        DdNode* prod1 = Cudd_zddProduct(manager, pairs[0], pairs[1]);
        Cudd_Ref(prod1);
        DdNode* prod2 = Cudd_zddProduct(manager, pairs[2], pairs[3]);
        Cudd_Ref(prod2);
        DdNode* final_zdd = Cudd_zddUnion(manager, prod1, prod2);
        Cudd_Ref(final_zdd);
        
        // Apply convergent linear sifting (will call backward multiple times)
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, final_zdd);
        Cudd_RecursiveDerefZdd(manager, prod2);
        Cudd_RecursiveDerefZdd(manager, prod1);
        for (int i = 0; i < 4; i++) {
            Cudd_RecursiveDerefZdd(manager, pairs[i]);
        }
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for extreme variable counts
// ============================================================================

TEST_CASE("cuddZddLin - Extreme variable counts", "[cuddZddLin]") {
    SECTION("Linear sifting with 15 variables") {
        DdManager* manager = Cudd_Init(0, 15, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex structure
        DdNode* accum = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(accum);
        
        for (int i = 0; i < 15; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* temp = Cudd_zddUnion(manager, accum, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDerefZdd(manager, accum);
            Cudd_RecursiveDerefZdd(manager, var);
            accum = temp;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, accum);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear sifting with 20 variables") {
        DdManager* manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create sparse structure
        DdNode* vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_zddIthVar(manager, i * 4);
            Cudd_Ref(vars[i]);
        }
        
        DdNode* accum = vars[0];
        Cudd_Ref(accum);
        for (int i = 1; i < 5; i++) {
            DdNode* temp = Cudd_zddUnion(manager, accum, vars[i]);
            Cudd_Ref(temp);
            if (i > 1) Cudd_RecursiveDerefZdd(manager, accum);
            accum = temp;
        }
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, accum);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to trigger specific uncovered code paths
// ============================================================================

// Global counter for termination callback
static int callbackCounter = 0;
static int triggerAfterCalls = 0;

static int countingTerminationCallback(const void* /* arg */) {
    callbackCounter++;
    // Trigger termination after specified number of calls
    return (triggerAfterCalls > 0 && callbackCounter >= triggerAfterCalls) ? 1 : 0;
}

TEST_CASE("cuddZddLin - Force termination callback trigger", "[cuddZddLin]") {
    SECTION("Termination callback triggered during sifting") {
        DdManager* manager = Cudd_Init(0, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a more complex ZDD structure to ensure many sifting iterations
        DdNode* vars[12];
        for (int i = 0; i < 12; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create complex interactions
        DdNode* products[6];
        for (int i = 0; i < 6; i++) {
            products[i] = Cudd_zddProduct(manager, vars[i*2], vars[i*2 + 1]);
            Cudd_Ref(products[i]);
        }
        
        DdNode* unions[3];
        for (int i = 0; i < 3; i++) {
            unions[i] = Cudd_zddUnion(manager, products[i*2], products[i*2 + 1]);
            Cudd_Ref(unions[i]);
        }
        
        DdNode* final1 = Cudd_zddUnion(manager, unions[0], unions[1]);
        Cudd_Ref(final1);
        DdNode* final_zdd = Cudd_zddUnion(manager, final1, unions[2]);
        Cudd_Ref(final_zdd);
        
        // Set up callback to trigger after 2 sifting operations
        callbackCounter = 0;
        triggerAfterCalls = 2;
        Cudd_RegisterTerminationCallback(manager, countingTerminationCallback, nullptr);
        
        // Reordering will call the callback during sifting
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        // Should succeed even if terminated early
        REQUIRE(result == 1);
        
        // Verify callback was called
        REQUIRE(callbackCounter >= 1);
        
        // Cleanup
        Cudd_UnregisterTerminationCallback(manager);
        triggerAfterCalls = 0;
        callbackCounter = 0;
        
        Cudd_RecursiveDerefZdd(manager, final_zdd);
        Cudd_RecursiveDerefZdd(manager, final1);
        for (int i = 0; i < 3; i++) {
            Cudd_RecursiveDerefZdd(manager, unions[i]);
        }
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDerefZdd(manager, products[i]);
        }
        for (int i = 0; i < 12; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddLin - Time limit during sifting", "[cuddZddLin]") {
    SECTION("Time limit triggers early termination") {
        DdManager* manager = Cudd_Init(0, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // First, burn some CPU time to ensure util_cpu_time() returns > 0
        volatile long dummy = 0;
        for (int j = 0; j < 10000000; j++) {
            dummy += j;
        }
        (void)dummy;  // Suppress unused variable warning
        
        // Create a large ZDD to increase sifting time
        DdNode* accum = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(accum);
        
        for (int i = 0; i < 16; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            
            DdNode* temp = Cudd_zddUnion(manager, accum, var);
            Cudd_Ref(temp);
            
            Cudd_RecursiveDerefZdd(manager, var);
            Cudd_RecursiveDerefZdd(manager, accum);
            accum = temp;
            
            // Add some products to make it more complex
            if (i > 0 && i % 4 == 0) {
                DdNode* prevVar = Cudd_zddIthVar(manager, i - 1);
                Cudd_Ref(prevVar);
                DdNode* prod = Cudd_zddProduct(manager, accum, prevVar);
                Cudd_Ref(prod);
                
                DdNode* uni = Cudd_zddUnion(manager, accum, prod);
                Cudd_Ref(uni);
                
                Cudd_RecursiveDerefZdd(manager, prod);
                Cudd_RecursiveDerefZdd(manager, prevVar);
                Cudd_RecursiveDerefZdd(manager, accum);
                accum = uni;
            }
        }
        
        // To trigger the time limit path, we need:
        // util_cpu_time() - startTime > timeLimit
        // Set startTime to 0 (long ago) and a small time limit
        Cudd_SetStartTime(manager, 0);  // Start time = 0 (way in the past)
        Cudd_SetTimeLimit(manager, 1);  // 1 ms timeout
        
        int result = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(result == 1);
        
        Cudd_UnsetTimeLimit(manager);
        
        Cudd_RecursiveDerefZdd(manager, accum);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddLin - Boundary conditions in cuddZddLinearAux", "[cuddZddLin]") {
    SECTION("x equals xLow exactly") {
        // Create scenario where variable at lowest position is sifted
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Make variable 0 the most important (first in sorted order)
        DdNode* var0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(var0);
        
        // Add many references through var0
        DdNode* result = var0;
        for (int i = 1; i < 8; i++) {
            DdNode* vari = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vari);
            DdNode* prod = Cudd_zddProduct(manager, result, vari);
            Cudd_Ref(prod);
            DdNode* uni = Cudd_zddUnion(manager, result, prod);
            Cudd_Ref(uni);
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, vari);
            if (result != var0) {
                Cudd_RecursiveDerefZdd(manager, result);
            }
            result = uni;
        }
        
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(res == 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, var0);
        Cudd_Quit(manager);
    }
    
    SECTION("x equals xHigh exactly") {
        // Create scenario where variable at highest position is sifted
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Make variable 7 the most important
        DdNode* var7 = Cudd_zddIthVar(manager, 7);
        Cudd_Ref(var7);
        
        // Add many references through var7
        DdNode* result = var7;
        for (int i = 6; i >= 0; i--) {
            DdNode* vari = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vari);
            DdNode* prod = Cudd_zddProduct(manager, result, vari);
            Cudd_Ref(prod);
            DdNode* uni = Cudd_zddUnion(manager, result, prod);
            Cudd_Ref(uni);
            Cudd_RecursiveDerefZdd(manager, prod);
            Cudd_RecursiveDerefZdd(manager, vari);
            if (result != var7) {
                Cudd_RecursiveDerefZdd(manager, result);
            }
            result = uni;
        }
        
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(res == 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, var7);
        Cudd_Quit(manager);
    }
    
    SECTION("x closer to xHigh - go down first") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD where middle variable near high end is most referenced
        DdNode* var7 = Cudd_zddIthVar(manager, 7);  // Near xHigh=9
        Cudd_Ref(var7);
        
        DdNode* result = var7;
        for (int i = 0; i < 10; i++) {
            if (i == 7) continue;
            DdNode* vari = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vari);
            DdNode* uni = Cudd_zddUnion(manager, result, vari);
            Cudd_Ref(uni);
            Cudd_RecursiveDerefZdd(manager, vari);
            if (result != var7) {
                Cudd_RecursiveDerefZdd(manager, result);
            }
            result = uni;
        }
        
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(res == 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, var7);
        Cudd_Quit(manager);
    }
    
    SECTION("x closer to xLow - go up first") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD where middle variable near low end is most referenced
        DdNode* var2 = Cudd_zddIthVar(manager, 2);  // Near xLow=0
        Cudd_Ref(var2);
        
        DdNode* result = var2;
        for (int i = 0; i < 10; i++) {
            if (i == 2) continue;
            DdNode* vari = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vari);
            DdNode* uni = Cudd_zddUnion(manager, result, vari);
            Cudd_Ref(uni);
            Cudd_RecursiveDerefZdd(manager, vari);
            if (result != var2) {
                Cudd_RecursiveDerefZdd(manager, result);
            }
            result = uni;
        }
        
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(res == 1);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, var2);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddLin - Dense ZDD structures for linear transforms", "[cuddZddLin]") {
    SECTION("All pairwise products to maximize linear transforms") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create all pairwise products
        DdNode* allProducts = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(allProducts);
        
        for (int i = 0; i < 6; i++) {
            for (int j = i + 1; j < 6; j++) {
                DdNode* prod = Cudd_zddProduct(manager, vars[i], vars[j]);
                Cudd_Ref(prod);
                DdNode* uni = Cudd_zddUnion(manager, allProducts, prod);
                Cudd_Ref(uni);
                Cudd_RecursiveDerefZdd(manager, prod);
                Cudd_RecursiveDerefZdd(manager, allProducts);
                allProducts = uni;
            }
        }
        
        // Multiple reorderings to exercise linear transforms
        for (int iter = 0; iter < 5; iter++) {
            int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
            REQUIRE(res == 1);
        }
        
        Cudd_RecursiveDerefZdd(manager, allProducts);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
    
    SECTION("Triple products for deeper structures") {
        DdManager* manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
            Cudd_Ref(vars[i]);
        }
        
        // Create triple products
        DdNode* allTriples = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(allTriples);
        
        for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 5; j++) {
                for (int k = j + 1; k < 6; k++) {
                    DdNode* p1 = Cudd_zddProduct(manager, vars[i], vars[j]);
                    Cudd_Ref(p1);
                    DdNode* p2 = Cudd_zddProduct(manager, p1, vars[k]);
                    Cudd_Ref(p2);
                    DdNode* uni = Cudd_zddUnion(manager, allTriples, p2);
                    Cudd_Ref(uni);
                    Cudd_RecursiveDerefZdd(manager, p2);
                    Cudd_RecursiveDerefZdd(manager, p1);
                    Cudd_RecursiveDerefZdd(manager, allTriples);
                    allTriples = uni;
                }
            }
        }
        
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR_CONVERGE, 0);
        REQUIRE(res == 1);
        
        Cudd_RecursiveDerefZdd(manager, allTriples);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDerefZdd(manager, vars[i]);
        }
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddLin - Swap limit handling", "[cuddZddLin]") {
    SECTION("Sift max swap limit") {
        DdManager* manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set very low swap limit
        Cudd_SetSiftMaxSwap(manager, 5);
        
        DdNode* zdd = createSimpleZdd(manager, 10);
        REQUIRE(zdd != nullptr);
        
        int res = Cudd_zddReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        REQUIRE(res == 1);
        
        // Reset swap limit
        Cudd_SetSiftMaxSwap(manager, 2000000);
        
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_Quit(manager);
    }
}
