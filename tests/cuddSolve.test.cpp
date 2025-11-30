#include <catch2/catch_test_macros.hpp>
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddSolve.c
 * 
 * This file contains comprehensive tests for the cuddSolve module.
 * The functions tested are:
 * - Cudd_SolveEqn: Solves boolean equation F(x,y) = 0
 * - Cudd_VerifySol: Verifies the solution by substituting back
 * - cuddSolveEqnRecur: Internal recursive implementation
 * - cuddVerifySol: Internal verification implementation
 */

TEST_CASE("Cudd_SolveEqn - Basic equation solving", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Solve simple equation x XOR y = 0") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddXor(manager, x, y);
        REQUIRE(F != nullptr);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        REQUIRE(G[0] != nullptr);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Solve equation F = y") {
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(y);
        
        DdNode *F = y;
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        free(G);
    }
    
    SECTION("Solve with multiple unknowns") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y0 = Cudd_bddNewVar(manager);
        DdNode *y1 = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y0);
        Cudd_Ref(y1);
        
        DdNode *xy0 = Cudd_bddAnd(manager, x, y0);
        Cudd_Ref(xy0);
        DdNode *F = Cudd_bddXor(manager, xy0, y1);
        REQUIRE(F != nullptr);
        Cudd_Ref(F);
        
        DdNode *Y = Cudd_bddAnd(manager, y0, y1);
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 2);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 2);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 2);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        for (int i = 0; i < 2; i++) {
            Cudd_RecursiveDeref(manager, G[i]);
        }
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, xy0);
        Cudd_RecursiveDeref(manager, y1);
        Cudd_RecursiveDeref(manager, y0);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Solve constant equation F = 0") {
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(y);
        
        DdNode *F = zero;
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        free(G);
    }
    
    SECTION("Solve constant equation F = 1") {
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(y);
        
        DdNode *F = one;
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SolveEqn - Complex equations", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Equation with AND operation") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Equation with OR operation") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddOr(manager, x, y);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Equation with XNOR operation") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddXnor(manager, x, y);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Equation with NOT on unknown") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *notY = Cudd_Not(y);
        DdNode *F = Cudd_bddXor(manager, x, notY);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Equation with many variables") {
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        DdNode *t0 = Cudd_bddAnd(manager, vars[0], vars[3]);
        Cudd_Ref(t0);
        DdNode *t1 = Cudd_bddAnd(manager, vars[1], vars[4]);
        Cudd_Ref(t1);
        DdNode *t2 = Cudd_bddAnd(manager, vars[2], vars[5]);
        Cudd_Ref(t2);
        DdNode *t01 = Cudd_bddXor(manager, t0, t1);
        Cudd_Ref(t01);
        DdNode *F = Cudd_bddXor(manager, t01, t2);
        Cudd_Ref(F);
        
        DdNode *Y = Cudd_bddAnd(manager, vars[3], vars[4]);
        Cudd_Ref(Y);
        DdNode *Yfull = Cudd_bddAnd(manager, Y, vars[5]);
        Cudd_Ref(Yfull);
        Cudd_RecursiveDeref(manager, Y);
        Y = Yfull;
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 3);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 3);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 3);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        for (int i = 0; i < 3; i++) {
            Cudd_RecursiveDeref(manager, G[i]);
        }
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, t01);
        Cudd_RecursiveDeref(manager, t2);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t0);
        for (int i = 0; i < 6; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SolveEqn - Edge cases", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Base case - Y equals one (no unknowns)") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *F = x;
        Cudd_Ref(F);
        
        DdNode *Y = one;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 0);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 0);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 0);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Equation involving ITE structure") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *F = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Complex nested equation") {
        DdNode *a = Cudd_bddNewVar(manager);
        DdNode *b = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(y);
        
        DdNode *aOrb = Cudd_bddOr(manager, a, b);
        Cudd_Ref(aOrb);
        DdNode *F = Cudd_bddXor(manager, aOrb, y);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, aOrb);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, a);
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_VerifySol - Direct verification", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Verify correct solution x XOR y with G=x") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddXor(manager, x, y);
        Cudd_Ref(F);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        G[0] = x;
        Cudd_Ref(G[0]);
        
        int *yIndex = (int *)malloc(sizeof(int) * 1);
        yIndex[0] = Cudd_NodeReadIndex(y);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        REQUIRE(verification == zero);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Verify incorrect solution x XOR y with G=NOT(x)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddXor(manager, x, y);
        Cudd_Ref(F);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        G[0] = Cudd_Not(x);
        Cudd_Ref(G[0]);
        
        int *yIndex = (int *)malloc(sizeof(int) * 1);
        yIndex[0] = Cudd_NodeReadIndex(y);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        REQUIRE(verification == one);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Verify with constant solution G=0") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(F);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        G[0] = zero;
        Cudd_Ref(G[0]);
        
        int *yIndex = (int *)malloc(sizeof(int) * 1);
        yIndex[0] = Cudd_NodeReadIndex(y);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        REQUIRE(verification == zero);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Verify with multiple substitutions") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y0 = Cudd_bddNewVar(manager);
        DdNode *y1 = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y0);
        Cudd_Ref(y1);
        
        DdNode *t0 = Cudd_bddXor(manager, x, y0);
        Cudd_Ref(t0);
        DdNode *t1 = Cudd_bddXor(manager, x, y1);
        Cudd_Ref(t1);
        DdNode *F = Cudd_bddAnd(manager, t0, t1);
        Cudd_Ref(F);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 2);
        G[0] = x;
        G[1] = x;
        Cudd_Ref(G[0]);
        Cudd_Ref(G[1]);
        
        int *yIndex = (int *)malloc(sizeof(int) * 2);
        yIndex[0] = Cudd_NodeReadIndex(y0);
        yIndex[1] = Cudd_NodeReadIndex(y1);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 2);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        REQUIRE(verification == zero);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, G[1]);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t0);
        Cudd_RecursiveDeref(manager, y1);
        Cudd_RecursiveDeref(manager, y0);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SolveEqn - Partial solvability", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Equation with parameter-only dependency") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = x;
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Complex consistency condition") {
        DdNode *a = Cudd_bddNewVar(manager);
        DdNode *b = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(a);
        Cudd_Ref(b);
        Cudd_Ref(y);
        
        DdNode *ab = Cudd_bddAnd(manager, a, b);
        Cudd_Ref(ab);
        DdNode *F = Cudd_bddAnd(manager, ab, y);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, ab);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, b);
        Cudd_RecursiveDeref(manager, a);
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SolveEqn - Deep recursion", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Multiple level equation solving") {
        const int numVars = 8;
        DdNode *vars[8];
        for (int i = 0; i < numVars; i++) {
            vars[i] = Cudd_bddNewVar(manager);
            Cudd_Ref(vars[i]);
        }
        
        DdNode *F = vars[0];
        Cudd_Ref(F);
        for (int i = 1; i < numVars; i++) {
            DdNode *temp = Cudd_bddXor(manager, F, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, F);
            F = temp;
        }
        
        DdNode *Y = vars[4];
        Cudd_Ref(Y);
        for (int i = 5; i < numVars; i++) {
            DdNode *temp = Cudd_bddAnd(manager, Y, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, Y);
            Y = temp;
        }
        
        int numUnknowns = 4;
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * numUnknowns);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, numUnknowns);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, numUnknowns);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        for (int i = 0; i < numUnknowns; i++) {
            Cudd_RecursiveDeref(manager, G[i]);
        }
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        for (int i = 0; i < numVars; i++) {
            Cudd_RecursiveDeref(manager, vars[i]);
        }
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SolveEqn - Complemented edges", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Equation with negated F") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *xXory = Cudd_bddXor(manager, x, y);
        Cudd_Ref(xXory);
        DdNode *F = Cudd_Not(xXory);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, xXory);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Equation with negated unknown") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *F = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_SolveEqn - All paths coverage", "[cuddSolve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Trigger substitution loop") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y0 = Cudd_bddNewVar(manager);
        DdNode *y1 = Cudd_bddNewVar(manager);
        DdNode *y2 = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y0);
        Cudd_Ref(y1);
        Cudd_Ref(y2);
        
        DdNode *t0 = Cudd_bddXor(manager, x, y0);
        Cudd_Ref(t0);
        DdNode *t1 = Cudd_bddXor(manager, y1, y2);
        Cudd_Ref(t1);
        DdNode *F = Cudd_bddAnd(manager, t0, t1);
        Cudd_Ref(F);
        
        DdNode *Y = Cudd_bddAnd(manager, y0, y1);
        Cudd_Ref(Y);
        DdNode *Yfull = Cudd_bddAnd(manager, Y, y2);
        Cudd_Ref(Yfull);
        Cudd_RecursiveDeref(manager, Y);
        Y = Yfull;
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 3);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 3);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 3);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        for (int i = 0; i < 3; i++) {
            Cudd_RecursiveDeref(manager, G[i]);
        }
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, t1);
        Cudd_RecursiveDeref(manager, t0);
        Cudd_RecursiveDeref(manager, y2);
        Cudd_RecursiveDeref(manager, y1);
        Cudd_RecursiveDeref(manager, y0);
        Cudd_RecursiveDeref(manager, x);
        free(G);
    }
    
    SECTION("Different variable orderings") {
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(y);
        Cudd_Ref(x);
        
        DdNode *F = Cudd_bddXor(manager, x, y);
        Cudd_Ref(F);
        
        DdNode *Y = y;
        Cudd_Ref(Y);
        
        DdNode **G = (DdNode **)malloc(sizeof(DdNode *) * 1);
        int *yIndex = nullptr;
        
        DdNode *consistency = Cudd_SolveEqn(manager, F, Y, G, &yIndex, 1);
        REQUIRE(consistency != nullptr);
        Cudd_Ref(consistency);
        
        DdNode *verification = Cudd_VerifySol(manager, F, G, yIndex, 1);
        REQUIRE(verification != nullptr);
        Cudd_Ref(verification);
        
        Cudd_RecursiveDeref(manager, verification);
        Cudd_RecursiveDeref(manager, G[0]);
        Cudd_RecursiveDeref(manager, consistency);
        Cudd_RecursiveDeref(manager, Y);
        Cudd_RecursiveDeref(manager, F);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        free(G);
    }
    
    Cudd_Quit(manager);
}
