#include <catch2/catch_test_macros.hpp>

#include "cudd/cudd.h"
#include "util.h"

#define DD_NON_CONSTANT ((DdNode *) 1)

TEST_CASE("Cudd_addIte - Basic ITE operations", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("ITE with constant selector f=1") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, one, var0, const2);
        REQUIRE(result == var0);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("ITE with constant selector f=0") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, zero, var0, const2);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("ITE with g==h returns g") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const3);
        DdNode *result = Cudd_addIte(manager, var0, const3, const3);
        REQUIRE(result == const3);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const3);
    }
    
    SECTION("ITE(F,1,0) returns F") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *result = Cudd_addIte(manager, var0, one, zero);
        REQUIRE(result == var0);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("ITE(F,F,H) - addVarToConst f==g case") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, var0, var0, const2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("ITE(F,G,F) - addVarToConst f==h case") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, var0, const2, var0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("ITE shortcut F=(x,1,0)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, var0, var1, const2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("ITE shortcut F=(x,0,1)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *f = Cudd_addIte(manager, var0, zero, one);
        REQUIRE(f != nullptr);
        Cudd_Ref(f);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, f, var1, const2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("Complex ITE with multiple variables") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        DdNode *var2 = Cudd_addIthVar(manager, 2);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *g = Cudd_addIte(manager, var1, const2, const3);
        Cudd_Ref(g);
        DdNode *h = Cudd_addIte(manager, var2, const3, const2);
        Cudd_Ref(h);
        DdNode *result = Cudd_addIte(manager, var0, g, h);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, var2);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addIteConstant - ITE constant check", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("f=1 returns g") {
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIteConstant(manager, one, const2, zero);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("f=0 returns h") {
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIteConstant(manager, zero, zero, const2);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("g==h returns g") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIteConstant(manager, var0, const2, const2);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("Both g,h constant but different returns DD_NON_CONSTANT") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *result = Cudd_addIteConstant(manager, var0, const2, const3);
        REQUIRE(result == DD_NON_CONSTANT);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
    }
    
    SECTION("Shortcut path F=(x,1,0) with x < top(G,H)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIteConstant(manager, var0, var1, const2);
        REQUIRE(result == DD_NON_CONSTANT);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("Recursive case returning constant") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *g = Cudd_addIte(manager, var0, const2, const2);
        Cudd_Ref(g);
        DdNode *result = Cudd_addIteConstant(manager, var0, g, const2);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Recursive case with non-constant result") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *g = Cudd_addIte(manager, var1, const2, const3);
        Cudd_Ref(g);
        DdNode *h = Cudd_addIte(manager, var1, const3, const2);
        Cudd_Ref(h);
        DdNode *result = Cudd_addIteConstant(manager, var0, g, h);
        REQUIRE(result == DD_NON_CONSTANT);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, h);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addEvalConst - Eval constant check", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("f=1 returns g") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *result = Cudd_addEvalConst(manager, one, var0);
        REQUIRE(result == var0);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("g is constant returns g") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addEvalConst(manager, var0, const2);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("f=0 returns background") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *result = Cudd_addEvalConst(manager, zero, var0);
        REQUIRE(result == Cudd_ReadBackground(manager));
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("Recursive case Fv != zero") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *g = Cudd_addIte(manager, var1, const2, const3);
        Cudd_Ref(g);
        DdNode *result = Cudd_addEvalConst(manager, var0, g);
        REQUIRE(result == DD_NON_CONSTANT);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Recursive case Fv == zero, Fnv != zero") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *f = Cudd_addIte(manager, var0, zero, one);
        Cudd_Ref(f);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *g = Cudd_addIte(manager, var1, const2, const2);
        Cudd_Ref(g);
        DdNode *result = Cudd_addEvalConst(manager, f, g);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Recursive case with t!=e returns DD_NON_CONSTANT") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *g = Cudd_addIte(manager, var0, const2, const3);
        Cudd_Ref(g);
        DdNode *result = Cudd_addEvalConst(manager, var1, g);
        REQUIRE(result == DD_NON_CONSTANT);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
        Cudd_RecursiveDeref(manager, g);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addCmpl - ADD complement", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("Complement of 0 is 1") {
        DdNode *result = Cudd_addCmpl(manager, zero);
        REQUIRE(result == one);
    }
    
    SECTION("Complement of 1 is 0") {
        DdNode *result = Cudd_addCmpl(manager, one);
        REQUIRE(result == zero);
    }
    
    SECTION("Complement of non-zero constant is 0") {
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addCmpl(manager, const2);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("Complement of variable ADD") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *result = Cudd_addCmpl(manager, var0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("Complement of complex ADD") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        DdNode *add = Cudd_addApply(manager, Cudd_addPlus, var0, var1);
        Cudd_Ref(add);
        DdNode *result = Cudd_addCmpl(manager, add);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addLeq - ADD less than or equal", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("f == g returns 1") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        REQUIRE(Cudd_addLeq(manager, var0, var0) == 1);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("Both constants: f <= g") {
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        REQUIRE(Cudd_addLeq(manager, const2, const3) == 1);
        REQUIRE(Cudd_addLeq(manager, const3, const2) == 0);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
    }
    
    SECTION("f == MINUS_INFINITY returns 1") {
        DdNode *minusInf = Cudd_ReadMinusInfinity(manager);
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        REQUIRE(Cudd_addLeq(manager, minusInf, var0) == 1);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("f == PLUS_INFINITY returns 0") {
        DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        REQUIRE(Cudd_addLeq(manager, plusInf, var0) == 0);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("g == PLUS_INFINITY returns 1") {
        DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        REQUIRE(Cudd_addLeq(manager, var0, plusInf) == 1);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("g == MINUS_INFINITY returns 0") {
        DdNode *minusInf = Cudd_ReadMinusInfinity(manager);
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        REQUIRE(Cudd_addLeq(manager, var0, minusInf) == 0);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("Recursive case") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *f = Cudd_addIte(manager, var0, const2, one);
        Cudd_Ref(f);
        DdNode *g = Cudd_addIte(manager, var0, const3, const2);
        Cudd_Ref(g);
        REQUIRE(Cudd_addLeq(manager, f, g) == 1);
        REQUIRE(Cudd_addLeq(manager, g, f) == 0);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("Cache hit test") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *f = Cudd_addIte(manager, var0, const2, one);
        Cudd_Ref(f);
        REQUIRE(Cudd_addLeq(manager, f, const2) == 1);
        REQUIRE(Cudd_addLeq(manager, f, const2) == 1);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddAddIteRecur - recursive ITE", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("t==e case returns t") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *g = Cudd_addIte(manager, var1, const2, const2);
        Cudd_Ref(g);
        DdNode *h = Cudd_addIte(manager, var1, const2, const2);
        Cudd_Ref(h);
        DdNode *result = Cudd_addIte(manager, var0, g, h);
        REQUIRE(result == const2);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, h);
    }
    
    SECTION("topf > v case (cofactors of f)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        DdNode *const3 = Cudd_addConst(manager, 3.0);
        Cudd_Ref(const2);
        Cudd_Ref(const3);
        DdNode *f = Cudd_addIte(manager, var1, one, zero);
        Cudd_Ref(f);
        DdNode *g = Cudd_addIte(manager, var0, const2, const3);
        Cudd_Ref(g);
        DdNode *h = Cudd_addIte(manager, var0, const3, const2);
        Cudd_Ref(h);
        DdNode *result = Cudd_addIte(manager, f, g, h);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, var1);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, const3);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, h);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddAddCmplRecur - recursive complement", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("Cache hit test") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *result1 = Cudd_addCmpl(manager, var0);
        Cudd_Ref(result1);
        DdNode *result2 = Cudd_addCmpl(manager, var0);
        Cudd_Ref(result2);
        REQUIRE(result1 == result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, var0);
    }
    
    SECTION("t==e case") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *add = Cudd_addIte(manager, var0, const2, const2);
        Cudd_Ref(add);
        DdNode *result = Cudd_addCmpl(manager, add);
        REQUIRE(result == zero);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
        Cudd_RecursiveDeref(manager, add);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("addVarToConst - Variable to constant conversion", "[cuddAddIte]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_ReadZero(manager);
    
    SECTION("f==g case: ITE(F,F,H) -> ITE(F,1,H)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, var0, var0, const2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    SECTION("f==h case: ITE(F,G,F) -> ITE(F,G,0)") {
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *const2 = Cudd_addConst(manager, 2.0);
        Cudd_Ref(const2);
        DdNode *result = Cudd_addIte(manager, var0, const2, var0);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, var0);
        Cudd_RecursiveDeref(manager, const2);
    }
    
    Cudd_Quit(manager);
}
