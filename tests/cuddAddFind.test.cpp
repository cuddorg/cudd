#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddAddFind.c
 * 
 * This file contains comprehensive tests to achieve 90%+ code coverage
 * of the cuddAddFind module which includes:
 * - Cudd_addFindMax: Finds the maximum discriminant of an ADD
 * - Cudd_addFindMin: Finds the minimum discriminant of an ADD
 * - Cudd_addIthBit: Extracts the i-th bit from an ADD
 */

// ============================================================================
// Tests for Cudd_addFindMax
// ============================================================================

TEST_CASE("Cudd_addFindMax - Constant ADD returns itself", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Test with constant ADD
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    DdNode *result = Cudd_addFindMax(manager, const5);
    REQUIRE(result != nullptr);
    REQUIRE(result == const5);
    REQUIRE(Cudd_V(result) == 5.0);

    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Simple ADD with two leaves", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD: if x0 then 10 else 3
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const10 = Cudd_addConst(manager, 10.0);
    REQUIRE(const10 != nullptr);
    Cudd_Ref(const10);

    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);

    DdNode *add = Cudd_addIte(manager, var0, const10, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 10.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Max in else branch", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD: if x0 then 3 else 10
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);

    DdNode *const10 = Cudd_addConst(manager, 10.0);
    REQUIRE(const10 != nullptr);
    Cudd_Ref(const10);

    DdNode *add = Cudd_addIte(manager, var0, const3, const10);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 10.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Complex ADD with multiple variables", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create a more complex ADD with multiple variables
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const7 = Cudd_addConst(manager, 7.0);
    DdNode *const100 = Cudd_addConst(manager, 100.0);
    REQUIRE(const1 != nullptr);
    REQUIRE(const5 != nullptr);
    REQUIRE(const7 != nullptr);
    REQUIRE(const100 != nullptr);
    Cudd_Ref(const1);
    Cudd_Ref(const5);
    Cudd_Ref(const7);
    Cudd_Ref(const100);

    // Create: if x0 then (if x1 then 100 else 5) else (if x1 then 7 else 1)
    DdNode *then_branch = Cudd_addIte(manager, var1, const100, const5);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);

    DdNode *else_branch = Cudd_addIte(manager, var1, const7, const1);
    REQUIRE(else_branch != nullptr);
    Cudd_Ref(else_branch);

    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 100.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_branch);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, const100);
    Cudd_RecursiveDeref(manager, const7);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Cache hit path", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const10 = Cudd_addConst(manager, 10.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const10 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const10);
    Cudd_Ref(const3);

    DdNode *add = Cudd_addIte(manager, var0, const10, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // First call - computes and caches result
    DdNode *result1 = Cudd_addFindMax(manager, add);
    REQUIRE(result1 != nullptr);
    
    // Second call - should hit cache
    DdNode *result2 = Cudd_addFindMax(manager, add);
    REQUIRE(result2 != nullptr);
    REQUIRE(result1 == result2);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - With plus infinity", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
    REQUIRE(plusInf != nullptr);

    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);

    // Create ADD: if x0 then +infinity else 3
    DdNode *add = Cudd_addIte(manager, var0, plusInf, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Should return +infinity immediately
    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result == plusInf);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Equal values in both branches", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    // Create ADD: if x0 then 5 else 5 (should simplify to constant 5)
    DdNode *add = Cudd_addIte(manager, var0, const5, const5);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_V(result) == 5.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - With negative values", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *constNeg5 = Cudd_addConst(manager, -5.0);
    DdNode *constNeg10 = Cudd_addConst(manager, -10.0);
    REQUIRE(constNeg5 != nullptr);
    REQUIRE(constNeg10 != nullptr);
    Cudd_Ref(constNeg5);
    Cudd_Ref(constNeg10);

    DdNode *add = Cudd_addIte(manager, var0, constNeg5, constNeg10);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_V(result) == -5.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, constNeg10);
    Cudd_RecursiveDeref(manager, constNeg5);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_addFindMin
// ============================================================================

TEST_CASE("Cudd_addFindMin - Constant ADD returns itself", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    DdNode *result = Cudd_addFindMin(manager, const5);
    REQUIRE(result != nullptr);
    REQUIRE(result == const5);
    REQUIRE(Cudd_V(result) == 5.0);

    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Simple ADD with two leaves", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const10 = Cudd_addConst(manager, 10.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const10 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const10);
    Cudd_Ref(const3);

    // Create ADD: if x0 then 10 else 3
    DdNode *add = Cudd_addIte(manager, var0, const10, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 3.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Min in then branch", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const10 = Cudd_addConst(manager, 10.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const10 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const10);

    // Create ADD: if x0 then 2 else 10
    DdNode *add = Cudd_addIte(manager, var0, const2, const10);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 2.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Complex ADD with multiple variables", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const7 = Cudd_addConst(manager, 7.0);
    DdNode *const100 = Cudd_addConst(manager, 100.0);
    REQUIRE(const1 != nullptr);
    REQUIRE(const5 != nullptr);
    REQUIRE(const7 != nullptr);
    REQUIRE(const100 != nullptr);
    Cudd_Ref(const1);
    Cudd_Ref(const5);
    Cudd_Ref(const7);
    Cudd_Ref(const100);

    // Create: if x0 then (if x1 then 100 else 5) else (if x1 then 7 else 1)
    DdNode *then_branch = Cudd_addIte(manager, var1, const100, const5);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);

    DdNode *else_branch = Cudd_addIte(manager, var1, const7, const1);
    REQUIRE(else_branch != nullptr);
    Cudd_Ref(else_branch);

    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_branch);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, const100);
    Cudd_RecursiveDeref(manager, const7);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Cache hit path", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const10 = Cudd_addConst(manager, 10.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const10 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const10);
    Cudd_Ref(const3);

    DdNode *add = Cudd_addIte(manager, var0, const10, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // First call - computes and caches result
    DdNode *result1 = Cudd_addFindMin(manager, add);
    REQUIRE(result1 != nullptr);
    
    // Second call - should hit cache
    DdNode *result2 = Cudd_addFindMin(manager, add);
    REQUIRE(result2 != nullptr);
    REQUIRE(result1 == result2);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - With minus infinity", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *minusInf = Cudd_ReadMinusInfinity(manager);
    REQUIRE(minusInf != nullptr);

    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);

    // Create ADD: if x0 then -infinity else 3
    DdNode *add = Cudd_addIte(manager, var0, minusInf, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Should return -infinity immediately
    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result == minusInf);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Equal values in both branches", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    DdNode *add = Cudd_addIte(manager, var0, const5, const5);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_V(result) == 5.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - With negative values", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *constNeg5 = Cudd_addConst(manager, -5.0);
    DdNode *constNeg10 = Cudd_addConst(manager, -10.0);
    REQUIRE(constNeg5 != nullptr);
    REQUIRE(constNeg10 != nullptr);
    Cudd_Ref(constNeg5);
    Cudd_Ref(constNeg10);

    DdNode *add = Cudd_addIte(manager, var0, constNeg5, constNeg10);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_V(result) == -10.0);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, constNeg10);
    Cudd_RecursiveDeref(manager, constNeg5);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_addIthBit
// ============================================================================

TEST_CASE("Cudd_addIthBit - Constant with bit 0 set", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Value 5 = 0b101, bit 0 is set
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    DdNode *result = Cudd_addIthBit(manager, const5, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Constant with bit 0 not set", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Value 4 = 0b100, bit 0 is not set
    DdNode *const4 = Cudd_addConst(manager, 4.0);
    REQUIRE(const4 != nullptr);
    Cudd_Ref(const4);

    DdNode *result = Cudd_addIthBit(manager, const4, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 0.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const4);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Constant with bit 2 set", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Value 5 = 0b101, bit 2 is set
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    DdNode *result = Cudd_addIthBit(manager, const5, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Constant with bit 1 not set", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Value 5 = 0b101, bit 1 is not set
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);

    DdNode *result = Cudd_addIthBit(manager, const5, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 0.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Non-constant ADD", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    // Value 3 = 0b011, bit 0 is set
    // Value 4 = 0b100, bit 0 is not set
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const4 = Cudd_addConst(manager, 4.0);
    REQUIRE(const3 != nullptr);
    REQUIRE(const4 != nullptr);
    Cudd_Ref(const3);
    Cudd_Ref(const4);

    // Create ADD: if x0 then 3 else 4
    DdNode *add = Cudd_addIte(manager, var0, const3, const4);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Extract bit 0: should be if x0 then 1 else 0
    DdNode *result = Cudd_addIthBit(manager, add, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const4);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Both branches have same bit value", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    // Value 3 = 0b011, bit 0 is set
    // Value 5 = 0b101, bit 0 is also set
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const3 != nullptr);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const3);
    Cudd_Ref(const5);

    // Create ADD: if x0 then 3 else 5
    DdNode *add = Cudd_addIte(manager, var0, const3, const5);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Extract bit 0: should be constant 1 (T == E case)
    DdNode *result = Cudd_addIthBit(manager, add, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Cache hit test", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const4 = Cudd_addConst(manager, 4.0);
    REQUIRE(const3 != nullptr);
    REQUIRE(const4 != nullptr);
    Cudd_Ref(const3);
    Cudd_Ref(const4);

    DdNode *add = Cudd_addIte(manager, var0, const3, const4);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // First call
    DdNode *result1 = Cudd_addIthBit(manager, add, 0);
    REQUIRE(result1 != nullptr);
    Cudd_Ref(result1);

    // Second call - should hit cache
    DdNode *result2 = Cudd_addIthBit(manager, add, 0);
    REQUIRE(result2 != nullptr);
    REQUIRE(result1 == result2);

    Cudd_RecursiveDeref(manager, result1);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const4);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Multiple variables", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    DdNode *const1 = Cudd_addConst(manager, 1.0);  // bit 0 = 1
    DdNode *const2 = Cudd_addConst(manager, 2.0);  // bit 0 = 0
    DdNode *const3 = Cudd_addConst(manager, 3.0);  // bit 0 = 1
    DdNode *const4 = Cudd_addConst(manager, 4.0);  // bit 0 = 0
    REQUIRE(const1 != nullptr);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    REQUIRE(const4 != nullptr);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    Cudd_Ref(const4);

    // Create: if x0 then (if x1 then 1 else 2) else (if x1 then 3 else 4)
    DdNode *then_branch = Cudd_addIte(manager, var1, const1, const2);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);

    DdNode *else_branch = Cudd_addIte(manager, var1, const3, const4);
    REQUIRE(else_branch != nullptr);
    Cudd_Ref(else_branch);

    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Extract bit 0
    DdNode *result = Cudd_addIthBit(manager, add, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be non-constant since different paths have different bit 0 values
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_branch);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, const4);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Higher bit position", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Value 8 = 0b1000, bit 3 is set
    DdNode *const8 = Cudd_addConst(manager, 8.0);
    REQUIRE(const8 != nullptr);
    Cudd_Ref(const8);

    DdNode *result = Cudd_addIthBit(manager, const8, 3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    // bit 2 should be 0
    DdNode *result2 = Cudd_addIthBit(manager, const8, 2);
    REQUIRE(result2 != nullptr);
    Cudd_Ref(result2);
    REQUIRE(Cudd_IsConstant(result2));
    REQUIRE(Cudd_V(result2) == 0.0);

    Cudd_RecursiveDeref(manager, result2);
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const8);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Zero value", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *zero = Cudd_ReadZero(manager);
    REQUIRE(zero != nullptr);

    // All bits of 0 should be 0
    for (int bit = 0; bit < 8; bit++) {
        DdNode *result = Cudd_addIthBit(manager, zero, bit);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE(Cudd_V(result) == 0.0);
        Cudd_RecursiveDeref(manager, result);
    }

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Value 255 has all low bits set", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const255 = Cudd_addConst(manager, 255.0);
    REQUIRE(const255 != nullptr);
    Cudd_Ref(const255);

    // Bits 0-7 should all be 1
    for (int bit = 0; bit < 8; bit++) {
        DdNode *result = Cudd_addIthBit(manager, const255, bit);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(Cudd_IsConstant(result));
        REQUIRE(Cudd_V(result) == 1.0);
        Cudd_RecursiveDeref(manager, result);
    }

    // Bit 8 should be 0
    DdNode *bit8 = Cudd_addIthBit(manager, const255, 8);
    REQUIRE(bit8 != nullptr);
    Cudd_Ref(bit8);
    REQUIRE(Cudd_IsConstant(bit8));
    REQUIRE(Cudd_V(bit8) == 0.0);
    Cudd_RecursiveDeref(manager, bit8);

    Cudd_RecursiveDeref(manager, const255);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Fractional values are truncated", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // 5.9 should be treated as 5 = 0b101
    DdNode *const5_9 = Cudd_addConst(manager, 5.9);
    REQUIRE(const5_9 != nullptr);
    Cudd_Ref(const5_9);

    DdNode *bit0 = Cudd_addIthBit(manager, const5_9, 0);
    REQUIRE(bit0 != nullptr);
    Cudd_Ref(bit0);
    REQUIRE(Cudd_V(bit0) == 1.0);

    DdNode *bit1 = Cudd_addIthBit(manager, const5_9, 1);
    REQUIRE(bit1 != nullptr);
    Cudd_Ref(bit1);
    REQUIRE(Cudd_V(bit1) == 0.0);

    DdNode *bit2 = Cudd_addIthBit(manager, const5_9, 2);
    REQUIRE(bit2 != nullptr);
    Cudd_Ref(bit2);
    REQUIRE(Cudd_V(bit2) == 1.0);

    Cudd_RecursiveDeref(manager, bit2);
    Cudd_RecursiveDeref(manager, bit1);
    Cudd_RecursiveDeref(manager, bit0);
    Cudd_RecursiveDeref(manager, const5_9);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional edge cases and stress tests
// ============================================================================

TEST_CASE("Cudd_addFindMax - Large ADD", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD with many variables
    DdNode *vars[5];
    for (int i = 0; i < 5; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }

    // Start with a base value
    DdNode *add = Cudd_addConst(manager, 0.0);
    Cudd_Ref(add);

    // Build a sum of variables to create a more complex ADD
    for (int i = 0; i < 5; i++) {
        DdNode *scaled = Cudd_addApply(manager, Cudd_addTimes, vars[i], 
                                        Cudd_addConst(manager, (double)(1 << i)));
        REQUIRE(scaled != nullptr);
        Cudd_Ref(scaled);
        
        DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, scaled);
        REQUIRE(tmp != nullptr);
        Cudd_Ref(tmp);
        
        Cudd_RecursiveDeref(manager, scaled);
        Cudd_RecursiveDeref(manager, add);
        add = tmp;
    }

    // Find max - should be sum of all 2^i = 31
    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 31.0);

    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < 5; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Large ADD", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *vars[5];
    for (int i = 0; i < 5; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }

    DdNode *add = Cudd_addConst(manager, 0.0);
    Cudd_Ref(add);

    for (int i = 0; i < 5; i++) {
        DdNode *scaled = Cudd_addApply(manager, Cudd_addTimes, vars[i], 
                                        Cudd_addConst(manager, (double)(1 << i)));
        REQUIRE(scaled != nullptr);
        Cudd_Ref(scaled);
        
        DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, scaled);
        REQUIRE(tmp != nullptr);
        Cudd_Ref(tmp);
        
        Cudd_RecursiveDeref(manager, scaled);
        Cudd_RecursiveDeref(manager, add);
        add = tmp;
    }

    // Find min - should be 0 (when all variables are 0)
    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 0.0);

    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < 5; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Complex ADD with T == E simplification", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    // All values have bit 0 set: 1, 3, 5, 7
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const7 = Cudd_addConst(manager, 7.0);
    REQUIRE(const1 != nullptr);
    REQUIRE(const3 != nullptr);
    REQUIRE(const5 != nullptr);
    REQUIRE(const7 != nullptr);
    Cudd_Ref(const1);
    Cudd_Ref(const3);
    Cudd_Ref(const5);
    Cudd_Ref(const7);

    DdNode *then_branch = Cudd_addIte(manager, var1, const7, const5);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);

    DdNode *else_branch = Cudd_addIte(manager, var1, const3, const1);
    REQUIRE(else_branch != nullptr);
    Cudd_Ref(else_branch);

    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Extract bit 0 - all values have bit 0 set, so result should be constant 1
    DdNode *result = Cudd_addIthBit(manager, add, 0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_branch);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, const7);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

// ============================================================================
// Memory constraint tests to try to trigger error paths
// ============================================================================

TEST_CASE("Cudd_addIthBit - Test under memory constraints", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Set a small memory limit
    Cudd_SetMaxMemory(manager, 1024 * 64); // 64KB
    
    // Create a few variables
    DdNode *vars[8];
    for (int i = 0; i < 8; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        if (vars[i] == nullptr) break;
        Cudd_Ref(vars[i]);
    }
    
    // Try to create a moderately complex ADD
    DdNode *add = Cudd_addConst(manager, 1.0);
    if (add != nullptr) {
        Cudd_Ref(add);
        
        for (int i = 0; i < 8 && vars[i] != nullptr; i++) {
            DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, vars[i]);
            if (tmp == nullptr) break;
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, add);
            add = tmp;
        }
        
        // Try addIthBit - it may or may not succeed under memory pressure
        DdNode *result = Cudd_addIthBit(manager, add, 0);
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, add);
    }
    
    for (int i = 0; i < 8 && vars[i] != nullptr; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Test under memory constraints", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    Cudd_SetMaxMemory(manager, 1024 * 64); // 64KB
    
    DdNode *vars[8];
    for (int i = 0; i < 8; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        if (vars[i] == nullptr) break;
        Cudd_Ref(vars[i]);
    }
    
    DdNode *add = Cudd_addConst(manager, 1.0);
    if (add != nullptr) {
        Cudd_Ref(add);
        
        for (int i = 0; i < 8 && vars[i] != nullptr; i++) {
            DdNode *scaled = Cudd_addApply(manager, Cudd_addTimes, vars[i], 
                                            Cudd_addConst(manager, (double)(1 << i)));
            if (scaled == nullptr) break;
            Cudd_Ref(scaled);
            
            DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, scaled);
            Cudd_RecursiveDeref(manager, scaled);
            if (tmp == nullptr) break;
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, add);
            add = tmp;
        }
        
        // Try to find max
        DdNode *result = Cudd_addFindMax(manager, add);
        if (result != nullptr) {
            REQUIRE(Cudd_IsConstant(result));
        }
        
        Cudd_RecursiveDeref(manager, add);
    }
    
    for (int i = 0; i < 8 && vars[i] != nullptr; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Test under memory constraints", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    Cudd_SetMaxMemory(manager, 1024 * 64); // 64KB
    
    DdNode *vars[8];
    for (int i = 0; i < 8; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        if (vars[i] == nullptr) break;
        Cudd_Ref(vars[i]);
    }
    
    DdNode *add = Cudd_addConst(manager, 1.0);
    if (add != nullptr) {
        Cudd_Ref(add);
        
        for (int i = 0; i < 8 && vars[i] != nullptr; i++) {
            DdNode *scaled = Cudd_addApply(manager, Cudd_addTimes, vars[i], 
                                            Cudd_addConst(manager, (double)(1 << i)));
            if (scaled == nullptr) break;
            Cudd_Ref(scaled);
            
            DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, scaled);
            Cudd_RecursiveDeref(manager, scaled);
            if (tmp == nullptr) break;
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, add);
            add = tmp;
        }
        
        // Try to find min
        DdNode *result = Cudd_addFindMin(manager, add);
        if (result != nullptr) {
            REQUIRE(Cudd_IsConstant(result));
        }
        
        Cudd_RecursiveDeref(manager, add);
    }
    
    for (int i = 0; i < 8 && vars[i] != nullptr; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIthBit - Deep recursive ADD", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create a deep ADD with 10 variables
    const int NVARS = 10;
    DdNode *vars[NVARS];
    for (int i = 0; i < NVARS; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }

    // Build a deeply nested ADD
    DdNode *add = Cudd_addConst(manager, 0.0);
    Cudd_Ref(add);

    for (int i = 0; i < NVARS; i++) {
        // Each level adds 2^i if the variable is true
        DdNode *constVal = Cudd_addConst(manager, (double)(1 << i));
        REQUIRE(constVal != nullptr);
        Cudd_Ref(constVal);

        // if var[i] then (add + 2^i) else add
        DdNode *addPlusConst = Cudd_addApply(manager, Cudd_addPlus, add, constVal);
        REQUIRE(addPlusConst != nullptr);
        Cudd_Ref(addPlusConst);

        DdNode *tmp = Cudd_addIte(manager, vars[i], addPlusConst, add);
        REQUIRE(tmp != nullptr);
        Cudd_Ref(tmp);

        Cudd_RecursiveDeref(manager, addPlusConst);
        Cudd_RecursiveDeref(manager, constVal);
        Cudd_RecursiveDeref(manager, add);
        add = tmp;
    }

    // Extract various bits
    for (int bit = 0; bit < 10; bit++) {
        DdNode *result = Cudd_addIthBit(manager, add, bit);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }

    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < NVARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMax - Deep recursive structure", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    const int NVARS = 10;
    DdNode *vars[NVARS];
    for (int i = 0; i < NVARS; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }

    // Build a deeply nested ADD with varying leaf values
    DdNode *add = Cudd_addConst(manager, 0.0);
    Cudd_Ref(add);

    for (int i = 0; i < NVARS; i++) {
        DdNode *constVal = Cudd_addConst(manager, (double)(1 << i));
        REQUIRE(constVal != nullptr);
        Cudd_Ref(constVal);

        DdNode *addPlusConst = Cudd_addApply(manager, Cudd_addPlus, add, constVal);
        REQUIRE(addPlusConst != nullptr);
        Cudd_Ref(addPlusConst);

        DdNode *tmp = Cudd_addIte(manager, vars[i], addPlusConst, add);
        REQUIRE(tmp != nullptr);
        Cudd_Ref(tmp);

        Cudd_RecursiveDeref(manager, addPlusConst);
        Cudd_RecursiveDeref(manager, constVal);
        Cudd_RecursiveDeref(manager, add);
        add = tmp;
    }

    // Max should be sum of all 2^i from 0 to 9 = 2^10 - 1 = 1023
    DdNode *result = Cudd_addFindMax(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1023.0);

    // Call again to test cache
    DdNode *result2 = Cudd_addFindMax(manager, add);
    REQUIRE(result2 == result);

    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < NVARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addFindMin - Deep recursive structure", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    const int NVARS = 10;
    DdNode *vars[NVARS];
    for (int i = 0; i < NVARS; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }

    // Build a deeply nested ADD with varying leaf values
    DdNode *add = Cudd_addConst(manager, 0.0);
    Cudd_Ref(add);

    for (int i = 0; i < NVARS; i++) {
        DdNode *constVal = Cudd_addConst(manager, (double)(1 << i));
        REQUIRE(constVal != nullptr);
        Cudd_Ref(constVal);

        DdNode *addPlusConst = Cudd_addApply(manager, Cudd_addPlus, add, constVal);
        REQUIRE(addPlusConst != nullptr);
        Cudd_Ref(addPlusConst);

        DdNode *tmp = Cudd_addIte(manager, vars[i], addPlusConst, add);
        REQUIRE(tmp != nullptr);
        Cudd_Ref(tmp);

        Cudd_RecursiveDeref(manager, addPlusConst);
        Cudd_RecursiveDeref(manager, constVal);
        Cudd_RecursiveDeref(manager, add);
        add = tmp;
    }

    // Min should be 0 (when all variables are false)
    DdNode *result = Cudd_addFindMin(manager, add);
    REQUIRE(result != nullptr);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 0.0);

    // Call again to test cache
    DdNode *result2 = Cudd_addFindMin(manager, add);
    REQUIRE(result2 == result);

    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < NVARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

// Test with extreme memory limits to potentially trigger error paths
TEST_CASE("Cudd_addIthBit - Extreme memory constraint test", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Set extremely small memory limit
    Cudd_SetMaxMemory(manager, 1024 * 16); // 16KB - very small
    
    // Try to create many variables and complex ADDs
    DdNode *vars[12];
    bool hasVars = true;
    for (int i = 0; i < 12 && hasVars; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        if (vars[i] == nullptr) {
            hasVars = false;
            break;
        }
        Cudd_Ref(vars[i]);
    }
    
    if (hasVars) {
        // Try to build a complex ADD that might exhaust memory
        DdNode *add = Cudd_addConst(manager, 1.0);
        bool success = (add != nullptr);
        if (success) {
            Cudd_Ref(add);
            
            for (int i = 0; i < 12 && success; i++) {
                DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, vars[i]);
                if (tmp == nullptr) {
                    success = false;
                    break;
                }
                Cudd_Ref(tmp);
                Cudd_RecursiveDeref(manager, add);
                add = tmp;
            }
            
            // Now try addIthBit which may fail due to memory constraints
            for (int bit = 0; bit < 8 && success; bit++) {
                DdNode *result = Cudd_addIthBit(manager, add, bit);
                // Result may be NULL if memory is exhausted
                if (result != nullptr) {
                    Cudd_Ref(result);
                    Cudd_RecursiveDeref(manager, result);
                }
            }
            
            Cudd_RecursiveDeref(manager, add);
        }
        
        for (int i = 0; i < 12 && vars[i] != nullptr; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    Cudd_Quit(manager);
}

// Test to ensure we exercise all constant value paths
TEST_CASE("Cudd_addIthBit - Various constant values", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Test with various constants to exercise mask and value calculation
    double testValues[] = {0.0, 1.0, 2.0, 3.0, 7.0, 8.0, 15.0, 16.0, 31.0, 32.0, 63.0, 64.0, 127.0, 128.0, 255.0, 256.0};
    
    for (double value : testValues) {
        DdNode *constNode = Cudd_addConst(manager, value);
        REQUIRE(constNode != nullptr);
        Cudd_Ref(constNode);
        
        // Test bits 0-10
        for (int bit = 0; bit <= 10; bit++) {
            DdNode *result = Cudd_addIthBit(manager, constNode, bit);
            REQUIRE(result != nullptr);
            Cudd_Ref(result);
            
            // Verify the result
            int intValue = (int)value;
            int expectedBit = (intValue >> bit) & 1;
            REQUIRE(Cudd_V(result) == (double)expectedBit);
            
            Cudd_RecursiveDeref(manager, result);
        }
        
        Cudd_RecursiveDeref(manager, constNode);
    }

    Cudd_Quit(manager);
}

// Test with maximum value to test high bit positions
TEST_CASE("Cudd_addIthBit - High bit positions", "[cuddAddFind]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Test with large values
    DdNode *const1024 = Cudd_addConst(manager, 1024.0);  // 2^10
    REQUIRE(const1024 != nullptr);
    Cudd_Ref(const1024);

    // Bit 10 should be 1
    DdNode *bit10 = Cudd_addIthBit(manager, const1024, 10);
    REQUIRE(bit10 != nullptr);
    Cudd_Ref(bit10);
    REQUIRE(Cudd_V(bit10) == 1.0);
    Cudd_RecursiveDeref(manager, bit10);

    // Bit 9 should be 0
    DdNode *bit9 = Cudd_addIthBit(manager, const1024, 9);
    REQUIRE(bit9 != nullptr);
    Cudd_Ref(bit9);
    REQUIRE(Cudd_V(bit9) == 0.0);
    Cudd_RecursiveDeref(manager, bit9);

    Cudd_RecursiveDeref(manager, const1024);
    Cudd_Quit(manager);
}
