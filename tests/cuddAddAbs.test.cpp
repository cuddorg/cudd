#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddAddAbs.c
 * 
 * This file contains comprehensive tests for the cuddAddAbs module
 * to achieve 100% code coverage of the source file.
 */

TEST_CASE("Cudd_addExistAbstract - Invalid cube returns NULL", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create a simple ADD
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    // Create an invalid cube (complemented node)
    DdNode *invalidCube = Cudd_Not(var0);
    
    // Test that addExistAbstract rejects invalid cube
    DdNode *result = Cudd_addExistAbstract(manager, var0, invalidCube);
    REQUIRE(result == nullptr);

    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Abstract with constant cube", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create a simple ADD
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    // Use constant one as cube (no variables to abstract)
    DdNode *one = Cudd_ReadOne(manager);
    
    // Should return the same ADD
    DdNode *result = Cudd_addExistAbstract(manager, var0, one);
    REQUIRE(result != nullptr);
    REQUIRE(result == var0);

    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Abstract zero ADD", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);

    // Abstracting from zero should return zero
    DdNode *result = Cudd_addExistAbstract(manager, zero, var0);
    REQUIRE(result == zero);

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Abstract single variable", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD: if x0 then 3 else 5
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);
    
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);
    
    // Create ADD using ITE
    DdNode *add = Cudd_addIte(manager, var0, const3, const5);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 - should sum 3 + 5 = 8
    DdNode *result = Cudd_addExistAbstract(manager, add, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be constant 8
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 8.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Abstract variable not in ADD", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD with var1
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var1);
    
    // Create cube with var0 (not in the ADD)
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);

    // Abstract var0 from ADD(var1) - should multiply by 2
    DdNode *result = Cudd_addExistAbstract(manager, var1, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Since var0 is not in the ADD, the result should be 2*var1
    // (summing over both possible values of var0)
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Abstract multiple variables", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD with two variables
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);
    
    // Create cube = var0 * var1
    DdNode *cube = Cudd_addApply(manager, Cudd_addTimes, var0, var1);
    REQUIRE(cube != nullptr);
    Cudd_Ref(cube);

    // Create a simple ADD
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    REQUIRE(const2 != nullptr);
    Cudd_Ref(const2);

    // Abstract both variables from constant
    DdNode *result = Cudd_addExistAbstract(manager, const2, cube);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be 2*4 = 8 (abstracting 2 variables multiplies by 2^2)
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 8.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, cube);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Invalid cube returns NULL", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    // Create an invalid cube (complemented node)
    DdNode *invalidCube = Cudd_Not(var0);
    
    // Test that addUnivAbstract rejects invalid cube
    DdNode *result = Cudd_addUnivAbstract(manager, var0, invalidCube);
    REQUIRE(result == nullptr);

    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Abstract with zero and one", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);

    // Abstracting from zero should return zero (0*0=0)
    DdNode *result1 = Cudd_addUnivAbstract(manager, zero, var0);
    REQUIRE(result1 == zero);

    // Abstracting from one should return one (1*1=1)
    DdNode *result2 = Cudd_addUnivAbstract(manager, one, var0);
    REQUIRE(result2 == one);

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Abstract single variable", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD: if x0 then 3 else 5
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const3);
    
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    REQUIRE(const5 != nullptr);
    Cudd_Ref(const5);
    
    DdNode *add = Cudd_addIte(manager, var0, const3, const5);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 - should multiply 3 * 5 = 15
    DdNode *result = Cudd_addUnivAbstract(manager, add, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 15.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Abstract variable not in ADD", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD with var1
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var1);
    
    // Create cube with var0 (not in the ADD)
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);

    // Abstract var0 from ADD(var1) - should square the result
    DdNode *result = Cudd_addUnivAbstract(manager, var1, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Invalid cube returns NULL", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);

    // Create an invalid cube (complemented node)
    DdNode *invalidCube = Cudd_Not(var0);
    
    // Test that addOrAbstract rejects invalid cube
    DdNode *result = Cudd_addOrAbstract(manager, var0, invalidCube);
    REQUIRE(result == nullptr);

    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Abstract constant ADDs", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);

    // Abstracting from zero should return zero
    DdNode *result1 = Cudd_addOrAbstract(manager, zero, var0);
    REQUIRE(result1 == zero);

    // Abstracting from one should return one
    DdNode *result2 = Cudd_addOrAbstract(manager, one, var0);
    REQUIRE(result2 == one);

    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Abstract single variable", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create 0-1 ADD: if x0 then 1 else 0
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *add = Cudd_addIte(manager, var0, one, zero);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 - should OR 1 and 0 = 1
    DdNode *result = Cudd_addOrAbstract(manager, add, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Abstract variable not in ADD", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD with var1
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var1);
    
    // Create cube with var0 (not in the ADD)
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);

    // Abstract var0 from ADD(var1) - should return same ADD
    DdNode *result = Cudd_addOrAbstract(manager, var1, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Early return when then-child is one", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create 0-1 ADD: if x0 then 1 else 1 (constant 1)
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *add = Cudd_addIte(manager, var0, one, one);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Should return one directly
    DdNode *result = Cudd_addOrAbstract(manager, add, var0);
    REQUIRE(result == one);

    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("addCheckPositiveCube - Test various cube forms", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);
    
    // Valid positive cube
    DdNode *cube = Cudd_addApply(manager, Cudd_addTimes, var0, var1);
    REQUIRE(cube != nullptr);
    Cudd_Ref(cube);
    
    // This should work (valid cube)
    DdNode *result1 = Cudd_addExistAbstract(manager, var0, cube);
    REQUIRE(result1 != nullptr);
    
    // Test with non-cube (has both then and else children non-zero)
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    REQUIRE(const2 != nullptr);
    Cudd_Ref(const2);
    
    DdNode *nonCube = Cudd_addIte(manager, var0, const2, const2);
    REQUIRE(nonCube != nullptr);
    Cudd_Ref(nonCube);
    
    // This should fail (not a valid cube)
    DdNode *result2 = Cudd_addExistAbstract(manager, var0, nonCube);
    REQUIRE(result2 == nullptr);
    
    // Test with cube that has non-zero else child (line 541 coverage)
    // Create a proper ADD that has different then/else children, both non-zero
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const1 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const1);
    Cudd_Ref(const3);
    
    DdNode *invalidCube2 = Cudd_addIte(manager, var0, const1, const3);
    REQUIRE(invalidCube2 != nullptr);
    Cudd_Ref(invalidCube2);
    
    // This should also fail (else child is not zero)
    DdNode *result3 = Cudd_addExistAbstract(manager, var1, invalidCube2);
    REQUIRE(result3 == nullptr);

    Cudd_RecursiveDeref(manager, invalidCube2);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, nonCube);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, cube);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddAddAbs - Test cache and reordering paths", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create a more complex ADD to exercise cache
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    DdNode *var2 = Cudd_addIthVar(manager, 2);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    REQUIRE(var2 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);
    Cudd_Ref(var2);

    // Create a more complex ADD: (v0 + v1) * v2
    DdNode *sum = Cudd_addApply(manager, Cudd_addPlus, var0, var1);
    REQUIRE(sum != nullptr);
    Cudd_Ref(sum);
    
    DdNode *add = Cudd_addApply(manager, Cudd_addTimes, sum, var2);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);
    
    // Create cube with multiple variables
    DdNode *cube01 = Cudd_addApply(manager, Cudd_addTimes, var0, var1);
    REQUIRE(cube01 != nullptr);
    Cudd_Ref(cube01);

    // Abstract var0 and var1 - testing Cudd_addExistAbstract cache
    DdNode *result = Cudd_addExistAbstract(manager, add, cube01);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);

    // Call again to potentially hit cache
    DdNode *result2 = Cudd_addExistAbstract(manager, add, cube01);
    REQUIRE(result2 == result);
    
    // Test Cudd_addUnivAbstract cache hit
    DdNode *result3 = Cudd_addUnivAbstract(manager, add, cube01);
    REQUIRE(result3 != nullptr);
    Cudd_Ref(result3);
    
    // Call again for cache hit
    DdNode *result4 = Cudd_addUnivAbstract(manager, add, cube01);
    REQUIRE(result4 == result3);
    
    // Test Cudd_addOrAbstract cache hit with 0-1 ADD
    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *add01 = Cudd_addIte(manager, var0, one, zero);
    REQUIRE(add01 != nullptr);
    Cudd_Ref(add01);
    
    DdNode *result5 = Cudd_addOrAbstract(manager, add01, var0);
    REQUIRE(result5 != nullptr);
    Cudd_Ref(result5);
    
    // Call again for cache hit
    DdNode *result6 = Cudd_addOrAbstract(manager, add01, var0);
    REQUIRE(result6 == result5);

    Cudd_RecursiveDeref(manager, result5);
    Cudd_RecursiveDeref(manager, add01);
    Cudd_RecursiveDeref(manager, result3);
    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, cube01);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, sum);
    Cudd_RecursiveDeref(manager, var2);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Abstract with f->index < cube->index", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD that depends on var0
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    // Create ADD: if var0 then 2 else 3
    DdNode *add = Cudd_addIte(manager, var0, const2, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);
    
    // Abstract var1 (higher index than var0)
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var1 != nullptr);
    
    // This triggers the f->index < cube->index path
    DdNode *result = Cudd_addExistAbstract(manager, add, var1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be 2*add (multiplied by 2 for the missing variable)
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Abstract with f->index < cube->index", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ADD that depends on var0
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    
    // Create ADD: if var0 then 2 else 3
    DdNode *add = Cudd_addIte(manager, var0, const2, const3);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);
    
    // Abstract var1 (higher index than var0)
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var1 != nullptr);
    
    // This triggers the f->index < cube->index path
    DdNode *result = Cudd_addUnivAbstract(manager, add, var1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be add squared (multiplied by itself for the missing variable)
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Abstract with f->index < cube->index", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create 0-1 ADD that depends on var0
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    REQUIRE(var0 != nullptr);
    Cudd_Ref(var0);
    
    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create ADD: if var0 then 1 else 0
    DdNode *add = Cudd_addIte(manager, var0, one, zero);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);
    
    // Abstract var1 (higher index than var0)
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var1 != nullptr);
    
    // This triggers the f->index < cube->index path
    DdNode *result = Cudd_addOrAbstract(manager, add, var1);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be same as add (ORing with itself)
    REQUIRE(!Cudd_IsConstant(result));

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Complex nested abstraction", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create multiple variables
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    DdNode *var2 = Cudd_addIthVar(manager, 2);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    REQUIRE(var2 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);
    Cudd_Ref(var2);

    // Create a nested structure: (var0 ? (var1 ? 1 : 2) : (var1 ? 3 : 4))
    DdNode *const1 = Cudd_addConst(manager, 1.0);
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const4 = Cudd_addConst(manager, 4.0);
    REQUIRE(const1 != nullptr);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    REQUIRE(const4 != nullptr);
    Cudd_Ref(const1);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    Cudd_Ref(const4);

    DdNode *then_branch = Cudd_addIte(manager, var1, const1, const2);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);
    
    DdNode *else_branch = Cudd_addIte(manager, var1, const3, const4);
    REQUIRE(else_branch != nullptr);
    Cudd_Ref(else_branch);
    
    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 and var1 using a cube
    DdNode *cube01 = Cudd_addApply(manager, Cudd_addTimes, var0, var1);
    REQUIRE(cube01 != nullptr);
    Cudd_Ref(cube01);

    DdNode *result = Cudd_addExistAbstract(manager, add, cube01);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be constant (1+2+3+4 = 10)
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 10.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, cube01);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_branch);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, const4);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, const1);
    Cudd_RecursiveDeref(manager, var2);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Complex nested abstraction", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create multiple variables
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    // Create a nested structure: (var0 ? (var1 ? 2 : 3) : (var1 ? 5 : 7))
    DdNode *const2 = Cudd_addConst(manager, 2.0);
    DdNode *const3 = Cudd_addConst(manager, 3.0);
    DdNode *const5 = Cudd_addConst(manager, 5.0);
    DdNode *const7 = Cudd_addConst(manager, 7.0);
    REQUIRE(const2 != nullptr);
    REQUIRE(const3 != nullptr);
    REQUIRE(const5 != nullptr);
    REQUIRE(const7 != nullptr);
    Cudd_Ref(const2);
    Cudd_Ref(const3);
    Cudd_Ref(const5);
    Cudd_Ref(const7);

    DdNode *then_branch = Cudd_addIte(manager, var1, const2, const3);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);
    
    DdNode *else_branch = Cudd_addIte(manager, var1, const5, const7);
    REQUIRE(else_branch != nullptr);
    Cudd_Ref(else_branch);
    
    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 and var1 using a cube
    DdNode *cube01 = Cudd_addApply(manager, Cudd_addTimes, var0, var1);
    REQUIRE(cube01 != nullptr);
    Cudd_Ref(cube01);

    DdNode *result = Cudd_addUnivAbstract(manager, add, cube01);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be constant (2*3*5*7 = 210)
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 210.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, cube01);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_branch);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, const7);
    Cudd_RecursiveDeref(manager, const5);
    Cudd_RecursiveDeref(manager, const3);
    Cudd_RecursiveDeref(manager, const2);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Complex nested abstraction", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create multiple variables
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);

    // Create a nested 0-1 ADD: (var0 ? (var1 ? 1 : 0) : (var1 ? 0 : 0))
    DdNode *then_branch = Cudd_addIte(manager, var1, one, zero);
    REQUIRE(then_branch != nullptr);
    Cudd_Ref(then_branch);
    
    DdNode *else_branch = zero;
    
    DdNode *add = Cudd_addIte(manager, var0, then_branch, else_branch);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 and var1 using a cube
    DdNode *cube01 = Cudd_addApply(manager, Cudd_addTimes, var0, var1);
    REQUIRE(cube01 != nullptr);
    Cudd_Ref(cube01);

    DdNode *result = Cudd_addOrAbstract(manager, add, cube01);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should be constant 1 (OR of all values)
    REQUIRE(Cudd_IsConstant(result));
    REQUIRE(Cudd_V(result) == 1.0);

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, cube01);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, then_branch);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Both branches non-one triggers OR operation", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create variables
    DdNode *var0 = Cudd_addIthVar(manager, 0);
    DdNode *var1 = Cudd_addIthVar(manager, 1);
    REQUIRE(var0 != nullptr);
    REQUIRE(var1 != nullptr);
    Cudd_Ref(var0);
    Cudd_Ref(var1);

    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);

    // Create nested 0-1 ADD: if var0 then (if var1 then 0 else 1) else (if var1 then 1 else 0)
    // This ensures when we abstract var1, the then-branch gives 1 (0 OR 1) and else-branch gives 1 (1 OR 0)
    // But if we abstract var0 first, we get different results that aren't immediately one
    DdNode *then_inner = Cudd_addIte(manager, var1, zero, one);
    REQUIRE(then_inner != nullptr);
    Cudd_Ref(then_inner);
    
    DdNode *else_inner = Cudd_addIte(manager, var1, one, zero);
    REQUIRE(else_inner != nullptr);
    Cudd_Ref(else_inner);
    
    DdNode *add = Cudd_addIte(manager, var0, then_inner, else_inner);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);

    // Abstract var0 - then_inner OR else_inner should trigger the res1 != one path
    DdNode *result = Cudd_addOrAbstract(manager, add, var0);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    
    // Result should either be non-constant or constant 1 (OR of the branches)
    if (Cudd_IsConstant(result)) {
        REQUIRE(Cudd_V(result) == 1.0);
    }

    Cudd_RecursiveDeref(manager, result);
    Cudd_RecursiveDeref(manager, add);
    Cudd_RecursiveDeref(manager, else_inner);
    Cudd_RecursiveDeref(manager, then_inner);
    Cudd_RecursiveDeref(manager, var1);
    Cudd_RecursiveDeref(manager, var0);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addExistAbstract - Test with memory constraints", "[cuddAddAbs]") {
    // Create a manager with very limited memory to potentially trigger allocation failures
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Set a very small memory limit
    Cudd_SetMaxMemory(manager, 1024 * 1024); // 1MB limit
    
    // Try to create a large structure that might exceed limits
    DdNode *vars[10];
    for (int i = 0; i < 10; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }
    
    // Build a complex ADD
    DdNode *add = Cudd_addConst(manager, 1.0);
    Cudd_Ref(add);
    
    for (int i = 0; i < 10; i++) {
        DdNode *tmp = Cudd_addApply(manager, Cudd_addPlus, add, vars[i]);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, add);
            add = tmp;
        }
    }
    
    // Create a cube with multiple variables
    DdNode *cube = vars[0];
    Cudd_Ref(cube);
    for (int i = 1; i < 5; i++) {
        DdNode *tmp = Cudd_addApply(manager, Cudd_addTimes, cube, vars[i]);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
        }
    }
    
    // Try to abstract - might succeed or fail due to memory constraints
    DdNode *result = Cudd_addExistAbstract(manager, add, cube);
    if (result != nullptr) {
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, cube);
    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < 10; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addUnivAbstract - Test with memory constraints", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    Cudd_SetMaxMemory(manager, 1024 * 1024);
    
    DdNode *vars[10];
    for (int i = 0; i < 10; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }
    
    DdNode *add = Cudd_addConst(manager, 2.0);
    Cudd_Ref(add);
    
    for (int i = 0; i < 10; i++) {
        DdNode *tmp = Cudd_addApply(manager, Cudd_addTimes, add, vars[i]);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, add);
            add = tmp;
        }
    }
    
    DdNode *cube = vars[0];
    Cudd_Ref(cube);
    for (int i = 1; i < 5; i++) {
        DdNode *tmp = Cudd_addApply(manager, Cudd_addTimes, cube, vars[i]);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
        }
    }
    
    DdNode *result = Cudd_addUnivAbstract(manager, add, cube);
    if (result != nullptr) {
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, cube);
    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < 10; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addOrAbstract - Test with memory constraints", "[cuddAddAbs]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    Cudd_SetMaxMemory(manager, 1024 * 1024);
    
    DdNode *vars[10];
    for (int i = 0; i < 10; i++) {
        vars[i] = Cudd_addIthVar(manager, i);
        REQUIRE(vars[i] != nullptr);
        Cudd_Ref(vars[i]);
    }
    
    DdNode *zero = Cudd_ReadZero(manager);
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *add = vars[0];
    Cudd_Ref(add);
    
    for (int i = 1; i < 8; i++) {
        DdNode *tmp = Cudd_addIte(manager, vars[i], add, zero);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, add);
            add = tmp;
        }
    }
    
    DdNode *cube = vars[0];
    Cudd_Ref(cube);
    for (int i = 1; i < 5; i++) {
        DdNode *tmp = Cudd_addApply(manager, Cudd_addTimes, cube, vars[i]);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, cube);
            cube = tmp;
        }
    }
    
    DdNode *result = Cudd_addOrAbstract(manager, add, cube);
    if (result != nullptr) {
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, cube);
    Cudd_RecursiveDeref(manager, add);
    for (int i = 0; i < 10; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}
