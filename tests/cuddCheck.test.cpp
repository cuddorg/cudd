#include <catch2/catch_test_macros.hpp>

#include "mtr.h"
#include "mtrInt.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

// Tests for Cudd_DebugCheck

TEST_CASE("Cudd_DebugCheck - Empty manager", "[cuddCheck]") {
    DdManager *m = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    REQUIRE(Cudd_DebugCheck(m) == 0);
    Cudd_Quit(m);
}

TEST_CASE("Cudd_DebugCheck - BDD variables", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    DdNode *y = Cudd_bddIthVar(m, 1);
    Cudd_Ref(x); Cudd_Ref(y);
    DdNode *f = Cudd_bddAnd(m, x, y);
    Cudd_Ref(f);
    REQUIRE(Cudd_DebugCheck(m) == 0);
    Cudd_RecursiveDeref(m, f);
    Cudd_RecursiveDeref(m, y);
    Cudd_RecursiveDeref(m, x);
    Cudd_Quit(m);
}

TEST_CASE("Cudd_DebugCheck - ZDD manager", "[cuddCheck]") {
    DdManager *m = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    REQUIRE(Cudd_ReadZddOne(m, 0) != nullptr);
    (void)Cudd_DebugCheck(m);
    Cudd_Quit(m);
}

TEST_CASE("Cudd_DebugCheck - ADD constants", "[cuddCheck]") {
    DdManager *m = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *c1 = Cudd_addConst(m, 1.0);
    DdNode *c2 = Cudd_addConst(m, 2.0);
    Cudd_Ref(c1); Cudd_Ref(c2);
    REQUIRE(Cudd_DebugCheck(m) == 0);
    Cudd_RecursiveDeref(m, c2);
    Cudd_RecursiveDeref(m, c1);
    Cudd_Quit(m);
}

TEST_CASE("Cudd_DebugCheck - Large vars", "[cuddCheck]") {
    DdManager *m = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *vars[20];
    for (int i = 0; i < 20; i++) {
        vars[i] = Cudd_bddIthVar(m, i);
        Cudd_Ref(vars[i]);
    }
    DdNode *f = Cudd_bddAnd(m, vars[0], vars[1]);
    Cudd_Ref(f);
    REQUIRE(Cudd_DebugCheck(m) == 0);
    Cudd_RecursiveDeref(m, f);
    for (int i = 0; i < 20; i++) Cudd_RecursiveDeref(m, vars[i]);
    Cudd_Quit(m);
}

// Tests for Cudd_CheckKeys

TEST_CASE("Cudd_CheckKeys - Empty manager", "[cuddCheck]") {
    DdManager *m = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(Cudd_CheckKeys(m) == 0);
    fclose(m->out);
    m->out = orig;
    Cudd_Quit(m);
}

TEST_CASE("Cudd_CheckKeys - BDD nodes", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    DdNode *y = Cudd_bddIthVar(m, 1);
    Cudd_Ref(x); Cudd_Ref(y);
    DdNode *f = Cudd_bddAnd(m, x, y);
    Cudd_Ref(f);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(Cudd_CheckKeys(m) == 0);
    fclose(m->out);
    m->out = orig;
    Cudd_RecursiveDeref(m, f);
    Cudd_RecursiveDeref(m, y);
    Cudd_RecursiveDeref(m, x);
    Cudd_Quit(m);
}

TEST_CASE("Cudd_CheckKeys - ZDD manager", "[cuddCheck]") {
    DdManager *m = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(Cudd_CheckKeys(m) == 0);
    fclose(m->out);
    m->out = orig;
    Cudd_Quit(m);
}

TEST_CASE("Cudd_CheckKeys - Constants", "[cuddCheck]") {
    DdManager *m = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *c1 = Cudd_addConst(m, 1.5);
    DdNode *c2 = Cudd_addConst(m, 2.5);
    Cudd_Ref(c1); Cudd_Ref(c2);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(Cudd_CheckKeys(m) == 0);
    fclose(m->out);
    m->out = orig;
    Cudd_RecursiveDeref(m, c2);
    Cudd_RecursiveDeref(m, c1);
    Cudd_Quit(m);
}

// Tests for cuddHeapProfile

TEST_CASE("cuddHeapProfile - Empty manager", "[cuddCheck]") {
    DdManager *m = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(cuddHeapProfile(m) == 1);
    fclose(m->out);
    m->out = orig;
    Cudd_Quit(m);
}

TEST_CASE("cuddHeapProfile - BDD nodes", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    DdNode *y = Cudd_bddIthVar(m, 1);
    Cudd_Ref(x); Cudd_Ref(y);
    DdNode *f = Cudd_bddAnd(m, x, y);
    Cudd_Ref(f);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(cuddHeapProfile(m) == 1);
    fclose(m->out);
    m->out = orig;
    Cudd_RecursiveDeref(m, f);
    Cudd_RecursiveDeref(m, y);
    Cudd_RecursiveDeref(m, x);
    Cudd_Quit(m);
}

// Tests for cuddPrintNode

TEST_CASE("cuddPrintNode - BDD variable", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    FILE *devnull = fopen("/dev/null", "w");
    cuddPrintNode(x, devnull);
    fclose(devnull);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintNode - Internal node", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    DdNode *y = Cudd_bddIthVar(m, 1);
    Cudd_Ref(x); Cudd_Ref(y);
    DdNode *f = Cudd_bddAnd(m, x, y);
    Cudd_Ref(f);
    FILE *devnull = fopen("/dev/null", "w");
    cuddPrintNode(f, devnull);
    fclose(devnull);
    Cudd_RecursiveDeref(m, f);
    Cudd_RecursiveDeref(m, y);
    Cudd_RecursiveDeref(m, x);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintNode - Complemented", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    Cudd_Ref(x);
    DdNode *notX = Cudd_Not(x);
    FILE *devnull = fopen("/dev/null", "w");
    cuddPrintNode(notX, devnull);
    fclose(devnull);
    Cudd_RecursiveDeref(m, x);
    Cudd_Quit(m);
}

// Tests for cuddPrintVarGroups - one comprehensive test

TEST_CASE("cuddPrintVarGroups - BDD with various flags", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    // Cudd_MakeTreeNode creates and sets the tree in the manager
    MtrNode *group = Cudd_MakeTreeNode(m, 0, 5, MTR_DEFAULT);
    REQUIRE(group != nullptr);
    // Get the actual tree from the manager (not the group returned)
    MtrNode *tree = Cudd_ReadTree(m);
    REQUIRE(tree != nullptr);
    cuddPrintVarGroups(m, tree, 0, 1);
    Cudd_Quit(m);
}

// Combined test

TEST_CASE("Combined DebugCheck and CheckKeys", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    DdNode *x = Cudd_bddIthVar(m, 0);
    DdNode *y = Cudd_bddIthVar(m, 1);
    Cudd_Ref(x); Cudd_Ref(y);
    DdNode *f = Cudd_bddAnd(m, x, y);
    Cudd_Ref(f);
    REQUIRE(Cudd_DebugCheck(m) == 0);
    FILE *orig = m->out;
    m->out = fopen("/dev/null", "w");
    REQUIRE(Cudd_CheckKeys(m) == 0);
    fclose(m->out);
    m->out = orig;
    Cudd_RecursiveDeref(m, f);
    Cudd_RecursiveDeref(m, y);
    Cudd_RecursiveDeref(m, x);
    Cudd_Quit(m);
}
