#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddBridge.c
 * 
 * This file contains comprehensive tests for the cuddBridge module
 * to achieve 90%+ code coverage. The cuddBridge module provides
 * translation from BDD to ADD and vice versa, and transfer between
 * different managers.
 */

TEST_CASE("Cudd_addBddThreshold - Basic threshold conversion", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    
    SECTION("Threshold with constant ADD") {
        // Create a constant ADD with value 5.0
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // Convert to BDD with threshold 3.0 (value >= threshold -> 1)
        DdNode *bdd = Cudd_addBddThreshold(manager, constAdd, 3.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);  // 5.0 >= 3.0, so result is 1
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Threshold with constant ADD below threshold") {
        // Create a constant ADD with value 2.0
        DdNode *constAdd = Cudd_addConst(manager, 2.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // Convert to BDD with threshold 5.0
        DdNode *bdd = Cudd_addBddThreshold(manager, constAdd, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);  // 2.0 < 5.0, so result is 0
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Threshold with variable ADD") {
        // Create ADD: if x then 10.0 else 2.0
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        REQUIRE(addVar != nullptr);
        Cudd_Ref(addVar);
        
        DdNode *val10 = Cudd_addConst(manager, 10.0);
        Cudd_Ref(val10);
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(val2);
        
        // Create ITE: x ? 10.0 : 2.0
        DdNode *add = Cudd_addIte(manager, addVar, val10, val2);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Convert with threshold 5.0: x ? 1 : 0
        DdNode *bdd = Cudd_addBddThreshold(manager, add, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Result should be variable x (10 >= 5 -> 1, 2 < 5 -> 0)
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        REQUIRE(bddVar != nullptr);
        Cudd_Ref(bddVar);
        REQUIRE(bdd == bddVar);
        
        Cudd_RecursiveDeref(manager, bddVar);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, val10);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    SECTION("Threshold equal to value") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // Value equals threshold: should be 1 (>= comparison)
        DdNode *bdd = Cudd_addBddThreshold(manager, constAdd, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addBddStrictThreshold - Strict threshold conversion", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    
    SECTION("Strict threshold with constant ADD above threshold") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // Strictly greater: 5.0 > 3.0 -> 1
        DdNode *bdd = Cudd_addBddStrictThreshold(manager, constAdd, 3.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Strict threshold equal to value") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // Value equals threshold: should be 0 (strictly greater)
        DdNode *bdd = Cudd_addBddStrictThreshold(manager, constAdd, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Strict threshold with constant ADD below threshold") {
        DdNode *constAdd = Cudd_addConst(manager, 2.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        DdNode *bdd = Cudd_addBddStrictThreshold(manager, constAdd, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Strict threshold with variable ADD") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        REQUIRE(addVar != nullptr);
        Cudd_Ref(addVar);
        
        DdNode *val10 = Cudd_addConst(manager, 10.0);
        Cudd_Ref(val10);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val5);
        
        // Create ITE: x ? 10.0 : 5.0
        DdNode *add = Cudd_addIte(manager, addVar, val10, val5);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Strictly greater than 5.0: x ? 1 : 0
        DdNode *bdd = Cudd_addBddStrictThreshold(manager, add, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddVar);
        REQUIRE(bdd == bddVar);
        
        Cudd_RecursiveDeref(manager, bddVar);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val10);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addBddInterval - Interval conversion", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    
    SECTION("Interval with value inside") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // 3.0 <= 5.0 <= 7.0 -> 1
        DdNode *bdd = Cudd_addBddInterval(manager, constAdd, 3.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Interval with value below lower bound") {
        DdNode *constAdd = Cudd_addConst(manager, 2.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // 2.0 < 3.0 -> 0
        DdNode *bdd = Cudd_addBddInterval(manager, constAdd, 3.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Interval with value above upper bound") {
        DdNode *constAdd = Cudd_addConst(manager, 10.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        // 10.0 > 7.0 -> 0
        DdNode *bdd = Cudd_addBddInterval(manager, constAdd, 3.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Interval with value at boundaries") {
        // Test lower boundary
        DdNode *constLower = Cudd_addConst(manager, 3.0);
        Cudd_Ref(constLower);
        DdNode *bddLower = Cudd_addBddInterval(manager, constLower, 3.0, 7.0);
        REQUIRE(bddLower != nullptr);
        Cudd_Ref(bddLower);
        REQUIRE(bddLower == one);  // 3.0 >= 3.0 and 3.0 <= 7.0
        Cudd_RecursiveDeref(manager, bddLower);
        Cudd_RecursiveDeref(manager, constLower);
        
        // Test upper boundary
        DdNode *constUpper = Cudd_addConst(manager, 7.0);
        Cudd_Ref(constUpper);
        DdNode *bddUpper = Cudd_addBddInterval(manager, constUpper, 3.0, 7.0);
        REQUIRE(bddUpper != nullptr);
        Cudd_Ref(bddUpper);
        REQUIRE(bddUpper == one);  // 7.0 >= 3.0 and 7.0 <= 7.0
        Cudd_RecursiveDeref(manager, bddUpper);
        Cudd_RecursiveDeref(manager, constUpper);
    }
    
    SECTION("Interval with variable ADD") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        REQUIRE(addVar != nullptr);
        Cudd_Ref(addVar);
        
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val5);
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(val2);
        
        // Create ITE: x ? 5.0 : 2.0
        DdNode *add = Cudd_addIte(manager, addVar, val5, val2);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Interval [3.0, 7.0]: 5 in interval, 2 not in interval
        DdNode *bdd = Cudd_addBddInterval(manager, add, 3.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddVar);
        REQUIRE(bdd == bddVar);
        
        Cudd_RecursiveDeref(manager, bddVar);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addBddIthBit - Extract ith bit from ADD", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    
    SECTION("Extract bit 0 from value 5 (binary: 101)") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        DdNode *bdd = Cudd_addBddIthBit(manager, constAdd, 0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);  // Bit 0 of 5 (101) is 1
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Extract bit 1 from value 5 (binary: 101)") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        DdNode *bdd = Cudd_addBddIthBit(manager, constAdd, 1);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);  // Bit 1 of 5 (101) is 0
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Extract bit 2 from value 5 (binary: 101)") {
        DdNode *constAdd = Cudd_addConst(manager, 5.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        DdNode *bdd = Cudd_addBddIthBit(manager, constAdd, 2);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);  // Bit 2 of 5 (101) is 1
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Extract bit from zero value") {
        DdNode *constAdd = Cudd_addConst(manager, 0.0);
        REQUIRE(constAdd != nullptr);
        Cudd_Ref(constAdd);
        
        DdNode *bdd = Cudd_addBddIthBit(manager, constAdd, 0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, constAdd);
    }
    
    SECTION("Extract ith bit from variable ADD") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        REQUIRE(addVar != nullptr);
        Cudd_Ref(addVar);
        
        DdNode *val6 = Cudd_addConst(manager, 6.0);  // binary: 110
        Cudd_Ref(val6);
        DdNode *val5 = Cudd_addConst(manager, 5.0);  // binary: 101
        Cudd_Ref(val5);
        
        // Create ITE: x ? 6.0 : 5.0
        DdNode *add = Cudd_addIte(manager, addVar, val6, val5);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Bit 0: 6=0, 5=1 -> NOT x
        DdNode *bdd0 = Cudd_addBddIthBit(manager, add, 0);
        REQUIRE(bdd0 != nullptr);
        Cudd_Ref(bdd0);
        
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddVar);
        REQUIRE(bdd0 == Cudd_Not(bddVar));
        Cudd_RecursiveDeref(manager, bdd0);
        
        // Bit 1: 6=1, 5=0 -> x
        DdNode *bdd1 = Cudd_addBddIthBit(manager, add, 1);
        REQUIRE(bdd1 != nullptr);
        Cudd_Ref(bdd1);
        REQUIRE(bdd1 == bddVar);
        Cudd_RecursiveDeref(manager, bdd1);
        
        // Bit 2: 6=1, 5=1 -> 1
        DdNode *bdd2 = Cudd_addBddIthBit(manager, add, 2);
        REQUIRE(bdd2 != nullptr);
        Cudd_Ref(bdd2);
        REQUIRE(bdd2 == one);
        Cudd_RecursiveDeref(manager, bdd2);
        
        Cudd_RecursiveDeref(manager, bddVar);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val6);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_BddToAdd - Convert BDD to 0-1 ADD", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    DdNode *addOne = Cudd_addConst(manager, 1.0);
    DdNode *addZero = Cudd_addConst(manager, 0.0);
    
    SECTION("Convert constant one BDD to ADD") {
        DdNode *add = Cudd_BddToAdd(manager, one);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        REQUIRE(add == addOne);
        Cudd_RecursiveDeref(manager, add);
    }
    
    SECTION("Convert constant zero BDD to ADD") {
        DdNode *add = Cudd_BddToAdd(manager, zero);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        REQUIRE(add == addZero);
        Cudd_RecursiveDeref(manager, add);
    }
    
    SECTION("Convert variable BDD to ADD") {
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        REQUIRE(bddVar != nullptr);
        Cudd_Ref(bddVar);
        
        DdNode *add = Cudd_BddToAdd(manager, bddVar);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Result should be ADD: x ? 1 : 0
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        Cudd_Ref(addVar);
        REQUIRE(add == addVar);
        
        Cudd_RecursiveDeref(manager, addVar);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, bddVar);
    }
    
    SECTION("Convert negated variable BDD to ADD") {
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        REQUIRE(bddVar != nullptr);
        Cudd_Ref(bddVar);
        
        DdNode *negVar = Cudd_Not(bddVar);
        
        DdNode *add = Cudd_BddToAdd(manager, negVar);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Result should be ADD: !x ? 1 : 0 = x ? 0 : 1
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        Cudd_Ref(addVar);
        DdNode *expected = Cudd_addIte(manager, addVar, addZero, addOne);
        Cudd_Ref(expected);
        REQUIRE(add == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, addVar);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, bddVar);
    }
    
    SECTION("Convert complex BDD to ADD") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create x AND y
        DdNode *bdd = Cudd_bddAnd(manager, x, y);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *add = Cudd_BddToAdd(manager, bdd);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Verify structure
        REQUIRE(!Cudd_IsConstant(add));
        
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addBddPattern - Convert ADD to BDD by pattern", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadLogicZero(manager);
    
    SECTION("Pattern with zero constant") {
        DdNode *addZero = Cudd_addConst(manager, 0.0);
        Cudd_Ref(addZero);
        
        DdNode *bdd = Cudd_addBddPattern(manager, addZero);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == zero);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, addZero);
    }
    
    SECTION("Pattern with non-zero constant") {
        DdNode *addConst = Cudd_addConst(manager, 5.0);
        Cudd_Ref(addConst);
        
        DdNode *bdd = Cudd_addBddPattern(manager, addConst);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, addConst);
    }
    
    SECTION("Pattern with variable ADD") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        REQUIRE(addVar != nullptr);
        Cudd_Ref(addVar);
        
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val5);
        DdNode *val0 = Cudd_addConst(manager, 0.0);
        Cudd_Ref(val0);
        
        // Create ITE: x ? 5.0 : 0.0
        DdNode *add = Cudd_addIte(manager, addVar, val5, val0);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddPattern(manager, add);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Result should be x (5 != 0 -> 1, 0 == 0 -> 0)
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddVar);
        REQUIRE(bdd == bddVar);
        
        Cudd_RecursiveDeref(manager, bddVar);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val0);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    SECTION("Pattern with all non-zero values") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        REQUIRE(addVar != nullptr);
        Cudd_Ref(addVar);
        
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val5);
        DdNode *val3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(val3);
        
        // Create ITE: x ? 5.0 : 3.0 (both non-zero)
        DdNode *add = Cudd_addIte(manager, addVar, val5, val3);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddPattern(manager, add);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        REQUIRE(bdd == one);  // All non-zero -> 1
        
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val3);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    SECTION("Pattern with multiple variables") {
        DdNode *addX = Cudd_addIthVar(manager, 0);
        DdNode *addY = Cudd_addIthVar(manager, 1);
        Cudd_Ref(addX);
        Cudd_Ref(addY);
        
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val5);
        DdNode *val0 = Cudd_addConst(manager, 0.0);
        Cudd_Ref(val0);
        
        // Create: if x then 5.0 else (if y then 5.0 else 0.0)
        DdNode *inner = Cudd_addIte(manager, addY, val5, val0);
        Cudd_Ref(inner);
        DdNode *add = Cudd_addIte(manager, addX, val5, inner);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddPattern(manager, add);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Result should be x OR y
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        DdNode *bddY = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(bddX);
        Cudd_Ref(bddY);
        DdNode *expected = Cudd_bddOr(manager, bddX, bddY);
        Cudd_Ref(expected);
        REQUIRE(bdd == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, bddY);
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, inner);
        Cudd_RecursiveDeref(manager, val0);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, addY);
        Cudd_RecursiveDeref(manager, addX);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddTransfer - Transfer BDD between managers", "[cuddBridge]") {
    DdManager *source = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    DdManager *dest = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(source != nullptr);
    REQUIRE(dest != nullptr);
    
    SECTION("Transfer constant one") {
        DdNode *srcOne = Cudd_ReadOne(source);
        DdNode *transferred = Cudd_bddTransfer(source, dest, srcOne);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        DdNode *destOne = Cudd_ReadOne(dest);
        REQUIRE(transferred == destOne);
        
        Cudd_RecursiveDeref(dest, transferred);
    }
    
    SECTION("Transfer constant zero") {
        DdNode *srcZero = Cudd_Not(Cudd_ReadOne(source));
        DdNode *transferred = Cudd_bddTransfer(source, dest, srcZero);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        DdNode *destZero = Cudd_Not(Cudd_ReadOne(dest));
        REQUIRE(transferred == destZero);
        
        Cudd_RecursiveDeref(dest, transferred);
    }
    
    SECTION("Transfer single variable") {
        DdNode *srcVar = Cudd_bddIthVar(source, 0);
        REQUIRE(srcVar != nullptr);
        Cudd_Ref(srcVar);
        
        DdNode *transferred = Cudd_bddTransfer(source, dest, srcVar);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        // Verify it's a variable in destination
        REQUIRE(!Cudd_IsConstant(transferred));
        REQUIRE(Cudd_NodeReadIndex(transferred) == 0);
        
        Cudd_RecursiveDeref(dest, transferred);
        Cudd_RecursiveDeref(source, srcVar);
    }
    
    SECTION("Transfer negated variable") {
        DdNode *srcVar = Cudd_bddIthVar(source, 0);
        REQUIRE(srcVar != nullptr);
        Cudd_Ref(srcVar);
        
        DdNode *srcNeg = Cudd_Not(srcVar);
        DdNode *transferred = Cudd_bddTransfer(source, dest, srcNeg);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        // Verify it's a complemented variable in destination
        REQUIRE(Cudd_IsComplement(transferred));
        
        Cudd_RecursiveDeref(dest, transferred);
        Cudd_RecursiveDeref(source, srcVar);
    }
    
    SECTION("Transfer complex BDD") {
        DdNode *x = Cudd_bddIthVar(source, 0);
        DdNode *y = Cudd_bddIthVar(source, 1);
        DdNode *z = Cudd_bddIthVar(source, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create (x AND y) OR z
        DdNode *xy = Cudd_bddAnd(source, x, y);
        Cudd_Ref(xy);
        DdNode *bdd = Cudd_bddOr(source, xy, z);
        Cudd_Ref(bdd);
        
        DdNode *transferred = Cudd_bddTransfer(source, dest, bdd);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        // Verify structure is preserved
        int srcSize = Cudd_DagSize(bdd);
        int destSize = Cudd_DagSize(transferred);
        REQUIRE(srcSize == destSize);
        
        Cudd_RecursiveDeref(dest, transferred);
        Cudd_RecursiveDeref(source, bdd);
        Cudd_RecursiveDeref(source, xy);
        Cudd_RecursiveDeref(source, z);
        Cudd_RecursiveDeref(source, y);
        Cudd_RecursiveDeref(source, x);
    }
    
    SECTION("Transfer and verify equivalence") {
        DdNode *x = Cudd_bddIthVar(source, 0);
        DdNode *y = Cudd_bddIthVar(source, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create x XOR y
        DdNode *bdd = Cudd_bddXor(source, x, y);
        Cudd_Ref(bdd);
        
        DdNode *transferred = Cudd_bddTransfer(source, dest, bdd);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        // Create same BDD in destination and compare
        DdNode *destX = Cudd_bddIthVar(dest, 0);
        DdNode *destY = Cudd_bddIthVar(dest, 1);
        Cudd_Ref(destX);
        Cudd_Ref(destY);
        DdNode *destXor = Cudd_bddXor(dest, destX, destY);
        Cudd_Ref(destXor);
        
        REQUIRE(transferred == destXor);
        
        Cudd_RecursiveDeref(dest, destXor);
        Cudd_RecursiveDeref(dest, destY);
        Cudd_RecursiveDeref(dest, destX);
        Cudd_RecursiveDeref(dest, transferred);
        Cudd_RecursiveDeref(source, bdd);
        Cudd_RecursiveDeref(source, y);
        Cudd_RecursiveDeref(source, x);
    }
    
    Cudd_Quit(dest);
    Cudd_Quit(source);
}

TEST_CASE("cuddBridge - Round trip conversions", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("BDD to ADD to BDD round trip") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *bdd = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(bdd);
        
        // BDD -> ADD
        DdNode *add = Cudd_BddToAdd(manager, bdd);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // ADD -> BDD (using pattern)
        DdNode *back = Cudd_addBddPattern(manager, add);
        REQUIRE(back != nullptr);
        Cudd_Ref(back);
        
        REQUIRE(back == bdd);
        
        Cudd_RecursiveDeref(manager, back);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Threshold and strict threshold comparison") {
        DdNode *addConst = Cudd_addConst(manager, 5.0);
        Cudd_Ref(addConst);
        
        // 5 >= 5 -> 1
        DdNode *threshold = Cudd_addBddThreshold(manager, addConst, 5.0);
        Cudd_Ref(threshold);
        REQUIRE(threshold == Cudd_ReadOne(manager));
        
        // 5 > 5 -> 0
        DdNode *strict = Cudd_addBddStrictThreshold(manager, addConst, 5.0);
        Cudd_Ref(strict);
        REQUIRE(strict == Cudd_Not(Cudd_ReadOne(manager)));
        
        Cudd_RecursiveDeref(manager, strict);
        Cudd_RecursiveDeref(manager, threshold);
        Cudd_RecursiveDeref(manager, addConst);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddBridge - Complex ADD structures", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Multi-level ADD threshold conversion") {
        // Create a 3-variable ADD with various values
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        DdNode *z = Cudd_addIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *val1 = Cudd_addConst(manager, 1.0);
        DdNode *val3 = Cudd_addConst(manager, 3.0);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        DdNode *val7 = Cudd_addConst(manager, 7.0);
        Cudd_Ref(val1);
        Cudd_Ref(val3);
        Cudd_Ref(val5);
        Cudd_Ref(val7);
        
        // Build: z ? (y ? 7 : 5) : (y ? 3 : 1)
        DdNode *inner1 = Cudd_addIte(manager, y, val7, val5);
        Cudd_Ref(inner1);
        DdNode *inner2 = Cudd_addIte(manager, y, val3, val1);
        Cudd_Ref(inner2);
        DdNode *add = Cudd_addIte(manager, z, inner1, inner2);
        Cudd_Ref(add);
        
        // Threshold at 4: values >= 4 (5, 7) become 1
        DdNode *bdd = Cudd_addBddThreshold(manager, add, 4.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Result should be z (since 5,7 >= 4 and 1,3 < 4)
        DdNode *bddZ = Cudd_bddIthVar(manager, 2);
        Cudd_Ref(bddZ);
        REQUIRE(bdd == bddZ);
        
        Cudd_RecursiveDeref(manager, bddZ);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, inner2);
        Cudd_RecursiveDeref(manager, inner1);
        Cudd_RecursiveDeref(manager, val7);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val3);
        Cudd_RecursiveDeref(manager, val1);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Interval conversion with multiple variables") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        DdNode *val4 = Cudd_addConst(manager, 4.0);
        DdNode *val6 = Cudd_addConst(manager, 6.0);
        DdNode *val8 = Cudd_addConst(manager, 8.0);
        Cudd_Ref(val2);
        Cudd_Ref(val4);
        Cudd_Ref(val6);
        Cudd_Ref(val8);
        
        // Build: x ? (y ? 8 : 6) : (y ? 4 : 2)
        DdNode *inner1 = Cudd_addIte(manager, y, val8, val6);
        Cudd_Ref(inner1);
        DdNode *inner2 = Cudd_addIte(manager, y, val4, val2);
        Cudd_Ref(inner2);
        DdNode *add = Cudd_addIte(manager, x, inner1, inner2);
        Cudd_Ref(add);
        
        // Interval [3, 7]: 4, 6 are in interval; 2, 8 are not
        DdNode *bdd = Cudd_addBddInterval(manager, add, 3.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Result should be XOR of x and y
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        DdNode *bddY = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(bddX);
        Cudd_Ref(bddY);
        DdNode *expected = Cudd_bddXor(manager, bddX, bddY);
        Cudd_Ref(expected);
        REQUIRE(bdd == expected);
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, bddY);
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, inner2);
        Cudd_RecursiveDeref(manager, inner1);
        Cudd_RecursiveDeref(manager, val8);
        Cudd_RecursiveDeref(manager, val6);
        Cudd_RecursiveDeref(manager, val4);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddBridge - Cache behavior", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Repeated conversion caching") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        Cudd_Ref(addVar);
        
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(val5);
        Cudd_Ref(val2);
        
        DdNode *add = Cudd_addIte(manager, addVar, val5, val2);
        Cudd_Ref(add);
        
        // Convert twice - second should use cache
        DdNode *bdd1 = Cudd_addBddPattern(manager, add);
        Cudd_Ref(bdd1);
        DdNode *bdd2 = Cudd_addBddPattern(manager, add);
        Cudd_Ref(bdd2);
        
        REQUIRE(bdd1 == bdd2);
        
        Cudd_RecursiveDeref(manager, bdd2);
        Cudd_RecursiveDeref(manager, bdd1);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    SECTION("BDD to ADD caching") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x);
        
        DdNode *add1 = Cudd_BddToAdd(manager, x);
        Cudd_Ref(add1);
        DdNode *add2 = Cudd_BddToAdd(manager, x);
        Cudd_Ref(add2);
        
        REQUIRE(add1 == add2);
        
        Cudd_RecursiveDeref(manager, add2);
        Cudd_RecursiveDeref(manager, add1);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddBridge - Edge cases with complement edges", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("BddToAdd with complement edges") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create NOT (x AND y) = NAND
        DdNode *nand = Cudd_bddNand(manager, x, y);
        Cudd_Ref(nand);
        
        DdNode *add = Cudd_BddToAdd(manager, nand);
        REQUIRE(add != nullptr);
        Cudd_Ref(add);
        
        // Verify conversion is correct by checking pattern
        DdNode *back = Cudd_addBddPattern(manager, add);
        Cudd_Ref(back);
        REQUIRE(back == nand);
        
        Cudd_RecursiveDeref(manager, back);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, nand);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Pattern conversion resulting in complement") {
        DdNode *addVar = Cudd_addIthVar(manager, 0);
        Cudd_Ref(addVar);
        
        DdNode *val0 = Cudd_addConst(manager, 0.0);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val0);
        Cudd_Ref(val5);
        
        // Create: x ? 0 : 5 (non-zero when x is false)
        DdNode *add = Cudd_addIte(manager, addVar, val0, val5);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddPattern(manager, add);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        // Result should be NOT x
        DdNode *bddVar = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddVar);
        REQUIRE(bdd == Cudd_Not(bddVar));
        
        Cudd_RecursiveDeref(manager, bddVar);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val0);
        Cudd_RecursiveDeref(manager, addVar);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddBridge - Complement edge coverage for recursive functions", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Threshold conversion with complement result") {
        // Create ADD where threshold gives complemented result
        // We need T to be complemented in the recursive function
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create values where threshold produces complement edges
        // Values: x=1,y=1 -> 1; x=1,y=0 -> 2; x=0,y=1 -> 3; x=0,y=0 -> 4
        DdNode *val1 = Cudd_addConst(manager, 1.0);
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        DdNode *val3 = Cudd_addConst(manager, 3.0);
        DdNode *val4 = Cudd_addConst(manager, 4.0);
        Cudd_Ref(val1);
        Cudd_Ref(val2);
        Cudd_Ref(val3);
        Cudd_Ref(val4);
        
        // Build nested ITE: x ? (y ? 1 : 2) : (y ? 3 : 4)
        DdNode *tBranch = Cudd_addIte(manager, y, val1, val2);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val3, val4);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        // Threshold at 2.5: values >= 2.5 (3, 4) -> 1, values < 2.5 (1, 2) -> 0
        // Result: NOT x (since x=0 gives values 3,4 which are >= 2.5)
        DdNode *bdd = Cudd_addBddThreshold(manager, add, 2.5);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val4);
        Cudd_RecursiveDeref(manager, val3);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, val1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Strict threshold with complement result") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val1 = Cudd_addConst(manager, 1.0);
        DdNode *val3 = Cudd_addConst(manager, 3.0);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        DdNode *val7 = Cudd_addConst(manager, 7.0);
        Cudd_Ref(val1);
        Cudd_Ref(val3);
        Cudd_Ref(val5);
        Cudd_Ref(val7);
        
        DdNode *tBranch = Cudd_addIte(manager, y, val1, val3);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val5, val7);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        // Strictly > 4: values 5, 7 are > 4, values 1, 3 are not
        DdNode *bdd = Cudd_addBddStrictThreshold(manager, add, 4.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val7);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val3);
        Cudd_RecursiveDeref(manager, val1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Interval conversion with complement result") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val1 = Cudd_addConst(manager, 1.0);
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        DdNode *val6 = Cudd_addConst(manager, 6.0);
        Cudd_Ref(val1);
        Cudd_Ref(val2);
        Cudd_Ref(val5);
        Cudd_Ref(val6);
        
        DdNode *tBranch = Cudd_addIte(manager, y, val1, val2);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val5, val6);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        // Interval [4, 7]: values 5, 6 are in interval, 1, 2 are not
        DdNode *bdd = Cudd_addBddInterval(manager, add, 4.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val6);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, val1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Ith bit with complement result") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Values: even (bit 0 = 0) on one branch, odd (bit 0 = 1) on other
        DdNode *val4 = Cudd_addConst(manager, 4.0);  // bit 0 = 0
        DdNode *val6 = Cudd_addConst(manager, 6.0);  // bit 0 = 0
        DdNode *val3 = Cudd_addConst(manager, 3.0);  // bit 0 = 1
        DdNode *val5 = Cudd_addConst(manager, 5.0);  // bit 0 = 1
        Cudd_Ref(val4);
        Cudd_Ref(val6);
        Cudd_Ref(val3);
        Cudd_Ref(val5);
        
        // x ? (y ? 4 : 6) : (y ? 3 : 5)
        // x=1: bit0 = 0, x=0: bit0 = 1 -> bit0 = NOT x
        DdNode *tBranch = Cudd_addIte(manager, y, val4, val6);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val3, val5);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddIthBit(manager, add, 0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val3);
        Cudd_RecursiveDeref(manager, val6);
        Cudd_RecursiveDeref(manager, val4);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Pattern with complement in result") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val0 = Cudd_addConst(manager, 0.0);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val0);
        Cudd_Ref(val5);
        
        // x ? (y ? 0 : 0) : (y ? 5 : 5) = x ? 0 : 5
        DdNode *tBranch = Cudd_addIte(manager, y, val0, val0);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val5, val5);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddPattern(manager, add);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val0);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Transfer with multiple complement edges") {
        DdManager *dest = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(dest != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *z = Cudd_bddIthVar(manager, 2);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create NOR: !(x OR y OR z)
        DdNode *xyorz = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xyorz);
        DdNode *xyzor = Cudd_bddOr(manager, xyorz, z);
        Cudd_Ref(xyzor);
        DdNode *nor = Cudd_Not(xyzor);
        
        DdNode *transferred = Cudd_bddTransfer(manager, dest, nor);
        REQUIRE(transferred != nullptr);
        Cudd_Ref(transferred);
        
        // Verify structure
        REQUIRE(Cudd_IsComplement(transferred));
        REQUIRE(Cudd_DagSize(nor) == Cudd_DagSize(transferred));
        
        Cudd_RecursiveDeref(dest, transferred);
        Cudd_RecursiveDeref(manager, xyzor);
        Cudd_RecursiveDeref(manager, xyorz);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        Cudd_Quit(dest);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddBridge - T == E branch coverage", "[cuddBridge]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Threshold where T equals E") {
        // Create ADD where threshold makes T == E in recursive call
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val3 = Cudd_addConst(manager, 3.0);
        DdNode *val7 = Cudd_addConst(manager, 7.0);
        Cudd_Ref(val3);
        Cudd_Ref(val7);
        
        // x ? (y ? 3 : 3) : (y ? 7 : 7)
        // This creates T == E for the inner branches
        DdNode *tBranch = Cudd_addIte(manager, y, val3, val3);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val7, val7);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddThreshold(manager, add, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val7);
        Cudd_RecursiveDeref(manager, val3);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Strict threshold where T equals E") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val2 = Cudd_addConst(manager, 2.0);
        DdNode *val8 = Cudd_addConst(manager, 8.0);
        Cudd_Ref(val2);
        Cudd_Ref(val8);
        
        DdNode *tBranch = Cudd_addIte(manager, y, val2, val2);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val8, val8);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        DdNode *bdd = Cudd_addBddStrictThreshold(manager, add, 5.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val8);
        Cudd_RecursiveDeref(manager, val2);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Interval where T equals E") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val1 = Cudd_addConst(manager, 1.0);
        DdNode *val5 = Cudd_addConst(manager, 5.0);
        Cudd_Ref(val1);
        Cudd_Ref(val5);
        
        DdNode *tBranch = Cudd_addIte(manager, y, val1, val1);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val5, val5);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        // Interval [3, 7]: 5 in, 1 out
        DdNode *bdd = Cudd_addBddInterval(manager, add, 3.0, 7.0);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val5);
        Cudd_RecursiveDeref(manager, val1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Ith bit where T equals E") {
        DdNode *x = Cudd_addIthVar(manager, 0);
        DdNode *y = Cudd_addIthVar(manager, 1);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *val4 = Cudd_addConst(manager, 4.0);  // bit 1 = 0
        DdNode *val6 = Cudd_addConst(manager, 6.0);  // bit 1 = 1
        Cudd_Ref(val4);
        Cudd_Ref(val6);
        
        DdNode *tBranch = Cudd_addIte(manager, y, val4, val4);
        Cudd_Ref(tBranch);
        DdNode *eBranch = Cudd_addIte(manager, y, val6, val6);
        Cudd_Ref(eBranch);
        DdNode *add = Cudd_addIte(manager, x, tBranch, eBranch);
        Cudd_Ref(add);
        
        // Bit 1: 4 = 100 (bit1=0), 6 = 110 (bit1=1)
        DdNode *bdd = Cudd_addBddIthBit(manager, add, 1);
        REQUIRE(bdd != nullptr);
        Cudd_Ref(bdd);
        
        DdNode *bddX = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(bddX);
        REQUIRE(bdd == Cudd_Not(bddX));
        
        Cudd_RecursiveDeref(manager, bddX);
        Cudd_RecursiveDeref(manager, bdd);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, eBranch);
        Cudd_RecursiveDeref(manager, tBranch);
        Cudd_RecursiveDeref(manager, val6);
        Cudd_RecursiveDeref(manager, val4);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}
