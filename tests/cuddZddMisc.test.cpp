#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"

/**
 * @brief Test file for cuddZddMisc.c
 * 
 * This file contains comprehensive tests for the cuddZddMisc module
 * to achieve 90% code coverage. Tests cover:
 * - Cudd_zddDagSize (counts nodes in a ZDD)
 * - Cudd_zddCountMinterm (counts minterms of a ZDD)
 * - Cudd_zddPrintSubtable (prints ZDD subtable for debugging)
 * - cuddZddDagInt (internal recursive helper)
 */

// ============================================================================
// TESTS FOR Cudd_zddDagSize
// ============================================================================

TEST_CASE("cuddZddMisc - Cudd_zddDagSize basic tests", "[cuddZddMisc]") {
    SECTION("DagSize of constant zero ZDD") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        
        // Zero is a constant node, DagSize should return 0 for constants
        int dagSize = Cudd_zddDagSize(zero);
        REQUIRE(dagSize == 0);
        
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_Quit(manager);
    }
    
    SECTION("DagSize of ZddOne") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* one = Cudd_ReadZddOne(manager, 0);
        Cudd_Ref(one);
        
        // ZddOne may include nodes depending on the number of ZDD variables
        int dagSize = Cudd_zddDagSize(one);
        REQUIRE(dagSize >= 0);
        
        Cudd_RecursiveDerefZdd(manager, one);
        Cudd_Quit(manager);
    }
    
    SECTION("DagSize of single variable ZDD") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        // Single variable ZDD should have at least 1 node
        int dagSize = Cudd_zddDagSize(z0);
        REQUIRE(dagSize >= 1);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("DagSize of union of two variables") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* unionZdd = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(unionZdd);
        
        int dagSize = Cudd_zddDagSize(unionZdd);
        REQUIRE(dagSize > 0);
        
        Cudd_RecursiveDerefZdd(manager, unionZdd);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("DagSize of product of two variables") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* prodZdd = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(prodZdd);
        
        // Product ZDD should have a non-negative dag size
        int dagSize = Cudd_zddDagSize(prodZdd);
        REQUIRE(dagSize >= 0);
        
        Cudd_RecursiveDerefZdd(manager, prodZdd);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddMisc - Cudd_zddDagSize complex structures", "[cuddZddMisc]") {
    SECTION("DagSize of complex union") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Build union of multiple variables to create deeper ZDD structure
        DdNode* result = Cudd_ReadZero(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 4; i++) {
            DdNode* var = Cudd_zddIthVar(manager, i);
            Cudd_Ref(var);
            DdNode* tmp = Cudd_zddUnion(manager, result, var);
            Cudd_Ref(tmp);
            Cudd_RecursiveDerefZdd(manager, result);
            Cudd_RecursiveDerefZdd(manager, var);
            result = tmp;
        }
        
        int dagSize = Cudd_zddDagSize(result);
        REQUIRE(dagSize > 0);
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_Quit(manager);
    }
    
    SECTION("DagSize with shared subnodes") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        // Create shared structure
        DdNode* p1 = Cudd_zddProduct(manager, z0, z2);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z1, z2);
        Cudd_Ref(p2);
        DdNode* u = Cudd_zddUnion(manager, p1, p2);
        Cudd_Ref(u);
        
        int dagSize = Cudd_zddDagSize(u);
        REQUIRE(dagSize >= 1);
        
        Cudd_RecursiveDerefZdd(manager, u);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddCountMinterm
// ============================================================================

TEST_CASE("cuddZddMisc - Cudd_zddCountMinterm basic tests", "[cuddZddMisc]") {
    SECTION("CountMinterm of empty ZDD") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* zero = Cudd_ReadZero(manager);
        Cudd_Ref(zero);
        
        // Empty ZDD has 0 minterms
        double minterms = Cudd_zddCountMinterm(manager, zero, 4);
        REQUIRE(minterms == 0.0);
        
        Cudd_RecursiveDerefZdd(manager, zero);
        Cudd_Quit(manager);
    }
    
    SECTION("CountMinterm of single variable") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        int numZddVars = Cudd_ReadZddSize(manager);
        double minterms = Cudd_zddCountMinterm(manager, z0, numZddVars);
        REQUIRE(minterms > 0.0);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("CountMinterm of union") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* unionZdd = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(unionZdd);
        
        int numZddVars = Cudd_ReadZddSize(manager);
        double minterms = Cudd_zddCountMinterm(manager, unionZdd, numZddVars);
        REQUIRE(minterms > 0.0);
        
        Cudd_RecursiveDerefZdd(manager, unionZdd);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("CountMinterm with different path values") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        // Test with different path values
        double minterms1 = Cudd_zddCountMinterm(manager, z0, 4);
        double minterms2 = Cudd_zddCountMinterm(manager, z0, 8);
        
        // Different path values should give different minterm counts
        REQUIRE(minterms1 != minterms2);
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// TESTS FOR Cudd_zddPrintSubtable
// ============================================================================

TEST_CASE("cuddZddMisc - Cudd_zddPrintSubtable tests", "[cuddZddMisc]") {
    SECTION("PrintSubtable with empty manager") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Just verify it doesn't crash on empty ZDD table
        // Redirect output to /dev/null to suppress printing
        FILE* oldOut = manager->out;
        FILE* devNull = fopen("/dev/null", "w");
        if (devNull != nullptr) {
            manager->out = devNull;
            Cudd_zddPrintSubtable(manager);
            manager->out = oldOut;
            fclose(devNull);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("PrintSubtable with single variable ZDD") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        
        // Verify it doesn't crash and exercises printing code
        FILE* oldOut = manager->out;
        FILE* devNull = fopen("/dev/null", "w");
        if (devNull != nullptr) {
            manager->out = devNull;
            Cudd_zddPrintSubtable(manager);
            manager->out = oldOut;
            fclose(devNull);
        }
        
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_Quit(manager);
    }
    
    SECTION("PrintSubtable with multiple variables - covers T and E branches") {
        DdManager* manager = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDDs with both constant and non-constant children
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        
        // Create product to get non-constant children
        DdNode* prod = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(prod);
        
        // Create union to get more complex structure
        DdNode* result = Cudd_zddUnion(manager, prod, z2);
        Cudd_Ref(result);
        
        // Verify it doesn't crash and exercises all printing branches
        FILE* oldOut = manager->out;
        FILE* devNull = fopen("/dev/null", "w");
        if (devNull != nullptr) {
            manager->out = devNull;
            Cudd_zddPrintSubtable(manager);
            manager->out = oldOut;
            fclose(devNull);
        }
        
        Cudd_RecursiveDerefZdd(manager, result);
        Cudd_RecursiveDerefZdd(manager, prod);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_Quit(manager);
    }
    
    SECTION("PrintSubtable with deeply nested ZDD structure") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a complex ZDD structure to cover all branches in printing
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        DdNode* z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z2);
        DdNode* z3 = Cudd_zddIthVar(manager, 3);
        Cudd_Ref(z3);
        
        // Create nested products
        DdNode* p1 = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(p1);
        DdNode* p2 = Cudd_zddProduct(manager, z2, z3);
        Cudd_Ref(p2);
        DdNode* p3 = Cudd_zddProduct(manager, p1, p2);
        Cudd_Ref(p3);
        
        // Create unions
        DdNode* u1 = Cudd_zddUnion(manager, p1, z0);
        Cudd_Ref(u1);
        DdNode* u2 = Cudd_zddUnion(manager, p3, u1);
        Cudd_Ref(u2);
        
        FILE* oldOut = manager->out;
        FILE* devNull = fopen("/dev/null", "w");
        if (devNull != nullptr) {
            manager->out = devNull;
            Cudd_zddPrintSubtable(manager);
            manager->out = oldOut;
            fclose(devNull);
        }
        
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, p3);
        Cudd_RecursiveDerefZdd(manager, p2);
        Cudd_RecursiveDerefZdd(manager, p1);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z3);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// EDGE CASE AND INTEGRATION TESTS
// ============================================================================

TEST_CASE("cuddZddMisc - Integration and edge cases", "[cuddZddMisc]") {
    SECTION("DagSize and CountMinterm consistency") {
        DdManager* manager = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode* z0 = Cudd_zddIthVar(manager, 0);
        Cudd_Ref(z0);
        DdNode* z1 = Cudd_zddIthVar(manager, 1);
        Cudd_Ref(z1);
        
        DdNode* unionZdd = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(unionZdd);
        
        int dagSize = Cudd_zddDagSize(unionZdd);
        int numZddVars = Cudd_ReadZddSize(manager);
        double minterms = Cudd_zddCountMinterm(manager, unionZdd, numZddVars);
        
        REQUIRE(dagSize > 0);
        REQUIRE(minterms > 0.0);
        
        Cudd_RecursiveDerefZdd(manager, unionZdd);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_Quit(manager);
    }
    
    SECTION("All functions with ZDD from ISOP") {
        DdManager* manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int status = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(status == 1);
        
        DdNode* x0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x0);
        
        DdNode* zdd_I = nullptr;
        DdNode* isop = Cudd_zddIsop(manager, x0, x0, &zdd_I);
        if (isop != nullptr && zdd_I != nullptr) {
            Cudd_Ref(isop);
            Cudd_Ref(zdd_I);
            
            int dagSize = Cudd_zddDagSize(zdd_I);
            REQUIRE(dagSize >= 0);
            
            int numZddVars = Cudd_ReadZddSize(manager);
            double minterms = Cudd_zddCountMinterm(manager, zdd_I, numZddVars);
            REQUIRE(minterms >= 0.0);
            
            FILE* oldOut = manager->out;
            FILE* devNull = fopen("/dev/null", "w");
            if (devNull != nullptr) {
                manager->out = devNull;
                Cudd_zddPrintSubtable(manager);
                manager->out = oldOut;
                fclose(devNull);
            }
            
            Cudd_RecursiveDeref(manager, isop);
            Cudd_RecursiveDerefZdd(manager, zdd_I);
        }
        
        Cudd_RecursiveDeref(manager, x0);
        Cudd_Quit(manager);
    }
}
