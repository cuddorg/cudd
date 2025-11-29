#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddMatMult.c
 * 
 * This file contains comprehensive tests for the cuddMatMult module
 * to achieve 90% code coverage of the source file.
 */

// ============================================================================
// Tests for Cudd_addMatrixMultiply
// ============================================================================

TEST_CASE("Cudd_addMatrixMultiply - Basic 2x2 identity matrix", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create variables: x0, x1 for rows of A, z0, z1 for columns of A / rows of B
    // y0, y1 for columns of B
    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *x1 = Cudd_addIthVar(manager, 1);
    DdNode *z0 = Cudd_addIthVar(manager, 2);
    DdNode *z1 = Cudd_addIthVar(manager, 3);
    DdNode *y0 = Cudd_addIthVar(manager, 4);
    DdNode *y1 = Cudd_addIthVar(manager, 5);
    
    REQUIRE(x0 != nullptr);
    REQUIRE(x1 != nullptr);
    REQUIRE(z0 != nullptr);
    REQUIRE(z1 != nullptr);
    REQUIRE(y0 != nullptr);
    REQUIRE(y1 != nullptr);
    
    Cudd_Ref(x0);
    Cudd_Ref(x1);
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    Cudd_Ref(y0);
    Cudd_Ref(y1);

    // Create identity matrix A (depends on x and z)
    // A[i][j] = 1 if i == j, 0 otherwise
    // Using ADD: (x0 <-> z0) AND (x1 <-> z1)
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    // Create XNOR for x0 and z0
    DdNode *xnor_xz0 = Cudd_addApply(manager, Cudd_addXnor, x0, z0);
    REQUIRE(xnor_xz0 != nullptr);
    Cudd_Ref(xnor_xz0);
    
    // Create XNOR for x1 and z1
    DdNode *xnor_xz1 = Cudd_addApply(manager, Cudd_addXnor, x1, z1);
    REQUIRE(xnor_xz1 != nullptr);
    Cudd_Ref(xnor_xz1);
    
    // A = xnor_xz0 AND xnor_xz1
    DdNode *A = Cudd_addApply(manager, Cudd_addTimes, xnor_xz0, xnor_xz1);
    REQUIRE(A != nullptr);
    Cudd_Ref(A);
    
    // Create identity matrix B (depends on z and y)
    DdNode *xnor_zy0 = Cudd_addApply(manager, Cudd_addXnor, z0, y0);
    REQUIRE(xnor_zy0 != nullptr);
    Cudd_Ref(xnor_zy0);
    
    DdNode *xnor_zy1 = Cudd_addApply(manager, Cudd_addXnor, z1, y1);
    REQUIRE(xnor_zy1 != nullptr);
    Cudd_Ref(xnor_zy1);
    
    DdNode *B = Cudd_addApply(manager, Cudd_addTimes, xnor_zy0, xnor_zy1);
    REQUIRE(B != nullptr);
    Cudd_Ref(B);
    
    // Create z variables array for summation
    DdNode *z[2] = {z0, z1};
    
    // Multiply A * B
    DdNode *result = Cudd_addMatrixMultiply(manager, A, B, z, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be identity matrix (x <-> y)
    // Clean up
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, B);
    Cudd_RecursiveDeref(manager, xnor_zy1);
    Cudd_RecursiveDeref(manager, xnor_zy0);
    Cudd_RecursiveDeref(manager, A);
    Cudd_RecursiveDeref(manager, xnor_xz1);
    Cudd_RecursiveDeref(manager, xnor_xz0);
    Cudd_RecursiveDeref(manager, y1);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z1);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x1);
    Cudd_RecursiveDeref(manager, x0);
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addMatrixMultiply - Zero matrix multiplication", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);
    
    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    
    // A = 0, B = any
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addMatrixMultiply(manager, zero, one, z, 1);
    REQUIRE(result == zero);
    
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addMatrixMultiply - Constant matrices", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    // 2 * 3 with 1 summation variable = 2 * 3 * 2 = 12
    DdNode *result = Cudd_addMatrixMultiply(manager, const2, const3, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 12.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addMatrixMultiply - Non-summation variable split", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Variables: x0 (row), z0 (sum), y0 (col)
    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *z0 = Cudd_addIthVar(manager, 1);
    DdNode *y0 = Cudd_addIthVar(manager, 2);
    
    REQUIRE(x0 != nullptr);
    REQUIRE(z0 != nullptr);
    REQUIRE(y0 != nullptr);
    
    Cudd_Ref(x0);
    Cudd_Ref(z0);
    Cudd_Ref(y0);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    // A depends on x0 and z0: if x0 then z0 else 1
    DdNode *A = Cudd_addIte(manager, x0, z0, one);
    REQUIRE(A != nullptr);
    Cudd_Ref(A);
    
    // B depends on z0 and y0: if z0 then y0 else 1
    DdNode *B = Cudd_addIte(manager, z0, y0, one);
    REQUIRE(B != nullptr);
    Cudd_Ref(B);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addMatrixMultiply(manager, A, B, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, B);
    Cudd_RecursiveDeref(manager, A);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addMatrixMultiply - Cache hit path", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *z0 = Cudd_addIthVar(manager, 1);
    DdNode *y0 = Cudd_addIthVar(manager, 2);
    
    Cudd_Ref(x0);
    Cudd_Ref(z0);
    Cudd_Ref(y0);
    
    DdNode *z[1] = {z0};
    
    // First call
    DdNode *result1 = Cudd_addMatrixMultiply(manager, x0, y0, z, 1);
    REQUIRE(result1 != nullptr);
    Cudd_Ref(result1);
    
    // Second call - should hit cache
    DdNode *result2 = Cudd_addMatrixMultiply(manager, x0, y0, z, 1);
    REQUIRE(result2 != nullptr);
    REQUIRE(result2 == result1);
    
    Cudd_RecursiveDeref(manager, result1);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addMatrixMultiply - Multiple summation variables", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *z0 = Cudd_addIthVar(manager, 1);
    DdNode *z1 = Cudd_addIthVar(manager, 2);
    DdNode *y0 = Cudd_addIthVar(manager, 3);
    
    Cudd_Ref(x0);
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    Cudd_Ref(y0);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *z[2] = {z0, z1};
    
    // Multiply constants with 2 summation variables
    DdNode *result = Cudd_addMatrixMultiply(manager, one, one, z, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    // Result should be 4 (2^2 for 2 summation variables)
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 4.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z1);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_addTimesPlus
// ============================================================================

TEST_CASE("Cudd_addTimesPlus - Basic multiplication", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addTimesPlus(manager, const2, const3, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    // 2 * 3 = 6, summed over z0 (2 values) = 12
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 12.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTimesPlus - Zero matrix", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);
    
    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addTimesPlus(manager, zero, one, z, 1);
    REQUIRE(result == zero);
    
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTimesPlus - Multiple summation variables", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    DdNode *z1 = Cudd_addIthVar(manager, 1);
    REQUIRE(z0 != nullptr);
    REQUIRE(z1 != nullptr);
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *z[2] = {z0, z1};
    
    DdNode *result = Cudd_addTimesPlus(manager, one, one, z, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    // 1 * 1 = 1, summed over z0 and z1 (4 combinations) = 4
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 4.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, z1);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTimesPlus - Variable-dependent matrices", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *z0 = Cudd_addIthVar(manager, 1);
    DdNode *y0 = Cudd_addIthVar(manager, 2);
    
    Cudd_Ref(x0);
    Cudd_Ref(z0);
    Cudd_Ref(y0);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addTimesPlus(manager, x0, y0, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_addTriangle
// ============================================================================

TEST_CASE("Cudd_addTriangle - Basic triangulation", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addTriangle(manager, const2, const3, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    // min(2+3, 2+3) = 5
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 5.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTriangle - Plus infinity operand", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);
    
    DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(plusInf != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    // If f is +infinity, result should be +infinity
    DdNode *result = Cudd_addTriangle(manager, plusInf, const3, z, 1);
    REQUIRE(result == plusInf);
    
    // If g is +infinity, result should be +infinity
    DdNode *result2 = Cudd_addTriangle(manager, const3, plusInf, z, 1);
    REQUIRE(result2 == plusInf);
    
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTriangle - Variable-dependent inputs", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *z0 = Cudd_addIthVar(manager, 1);
    DdNode *y0 = Cudd_addIthVar(manager, 2);
    
    Cudd_Ref(x0);
    Cudd_Ref(z0);
    Cudd_Ref(y0);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    Cudd_Ref(const5);
    
    // f depends on x0 and z0
    DdNode *f = Cudd_addIte(manager, x0, Cudd_addIte(manager, z0, const1, const2), Cudd_addIte(manager, z0, const3, const5));
    REQUIRE(f != nullptr);
    Cudd_Ref(f);
    
    // g depends on z0 and y0
    DdNode *g = Cudd_addIte(manager, z0, Cudd_addIte(manager, y0, const2, const1), Cudd_addIte(manager, y0, const5, const3));
    REQUIRE(g != nullptr);
    Cudd_Ref(g);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addTriangle(manager, f, g, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, g);
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTriangle - Non-abstraction variable split", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *z0 = Cudd_addIthVar(manager, 1);
    
    Cudd_Ref(x0);
    Cudd_Ref(z0);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    
    // f depends only on x0 (not a summation variable)
    DdNode *f = Cudd_addIte(manager, x0, const1, const2);
    REQUIRE(f != nullptr);
    Cudd_Ref(f);
    
    DdNode *z[1] = {z0};
    
    DdNode *result = Cudd_addTriangle(manager, f, const1, z, 1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, f);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTriangle - Cache hit path", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    Cudd_Ref(z0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    // First call
    DdNode *result1 = Cudd_addTriangle(manager, const2, const3, z, 1);
    REQUIRE(result1 != nullptr);
    Cudd_Ref(result1);
    
    // Second call - should hit cache
    DdNode *result2 = Cudd_addTriangle(manager, const2, const3, z, 1);
    REQUIRE(result2 != nullptr);
    REQUIRE(result2 == result1);
    
    Cudd_RecursiveDeref(manager, result1);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

// ============================================================================
// Tests for Cudd_addOuterSum
// ============================================================================

TEST_CASE("Cudd_addOuterSum - Basic outer sum", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const5 != nullptr);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const5);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    // M = 5, r = 2, c = 3
    // OuterSum returns min(M, r+c) = min(5, 5) = 5
    DdNode *result = Cudd_addOuterSum(manager, const5, const2, const3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 5.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - OuterSum smaller than M", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const10 = Cudd_addConst(manager, 10.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const10 != nullptr);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const10);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    // M = 10, r = 2, c = 3
    // OuterSum returns min(M, r+c) = min(10, 5) = 5
    DdNode *result = Cudd_addOuterSum(manager, const10, const2, const3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 5.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - Plus infinity in r", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const5 != nullptr);
    REQUIRE(plusInf != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const5);
    Cudd_Ref(const3);
    
    // If r is +infinity, result should be M
    DdNode *result = Cudd_addOuterSum(manager, const5, plusInf, const3);
    REQUIRE(result == const5);
    
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - Plus infinity in c", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
    REQUIRE(const5 != nullptr);
    REQUIRE(const2 != nullptr);
    REQUIRE(plusInf != nullptr);
    Cudd_Ref(const5);
    Cudd_Ref(const2);
    
    // If c is +infinity, result should be M
    DdNode *result = Cudd_addOuterSum(manager, const5, const2, plusInf);
    REQUIRE(result == const5);
    
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - Variable-dependent inputs", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *y0 = Cudd_addIthVar(manager, 1);
    
    Cudd_Ref(x0);
    Cudd_Ref(y0);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const10 = Cudd_addConst(manager, 10.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    Cudd_Ref(const10);
    
    // M depends on x0
    DdNode *M = Cudd_addIte(manager, x0, const10, const2);
    REQUIRE(M != nullptr);
    Cudd_Ref(M);
    
    // r depends on x0
    DdNode *r = Cudd_addIte(manager, x0, const1, const3);
    REQUIRE(r != nullptr);
    Cudd_Ref(r);
    
    // c depends on y0
    DdNode *c = Cudd_addIte(manager, y0, const2, const1);
    REQUIRE(c != nullptr);
    Cudd_Ref(c);
    
    DdNode *result = Cudd_addOuterSum(manager, M, r, c);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, c);
    Cudd_RecursiveDeref(manager, r);
    Cudd_RecursiveDeref(manager, M);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - Cache hit path", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    Cudd_Ref(x0);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const10 = Cudd_addConst(manager, 10.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const10);
    
    DdNode *M = Cudd_addIte(manager, x0, const10, const2);
    Cudd_Ref(M);
    
    // First call
    DdNode *result1 = Cudd_addOuterSum(manager, M, const1, const2);
    REQUIRE(result1 != nullptr);
    Cudd_Ref(result1);
    
    // Second call - should hit cache
    DdNode *result2 = Cudd_addOuterSum(manager, M, const1, const2);
    REQUIRE(result2 != nullptr);
    REQUIRE(result2 == result1);
    
    Cudd_RecursiveDeref(manager, result1);
    Cudd_RecursiveDeref(manager, M);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - M is constant but R is not", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    Cudd_Ref(x0);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const10 = Cudd_addConst(manager, 10.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const10);
    
    // r depends on x0
    DdNode *r = Cudd_addIte(manager, x0, const1, const2);
    REQUIRE(r != nullptr);
    Cudd_Ref(r);
    
    // M is constant, r is variable, c is constant
    DdNode *result = Cudd_addOuterSum(manager, const10, r, const1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, r);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - R+C greater than M (constant case)", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const4 = Cudd_addConst(manager, 4.0);
    REQUIRE(const3 != nullptr);
    REQUIRE(const5 != nullptr);
    REQUIRE(const4 != nullptr);
    Cudd_Ref(const3);
    Cudd_Ref(const5);
    Cudd_Ref(const4);
    
    // M = 3, r = 5, c = 4
    // OuterSum returns min(M, r+c) = min(3, 9) = 3
    DdNode *result = Cudd_addOuterSum(manager, const3, const5, const4);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 3.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const4);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_Quit(manager);
}

// ============================================================================
// Additional coverage tests
// ============================================================================

TEST_CASE("Cudd_addMatrixMultiply - A > B standardization path", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    Cudd_Ref(z0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    // Call with A > B (pointer comparison) to trigger standardization
    DdNode *result1 = Cudd_addMatrixMultiply(manager, const3, const2, z, 1);
    REQUIRE(result1 != nullptr);
    Cudd_Ref(result1);
    
    // Call with A < B
    DdNode *result2 = Cudd_addMatrixMultiply(manager, const2, const3, z, 1);
    REQUIRE(result2 != nullptr);
    Cudd_Ref(result2);
    
    // Results should be equal
    REQUIRE(result1 == result2);
    
    Cudd_RecursiveDeref(manager, result2);
    Cudd_RecursiveDeref(manager, result1);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTriangle - f > g standardization path", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    Cudd_Ref(z0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    DdNode *z[1] = {z0};
    
    // Call with f > g (pointer comparison) to trigger standardization
    DdNode *result1 = Cudd_addTriangle(manager, const3, const2, z, 1);
    REQUIRE(result1 != nullptr);
    Cudd_Ref(result1);
    
    // Call with f < g
    DdNode *result2 = Cudd_addTriangle(manager, const2, const3, z, 1);
    REQUIRE(result2 != nullptr);
    Cudd_Ref(result2);
    
    // Results should be equal (addition is commutative)
    REQUIRE(result1 == result2);
    
    Cudd_RecursiveDeref(manager, result2);
    Cudd_RecursiveDeref(manager, result1);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - Recursive with different top variables", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *x1 = Cudd_addIthVar(manager, 1);
    DdNode *x2 = Cudd_addIthVar(manager, 2);
    
    Cudd_Ref(x0);
    Cudd_Ref(x1);
    Cudd_Ref(x2);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const10 = Cudd_addConst(manager, 10.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    Cudd_Ref(const10);
    
    // M depends on x0
    DdNode *M = Cudd_addIte(manager, x0, const10, const3);
    Cudd_Ref(M);
    
    // r depends on x1
    DdNode *r = Cudd_addIte(manager, x1, const1, const2);
    Cudd_Ref(r);
    
    // c depends on x2
    DdNode *c = Cudd_addIte(manager, x2, const2, const1);
    Cudd_Ref(c);
    
    // All three have different top variables
    DdNode *result = Cudd_addOuterSum(manager, M, r, c);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, c);
    Cudd_RecursiveDeref(manager, r);
    Cudd_RecursiveDeref(manager, M);
    Cudd_RecursiveDeref(manager, const10);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, x2);
    Cudd_RecursiveDeref(manager, x1);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addMatrixMultiply - Complex matrix with scaling", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create more variables to exercise scaling paths
    DdNode *x0 = Cudd_addIthVar(manager, 0);
    DdNode *x1 = Cudd_addIthVar(manager, 1);
    DdNode *z0 = Cudd_addIthVar(manager, 2);
    DdNode *z1 = Cudd_addIthVar(manager, 3);
    DdNode *y0 = Cudd_addIthVar(manager, 4);
    DdNode *y1 = Cudd_addIthVar(manager, 5);
    
    Cudd_Ref(x0);
    Cudd_Ref(x1);
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    Cudd_Ref(y0);
    Cudd_Ref(y1);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    Cudd_Ref(const2);
    
    // A depends on x0, z0
    DdNode *A = Cudd_addApply(manager, Cudd_addTimes, x0, z0);
    Cudd_Ref(A);
    
    // B depends on z0, y0
    DdNode *B = Cudd_addApply(manager, Cudd_addTimes, z0, y0);
    Cudd_Ref(B);
    
    // Use multiple summation variables
    DdNode *z[2] = {z0, z1};
    
    DdNode *result = Cudd_addMatrixMultiply(manager, A, B, z, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, B);
    Cudd_RecursiveDeref(manager, A);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, y1);
    Cudd_RecursiveDeref(manager, y0);
    Cudd_RecursiveDeref(manager, z1);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_RecursiveDeref(manager, x1);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addTriangle - Multiple abstraction variables", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *z0 = Cudd_addIthVar(manager, 0);
    DdNode *z1 = Cudd_addIthVar(manager, 1);
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    
    DdNode *z[2] = {z0, z1};
    
    DdNode *result = Cudd_addTriangle(manager, const1, const2, z, 2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    // min over all paths of (1 + 2) = 3
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 3.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, z1);
    Cudd_RecursiveDeref(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOuterSum - Same then/else children path", "[cuddMatMult]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *x0 = Cudd_addIthVar(manager, 0);
    Cudd_Ref(x0);
    
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    Cudd_Ref(const5);
    Cudd_Ref(const1);
    
    // M = x0 ? 5 : 5 (same then/else)
    DdNode *M = Cudd_addIte(manager, x0, const5, const5);
    Cudd_Ref(M);
    
    DdNode *result = Cudd_addOuterSum(manager, M, const1, const1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    // min(5, 1+1) = min(5, 2) = 2
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 2.0);
    
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, M);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}
