#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddInit.c
 * 
 * This file contains comprehensive tests for the cuddInit module
 * to achieve 100% code coverage and ensure correct functionality
 * of DD manager initialization and cleanup.
 */

TEST_CASE("Cudd_Init - Basic initialization", "[cuddInit]") {
    SECTION("Initialize with no variables") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Verify basic constants are initialized
        DdNode *one = Cudd_ReadOne(manager);
        REQUIRE(one != nullptr);
        
        DdNode *logicZero = Cudd_ReadLogicZero(manager);
        REQUIRE(logicZero != nullptr);
        REQUIRE(logicZero == Cudd_Not(one));
        
        DdNode *zero = Cudd_ReadZero(manager);
        REQUIRE(zero != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with BDD variables") {
        unsigned int numVars = 5;
        DdManager *manager = Cudd_Init(numVars, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Verify variables are created
        REQUIRE(Cudd_ReadSize(manager) == numVars);
        
        // Test that projection functions are accessible
        for (unsigned int i = 0; i < numVars; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with ZDD variables") {
        unsigned int numVarsZ = 5;
        DdManager *manager = Cudd_Init(0, numVarsZ, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadZddSize(manager) == numVarsZ);
        
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with both BDD and ZDD variables") {
        unsigned int numVars = 3;
        unsigned int numVarsZ = 4;
        DdManager *manager = Cudd_Init(numVars, numVarsZ, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadSize(manager) == numVars);
        REQUIRE(Cudd_ReadZddSize(manager) == numVarsZ);
        
        // Verify BDD variables
        for (unsigned int i = 0; i < numVars; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        // Verify ZDD variables through operations
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with custom memory limit") {
        size_t maxMemory = 1024 * 1024 * 10; // 10 MB
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, maxMemory);
        REQUIRE(manager != nullptr);
        
        // Manager should be created successfully with memory limit
        DdNode *one = Cudd_ReadOne(manager);
        REQUIRE(one != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with zero memory limit (auto-detect)") {
        // When maxMemory is 0, it should auto-detect based on system
        DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Verify that variables are still created
        REQUIRE(Cudd_ReadSize(manager) == 2);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with custom unique table and cache sizes") {
        unsigned int uniqueSlots = 512;
        unsigned int cacheSlots = 512;
        DdManager *manager = Cudd_Init(0, 0, uniqueSlots, cacheSlots, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *one = Cudd_ReadOne(manager);
        REQUIRE(one != nullptr);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Cudd_Init - Test constant initialization", "[cuddInit]") {
    SECTION("Verify infinity constants") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
        DdNode *minusInf = Cudd_ReadMinusInfinity(manager);
        
        REQUIRE(plusInf != nullptr);
        REQUIRE(minusInf != nullptr);
        REQUIRE(plusInf != minusInf);
        
        // Verify they represent infinity values
        CUDD_VALUE_TYPE plusVal = Cudd_V(plusInf);
        CUDD_VALUE_TYPE minusVal = Cudd_V(minusInf);
        
        REQUIRE(plusVal > 0);
        REQUIRE(minusVal < 0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Verify background value") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *background = Cudd_ReadBackground(manager);
        DdNode *zero = Cudd_ReadZero(manager);
        
        // Background is initialized to zero
        REQUIRE(background == zero);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Cudd_Quit - Manager cleanup", "[cuddInit]") {
    SECTION("Quit with valid manager") {
        DdManager *manager = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some nodes
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Cleanup
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        
        // Quit should free all resources
        Cudd_Quit(manager);
    }
    
    SECTION("Quit with NULL manager (should be safe)") {
        DdManager *manager = nullptr;
        Cudd_Quit(manager);
    }
    
    SECTION("Quit manager with ZDD nodes") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD nodes
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        // Quit should properly clean up ZDD universe
        Cudd_Quit(manager);
    }
    
    SECTION("Quit manager with both BDD and ZDD") {
        DdManager *manager = Cudd_Init(3, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDD nodes
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Access ZDD nodes
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        Cudd_RecursiveDeref(manager, x);
        
        // Quit should clean up both BDD and ZDD structures
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddInitUniv - ZDD universe initialization", "[cuddInit]") {
    SECTION("Initialize ZDD universe during Cudd_Init") {
        unsigned int numVarsZ = 5;
        DdManager *manager = Cudd_Init(0, numVarsZ, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        for (unsigned int i = 0; i < numVarsZ; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD universe with multiple variables") {
        unsigned int numVarsZ = 10;
        DdManager *manager = Cudd_Init(0, numVarsZ, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create and test ZDD operations
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        // Build a simple ZDD to ensure universe is properly initialized
        DdNode *var0 = Cudd_zddIthVar(manager, 0);
        DdNode *var1 = Cudd_zddIthVar(manager, 1);
        REQUIRE(var0 != nullptr);
        REQUIRE(var1 != nullptr);
        
        DdNode *result = Cudd_zddUnion(manager, var0, var1);
        REQUIRE(result != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Test cuddZddFreeUniv during cleanup") {
        unsigned int numVarsZ = 7;
        DdManager *manager = Cudd_Init(0, numVarsZ, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Use ZDD operations
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        for (unsigned int i = 0; i < numVarsZ; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        // Quit will call cuddZddFreeUniv internally
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddInit - Edge cases and stress tests", "[cuddInit]") {
    SECTION("Initialize with large number of variables") {
        unsigned int numVars = 50;
        DdManager *manager = Cudd_Init(numVars, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadSize(manager) == numVars);
        
        // Verify some variables
        for (unsigned int i = 0; i < numVars; i += 10) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with large number of ZDD variables") {
        unsigned int numVarsZ = 50;
        DdManager *manager = Cudd_Init(0, numVarsZ, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadZddSize(manager) == numVarsZ);
        
        // Test ZDD operations
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple manager instances") {
        DdManager *mgr1 = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        DdManager *mgr2 = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        DdManager *mgr3 = Cudd_Init(2, 2, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        
        REQUIRE(mgr1 != nullptr);
        REQUIRE(mgr2 != nullptr);
        REQUIRE(mgr3 != nullptr);
        
        // All managers should be independent
        REQUIRE(mgr1 != mgr2);
        REQUIRE(mgr2 != mgr3);
        REQUIRE(mgr1 != mgr3);
        
        Cudd_Quit(mgr1);
        Cudd_Quit(mgr2);
        Cudd_Quit(mgr3);
    }
    
    SECTION("Test manager with minimal settings") {
        DdManager *manager = Cudd_Init(1, 1, 2, 2, 1024);
        REQUIRE(manager != nullptr);
        
        // Even with minimal settings, basic operations should work
        DdNode *var = Cudd_bddNewVar(manager);
        REQUIRE(var != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Sequential init and quit") {
        for (int i = 0; i < 10; i++) {
            DdManager *manager = Cudd_Init(2, 2, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
            REQUIRE(manager != nullptr);
            
            DdNode *x = Cudd_bddNewVar(manager);
            REQUIRE(x != nullptr);
            
            Cudd_Quit(manager);
        }
    }
}

TEST_CASE("cuddInit - Verify all manager components", "[cuddInit]") {
    SECTION("Check all basic constants after initialization") {
        DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Verify one constant
        DdNode *one = Cudd_ReadOne(manager);
        REQUIRE(one != nullptr);
        REQUIRE(Cudd_V(one) == 1.0);
        
        // Verify zero constant (ADD constant 0.0, not logical zero)
        DdNode *zero = Cudd_ReadZero(manager);
        REQUIRE(zero != nullptr);
        REQUIRE(Cudd_V(Cudd_Regular(zero)) == 0.0);
        
        // Verify logical zero for BDD
        DdNode *logicZero = Cudd_ReadLogicZero(manager);
        REQUIRE(logicZero == Cudd_Not(one));
        
        // Verify infinity constants
        DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
        DdNode *minusInf = Cudd_ReadMinusInfinity(manager);
        REQUIRE(plusInf != nullptr);
        REQUIRE(minusInf != nullptr);
        
        // Verify background
        DdNode *background = Cudd_ReadBackground(manager);
        REQUIRE(background == zero);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Test projection functions for all variables") {
        unsigned int numVars = 10;
        DdManager *manager = Cudd_Init(numVars, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // All projection functions should be accessible
        for (unsigned int i = 0; i < numVars; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
            
            // Verify it's actually a variable (has index i)
            REQUIRE(Cudd_NodeReadIndex(var) == i);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Test that variables are properly referenced") {
        unsigned int numVars = 5;
        DdManager *manager = Cudd_Init(numVars, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Variables should be internally referenced and not garbage collected
        for (unsigned int i = 0; i < numVars; i++) {
            DdNode *var1 = Cudd_bddIthVar(manager, i);
            DdNode *var2 = Cudd_bddIthVar(manager, i);
            
            // Same variable should return same pointer
            REQUIRE(var1 == var2);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddInit - Memory and resource management", "[cuddInit]") {
    SECTION("Manager cleanup with unreferenced nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *f = Cudd_bddAnd(manager, x, y);
        
        REQUIRE(f != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Manager with operations before cleanup") {
        DdManager *manager = Cudd_Init(10, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Perform various operations
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *g = Cudd_bddOr(manager, x, y);
        Cudd_Ref(g);
        
        DdNode *h = Cudd_bddXor(manager, f, g);
        Cudd_Ref(h);
        
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddInit - Comprehensive coverage test", "[cuddInit]") {
    SECTION("Full initialization and cleanup cycle") {
        // Test with various parameter combinations
        unsigned int numVars = 8;
        unsigned int numVarsZ = 6;
        unsigned int uniqueSlots = 1024;
        unsigned int cacheSlots = 1024;
        size_t maxMemory = 1024 * 1024 * 50; // 50 MB
        
        DdManager *manager = Cudd_Init(numVars, numVarsZ, uniqueSlots, cacheSlots, maxMemory);
        REQUIRE(manager != nullptr);
        
        // Verify BDD setup
        REQUIRE(Cudd_ReadSize(manager) == numVars);
        for (unsigned int i = 0; i < numVars; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
            REQUIRE(Cudd_NodeReadIndex(var) == i);
        }
        
        // Verify ZDD setup
        REQUIRE(Cudd_ReadZddSize(manager) == numVarsZ);
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        // Test ZDD variables
        for (unsigned int i = 0; i < numVarsZ; i++) {
            DdNode *zvar = Cudd_zddIthVar(manager, i);
            REQUIRE(zvar != nullptr);
        }
        
        // Verify constants
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_ReadZero(manager);
        DdNode *logicZero = Cudd_ReadLogicZero(manager);
        DdNode *plusInf = Cudd_ReadPlusInfinity(manager);
        DdNode *minusInf = Cudd_ReadMinusInfinity(manager);
        DdNode *background = Cudd_ReadBackground(manager);
        
        REQUIRE(one != nullptr);
        REQUIRE(zero != nullptr);
        REQUIRE(logicZero == Cudd_Not(one));
        REQUIRE(plusInf != nullptr);
        REQUIRE(minusInf != nullptr);
        REQUIRE(background == zero);
        
        // Perform some operations to exercise the manager
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *z = Cudd_bddIthVar(manager, 2);
        
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddOr(manager, f1, z);
        Cudd_Ref(f2);
        
        REQUIRE(f2 != nullptr);
        
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        
        // ZDD operations
        DdNode *zvar0 = Cudd_zddIthVar(manager, 0);
        DdNode *zvar1 = Cudd_zddIthVar(manager, 1);
        DdNode *zResult = Cudd_zddUnion(manager, zvar0, zvar1);
        REQUIRE(zResult != nullptr);
        
        // Final cleanup
        Cudd_Quit(manager);
    }
}
