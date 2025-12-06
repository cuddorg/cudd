#include <catch2/catch_test_macros.hpp>
#include "cudd/cudd.h"
#include "util.h"

// Test Cudd_addApply with Cudd_addPlus
TEST_CASE("Cudd_addPlus - Zero operands", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addPlus, zero, one);
    REQUIRE(result == one);
    result = Cudd_addApply(dd, Cudd_addPlus, one, zero);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addPlus - Constant addition", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c2 = Cudd_addConst(dd, 2.0);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    Cudd_Ref(c2); Cudd_Ref(c3);
    DdNode *result = Cudd_addApply(dd, Cudd_addPlus, c2, c3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_V(result) == 5.0);
    Cudd_RecursiveDeref(dd, result);
    Cudd_RecursiveDeref(dd, c2);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addPlus - Swap operands", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    DdNode *v1 = Cudd_addIthVar(dd, 1);
    Cudd_Ref(v0); Cudd_Ref(v1);
    DdNode *result = Cudd_addApply(dd, Cudd_addPlus, v0, v1);
    REQUIRE(result != nullptr);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_RecursiveDeref(dd, v1);
    Cudd_Quit(dd);
}

// Test Cudd_addTimes
TEST_CASE("Cudd_addTimes - Zero operands", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addTimes, zero, one);
    REQUIRE(result == zero);
    result = Cudd_addApply(dd, Cudd_addTimes, one, zero);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addTimes - One operands", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addTimes, one, c5);
    REQUIRE(result == c5);
    result = Cudd_addApply(dd, Cudd_addTimes, c5, one);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addTimes - Constant multiplication", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c2 = Cudd_addConst(dd, 2.0);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    Cudd_Ref(c2); Cudd_Ref(c3);
    DdNode *result = Cudd_addApply(dd, Cudd_addTimes, c2, c3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_V(result) == 6.0);
    Cudd_RecursiveDeref(dd, result);
    Cudd_RecursiveDeref(dd, c2);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_Quit(dd);
}

// Test Cudd_addThreshold
TEST_CASE("Cudd_addThreshold - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addThreshold, c5, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addThreshold - F is plus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addThreshold, pinf, c5);
    REQUIRE(result == pinf);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addThreshold - F >= G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    Cudd_Ref(c5); Cudd_Ref(c3);
    DdNode *result = Cudd_addApply(dd, Cudd_addThreshold, c5, c3);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addThreshold - F < G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addThreshold, c3, c5);
    REQUIRE(result == zero);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addSetNZ
TEST_CASE("Cudd_addSetNZ - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addSetNZ, c5, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addSetNZ - F is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addSetNZ, zero, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addSetNZ - G is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addSetNZ, c5, zero);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addSetNZ - G is constant", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addSetNZ, c3, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addDivide
TEST_CASE("Cudd_addDivide - F is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDivide, zero, c5);
    REQUIRE(result == zero);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addDivide - G is one", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDivide, c5, one);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addDivide - Constant division", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c6 = Cudd_addConst(dd, 6.0);
    DdNode *c2 = Cudd_addConst(dd, 2.0);
    Cudd_Ref(c6); Cudd_Ref(c2);
    DdNode *result = Cudd_addApply(dd, Cudd_addDivide, c6, c2);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_V(result) == 3.0);
    Cudd_RecursiveDeref(dd, result);
    Cudd_RecursiveDeref(dd, c6);
    Cudd_RecursiveDeref(dd, c2);
    Cudd_Quit(dd);
}

// Test Cudd_addMinus
TEST_CASE("Cudd_addMinus - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinus, c5, c5);
    REQUIRE(result == zero);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinus - F is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinus, zero, c5);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_V(result) == -5.0);
    Cudd_RecursiveDeref(dd, result);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinus - G is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinus, c5, zero);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinus - Constant subtraction", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    Cudd_Ref(c5); Cudd_Ref(c3);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinus, c5, c3);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_V(result) == 2.0);
    Cudd_RecursiveDeref(dd, result);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_Quit(dd);
}

// Test Cudd_addMinimum
TEST_CASE("Cudd_addMinimum - F is plus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinimum, pinf, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinimum - G is plus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinimum, c5, pinf);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinimum - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinimum, c5, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinimum - F < G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinimum, c3, c5);
    REQUIRE(result == c3);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMinimum - F > G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMinimum, c5, c3);
    REQUIRE(result == c3);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addMaximum
TEST_CASE("Cudd_addMaximum - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMaximum, c5, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMaximum - F is minus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *minf = Cudd_ReadMinusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMaximum, minf, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMaximum - G is minus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *minf = Cudd_ReadMinusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMaximum, c5, minf);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMaximum - F > G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMaximum, c5, c3);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addMaximum - F < G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addMaximum, c3, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addOneZeroMaximum
TEST_CASE("Cudd_addOneZeroMaximum - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addOneZeroMaximum, c5, c5);
    REQUIRE(result == zero);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOneZeroMaximum - G is plus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addOneZeroMaximum, c5, pinf);
    REQUIRE(result == zero);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOneZeroMaximum - F > G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addOneZeroMaximum, c5, c3);
    REQUIRE(result == one);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOneZeroMaximum - F <= G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addOneZeroMaximum, c3, c5);
    REQUIRE(result == zero);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addDiff
TEST_CASE("Cudd_addDiff - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDiff, c5, c5);
    REQUIRE(result == pinf);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addDiff - F is plus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDiff, pinf, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addDiff - G is plus infinity", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *pinf = Cudd_ReadPlusInfinity(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDiff, c5, pinf);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addDiff - F < G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDiff, c3, c5);
    REQUIRE(result == c3);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addDiff - F > G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addDiff, c5, c3);
    REQUIRE(result == c3);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addAgreement
TEST_CASE("Cudd_addAgreement - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addAgreement, c5, c5);
    REQUIRE(result == c5);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addAgreement - F is background", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *bg = Cudd_ReadBackground(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addAgreement, bg, c5);
    REQUIRE(result == bg);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addAgreement - G is background", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *bg = Cudd_ReadBackground(dd);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addAgreement, c5, bg);
    REQUIRE(result == bg);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addAgreement - Different constants", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *bg = Cudd_ReadBackground(dd);
    DdNode *c3 = Cudd_addConst(dd, 3.0);
    DdNode *c5 = Cudd_addConst(dd, 5.0);
    Cudd_Ref(c3); Cudd_Ref(c5);
    DdNode *result = Cudd_addApply(dd, Cudd_addAgreement, c3, c5);
    REQUIRE(result == bg);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, c5);
    Cudd_Quit(dd);
}

// Test Cudd_addOr
TEST_CASE("Cudd_addOr - F is one", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addOr, one, zero);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOr - G is one", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addOr, zero, one);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOr - F is constant zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    Cudd_Ref(v0);
    DdNode *result = Cudd_addApply(dd, Cudd_addOr, zero, v0);
    REQUIRE(result == v0);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOr - G is constant zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    Cudd_Ref(v0);
    DdNode *result = Cudd_addApply(dd, Cudd_addOr, v0, zero);
    REQUIRE(result == v0);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addOr - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    Cudd_Ref(v0);
    DdNode *result = Cudd_addApply(dd, Cudd_addOr, v0, v0);
    REQUIRE(result == v0);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_Quit(dd);
}

// Test Cudd_addNand
TEST_CASE("Cudd_addNand - F is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addNand, zero, one);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addNand - G is zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addNand, one, zero);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addNand - Both constants non-zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addNand, one, one);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

// Test Cudd_addNor
TEST_CASE("Cudd_addNor - F is one", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addNor, one, zero);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addNor - G is one", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addNor, zero, one);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addNor - Both constants zero", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addNor, zero, zero);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

// Test Cudd_addXor
TEST_CASE("Cudd_addXor - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXor, one, one);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addXor - F=1 G=0", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXor, one, zero);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addXor - F=0 G=1", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXor, zero, one);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addXor - Both constants same", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXor, zero, zero);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

// Test Cudd_addXnor
TEST_CASE("Cudd_addXnor - F equals G", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXnor, one, one);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addXnor - Both zeros", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXnor, zero, zero);
    REQUIRE(result == one);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addXnor - Different values", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_ReadZero(dd);
    DdNode *result = Cudd_addApply(dd, Cudd_addXnor, one, zero);
    REQUIRE(result == zero);
    Cudd_Quit(dd);
}

// Test Cudd_addMonadicApply and Cudd_addLog
TEST_CASE("Cudd_addLog - Constant", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *e = Cudd_addConst(dd, 2.718281828);
    Cudd_Ref(e);
    DdNode *result = Cudd_addMonadicApply(dd, Cudd_addLog, e);
    REQUIRE(result != nullptr);
    Cudd_Ref(result);
    REQUIRE(Cudd_V(result) > 0.99);
    REQUIRE(Cudd_V(result) < 1.01);
    Cudd_RecursiveDeref(dd, result);
    Cudd_RecursiveDeref(dd, e);
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addLog - Variable ADD", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c1 = Cudd_addConst(dd, 1.0);
    DdNode *c2 = Cudd_addConst(dd, 2.0);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    Cudd_Ref(c1); Cudd_Ref(c2); Cudd_Ref(v0);
    DdNode *add = Cudd_addIte(dd, v0, c2, c1);
    REQUIRE(add != nullptr);
    Cudd_Ref(add);
    DdNode *result = Cudd_addMonadicApply(dd, Cudd_addLog, add);
    REQUIRE(result != nullptr);
    Cudd_RecursiveDeref(dd, add);
    Cudd_RecursiveDeref(dd, c1);
    Cudd_RecursiveDeref(dd, c2);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_Quit(dd);
}

// Test cuddAddApplyRecur with non-constant ADDs
TEST_CASE("cuddAddApplyRecur - Non-constant operands", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    DdNode *v1 = Cudd_addIthVar(dd, 1);
    Cudd_Ref(v0); Cudd_Ref(v1);
    DdNode *result = Cudd_addApply(dd, Cudd_addPlus, v0, v1);
    REQUIRE(result != nullptr);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_RecursiveDeref(dd, v1);
    Cudd_Quit(dd);
}

// Test with different variable orderings
TEST_CASE("cuddAddApplyRecur - Different variable orders", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *c1 = Cudd_addConst(dd, 1.0);
    DdNode *c2 = Cudd_addConst(dd, 2.0);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    DdNode *v1 = Cudd_addIthVar(dd, 1);
    Cudd_Ref(c1); Cudd_Ref(c2); Cudd_Ref(v0); Cudd_Ref(v1);
    DdNode *f = Cudd_addIte(dd, v0, c2, c1);
    DdNode *g = Cudd_addIte(dd, v1, c2, c1);
    Cudd_Ref(f); Cudd_Ref(g);
    DdNode *result = Cudd_addApply(dd, Cudd_addPlus, f, g);
    REQUIRE(result != nullptr);
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, g);
    Cudd_RecursiveDeref(dd, c1);
    Cudd_RecursiveDeref(dd, c2);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_RecursiveDeref(dd, v1);
    Cudd_Quit(dd);
}

// Test swap path in operators
TEST_CASE("Operator swap paths", "[cuddAddApply]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    DdNode *v0 = Cudd_addIthVar(dd, 0);
    DdNode *v1 = Cudd_addIthVar(dd, 1);
    Cudd_Ref(v0); Cudd_Ref(v1);
    // Test swap in Cudd_addTimes
    DdNode *r1 = Cudd_addApply(dd, Cudd_addTimes, v0, v1);
    REQUIRE(r1 != nullptr);
    // Test swap in Cudd_addMinimum
    DdNode *r2 = Cudd_addApply(dd, Cudd_addMinimum, v0, v1);
    REQUIRE(r2 != nullptr);
    // Test swap in Cudd_addMaximum
    DdNode *r3 = Cudd_addApply(dd, Cudd_addMaximum, v0, v1);
    REQUIRE(r3 != nullptr);
    // Test swap in Cudd_addNand
    DdNode *r4 = Cudd_addApply(dd, Cudd_addNand, v0, v1);
    REQUIRE(r4 != nullptr);
    // Test swap in Cudd_addNor
    DdNode *r5 = Cudd_addApply(dd, Cudd_addNor, v0, v1);
    REQUIRE(r5 != nullptr);
    // Test swap in Cudd_addXor
    DdNode *r6 = Cudd_addApply(dd, Cudd_addXor, v0, v1);
    REQUIRE(r6 != nullptr);
    // Test swap in Cudd_addXnor
    DdNode *r7 = Cudd_addApply(dd, Cudd_addXnor, v0, v1);
    REQUIRE(r7 != nullptr);
    // Test swap in Cudd_addOr
    DdNode *r8 = Cudd_addApply(dd, Cudd_addOr, v0, v1);
    REQUIRE(r8 != nullptr);
    Cudd_RecursiveDeref(dd, v0);
    Cudd_RecursiveDeref(dd, v1);
    Cudd_Quit(dd);
}
