#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddZddPort.c
 *
 * Comprehensive tests for BDD to ZDD and ZDD to BDD conversion functions.
 * Target: 90% code coverage for cuddZddPort.c
 */

// ============================================================================
// TESTS FOR Cudd_zddPortFromBdd
// ============================================================================

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with constant false", "[cuddZddPort]") {
    // Test terminal case: B is logical false (complemented one)
    // Covers line 185-186: if (B == Cudd_Not(DD_ONE(dd))) return(DD_ZERO(dd));
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get logical false (complement of one)
    DdNode* bddFalse = Cudd_Not(Cudd_ReadOne(manager));

    // Convert BDD false to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddFalse);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    // Result should be ZDD zero
    DdNode* zddZero = Cudd_ReadZero(manager);
    REQUIRE(zddResult == zddZero);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with constant true and expected >= sizeZ", "[cuddZddPort]") {
    // Test terminal case: B is DD_ONE and expected >= dd->sizeZ
    // Covers lines 187-189
    DdManager* manager = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Get logical true
    DdNode* bddTrue = Cudd_ReadOne(manager);

    // Convert BDD true to ZDD (no ZDD variables exist)
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddTrue);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    // Result should be ZDD one
    REQUIRE(Cudd_IsConstant(zddResult));

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with constant true and expected < sizeZ", "[cuddZddPort]") {
    // Test terminal case: B is DD_ONE and expected < dd->sizeZ
    // Covers lines 190-191: return(dd->univ[expected]);
    DdManager* manager = Cudd_Init(2, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get logical true
    DdNode* bddTrue = Cudd_ReadOne(manager);

    // Convert BDD true to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddTrue);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with single variable", "[cuddZddPort]") {
    // Test with a single BDD variable
    // Covers the main recursive path
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variable x0
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    REQUIRE(x0 != nullptr);
    Cudd_Ref(x0);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, x0);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with complemented BDD", "[cuddZddPort]") {
    // Test with complemented BDD
    // Covers lines 221-223: if (Cudd_IsComplement(B))
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variable x0 and complement it
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    REQUIRE(x0 != nullptr);
    Cudd_Ref(x0);

    DdNode* notX0 = Cudd_Not(x0);
    Cudd_Ref(notX0);

    // Convert complemented BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, notX0);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, notX0);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with non-complemented BDD", "[cuddZddPort]") {
    // Test with non-complemented BDD (regular)
    // Covers lines 224-226: else branch (Bt = cuddT(Breg); Be = cuddE(Breg))
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variable x0 (already non-complemented)
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    REQUIRE(x0 != nullptr);
    Cudd_Ref(x0);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, x0);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with AND of two variables", "[cuddZddPort]") {
    // Test with AND of two BDD variables
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variables x0 and x1
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    REQUIRE(x0 != nullptr);
    REQUIRE(x1 != nullptr);

    // Create AND of x0 and x1
    DdNode* bddAnd = Cudd_bddAnd(manager, x0, x1);
    REQUIRE(bddAnd != nullptr);
    Cudd_Ref(bddAnd);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddAnd);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddAnd);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with OR of two variables", "[cuddZddPort]") {
    // Test with OR of two BDD variables
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variables x0 and x1
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    REQUIRE(x0 != nullptr);
    REQUIRE(x1 != nullptr);

    // Create OR of x0 and x1
    DdNode* bddOr = Cudd_bddOr(manager, x0, x1);
    REQUIRE(bddOr != nullptr);
    Cudd_Ref(bddOr);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddOr);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddOr);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd cache hit test", "[cuddZddPort]") {
    // Test cache hit path
    // Covers lines 198-219: cache lookup and adding suppressed variables
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variable x0
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    REQUIRE(x0 != nullptr);
    Cudd_Ref(x0);

    // First conversion - will cache the result
    DdNode* zddResult1 = Cudd_zddPortFromBdd(manager, x0);
    REQUIRE(zddResult1 != nullptr);
    Cudd_Ref(zddResult1);

    // Second conversion - should hit the cache
    DdNode* zddResult2 = Cudd_zddPortFromBdd(manager, x0);
    REQUIRE(zddResult2 != nullptr);
    Cudd_Ref(zddResult2);

    // Both results should be the same
    REQUIRE(zddResult1 == zddResult2);

    Cudd_RecursiveDerefZdd(manager, zddResult1);
    Cudd_RecursiveDerefZdd(manager, zddResult2);
    Cudd_RecursiveDeref(manager, x0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with multiple variables", "[cuddZddPort]") {
    // Test with multiple BDD variables to exercise suppressed variables path
    DdManager* manager = Cudd_Init(8, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get BDD variable x4 (has gap before it)
    DdNode* x4 = Cudd_bddIthVar(manager, 4);
    REQUIRE(x4 != nullptr);
    Cudd_Ref(x4);

    // Convert BDD to ZDD - should add suppressed variables
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, x4);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, x4);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with complex BDD", "[cuddZddPort]") {
    // Test with a more complex BDD (x0 AND x1) OR (x2 AND x3)
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* x2 = Cudd_bddIthVar(manager, 2);
    DdNode* x3 = Cudd_bddIthVar(manager, 3);

    // Build (x0 AND x1) OR (x2 AND x3)
    DdNode* and1 = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(and1);
    DdNode* and2 = Cudd_bddAnd(manager, x2, x3);
    Cudd_Ref(and2);
    DdNode* bddOr = Cudd_bddOr(manager, and1, and2);
    Cudd_Ref(bddOr);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddOr);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddOr);
    Cudd_RecursiveDeref(manager, and2);
    Cudd_RecursiveDeref(manager, and1);
    Cudd_Quit(manager);
}

// ============================================================================
// TESTS FOR Cudd_zddPortToBdd
// ============================================================================

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with ZDD zero", "[cuddZddPort]") {
    // Test terminal case: f == zero
    // Covers line 289: if (f == zero) return(Cudd_Not(one));
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD zero
    DdNode* zddZero = Cudd_ReadZero(manager);
    Cudd_Ref(zddZero);

    // Convert ZDD zero to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zddZero);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    // Result should be BDD false (complement of one)
    DdNode* bddFalse = Cudd_Not(Cudd_ReadOne(manager));
    REQUIRE(bddResult == bddFalse);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zddZero);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with depth == sizeZ", "[cuddZddPort]") {
    // Test terminal case: depth == dd->sizeZ
    // Covers line 291: if (depth == dd->sizeZ) return(one);
    DdManager* manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Get ZDD one (empty set tautology)
    DdNode* zddOne = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(zddOne);

    // Convert ZDD one to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zddOne);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    // Result should be BDD true
    DdNode* bddTrue = Cudd_ReadOne(manager);
    REQUIRE(bddResult == bddTrue);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zddOne);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with single ZDD variable", "[cuddZddPort]") {
    // Test with a single ZDD variable
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD variable z0
    DdNode* z0 = Cudd_zddIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);

    // Convert ZDD to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, z0);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with level > depth path", "[cuddZddPort]") {
    // Test the level > depth path (variable is missing from ZDD)
    // Covers lines 299-317
    DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD variable z2 (index 2, higher than 0)
    DdNode* z2 = Cudd_zddIthVar(manager, 2);
    REQUIRE(z2 != nullptr);
    Cudd_Ref(z2);

    // Convert ZDD to BDD - should exercise level > depth path
    DdNode* bddResult = Cudd_zddPortToBdd(manager, z2);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, z2);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd cache hit test", "[cuddZddPort]") {
    // Test cache hit path
    // Covers lines 319-323
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD variable z0
    DdNode* z0 = Cudd_zddIthVar(manager, 0);
    REQUIRE(z0 != nullptr);
    Cudd_Ref(z0);

    // First conversion - will cache the result
    DdNode* bddResult1 = Cudd_zddPortToBdd(manager, z0);
    REQUIRE(bddResult1 != nullptr);
    Cudd_Ref(bddResult1);

    // Second conversion - should hit the cache
    DdNode* bddResult2 = Cudd_zddPortToBdd(manager, z0);
    REQUIRE(bddResult2 != nullptr);
    Cudd_Ref(bddResult2);

    // Both results should be the same
    REQUIRE(bddResult1 == bddResult2);

    Cudd_RecursiveDeref(manager, bddResult1);
    Cudd_RecursiveDeref(manager, bddResult2);
    Cudd_RecursiveDerefZdd(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with union of ZDD variables", "[cuddZddPort]") {
    // Test with union of two ZDD variables
    // Covers lines 325-354: full recursive computation
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD variables
    DdNode* z0 = Cudd_zddIthVar(manager, 0);
    Cudd_Ref(z0);
    DdNode* z1 = Cudd_zddIthVar(manager, 1);
    Cudd_Ref(z1);

    // Create union of z0 and z1
    DdNode* zddUnion = Cudd_zddUnion(manager, z0, z1);
    REQUIRE(zddUnion != nullptr);
    Cudd_Ref(zddUnion);

    // Convert ZDD to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zddUnion);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zddUnion);
    Cudd_RecursiveDerefZdd(manager, z0);
    Cudd_RecursiveDerefZdd(manager, z1);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with product of ZDD variables", "[cuddZddPort]") {
    // Test with product of two ZDD variables
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD variables
    DdNode* z0 = Cudd_zddIthVar(manager, 0);
    Cudd_Ref(z0);
    DdNode* z1 = Cudd_zddIthVar(manager, 1);
    Cudd_Ref(z1);

    // Create product of z0 and z1
    DdNode* zddProduct = Cudd_zddProduct(manager, z0, z1);
    REQUIRE(zddProduct != nullptr);
    Cudd_Ref(zddProduct);

    // Convert ZDD to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zddProduct);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zddProduct);
    Cudd_RecursiveDerefZdd(manager, z0);
    Cudd_RecursiveDerefZdd(manager, z1);
    Cudd_Quit(manager);
}

// ============================================================================
// ROUND-TRIP TESTS (BDD -> ZDD -> BDD and ZDD -> BDD -> ZDD)
// ============================================================================

TEST_CASE("cuddZddPort - Round-trip BDD to ZDD to BDD", "[cuddZddPort]") {
    // Test that converting BDD -> ZDD -> BDD gives back the original
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Create a BDD: x0 AND x1
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* bddOriginal = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(bddOriginal);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddOriginal);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    // Convert ZDD back to BDD
    DdNode* bddRoundTrip = Cudd_zddPortToBdd(manager, zddResult);
    REQUIRE(bddRoundTrip != nullptr);
    Cudd_Ref(bddRoundTrip);

    // The round-trip BDD should be equivalent to the original
    REQUIRE(bddRoundTrip == bddOriginal);

    Cudd_RecursiveDeref(manager, bddRoundTrip);
    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddOriginal);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Round-trip with complex BDD", "[cuddZddPort]") {
    // Test round-trip with a more complex BDD
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Create BDD: x0 OR x1
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* bddOriginal = Cudd_bddOr(manager, x0, x1);
    Cudd_Ref(bddOriginal);

    // Convert BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddOriginal);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    // Convert ZDD back to BDD
    DdNode* bddRoundTrip = Cudd_zddPortToBdd(manager, zddResult);
    REQUIRE(bddRoundTrip != nullptr);
    Cudd_Ref(bddRoundTrip);

    // The round-trip BDD should be equivalent to the original
    REQUIRE(bddRoundTrip == bddOriginal);

    Cudd_RecursiveDeref(manager, bddRoundTrip);
    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddOriginal);
    Cudd_Quit(manager);
}

// ============================================================================
// ADDITIONAL TESTS FOR HIGHER COVERAGE
// ============================================================================

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with XOR of variables", "[cuddZddPort]") {
    // Test with XOR which creates a more complex structure
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);

    // Create XOR
    DdNode* bddXor = Cudd_bddXor(manager, x0, x1);
    Cudd_Ref(bddXor);

    // Convert to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddXor);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddXor);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd repeated conversions", "[cuddZddPort]") {
    // Test multiple conversions with different BDDs to exercise caching
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* x2 = Cudd_bddIthVar(manager, 2);

    // Multiple conversions
    DdNode* bdd1 = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(bdd1);
    DdNode* zdd1 = Cudd_zddPortFromBdd(manager, bdd1);
    Cudd_Ref(zdd1);

    DdNode* bdd2 = Cudd_bddAnd(manager, x1, x2);
    Cudd_Ref(bdd2);
    DdNode* zdd2 = Cudd_zddPortFromBdd(manager, bdd2);
    Cudd_Ref(zdd2);

    DdNode* bdd3 = Cudd_bddOr(manager, bdd1, bdd2);
    Cudd_Ref(bdd3);
    DdNode* zdd3 = Cudd_zddPortFromBdd(manager, bdd3);
    Cudd_Ref(zdd3);

    // All conversions should succeed
    REQUIRE(zdd1 != nullptr);
    REQUIRE(zdd2 != nullptr);
    REQUIRE(zdd3 != nullptr);

    Cudd_RecursiveDerefZdd(manager, zdd3);
    Cudd_RecursiveDerefZdd(manager, zdd2);
    Cudd_RecursiveDerefZdd(manager, zdd1);
    Cudd_RecursiveDeref(manager, bdd3);
    Cudd_RecursiveDeref(manager, bdd2);
    Cudd_RecursiveDeref(manager, bdd1);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with complex ZDD", "[cuddZddPort]") {
    // Test with a more complex ZDD structure
    DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Create complex ZDD: (z0 | z1) | (z2 | z3)
    DdNode* z0 = Cudd_zddIthVar(manager, 0);
    Cudd_Ref(z0);
    DdNode* z1 = Cudd_zddIthVar(manager, 1);
    Cudd_Ref(z1);
    DdNode* z2 = Cudd_zddIthVar(manager, 2);
    Cudd_Ref(z2);
    DdNode* z3 = Cudd_zddIthVar(manager, 3);
    Cudd_Ref(z3);

    DdNode* union1 = Cudd_zddUnion(manager, z0, z1);
    Cudd_Ref(union1);
    DdNode* union2 = Cudd_zddUnion(manager, z2, z3);
    Cudd_Ref(union2);
    DdNode* zddComplex = Cudd_zddUnion(manager, union1, union2);
    Cudd_Ref(zddComplex);

    // Convert ZDD to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zddComplex);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zddComplex);
    Cudd_RecursiveDerefZdd(manager, union2);
    Cudd_RecursiveDerefZdd(manager, union1);
    Cudd_RecursiveDerefZdd(manager, z3);
    Cudd_RecursiveDerefZdd(manager, z2);
    Cudd_RecursiveDerefZdd(manager, z1);
    Cudd_RecursiveDerefZdd(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with complemented complex BDD", "[cuddZddPort]") {
    // Test with a complemented complex BDD
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);

    // Create AND and then complement it
    DdNode* bddAnd = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(bddAnd);
    DdNode* bddNand = Cudd_Not(bddAnd);
    Cudd_Ref(bddNand);

    // Convert complemented BDD to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bddNand);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bddNand);
    Cudd_RecursiveDeref(manager, bddAnd);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd repeated conversions", "[cuddZddPort]") {
    // Test multiple ZDD to BDD conversions
    DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* z0 = Cudd_zddIthVar(manager, 0);
    Cudd_Ref(z0);
    DdNode* z1 = Cudd_zddIthVar(manager, 1);
    Cudd_Ref(z1);
    DdNode* z2 = Cudd_zddIthVar(manager, 2);
    Cudd_Ref(z2);

    // Multiple conversions
    DdNode* bdd1 = Cudd_zddPortToBdd(manager, z0);
    Cudd_Ref(bdd1);
    DdNode* bdd2 = Cudd_zddPortToBdd(manager, z1);
    Cudd_Ref(bdd2);
    DdNode* bdd3 = Cudd_zddPortToBdd(manager, z2);
    Cudd_Ref(bdd3);

    REQUIRE(bdd1 != nullptr);
    REQUIRE(bdd2 != nullptr);
    REQUIRE(bdd3 != nullptr);

    Cudd_RecursiveDeref(manager, bdd3);
    Cudd_RecursiveDeref(manager, bdd2);
    Cudd_RecursiveDeref(manager, bdd1);
    Cudd_RecursiveDerefZdd(manager, z2);
    Cudd_RecursiveDerefZdd(manager, z1);
    Cudd_RecursiveDerefZdd(manager, z0);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Test suppressed variable loop in zddPortFromBddStep", "[cuddZddPort]") {
    // Test to exercise the loop for adding suppressed variables (lines 252-262)
    DdManager* manager = Cudd_Init(8, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Create a BDD that skips some variables
    DdNode* x3 = Cudd_bddIthVar(manager, 3);
    DdNode* x5 = Cudd_bddIthVar(manager, 5);

    DdNode* bdd = Cudd_bddAnd(manager, x3, x5);
    Cudd_Ref(bdd);

    // Convert to ZDD - should exercise the suppressed variable addition
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bdd);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bdd);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Test deep recursion in zddPortToBddStep", "[cuddZddPort]") {
    // Test with multiple variables to exercise deeper recursion
    DdManager* manager = Cudd_Init(8, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Create ZDD with gaps between variables
    DdNode* z1 = Cudd_zddIthVar(manager, 1);
    Cudd_Ref(z1);
    DdNode* z4 = Cudd_zddIthVar(manager, 4);
    Cudd_Ref(z4);
    DdNode* z7 = Cudd_zddIthVar(manager, 7);
    Cudd_Ref(z7);

    // Create union of these variables
    DdNode* tmp = Cudd_zddUnion(manager, z1, z4);
    Cudd_Ref(tmp);
    DdNode* zdd = Cudd_zddUnion(manager, tmp, z7);
    Cudd_Ref(zdd);

    // Convert to BDD - should exercise level > depth paths
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zdd);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zdd);
    Cudd_RecursiveDerefZdd(manager, tmp);
    Cudd_RecursiveDerefZdd(manager, z7);
    Cudd_RecursiveDerefZdd(manager, z4);
    Cudd_RecursiveDerefZdd(manager, z1);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Edge case with ZDD one in zddPortToBdd", "[cuddZddPort]") {
    // Test converting ZDD one
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Get ZDD one (empty set)
    DdNode* zddOne = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(zddOne);

    // Convert to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zddOne);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, zddOne);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Round-trip with constants", "[cuddZddPort]") {
    // Test round-trip with constant functions
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Test with constant true
    DdNode* bddTrue = Cudd_ReadOne(manager);
    DdNode* zddFromTrue = Cudd_zddPortFromBdd(manager, bddTrue);
    REQUIRE(zddFromTrue != nullptr);
    Cudd_Ref(zddFromTrue);

    DdNode* bddBack = Cudd_zddPortToBdd(manager, zddFromTrue);
    REQUIRE(bddBack != nullptr);
    Cudd_Ref(bddBack);
    REQUIRE(bddBack == bddTrue);

    Cudd_RecursiveDeref(manager, bddBack);
    Cudd_RecursiveDerefZdd(manager, zddFromTrue);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Round-trip with constant false", "[cuddZddPort]") {
    // Test round-trip with constant false
    DdManager* manager = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Test with constant false
    DdNode* bddFalse = Cudd_Not(Cudd_ReadOne(manager));
    DdNode* zddFromFalse = Cudd_zddPortFromBdd(manager, bddFalse);
    REQUIRE(zddFromFalse != nullptr);
    Cudd_Ref(zddFromFalse);

    DdNode* bddBack = Cudd_zddPortToBdd(manager, zddFromFalse);
    REQUIRE(bddBack != nullptr);
    Cudd_Ref(bddBack);
    REQUIRE(bddBack == bddFalse);

    Cudd_RecursiveDeref(manager, bddBack);
    Cudd_RecursiveDerefZdd(manager, zddFromFalse);
    Cudd_Quit(manager);
}

// ============================================================================
// TESTS FOR level > depth PATH IN zddPortToBddStep
// ============================================================================

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with raw ZDD node (level > depth)", "[cuddZddPort]") {
    // This test creates a ZDD node directly without filler nodes
    // to exercise the level > depth path in zddPortToBddStep (lines 299-316)
    DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    // Create ZDD variables from BDD variables
    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Create a ZDD node directly at index 2 without filler nodes
    // This represents a set containing just variable 2
    DdNode* zddOne = DD_ONE(manager);
    DdNode* zddZero = DD_ZERO(manager);

    // Create node at index 2 (level 2) directly
    DdNode* raw_z2 = cuddUniqueInterZdd(manager, 2, zddOne, zddZero);
    REQUIRE(raw_z2 != nullptr);
    Cudd_Ref(raw_z2);

    // Verify the node has the expected properties
    REQUIRE(raw_z2->index == 2);
    REQUIRE(cuddIZ(manager, raw_z2->index) == 2);

    // Convert ZDD to BDD - at depth=0, level=2 > depth=0
    // This should trigger the level > depth path
    DdNode* bddResult = Cudd_zddPortToBdd(manager, raw_z2);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, raw_z2);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with raw ZDD node at higher level", "[cuddZddPort]") {
    // Create a ZDD node at a higher level (4) to exercise deeper recursion
    DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* zddOne = DD_ONE(manager);
    DdNode* zddZero = DD_ZERO(manager);

    // Create node at index 4 (level 4) directly
    DdNode* raw_z4 = cuddUniqueInterZdd(manager, 4, zddOne, zddZero);
    REQUIRE(raw_z4 != nullptr);
    Cudd_Ref(raw_z4);

    // Convert ZDD to BDD - should hit level > depth multiple times
    // depth=0: level=4 > 0, add !x0 
    // depth=1: level=4 > 1, add !x1
    // depth=2: level=4 > 2, add !x2
    // depth=3: level=4 > 3, add !x3
    // depth=4: level=4 == 4, process normally
    DdNode* bddResult = Cudd_zddPortToBdd(manager, raw_z4);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, raw_z4);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with raw ZDD union (sparse structure)", "[cuddZddPort]") {
    // Create a ZDD union of raw nodes at different levels
    DdManager* manager = Cudd_Init(8, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* zddOne = DD_ONE(manager);
    DdNode* zddZero = DD_ZERO(manager);

    // Create raw nodes at indices 2 and 5
    DdNode* raw_z2 = cuddUniqueInterZdd(manager, 2, zddOne, zddZero);
    REQUIRE(raw_z2 != nullptr);
    Cudd_Ref(raw_z2);

    DdNode* raw_z5 = cuddUniqueInterZdd(manager, 5, zddOne, zddZero);
    REQUIRE(raw_z5 != nullptr);
    Cudd_Ref(raw_z5);

    // Create a node that combines them (z2 as then child, z5 as else child)
    // This is at index 0 but has sparse children
    DdNode* combined = cuddUniqueInterZdd(manager, 0, raw_z2, raw_z5);
    REQUIRE(combined != nullptr);
    Cudd_Ref(combined);

    // Convert to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, combined);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, combined);
    Cudd_RecursiveDerefZdd(manager, raw_z5);
    Cudd_RecursiveDerefZdd(manager, raw_z2);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with raw ZDD sparse children", "[cuddZddPort]") {
    // Create a ZDD where children are at non-consecutive levels
    DdManager* manager = Cudd_Init(8, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* zddOne = DD_ONE(manager);
    DdNode* zddZero = DD_ZERO(manager);

    // Create a node at index 3
    DdNode* raw_z3 = cuddUniqueInterZdd(manager, 3, zddOne, zddZero);
    REQUIRE(raw_z3 != nullptr);
    Cudd_Ref(raw_z3);

    // Create a node at index 1 with z3 as then child and zero as else
    // This creates a sparse structure where recursing on then child
    // goes from level 1 directly to level 3
    DdNode* node_1 = cuddUniqueInterZdd(manager, 1, raw_z3, zddZero);
    REQUIRE(node_1 != nullptr);
    Cudd_Ref(node_1);

    // Convert to BDD
    DdNode* bddResult = Cudd_zddPortToBdd(manager, node_1);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, node_1);
    Cudd_RecursiveDerefZdd(manager, raw_z3);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with multiple sparse levels", "[cuddZddPort]") {
    // Create a ZDD chain with multiple level gaps
    DdManager* manager = Cudd_Init(8, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* zddOne = DD_ONE(manager);
    DdNode* zddZero = DD_ZERO(manager);

    // Create chain: node at 6 -> node at 3 -> one
    DdNode* raw_z6 = cuddUniqueInterZdd(manager, 6, zddOne, zddZero);
    REQUIRE(raw_z6 != nullptr);
    Cudd_Ref(raw_z6);

    DdNode* node_3 = cuddUniqueInterZdd(manager, 3, raw_z6, zddZero);
    REQUIRE(node_3 != nullptr);
    Cudd_Ref(node_3);

    DdNode* node_0 = cuddUniqueInterZdd(manager, 0, node_3, zddZero);
    REQUIRE(node_0 != nullptr);
    Cudd_Ref(node_0);

    // Convert to BDD - should hit level > depth in children
    DdNode* bddResult = Cudd_zddPortToBdd(manager, node_0);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, node_0);
    Cudd_RecursiveDerefZdd(manager, node_3);
    Cudd_RecursiveDerefZdd(manager, raw_z6);
    Cudd_Quit(manager);
}

// ============================================================================
// TESTS FOR ERROR HANDLING PATHS (attempting to trigger memory allocation failures)
// ============================================================================

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with memory constraints", "[cuddZddPort]") {
    // Try to trigger memory allocation failures by setting strict memory limits
    DdManager* manager = Cudd_Init(16, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Set a restrictive memory limit
    Cudd_SetMaxMemory(manager, 512 * 1024); // 512KB limit

    // Create BDD variables and build a complex BDD
    DdNode* bdd = Cudd_ReadOne(manager);
    Cudd_Ref(bdd);

    for (int i = 0; i < 12; i++) {
        DdNode* var = Cudd_bddIthVar(manager, i);
        if (var == nullptr) break;
        
        DdNode* tmp = Cudd_bddOr(manager, bdd, var);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDeref(manager, bdd);
            bdd = tmp;
        }
    }

    // Try to convert - might succeed or fail due to memory constraints
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bdd);
    if (zddResult != nullptr) {
        Cudd_Ref(zddResult);
        Cudd_RecursiveDerefZdd(manager, zddResult);
    }

    Cudd_RecursiveDeref(manager, bdd);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with memory constraints", "[cuddZddPort]") {
    // Try to trigger memory allocation failures by setting strict memory limits
    DdManager* manager = Cudd_Init(16, 16, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Set a restrictive memory limit
    Cudd_SetMaxMemory(manager, 512 * 1024); // 512KB limit

    // Create a complex ZDD
    DdNode* zdd = Cudd_ReadZddOne(manager, 0);
    Cudd_Ref(zdd);

    for (int i = 0; i < 12; i++) {
        DdNode* var = Cudd_zddIthVar(manager, i);
        if (var == nullptr) break;
        Cudd_Ref(var);
        
        DdNode* tmp = Cudd_zddUnion(manager, zdd, var);
        if (tmp != nullptr) {
            Cudd_Ref(tmp);
            Cudd_RecursiveDerefZdd(manager, zdd);
            zdd = tmp;
        }
        Cudd_RecursiveDerefZdd(manager, var);
    }

    // Try to convert - might succeed or fail due to memory constraints
    DdNode* bddResult = Cudd_zddPortToBdd(manager, zdd);
    if (bddResult != nullptr) {
        Cudd_Ref(bddResult);
        Cudd_RecursiveDeref(manager, bddResult);
    }

    Cudd_RecursiveDerefZdd(manager, zdd);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortFromBdd with very large BDD", "[cuddZddPort]") {
    // Create a very large BDD to exercise cache and memory paths
    DdManager* manager = Cudd_Init(20, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    // Build a complex BDD with many variables
    DdNode* x0 = Cudd_bddIthVar(manager, 0);
    DdNode* x1 = Cudd_bddIthVar(manager, 1);
    DdNode* x2 = Cudd_bddIthVar(manager, 2);
    DdNode* x3 = Cudd_bddIthVar(manager, 3);
    DdNode* x4 = Cudd_bddIthVar(manager, 4);
    DdNode* x5 = Cudd_bddIthVar(manager, 5);
    DdNode* x6 = Cudd_bddIthVar(manager, 6);
    DdNode* x7 = Cudd_bddIthVar(manager, 7);

    // Build complex expression: (x0 & x1) | (x2 & x3) | (x4 & x5) | (x6 & x7)
    DdNode* and1 = Cudd_bddAnd(manager, x0, x1);
    Cudd_Ref(and1);
    DdNode* and2 = Cudd_bddAnd(manager, x2, x3);
    Cudd_Ref(and2);
    DdNode* and3 = Cudd_bddAnd(manager, x4, x5);
    Cudd_Ref(and3);
    DdNode* and4 = Cudd_bddAnd(manager, x6, x7);
    Cudd_Ref(and4);

    DdNode* or1 = Cudd_bddOr(manager, and1, and2);
    Cudd_Ref(or1);
    DdNode* or2 = Cudd_bddOr(manager, and3, and4);
    Cudd_Ref(or2);
    DdNode* bdd = Cudd_bddOr(manager, or1, or2);
    Cudd_Ref(bdd);

    // Convert to ZDD
    DdNode* zddResult = Cudd_zddPortFromBdd(manager, bdd);
    REQUIRE(zddResult != nullptr);
    Cudd_Ref(zddResult);

    // Convert back to BDD
    DdNode* bddBack = Cudd_zddPortToBdd(manager, zddResult);
    REQUIRE(bddBack != nullptr);
    Cudd_Ref(bddBack);
    REQUIRE(bddBack == bdd);

    Cudd_RecursiveDeref(manager, bddBack);
    Cudd_RecursiveDerefZdd(manager, zddResult);
    Cudd_RecursiveDeref(manager, bdd);
    Cudd_RecursiveDeref(manager, or2);
    Cudd_RecursiveDeref(manager, or1);
    Cudd_RecursiveDeref(manager, and4);
    Cudd_RecursiveDeref(manager, and3);
    Cudd_RecursiveDeref(manager, and2);
    Cudd_RecursiveDeref(manager, and1);
    Cudd_Quit(manager);
}

TEST_CASE("cuddZddPort - Cudd_zddPortToBdd with very large sparse ZDD", "[cuddZddPort]") {
    // Create a very large sparse ZDD to exercise all paths
    DdManager* manager = Cudd_Init(16, 32, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    int status = Cudd_zddVarsFromBddVars(manager, 1);
    REQUIRE(status == 1);

    DdNode* zddOne = DD_ONE(manager);
    DdNode* zddZero = DD_ZERO(manager);

    // Create a complex sparse ZDD structure with multiple paths
    // Node at level 14
    DdNode* z14 = cuddUniqueInterZdd(manager, 14, zddOne, zddZero);
    REQUIRE(z14 != nullptr);
    Cudd_Ref(z14);

    // Node at level 10 with z14 as then child
    DdNode* z10 = cuddUniqueInterZdd(manager, 10, z14, zddZero);
    REQUIRE(z10 != nullptr);
    Cudd_Ref(z10);

    // Node at level 5 with z10 as then child
    DdNode* z5 = cuddUniqueInterZdd(manager, 5, z10, zddZero);
    REQUIRE(z5 != nullptr);
    Cudd_Ref(z5);

    // Node at level 2 with z5 as then child
    DdNode* z2 = cuddUniqueInterZdd(manager, 2, z5, zddZero);
    REQUIRE(z2 != nullptr);
    Cudd_Ref(z2);

    // Convert to BDD - this will exercise many level > depth paths
    DdNode* bddResult = Cudd_zddPortToBdd(manager, z2);
    REQUIRE(bddResult != nullptr);
    Cudd_Ref(bddResult);

    Cudd_RecursiveDeref(manager, bddResult);
    Cudd_RecursiveDerefZdd(manager, z2);
    Cudd_RecursiveDerefZdd(manager, z5);
    Cudd_RecursiveDerefZdd(manager, z10);
    Cudd_RecursiveDerefZdd(manager, z14);
    Cudd_Quit(manager);
}
