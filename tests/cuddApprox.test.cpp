#include <catch2/catch_test_macros.hpp>
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Comprehensive tests for cuddApprox.c to achieve >90% code coverage
 */

// Helper: Create simple AND BDD
static DdNode* createSimpleBDD(DdManager* dd) {
    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *result = Cudd_bddAnd(dd, x0, x1);
    Cudd_Ref(result);
    return result;
}

// Helper: Create complex BDD (x0*x1 + x2*x3 + ...)
static DdNode* createComplexBDD(DdManager* dd, int numVars) {
    if (numVars < 2) return Cudd_ReadOne(dd);
    DdNode *result = Cudd_ReadLogicZero(dd);
    Cudd_Ref(result);
    for (int i = 0; i < numVars - 1; i += 2) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *xi1 = Cudd_bddIthVar(dd, i + 1);
        DdNode *term = Cudd_bddAnd(dd, xi, xi1);
        Cudd_Ref(term);
        DdNode *newResult = Cudd_bddOr(dd, result, term);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(dd, term);
        Cudd_RecursiveDeref(dd, result);
        result = newResult;
    }
    return result;
}

// Helper: Create deep AND chain
static DdNode* createDeepBDD(DdManager* dd, int numVars) {
    DdNode *result = Cudd_ReadOne(dd);
    Cudd_Ref(result);
    for (int i = 0; i < numVars; i++) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *newResult = Cudd_bddAnd(dd, result, xi);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(dd, result);
        result = newResult;
    }
    return result;
}

// Helper: Create wide OR chain
static DdNode* createWideBDD(DdManager* dd, int numVars) {
    DdNode *result = Cudd_ReadLogicZero(dd);
    Cudd_Ref(result);
    for (int i = 0; i < numVars; i++) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *newResult = Cudd_bddOr(dd, result, xi);
        Cudd_Ref(newResult);
        Cudd_RecursiveDeref(dd, result);
        result = newResult;
    }
    return result;
}

// ============== Cudd_UnderApprox Tests ==============

TEST_CASE("Cudd_UnderApprox - Basic functionality", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Simple BDD") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *subset = Cudd_UnderApprox(dd, f, 2, 10, 0, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("With safe=1") {
        DdNode *f = createComplexBDD(dd, 6);
        DdNode *subset = Cudd_UnderApprox(dd, f, 6, 5, 1, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Various quality values") {
        DdNode *f = createComplexBDD(dd, 8);
        for (double q = 0.5; q <= 2.0; q += 0.5) {
            DdNode *subset = Cudd_UnderApprox(dd, f, 8, 5, 0, q);
            REQUIRE(subset != nullptr);
            Cudd_Ref(subset);
            REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
            Cudd_RecursiveDeref(dd, subset);
        }
        Cudd_RecursiveDeref(dd, f);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_UnderApprox - Constants", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);

    SECTION("Constant one") {
        DdNode *result = Cudd_UnderApprox(dd, one, 5, 10, 0, 1.0);
        REQUIRE(result == one);
    }

    SECTION("Constant zero") {
        DdNode *result = Cudd_UnderApprox(dd, zero, 5, 10, 0, 1.0);
        REQUIRE(result == zero);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_UnderApprox - numVars=0", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *f = createSimpleBDD(dd);
    DdNode *subset = Cudd_UnderApprox(dd, f, 0, 10, 0, 1.0);
    REQUIRE(subset != nullptr);
    Cudd_Ref(subset);
    REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
    Cudd_RecursiveDeref(dd, subset);
    Cudd_RecursiveDeref(dd, f);

    Cudd_Quit(dd);
}

// ============== Cudd_OverApprox Tests ==============

TEST_CASE("Cudd_OverApprox - Basic functionality", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Simple BDD") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *superset = Cudd_OverApprox(dd, f, 2, 10, 0, 1.0);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("With safe=1") {
        DdNode *f = createComplexBDD(dd, 6);
        DdNode *superset = Cudd_OverApprox(dd, f, 6, 5, 1, 1.0);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_OverApprox - Constants", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);

    REQUIRE(Cudd_OverApprox(dd, one, 5, 10, 0, 1.0) == one);
    REQUIRE(Cudd_OverApprox(dd, zero, 5, 10, 0, 1.0) == zero);

    Cudd_Quit(dd);
}

// ============== Cudd_RemapUnderApprox Tests ==============

TEST_CASE("Cudd_RemapUnderApprox - Basic functionality", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Simple BDD") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *subset = Cudd_RemapUnderApprox(dd, f, 2, 10, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Complex BDD") {
        DdNode *f = createComplexBDD(dd, 8);
        DdNode *subset = Cudd_RemapUnderApprox(dd, f, 8, 5, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Deep BDD") {
        DdNode *f = createDeepBDD(dd, 10);
        DdNode *subset = Cudd_RemapUnderApprox(dd, f, 10, 5, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Wide BDD") {
        DdNode *f = createWideBDD(dd, 8);
        DdNode *subset = Cudd_RemapUnderApprox(dd, f, 8, 5, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_RemapUnderApprox - Constants", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);

    REQUIRE(Cudd_RemapUnderApprox(dd, one, 5, 10, 1.0) == one);
    REQUIRE(Cudd_RemapUnderApprox(dd, zero, 5, 10, 1.0) == zero);

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_RemapUnderApprox - Various thresholds", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *f = createComplexBDD(dd, 10);
    for (int thresh = 1; thresh <= 20; thresh += 3) {
        DdNode *subset = Cudd_RemapUnderApprox(dd, f, 10, thresh, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
    }
    Cudd_RecursiveDeref(dd, f);

    Cudd_Quit(dd);
}

// ============== Cudd_RemapOverApprox Tests ==============

TEST_CASE("Cudd_RemapOverApprox - Basic functionality", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Simple BDD") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *superset = Cudd_RemapOverApprox(dd, f, 2, 10, 1.0);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Complex BDD") {
        DdNode *f = createComplexBDD(dd, 8);
        DdNode *superset = Cudd_RemapOverApprox(dd, f, 8, 5, 1.0);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_RemapOverApprox - Constants", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);

    REQUIRE(Cudd_RemapOverApprox(dd, one, 5, 10, 1.0) == one);
    REQUIRE(Cudd_RemapOverApprox(dd, zero, 5, 10, 1.0) == zero);

    Cudd_Quit(dd);
}

// ============== Cudd_BiasedUnderApprox Tests ==============

TEST_CASE("Cudd_BiasedUnderApprox - Basic functionality", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Simple BDD with bias=one") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *bias = Cudd_ReadOne(dd);
        DdNode *subset = Cudd_BiasedUnderApprox(dd, f, bias, 2, 10, 1.0, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Simple BDD with bias=zero") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *bias = Cudd_Not(Cudd_ReadOne(dd));
        DdNode *subset = Cudd_BiasedUnderApprox(dd, f, bias, 2, 10, 1.0, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Complex BDD with variable bias") {
        DdNode *f = createComplexBDD(dd, 8);
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *subset = Cudd_BiasedUnderApprox(dd, f, x0, 8, 5, 1.0, 0.5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Different quality1 and quality0") {
        DdNode *f = createComplexBDD(dd, 6);
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *subset = Cudd_BiasedUnderApprox(dd, f, x0, 6, 5, 2.0, 0.5);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
        Cudd_RecursiveDeref(dd, f);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_BiasedUnderApprox - Constants", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);

    REQUIRE(Cudd_BiasedUnderApprox(dd, one, one, 5, 10, 1.0, 1.0) == one);
    REQUIRE(Cudd_BiasedUnderApprox(dd, zero, one, 5, 10, 1.0, 1.0) == zero);

    Cudd_Quit(dd);
}

// ============== Cudd_BiasedOverApprox Tests ==============

TEST_CASE("Cudd_BiasedOverApprox - Basic functionality", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Simple BDD with bias=one") {
        DdNode *f = createSimpleBDD(dd);
        DdNode *bias = Cudd_ReadOne(dd);
        DdNode *superset = Cudd_BiasedOverApprox(dd, f, bias, 2, 10, 1.0, 1.0);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }

    SECTION("Complex BDD") {
        DdNode *f = createComplexBDD(dd, 8);
        DdNode *x0 = Cudd_bddIthVar(dd, 0);
        DdNode *superset = Cudd_BiasedOverApprox(dd, f, x0, 8, 5, 1.0, 0.5);
        REQUIRE(superset != nullptr);
        Cudd_Ref(superset);
        REQUIRE(Cudd_bddLeq(dd, f, superset) == 1);
        Cudd_RecursiveDeref(dd, superset);
        Cudd_RecursiveDeref(dd, f);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_BiasedOverApprox - Constants", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *one = Cudd_ReadOne(dd);
    DdNode *zero = Cudd_Not(one);

    REQUIRE(Cudd_BiasedOverApprox(dd, one, one, 5, 10, 1.0, 1.0) == one);
    REQUIRE(Cudd_BiasedOverApprox(dd, zero, one, 5, 10, 1.0, 1.0) == zero);

    Cudd_Quit(dd);
}

// ============== Edge Cases and Complex Structures ==============

TEST_CASE("cuddApprox - Complemented BDDs", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *f = createSimpleBDD(dd);
    DdNode *notF = Cudd_Not(f);
    Cudd_Ref(notF);

    SECTION("UnderApprox on complemented") {
        DdNode *subset = Cudd_UnderApprox(dd, notF, 2, 10, 0, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, notF) == 1);
        Cudd_RecursiveDeref(dd, subset);
    }

    SECTION("RemapUnderApprox on complemented") {
        DdNode *subset = Cudd_RemapUnderApprox(dd, notF, 2, 10, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, notF) == 1);
        Cudd_RecursiveDeref(dd, subset);
    }

    Cudd_RecursiveDeref(dd, notF);
    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - XOR structures", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *f = Cudd_bddXor(dd, x0, x1);
    Cudd_Ref(f);

    DdNode *subset = Cudd_UnderApprox(dd, f, 2, 10, 0, 1.0);
    REQUIRE(subset != nullptr);
    Cudd_Ref(subset);
    REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
    Cudd_RecursiveDeref(dd, subset);

    DdNode *remapSubset = Cudd_RemapUnderApprox(dd, f, 2, 10, 1.0);
    REQUIRE(remapSubset != nullptr);
    Cudd_Ref(remapSubset);
    REQUIRE(Cudd_bddLeq(dd, remapSubset, f) == 1);
    Cudd_RecursiveDeref(dd, remapSubset);

    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - Small threshold forces approximation", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *f = createComplexBDD(dd, 12);

    SECTION("UnderApprox with threshold=1") {
        DdNode *subset = Cudd_UnderApprox(dd, f, 12, 1, 0, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
    }

    SECTION("RemapUnderApprox with threshold=1") {
        DdNode *subset = Cudd_RemapUnderApprox(dd, f, 12, 1, 1.0);
        REQUIRE(subset != nullptr);
        Cudd_Ref(subset);
        REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);
        Cudd_RecursiveDeref(dd, subset);
    }

    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - Large BDD for internal function coverage", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    // Build a larger BDD to exercise more code paths
    DdNode *f = Cudd_ReadOne(dd);
    Cudd_Ref(f);

    for (int i = 0; i < 15; i++) {
        DdNode *xi = Cudd_bddIthVar(dd, i);
        DdNode *newF;
        if (i % 3 == 0) {
            newF = Cudd_bddAnd(dd, f, xi);
        } else if (i % 3 == 1) {
            newF = Cudd_bddOr(dd, f, xi);
        } else {
            newF = Cudd_bddXor(dd, f, xi);
        }
        Cudd_Ref(newF);
        Cudd_RecursiveDeref(dd, f);
        f = newF;
    }

    DdNode *subset1 = Cudd_UnderApprox(dd, f, 15, 10, 0, 1.0);
    REQUIRE(subset1 != nullptr);
    Cudd_Ref(subset1);
    REQUIRE(Cudd_bddLeq(dd, subset1, f) == 1);
    Cudd_RecursiveDeref(dd, subset1);

    DdNode *subset2 = Cudd_RemapUnderApprox(dd, f, 15, 10, 1.0);
    REQUIRE(subset2 != nullptr);
    Cudd_Ref(subset2);
    REQUIRE(Cudd_bddLeq(dd, subset2, f) == 1);
    Cudd_RecursiveDeref(dd, subset2);

    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *subset3 = Cudd_BiasedUnderApprox(dd, f, x0, 15, 10, 1.0, 0.5);
    REQUIRE(subset3 != nullptr);
    Cudd_Ref(subset3);
    REQUIRE(Cudd_bddLeq(dd, subset3, f) == 1);
    Cudd_RecursiveDeref(dd, subset3);

    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - Shared structure coverage", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    // Create BDD with shared substructure
    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *x2 = Cudd_bddIthVar(dd, 2);
    DdNode *x3 = Cudd_bddIthVar(dd, 3);

    DdNode *shared = Cudd_bddAnd(dd, x2, x3);
    Cudd_Ref(shared);

    DdNode *t1 = Cudd_bddAnd(dd, x0, shared);
    Cudd_Ref(t1);
    DdNode *t2 = Cudd_bddAnd(dd, x1, shared);
    Cudd_Ref(t2);

    DdNode *f = Cudd_bddOr(dd, t1, t2);
    Cudd_Ref(f);

    DdNode *subset = Cudd_RemapUnderApprox(dd, f, 4, 3, 1.0);
    REQUIRE(subset != nullptr);
    Cudd_Ref(subset);
    REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);

    Cudd_RecursiveDeref(dd, subset);
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, t1);
    Cudd_RecursiveDeref(dd, t2);
    Cudd_RecursiveDeref(dd, shared);

    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - ITE structures for REPLACE_TT/REPLACE_TE", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *x2 = Cudd_bddIthVar(dd, 2);
    DdNode *x3 = Cudd_bddIthVar(dd, 3);

    // Create structure where T and E share grandchildren
    DdNode *inner = Cudd_bddAnd(dd, x2, x3);
    Cudd_Ref(inner);
    
    DdNode *tBranch = Cudd_bddOr(dd, x1, inner);
    Cudd_Ref(tBranch);
    DdNode *eBranch = Cudd_bddAnd(dd, x1, inner);
    Cudd_Ref(eBranch);

    DdNode *f = Cudd_bddIte(dd, x0, tBranch, eBranch);
    Cudd_Ref(f);

    DdNode *subset = Cudd_RemapUnderApprox(dd, f, 4, 3, 1.0);
    REQUIRE(subset != nullptr);
    Cudd_Ref(subset);
    REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);

    Cudd_RecursiveDeref(dd, subset);
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, tBranch);
    Cudd_RecursiveDeref(dd, eBranch);
    Cudd_RecursiveDeref(dd, inner);

    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - Parity coverage", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    // Create BDD where nodes have both parities
    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *x2 = Cudd_bddIthVar(dd, 2);

    DdNode *t1 = Cudd_bddAnd(dd, x0, x1);
    Cudd_Ref(t1);
    DdNode *t2 = Cudd_bddXor(dd, t1, x2);
    Cudd_Ref(t2);

    DdNode *subset = Cudd_UnderApprox(dd, t2, 3, 3, 1, 1.0);
    REQUIRE(subset != nullptr);
    Cudd_Ref(subset);
    REQUIRE(Cudd_bddLeq(dd, subset, t2) == 1);

    Cudd_RecursiveDeref(dd, subset);
    Cudd_RecursiveDeref(dd, t2);
    Cudd_RecursiveDeref(dd, t1);

    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - Leq relationship coverage", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    // Create T <= E structure
    DdNode *x0 = Cudd_bddIthVar(dd, 0);
    DdNode *x1 = Cudd_bddIthVar(dd, 1);
    DdNode *x2 = Cudd_bddIthVar(dd, 2);

    DdNode *tBranch = Cudd_bddAnd(dd, x1, x2);
    Cudd_Ref(tBranch);
    DdNode *eBranch = Cudd_bddOr(dd, x1, x2);
    Cudd_Ref(eBranch);

    DdNode *f = Cudd_bddIte(dd, x0, tBranch, eBranch);
    Cudd_Ref(f);

    DdNode *subset = Cudd_RemapUnderApprox(dd, f, 3, 3, 1.0);
    REQUIRE(subset != nullptr);
    Cudd_Ref(subset);
    REQUIRE(Cudd_bddLeq(dd, subset, f) == 1);

    Cudd_RecursiveDeref(dd, subset);
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, tBranch);
    Cudd_RecursiveDeref(dd, eBranch);

    Cudd_Quit(dd);
}

TEST_CASE("cuddApprox - Quality parameter effects", "[cuddApprox]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    DdNode *f = createComplexBDD(dd, 10);

    // Very low quality - more aggressive approximation
    DdNode *subset1 = Cudd_UnderApprox(dd, f, 10, 5, 0, 0.1);
    REQUIRE(subset1 != nullptr);
    Cudd_Ref(subset1);
    REQUIRE(Cudd_bddLeq(dd, subset1, f) == 1);

    // High quality - more conservative
    DdNode *subset2 = Cudd_UnderApprox(dd, f, 10, 5, 0, 5.0);
    REQUIRE(subset2 != nullptr);
    Cudd_Ref(subset2);
    REQUIRE(Cudd_bddLeq(dd, subset2, f) == 1);

    Cudd_RecursiveDeref(dd, subset1);
    Cudd_RecursiveDeref(dd, subset2);
    Cudd_RecursiveDeref(dd, f);

    Cudd_Quit(dd);
}
