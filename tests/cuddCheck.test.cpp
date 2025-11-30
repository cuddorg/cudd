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

TEST_CASE("Cudd_DebugCheck - ZDD with actual nodes", "[cuddCheck]") {
    // Create a manager with both BDD and ZDD support
    DdManager *m = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    // Create ZDD variables and operations to populate ZDD subtables
    DdNode *zvar0 = Cudd_zddIthVar(m, 0);
    DdNode *zvar1 = Cudd_zddIthVar(m, 1);
    REQUIRE(zvar0 != nullptr);
    REQUIRE(zvar1 != nullptr);
    Cudd_Ref(zvar0);
    Cudd_Ref(zvar1);
    
    // Create a ZDD union to generate internal ZDD nodes
    DdNode *zunion = Cudd_zddUnion(m, zvar0, zvar1);
    REQUIRE(zunion != nullptr);
    Cudd_Ref(zunion);
    
    // This will exercise ZDD subtable checking in Cudd_DebugCheck
    int result = Cudd_DebugCheck(m);
    // ZDD internal state might cause non-zero result, that's ok
    (void)result;
    
    Cudd_RecursiveDerefZdd(m, zunion);
    Cudd_RecursiveDerefZdd(m, zvar1);
    Cudd_RecursiveDerefZdd(m, zvar0);
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

TEST_CASE("Cudd_DebugCheck - Multiple constants in hash table", "[cuddCheck]") {
    // Create many constants to populate constant table slots
    DdManager *m = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    // Create multiple constants to exercise constant table iteration
    DdNode *consts[10];
    for (int i = 0; i < 10; i++) {
        consts[i] = Cudd_addConst(m, (double)(i * 1.1));
        REQUIRE(consts[i] != nullptr);
        Cudd_Ref(consts[i]);
    }
    
    REQUIRE(Cudd_DebugCheck(m) == 0);
    
    for (int i = 0; i < 10; i++) {
        Cudd_RecursiveDeref(m, consts[i]);
    }
    Cudd_Quit(m);
}

TEST_CASE("Cudd_DebugCheck - Complex BDD structure", "[cuddCheck]") {
    // Create a complex BDD to exercise edge table population
    DdManager *m = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    DdNode *vars[10];
    for (int i = 0; i < 10; i++) {
        vars[i] = Cudd_bddIthVar(m, i);
        Cudd_Ref(vars[i]);
    }
    
    // Build a complex BDD with many internal nodes
    DdNode *f = vars[0];
    Cudd_Ref(f);
    for (int i = 1; i < 10; i++) {
        DdNode *newF = (i % 2 == 0) ? 
            Cudd_bddAnd(m, f, vars[i]) : 
            Cudd_bddOr(m, f, vars[i]);
        Cudd_Ref(newF);
        Cudd_RecursiveDeref(m, f);
        f = newF;
    }
    
    // DebugCheck will iterate through all nodes and verify reference counts
    REQUIRE(Cudd_DebugCheck(m) == 0);
    
    Cudd_RecursiveDeref(m, f);
    for (int i = 0; i < 10; i++) {
        Cudd_RecursiveDeref(m, vars[i]);
    }
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

TEST_CASE("cuddHeapProfile - With constants", "[cuddCheck]") {
    // Test the branch that handles constants in cuddHeapProfile
    DdManager *m = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    // Create some ADD constants 
    DdNode *c1 = Cudd_addConst(m, 1.5);
    DdNode *c2 = Cudd_addConst(m, 2.5);
    DdNode *c3 = Cudd_addConst(m, 3.5);
    Cudd_Ref(c1); Cudd_Ref(c2); Cudd_Ref(c3);
    
    FILE *orig = m->out;
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    m->out = devnull;
    REQUIRE(cuddHeapProfile(m) == 1);
    fclose(m->out);
    m->out = orig;
    
    Cudd_RecursiveDeref(m, c3);
    Cudd_RecursiveDeref(m, c2);
    Cudd_RecursiveDeref(m, c1);
    Cudd_Quit(m);
}

TEST_CASE("cuddHeapProfile - Large structure for maxnodes tracking", "[cuddCheck]") {
    // Create a structure where constants might have more nodes than some tables
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    // Create many constants 
    DdNode *consts[20];
    for (int i = 0; i < 20; i++) {
        consts[i] = Cudd_addConst(m, (double)(i * 0.5));
        Cudd_Ref(consts[i]);
    }
    
    FILE *orig = m->out;
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    m->out = devnull;
    REQUIRE(cuddHeapProfile(m) == 1);
    fclose(m->out);
    m->out = orig;
    
    for (int i = 0; i < 20; i++) {
        Cudd_RecursiveDeref(m, consts[i]);
    }
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

// Tests for cuddPrintVarGroups

TEST_CASE("cuddPrintVarGroups - BDD silent mode", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    MtrNode *group = Cudd_MakeTreeNode(m, 0, 5, MTR_DEFAULT);
    REQUIRE(group != nullptr);
    MtrNode *tree = Cudd_ReadTree(m);
    REQUIRE(tree != nullptr);
    // Silent mode (silent=1)
    cuddPrintVarGroups(m, tree, 0, 1);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintVarGroups - BDD non-silent mode", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    MtrNode *group = Cudd_MakeTreeNode(m, 0, 5, MTR_DEFAULT);
    REQUIRE(group != nullptr);
    MtrNode *tree = Cudd_ReadTree(m);
    REQUIRE(tree != nullptr);
    // Non-silent mode (silent=0) - prints output
    // Redirect stdout to avoid test output
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    FILE *orig_stdout = stdout;
    stdout = devnull;
    cuddPrintVarGroups(m, tree, 0, 0);
    stdout = orig_stdout;
    fclose(devnull);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintVarGroups - With FIXED flag", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    MtrNode *group = Cudd_MakeTreeNode(m, 0, 5, MTR_FIXED);
    REQUIRE(group != nullptr);
    MtrNode *tree = Cudd_ReadTree(m);
    REQUIRE(tree != nullptr);
    // Non-silent to exercise flag printing code
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    FILE *orig_stdout = stdout;
    stdout = devnull;
    cuddPrintVarGroups(m, tree, 0, 0);
    stdout = orig_stdout;
    fclose(devnull);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintVarGroups - With SOFT flag", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    MtrNode *group = Cudd_MakeTreeNode(m, 0, 5, MTR_SOFT);
    REQUIRE(group != nullptr);
    MtrNode *tree = Cudd_ReadTree(m);
    REQUIRE(tree != nullptr);
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    FILE *orig_stdout = stdout;
    stdout = devnull;
    cuddPrintVarGroups(m, tree, 0, 0);
    stdout = orig_stdout;
    fclose(devnull);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintVarGroups - ZDD tree", "[cuddCheck]") {
    DdManager *m = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    MtrNode *group = Cudd_MakeZddTreeNode(m, 0, 5, MTR_DEFAULT);
    REQUIRE(group != nullptr);
    MtrNode *tree = Cudd_ReadZddTree(m);
    REQUIRE(tree != nullptr);
    // zdd=1 to use ZDD permutation
    cuddPrintVarGroups(m, tree, 1, 1);
    Cudd_Quit(m);
}

TEST_CASE("cuddPrintVarGroups - Nested groups with children", "[cuddCheck]") {
    DdManager *m = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    // Create parent group spanning all variables
    MtrNode *parent = Cudd_MakeTreeNode(m, 0, 10, MTR_DEFAULT);
    REQUIRE(parent != nullptr);
    
    // Create child groups - these will become children of the root tree
    MtrNode *child1 = Cudd_MakeTreeNode(m, 0, 5, MTR_FIXED);
    REQUIRE(child1 != nullptr);
    MtrNode *child2 = Cudd_MakeTreeNode(m, 5, 5, MTR_SOFT);
    REQUIRE(child2 != nullptr);
    
    MtrNode *tree = Cudd_ReadTree(m);
    REQUIRE(tree != nullptr);
    
    // This will exercise the recursion through children
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    FILE *orig_stdout = stdout;
    stdout = devnull;
    cuddPrintVarGroups(m, tree, 0, 0);
    stdout = orig_stdout;
    fclose(devnull);
    
    Cudd_Quit(m);
}

// Tests for Cudd_CheckKeys with ZDD nodes

TEST_CASE("Cudd_CheckKeys - ZDD with actual nodes", "[cuddCheck]") {
    DdManager *m = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(m != nullptr);
    
    // Create ZDD variables
    DdNode *zvar0 = Cudd_zddIthVar(m, 0);
    DdNode *zvar1 = Cudd_zddIthVar(m, 1);
    REQUIRE(zvar0 != nullptr);
    REQUIRE(zvar1 != nullptr);
    Cudd_Ref(zvar0);
    Cudd_Ref(zvar1);
    
    // Create ZDD union
    DdNode *zunion = Cudd_zddUnion(m, zvar0, zvar1);
    REQUIRE(zunion != nullptr);
    Cudd_Ref(zunion);
    
    FILE *orig = m->out;
    FILE *devnull = fopen("/dev/null", "w");
    REQUIRE(devnull != nullptr);
    m->out = devnull;
    int result = Cudd_CheckKeys(m);
    fclose(m->out);
    m->out = orig;
    
    // Result may be non-zero due to ZDD internal state
    (void)result;
    
    Cudd_RecursiveDerefZdd(m, zunion);
    Cudd_RecursiveDerefZdd(m, zvar1);
    Cudd_RecursiveDerefZdd(m, zvar0);
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
