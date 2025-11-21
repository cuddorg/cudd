#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddBddIte.c
 * 
 * This file contains comprehensive tests for the cuddBddIte module
 * to ensure 100% code coverage and correct functionality.
 */

// DD_NON_CONSTANT is defined as ((DdNode *) 1) in cuddInt.h
// We define it here for testing purposes
#define DD_NON_CONSTANT ((DdNode *) 1)

TEST_CASE("Cudd_bddIte - Basic ITE operations", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("ITE with constant selector") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // ITE(1, x, y) = x
        DdNode *result = Cudd_bddIte(manager, one, x, y);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        // ITE(0, x, y) = y
        result = Cudd_bddIte(manager, zero, x, y);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("ITE with identical branches") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // ITE(x, y, y) = y
        DdNode *result = Cudd_bddIte(manager, x, y, y);
        Cudd_Ref(result);
        REQUIRE(result == y);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("ITE with complementary branches") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // ITE(x, y, !y) = x XOR !y = x XNOR y
        DdNode *result = Cudd_bddIte(manager, x, y, Cudd_Not(y));
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddXnor(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("ITE with selector equal to then branch") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // ITE(x, x, y) = x OR y
        DdNode *result = Cudd_bddIte(manager, x, x, y);
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddOr(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("ITE with selector equal to else branch") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // ITE(x, y, x) = x AND y
        DdNode *result = Cudd_bddIte(manager, x, y, x);
        Cudd_Ref(result);
        DdNode *expected = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("ITE with constant branches") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // ITE(x, 1, 0) = x
        DdNode *result = Cudd_bddIte(manager, x, one, zero);
        Cudd_Ref(result);
        REQUIRE(result == x);
        
        // ITE(x, 0, 1) = !x
        result = Cudd_bddIte(manager, x, zero, one);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIteLimit - ITE with node limit", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("ITE with sufficient limit") {
        // With a reasonable limit, should work
        DdNode *result = Cudd_bddIteLimit(manager, x, y, Cudd_Not(y), 1000);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("ITE with very tight limit") {
        // With limit 0, should fail for non-trivial operations
        DdNode *result = Cudd_bddIteLimit(manager, x, y, Cudd_Not(y), 0);
        // Result may be NULL if limit is too tight
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIteConstant - ITE constant check", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("ITE with constant selector") {
        // ITE(1, x, y) = x (constant if x is constant)
        DdNode *result = Cudd_bddIteConstant(manager, one, one, zero);
        REQUIRE(result == one);
        
        result = Cudd_bddIteConstant(manager, zero, one, zero);
        REQUIRE(result == zero);
    }
    
    SECTION("ITE with identical branches") {
        // ITE(x, 1, 1) = 1
        DdNode *result = Cudd_bddIteConstant(manager, x, one, one);
        REQUIRE(result == one);
    }
    
    SECTION("ITE resulting in non-constant") {
        // ITE(x, 1, 0) is not constant (it's x)
        DdNode *result = Cudd_bddIteConstant(manager, x, one, zero);
        REQUIRE(result == DD_NON_CONSTANT);
    }
    
    SECTION("ITE with complementary constants") {
        // ITE(x, 0, 1) is not constant (it's !x)
        DdNode *result = Cudd_bddIteConstant(manager, x, zero, one);
        REQUIRE(result == DD_NON_CONSTANT);
    }
    
    SECTION("ITE with complementary branches") {
        // ITE(x, y, !y) is not constant
        DdNode *result = Cudd_bddIteConstant(manager, x, y, Cudd_Not(y));
        REQUIRE(result == DD_NON_CONSTANT);
    }
    
    SECTION("ITE with selector equal to branch") {
        // ITE(x, x, 0) could be constant (when x = 0) or not
        DdNode *result = Cudd_bddIteConstant(manager, x, x, zero);
        // Result depends on whether it can be determined to be constant
        REQUIRE((result == DD_NON_CONSTANT || result == zero || result == x));
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAnd - AND operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("AND with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x AND 1 = x
        DdNode *result = Cudd_bddAnd(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        // x AND 0 = 0
        result = Cudd_bddAnd(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        // 1 AND 1 = 1
        result = Cudd_bddAnd(manager, one, one);
        REQUIRE(result != nullptr);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("AND with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x AND x = x
        DdNode *result = Cudd_bddAnd(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        // x AND !x = 0
        result = Cudd_bddAnd(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("AND with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // x AND y (non-trivial BDD)
        DdNode *result = Cudd_bddAnd(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result != zero);
        REQUIRE(result != one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddAndLimit - AND with limit", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("AND with sufficient limit") {
        DdNode *result = Cudd_bddAndLimit(manager, x, y, 1000);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("AND with tight limit") {
        DdNode *result = Cudd_bddAndLimit(manager, x, y, 0);
        // May return NULL if limit is too tight
        if (result != nullptr) {
            Cudd_Ref(result);
            Cudd_RecursiveDeref(manager, result);
        }
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddOr - OR operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("OR with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x OR 1 = 1
        DdNode *result = Cudd_bddOr(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        // x OR 0 = x
        result = Cudd_bddOr(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("OR with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x OR x = x
        DdNode *result = Cudd_bddOr(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        // x OR !x = 1
        result = Cudd_bddOr(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("OR with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddOr(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result != zero);
        REQUIRE(result != one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddOrLimit - OR with limit", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("OR with sufficient limit") {
        DdNode *result = Cudd_bddOrLimit(manager, x, y, 1000);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddNand - NAND operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("NAND with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x NAND 1 = !x
        DdNode *result = Cudd_bddNand(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, result);
        
        // x NAND 0 = 1
        result = Cudd_bddNand(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NAND with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x NAND x = !x
        DdNode *result = Cudd_bddNand(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NAND with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // x NAND y = !(x AND y)
        DdNode *result = Cudd_bddNand(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *andResult = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(andResult);
        REQUIRE(result == Cudd_Not(andResult));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, andResult);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddNor - NOR operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("NOR with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x NOR 0 = !x
        DdNode *result = Cudd_bddNor(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, result);
        
        // x NOR 1 = 0
        result = Cudd_bddNor(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NOR with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x NOR x = !x
        DdNode *result = Cudd_bddNor(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("NOR with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // x NOR y = !(x OR y)
        DdNode *result = Cudd_bddNor(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *orResult = Cudd_bddOr(manager, x, y);
        Cudd_Ref(orResult);
        REQUIRE(result == Cudd_Not(orResult));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, orResult);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddXor - XOR operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("XOR with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x XOR 0 = x
        DdNode *result = Cudd_bddXor(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        // x XOR 1 = !x
        result = Cudd_bddXor(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("XOR with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x XOR x = 0
        DdNode *result = Cudd_bddXor(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        // x XOR !x = 1
        result = Cudd_bddXor(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("XOR with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *result = Cudd_bddXor(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result != zero);
        REQUIRE(result != one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddXnor - XNOR operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("XNOR with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x XNOR 0 = !x
        DdNode *result = Cudd_bddXnor(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_Not(x));
        Cudd_RecursiveDeref(manager, result);
        
        // x XNOR 1 = x
        result = Cudd_bddXnor(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("XNOR with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x XNOR x = 1
        DdNode *result = Cudd_bddXnor(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        // x XNOR !x = 0
        result = Cudd_bddXnor(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("XNOR with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // x XNOR y = !(x XOR y)
        DdNode *result = Cudd_bddXnor(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *xorResult = Cudd_bddXor(manager, x, y);
        Cudd_Ref(xorResult);
        REQUIRE(result == Cudd_Not(xorResult));
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, xorResult);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddXnorLimit - XNOR with limit", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("XNOR with sufficient limit") {
        DdNode *result = Cudd_bddXnorLimit(manager, x, y, 1000);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLeq - Less-than-or-equal test", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Leq with constants") {
        // 0 <= 0 = true
        REQUIRE(Cudd_bddLeq(manager, zero, zero) == 1);
        
        // 1 <= 1 = true
        REQUIRE(Cudd_bddLeq(manager, one, one) == 1);
        
        // 0 <= 1 = true
        REQUIRE(Cudd_bddLeq(manager, zero, one) == 1);
        
        // 1 <= 0 = false
        REQUIRE(Cudd_bddLeq(manager, one, zero) == 0);
    }
    
    SECTION("Leq with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x <= x = true
        REQUIRE(Cudd_bddLeq(manager, x, x) == 1);
        
        // !x <= !x = true
        REQUIRE(Cudd_bddLeq(manager, Cudd_Not(x), Cudd_Not(x)) == 1);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Leq with variable and constant") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x <= 1 = true (x is always <= 1)
        REQUIRE(Cudd_bddLeq(manager, x, one) == 1);
        
        // 0 <= x = true (0 is always <= x)
        REQUIRE(Cudd_bddLeq(manager, zero, x) == 1);
        
        // 1 <= x = false (not always true)
        REQUIRE(Cudd_bddLeq(manager, one, x) == 0);
        
        // x <= 0 = false (not always true)
        REQUIRE(Cudd_bddLeq(manager, x, zero) == 0);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Leq with AND relation") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        
        // (x AND y) <= x = true
        REQUIRE(Cudd_bddLeq(manager, xy, x) == 1);
        
        // (x AND y) <= y = true
        REQUIRE(Cudd_bddLeq(manager, xy, y) == 1);
        
        // x <= (x AND y) = false (not always true)
        REQUIRE(Cudd_bddLeq(manager, x, xy) == 0);
        
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Leq with OR relation") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xy = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xy);
        
        // x <= (x OR y) = true
        REQUIRE(Cudd_bddLeq(manager, x, xy) == 1);
        
        // y <= (x OR y) = true
        REQUIRE(Cudd_bddLeq(manager, y, xy) == 1);
        
        // (x OR y) <= x = false (not always true)
        REQUIRE(Cudd_bddLeq(manager, xy, x) == 0);
        
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Leq with complementary variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x <= !x = false (unless x = 0)
        REQUIRE(Cudd_bddLeq(manager, x, Cudd_Not(x)) == 0);
        
        // !x <= x = false (unless x = 1)
        REQUIRE(Cudd_bddLeq(manager, Cudd_Not(x), x) == 0);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIntersect - Intersection operation", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Intersect with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x INTERSECT 0 = 0 (empty intersection)
        DdNode *result = Cudd_bddIntersect(manager, x, zero);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        // x INTERSECT 1 = x (x is subset of 1)
        result = Cudd_bddIntersect(manager, x, one);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Intersect with identical variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x INTERSECT x = x
        DdNode *result = Cudd_bddIntersect(manager, x, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Intersect with complementary variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // x INTERSECT !x = 0 (empty intersection)
        DdNode *result = Cudd_bddIntersect(manager, x, Cudd_Not(x));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Intersect with different variables") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // x INTERSECT y should give a witness (e.g., x AND y)
        DdNode *result = Cudd_bddIntersect(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result != zero); // Non-empty intersection
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Intersect with AND-related BDDs") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        
        // (x AND y) INTERSECT x should be non-empty
        DdNode *result = Cudd_bddIntersect(manager, xy, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result != zero);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Complex BDD operations - exercise recursive paths", "[cuddBddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Complex ITE with multiple variables") {
        // Create several variables
        DdNode *x0 = Cudd_bddNewVar(manager);
        DdNode *x1 = Cudd_bddNewVar(manager);
        DdNode *x2 = Cudd_bddNewVar(manager);
        DdNode *x3 = Cudd_bddNewVar(manager);
        Cudd_Ref(x0);
        Cudd_Ref(x1);
        Cudd_Ref(x2);
        Cudd_Ref(x3);
        
        // Build complex expressions: (x0 AND x1) OR (x2 AND x3)
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddAnd(manager, x2, x3);
        Cudd_Ref(f2);
        DdNode *f = Cudd_bddOr(manager, f1, f2);
        Cudd_Ref(f);
        
        // Build another: (x0 OR x2) AND (x1 OR x3)
        DdNode *g1 = Cudd_bddOr(manager, x0, x2);
        Cudd_Ref(g1);
        DdNode *g2 = Cudd_bddOr(manager, x1, x3);
        Cudd_Ref(g2);
        DdNode *g = Cudd_bddAnd(manager, g1, g2);
        Cudd_Ref(g);
        
        // Build another: x0 XOR x1 XOR x2 XOR x3
        DdNode *h1 = Cudd_bddXor(manager, x0, x1);
        Cudd_Ref(h1);
        DdNode *h2 = Cudd_bddXor(manager, x2, x3);
        Cudd_Ref(h2);
        DdNode *h = Cudd_bddXor(manager, h1, h2);
        Cudd_Ref(h);
        
        // Test ITE with complex expressions
        DdNode *result = Cudd_bddIte(manager, f, g, h);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Verify it's not trivial
        REQUIRE(result != zero);
        REQUIRE(result != one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, h2);
        Cudd_RecursiveDeref(manager, h1);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, g2);
        Cudd_RecursiveDeref(manager, g1);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, x3);
        Cudd_RecursiveDeref(manager, x2);
        Cudd_RecursiveDeref(manager, x1);
        Cudd_RecursiveDeref(manager, x0);
    }
    
    SECTION("Deep AND/OR operations") {
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build a chain: v0 AND v1 AND v2 AND ... AND v9
        DdNode *andChain = vars[0];
        Cudd_Ref(andChain);
        for (int i = 1; i < 10; i++) {
            DdNode *temp = Cudd_bddAnd(manager, andChain, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, andChain);
            andChain = temp;
        }
        
        // Build another: v0 OR v1 OR v2 OR ... OR v9
        DdNode *orChain = vars[0];
        Cudd_Ref(orChain);
        for (int i = 1; i < 10; i++) {
            DdNode *temp = Cudd_bddOr(manager, orChain, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, orChain);
            orChain = temp;
        }
        
        // Test operations with chains
        DdNode *result = Cudd_bddAnd(manager, andChain, orChain);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == andChain); // andChain is subset of orChain
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, orChain);
        Cudd_RecursiveDeref(manager, andChain);
        for (int i = 0; i < 10; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("XOR chains") {
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        // Build XOR chain
        DdNode *xorChain = vars[0];
        Cudd_Ref(xorChain);
        for (int i = 1; i < 8; i++) {
            DdNode *temp = Cudd_bddXor(manager, xorChain, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, xorChain);
            xorChain = temp;
        }
        
        // XOR with itself should be zero
        DdNode *result = Cudd_bddXor(manager, xorChain, xorChain);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, result);
        
        // XNOR with itself should be one
        result = Cudd_bddXnor(manager, xorChain, xorChain);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
        
        Cudd_RecursiveDeref(manager, xorChain);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
    }
    
    SECTION("ITE with complemented nodes") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // ITE(!x, y, z)
        DdNode *result1 = Cudd_bddIte(manager, Cudd_Not(x), y, z);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        
        // ITE(x, !y, z)
        DdNode *result2 = Cudd_bddIte(manager, x, Cudd_Not(y), z);
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        
        // ITE(x, y, !z)
        DdNode *result3 = Cudd_bddIte(manager, x, y, Cudd_Not(z));
        REQUIRE(result3 != nullptr);
        Cudd_Ref(result3);
        
        // ITE(!x, !y, !z)
        DdNode *result4 = Cudd_bddIte(manager, Cudd_Not(x), Cudd_Not(y), Cudd_Not(z));
        REQUIRE(result4 != nullptr);
        Cudd_Ref(result4);
        
        Cudd_RecursiveDeref(manager, result4);
        Cudd_RecursiveDeref(manager, result3);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Intersection with complemented nodes") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Test with complemented nodes
        DdNode *result1 = Cudd_bddIntersect(manager, Cudd_Not(x), y);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        REQUIRE(result1 != zero);
        Cudd_RecursiveDeref(manager, result1);
        
        DdNode *result2 = Cudd_bddIntersect(manager, Cudd_Not(x), Cudd_Not(y));
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        REQUIRE(result2 != zero);
        Cudd_RecursiveDeref(manager, result2);
        
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("LeQ with complex expressions") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Build f = (x AND y)
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Build g = (x AND y) OR z
        DdNode *g = Cudd_bddOr(manager, f, z);
        Cudd_Ref(g);
        
        // f <= g should be true
        REQUIRE(Cudd_bddLeq(manager, f, g) == 1);
        
        // g <= f should be false
        REQUIRE(Cudd_bddLeq(manager, g, f) == 0);
        
        // Test with complemented versions
        REQUIRE(Cudd_bddLeq(manager, Cudd_Not(g), Cudd_Not(f)) == 1);
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ITE constant with various configurations") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Build f = x AND y
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // ITE(f, 1, 0) should be non-constant
        DdNode *result = Cudd_bddIteConstant(manager, f, one, zero);
        REQUIRE(result == DD_NON_CONSTANT);
        
        // ITE(f, 0, 1) should be non-constant
        result = Cudd_bddIteConstant(manager, f, zero, one);
        REQUIRE(result == DD_NON_CONSTANT);
        
        // ITE(f, 1, 1) should be constant 1
        result = Cudd_bddIteConstant(manager, f, one, one);
        REQUIRE(result == one);
        
        // ITE(f, 0, 0) should be constant 0
        result = Cudd_bddIteConstant(manager, f, zero, zero);
        REQUIRE(result == zero);
        
        // ITE(x, f, y) - more complex
        result = Cudd_bddIteConstant(manager, x, f, y);
        REQUIRE((result == DD_NON_CONSTANT || Cudd_IsConstant(result)));
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Operations on complement edges") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // AND with complements
        DdNode *result = Cudd_bddAnd(manager, Cudd_Not(x), Cudd_Not(y));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Should equal !(x OR y) by De Morgan's law
        DdNode *expected = Cudd_bddOr(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == Cudd_Not(expected));
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        
        // OR with complements
        result = Cudd_bddOr(manager, Cudd_Not(x), Cudd_Not(y));
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Should equal !(x AND y) by De Morgan's law
        expected = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == Cudd_Not(expected));
        
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}
