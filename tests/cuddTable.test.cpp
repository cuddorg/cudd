#include <catch2/catch_test_macros.hpp>
#include <vector>

// Include CUDD headers
#include "mtr.h"
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddTable.c
 * 
 * This file contains comprehensive tests to achieve 90% code coverage
 * for the cuddTable module, which manages unique table operations.
 */

// ============================================================================
// Tests for Cudd_Prime - Returns the next prime >= p
// ============================================================================

TEST_CASE("Cudd_Prime - Prime number generation", "[cuddTable]") {
    SECTION("Small primes") {
        // Cudd_Prime returns next prime >= p
        // Note: It first decrements p, then increments, so Cudd_Prime(2) returns 3
        REQUIRE(Cudd_Prime(3) == 3);
        REQUIRE(Cudd_Prime(5) == 5);
        REQUIRE(Cudd_Prime(7) == 7);
        REQUIRE(Cudd_Prime(11) == 11);
        REQUIRE(Cudd_Prime(13) == 13);
    }
    
    SECTION("Non-prime inputs") {
        REQUIRE(Cudd_Prime(4) == 5);
        REQUIRE(Cudd_Prime(6) == 7);
        REQUIRE(Cudd_Prime(8) == 11);
        REQUIRE(Cudd_Prime(9) == 11);
        REQUIRE(Cudd_Prime(10) == 11);
        REQUIRE(Cudd_Prime(12) == 13);
        REQUIRE(Cudd_Prime(14) == 17);
        REQUIRE(Cudd_Prime(15) == 17);
        REQUIRE(Cudd_Prime(16) == 17);
    }
    
    SECTION("Edge cases") {
        // Even numbers should go to next prime
        REQUIRE(Cudd_Prime(100) == 101);
        REQUIRE(Cudd_Prime(1000) == 1009);
    }
    
    SECTION("Larger numbers") {
        // Test with larger primes
        REQUIRE(Cudd_Prime(251) == 251);  // 251 is prime
        REQUIRE(Cudd_Prime(252) == 257);  // Next prime after 252
        REQUIRE(Cudd_Prime(256) == 257);  // Power of 2
    }
}

// ============================================================================
// Tests for Cudd_Reserve - Expand manager without creating variables
// ============================================================================

TEST_CASE("Cudd_Reserve - Manager table reservation", "[cuddTable]") {
    SECTION("Reserve with valid amount") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int result = Cudd_Reserve(manager, 10);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reserve with zero amount") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Zero amount should succeed without changes
        int result = Cudd_Reserve(manager, 0);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reserve with negative amount") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Negative amount should fail
        int result = Cudd_Reserve(manager, -1);
        REQUIRE(result == 0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reserve enough space and create variables") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Reserve space for 20 variables
        int result = Cudd_Reserve(manager, 20);
        REQUIRE(result == 1);
        
        // Now create variables - should use the reserved space
        for (int i = 0; i < 20; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 20);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reserve small amount when space already exists") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Small reservation when space likely exists
        int result = Cudd_Reserve(manager, 1);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reserve large amount to force table resize") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Large reservation to force resize
        int result = Cudd_Reserve(manager, 100);
        REQUIRE(result == 1);
        
        // Create variables in the reserved space
        for (int i = 0; i < 50; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddUniqueInter - BDD unique table operations
// ============================================================================

TEST_CASE("cuddUniqueInter - BDD node creation and lookup", "[cuddTable]") {
    SECTION("Create multiple nodes at different levels") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some BDD operations to exercise unique table
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        REQUIRE(x0 != nullptr);
        REQUIRE(x1 != nullptr);
        REQUIRE(x2 != nullptr);
        
        // Create compound BDDs
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        REQUIRE(f != nullptr);
        
        DdNode *g = Cudd_bddOr(manager, f, x2);
        Cudd_Ref(g);
        REQUIRE(g != nullptr);
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Lookup existing nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Same operation should return same pointer (unique table lookup)
        DdNode *f1 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f1);
        
        DdNode *f2 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f2);
        
        // Should be the same node
        REQUIRE(f1 == f2);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddUniqueInterIVO - Independent of variable ordering
// ============================================================================

TEST_CASE("cuddUniqueInterIVO - Variable order independent operations", "[cuddTable]") {
    SECTION("Create node independent of variable ordering") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables at different levels
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x5 = Cudd_bddIthVar(manager, 5);
        DdNode *x9 = Cudd_bddIthVar(manager, 9);
        
        // Operations that may use cuddUniqueInterIVO internally
        DdNode *f = Cudd_bddAnd(manager, x0, x9);
        Cudd_Ref(f);
        
        DdNode *g = Cudd_bddOr(manager, f, x5);
        Cudd_Ref(g);
        
        REQUIRE(g != nullptr);
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddUniqueInterZdd - ZDD unique table operations
// ============================================================================

TEST_CASE("cuddUniqueInterZdd - ZDD node creation", "[cuddTable]") {
    SECTION("Create ZDD nodes") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *zddOne = Cudd_ReadZddOne(manager, 0);
        REQUIRE(zddOne != nullptr);
        
        // Create ZDD variables
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        
        // Create compound ZDD
        DdNode *f = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(f != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD operations with multiple variables") {
        DdManager *manager = Cudd_Init(0, 20, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create several ZDD variables
        for (int i = 0; i < 10; i++) {
            DdNode *z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddGetNode - ZDD node wrapper
// ============================================================================

TEST_CASE("cuddZddGetNode - ZDD node creation with reduction", "[cuddTable]") {
    SECTION("Create ZDD nodes with reduction rule") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        // Union creates nodes using cuddZddGetNode
        DdNode *f = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(f != nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddZddGetNodeIVO - ZDD IVO wrapper
// ============================================================================

TEST_CASE("cuddZddGetNodeIVO - ZDD node IVO operations", "[cuddTable]") {
    SECTION("Create ZDD using IVO wrapper") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Operations that may use cuddZddGetNodeIVO
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z5 = Cudd_zddIthVar(manager, 5);
        
        DdNode *f = Cudd_zddUnion(manager, z0, z5);
        REQUIRE(f != nullptr);
        
        // Product operations may use IVO
        DdNode *g = Cudd_zddProduct(manager, z0, z5);
        REQUIRE(g != nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddUniqueConst - Constant node management
// ============================================================================

TEST_CASE("cuddUniqueConst - Constant node creation", "[cuddTable]") {
    SECTION("Create various constant nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ADD constants
        DdNode *c1 = Cudd_addConst(manager, 1.0);
        REQUIRE(c1 != nullptr);
        REQUIRE(Cudd_V(c1) == 1.0);
        
        DdNode *c2 = Cudd_addConst(manager, 2.5);
        REQUIRE(c2 != nullptr);
        REQUIRE(Cudd_V(c2) == 2.5);
        
        DdNode *c3 = Cudd_addConst(manager, -3.14);
        REQUIRE(c3 != nullptr);
        REQUIRE(Cudd_V(c3) == -3.14);
        
        // Same value should return same pointer
        DdNode *c1_again = Cudd_addConst(manager, 1.0);
        REQUIRE(c1_again == c1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create many constant nodes") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many constants to potentially trigger rehashing
        for (int i = 0; i < 100; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i);
            REQUIRE(c != nullptr);
            REQUIRE(Cudd_V(c) == (double)i);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddGarbageCollect - Garbage collection
// ============================================================================

TEST_CASE("cuddGarbageCollect - Garbage collection tests", "[cuddTable]") {
    SECTION("Basic garbage collection") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create nodes
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Deref to create dead nodes
        Cudd_RecursiveDeref(manager, f);
        
        // GC count before
        int gc_before = Cudd_ReadGarbageCollections(manager);
        
        // Force garbage collection via cuddGarbageCollect (internal function)
        // We can trigger it by creating/deleting many nodes
        for (int i = 0; i < 1000; i++) {
            DdNode *temp = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, temp);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("GC with dead nodes") {
        DdManager *manager = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create and immediately deref many nodes to accumulate dead nodes
        for (int i = 0; i < 10; i++) {
            DdNode *vars[5];
            for (int j = 0; j < 5; j++) {
                vars[j] = Cudd_bddIthVar(manager, i * 2 + j % 20);
            }
            
            DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
            Cudd_Ref(f);
            DdNode *g = Cudd_bddOr(manager, f, vars[2]);
            Cudd_Ref(g);
            
            Cudd_RecursiveDeref(manager, g);
            Cudd_RecursiveDeref(manager, f);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("GC with cache clearing") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        // Create many operations to fill cache
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            DdNode *g = Cudd_bddOr(manager, f, x2);
            Cudd_Ref(g);
            
            Cudd_RecursiveDeref(manager, g);
            Cudd_RecursiveDeref(manager, f);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddRehash - Table rehashing
// ============================================================================

TEST_CASE("cuddRehash - Table rehashing tests", "[cuddTable]") {
    SECTION("Force subtable rehashing by creating many nodes") {
        // Start with small initial slots
        DdManager *manager = Cudd_Init(5, 0, 32, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create many unique nodes to trigger rehashing
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 200; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 5], vars[(i + 1) % 5]);
            Cudd_Ref(f);
            nodes.push_back(f);
            
            // Create more complex structures
            if (i > 0) {
                DdNode *g = Cudd_bddOr(manager, nodes[i-1], f);
                Cudd_Ref(g);
                nodes.push_back(g);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddShrinkSubtable - Table shrinking
// ============================================================================

TEST_CASE("cuddShrinkSubtable - Table shrinking after reordering", "[cuddTable]") {
    SECTION("Shrink after creating and destroying many nodes") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create many nodes then remove them
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            nodes.push_back(f);
        }
        
        // Remove all nodes
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddInsertSubtables - Insert new subtables
// ============================================================================

TEST_CASE("cuddInsertSubtables - Insert subtables for new variables", "[cuddTable]") {
    SECTION("Insert variables at different levels") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadSize(manager) == 5);
        
        // Create new variable at a specific level using Cudd_bddNewVarAtLevel
        DdNode *v_at_0 = Cudd_bddNewVarAtLevel(manager, 0);
        REQUIRE(v_at_0 != nullptr);
        
        DdNode *v_at_3 = Cudd_bddNewVarAtLevel(manager, 3);
        REQUIRE(v_at_3 != nullptr);
        
        // Size should increase
        REQUIRE(Cudd_ReadSize(manager) >= 7);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert multiple subtables") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert several new variables at different levels
        for (int i = 0; i < 5; i++) {
            DdNode *v = Cudd_bddNewVarAtLevel(manager, i);
            REQUIRE(v != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddDestroySubtables - Destroy subtables
// ============================================================================

TEST_CASE("cuddDestroySubtables - Destroy unused subtables", "[cuddTable]") {
    SECTION("Create and remove variables") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Check that we can read the size
        int initial_size = Cudd_ReadSize(manager);
        REQUIRE(initial_size == 10);
        
        // Create some BDDs
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        Cudd_RecursiveDeref(manager, f);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddResizeTableZdd - ZDD table resizing
// ============================================================================

TEST_CASE("cuddResizeTableZdd - ZDD table resize", "[cuddTable]") {
    SECTION("Resize ZDD table by creating many variables") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadZddSize(manager) == 5);
        
        // Access ZDD variables beyond current size to force resize
        for (int i = 0; i < 20; i++) {
            DdNode *z = Cudd_zddIthVar(manager, i);
            REQUIRE(z != nullptr);
        }
        
        REQUIRE(Cudd_ReadZddSize(manager) >= 20);
        
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD resize with operations") {
        DdManager *manager = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        // Force resize by accessing higher index
        DdNode *z10 = Cudd_zddIthVar(manager, 10);
        REQUIRE(z10 != nullptr);
        
        // Operations should still work
        DdNode *f = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(f != nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddSlowTableGrowth - Slow down table growth
// ============================================================================

TEST_CASE("cuddSlowTableGrowth - Slow growth mode", "[cuddTable]") {
    SECTION("Trigger slow growth with memory pressure") {
        // Create manager with limited memory
        size_t maxMem = 1024 * 1024; // 1 MB
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, maxMem);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create complex BDDs that use memory
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 50; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 10], vars[(i + 1) % 10]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddRehashZdd - ZDD subtable rehashing
// ============================================================================

TEST_CASE("ddRehashZdd - ZDD subtable rehashing", "[cuddTable]") {
    SECTION("Create many ZDD nodes to trigger rehashing") {
        DdManager *manager = Cudd_Init(0, 5, 32, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        DdNode *z[5];
        for (int i = 0; i < 5; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create many ZDD operations
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_zddUnion(manager, z[i % 5], z[(i + 1) % 5]);
            if (f != nullptr) {
                Cudd_Ref(f);
                Cudd_RecursiveDerefZdd(manager, f);
            }
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ddPatchTree - Fix variable tree after insertion
// ============================================================================

TEST_CASE("ddPatchTree - Variable tree patching", "[cuddTable]") {
    SECTION("Create variable tree and insert variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a variable group (tree)
        MtrNode *tree = Cudd_MakeTreeNode(manager, 0, 3, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        
        Cudd_SetTree(manager, tree);
        
        // Insert new variable which should trigger ddPatchTree
        DdNode *v = Cudd_bddNewVarAtLevel(manager, 1);
        REQUIRE(v != nullptr);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddAllocNode - Node allocation
// ============================================================================

TEST_CASE("cuddAllocNode - Node allocation tests", "[cuddTable]") {
    SECTION("Allocate many nodes") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create many nodes to test allocation
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 500; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            nodes.push_back(f);
            
            DdNode *g = Cudd_bddOr(manager, x0, x1);
            Cudd_Ref(g);
            nodes.push_back(g);
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Allocation with garbage collection") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create and free nodes repeatedly to test reclamation
        for (int j = 0; j < 10; j++) {
            std::vector<DdNode*> batch;
            for (int i = 0; i < 100; i++) {
                DdNode *f = Cudd_bddAnd(manager, x0, x1);
                Cudd_Ref(f);
                batch.push_back(f);
            }
            
            // Free all nodes in batch
            for (auto n : batch) {
                Cudd_RecursiveDeref(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for cuddInitTable and cuddFreeTable - Table initialization/cleanup
// ============================================================================

TEST_CASE("cuddInitTable/cuddFreeTable - Table lifecycle", "[cuddTable]") {
    SECTION("Multiple init/free cycles") {
        for (int i = 0; i < 5; i++) {
            DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
            REQUIRE(manager != nullptr);
            
            // Use the manager
            DdNode *x = Cudd_bddNewVar(manager);
            REQUIRE(x != nullptr);
            
            Cudd_Quit(manager);
        }
    }
    
    SECTION("Init with various parameters") {
        // Different combinations of parameters
        DdManager *m1 = Cudd_Init(0, 0, 128, 256, 0);
        REQUIRE(m1 != nullptr);
        Cudd_Quit(m1);
        
        DdManager *m2 = Cudd_Init(10, 0, 512, 1024, 0);
        REQUIRE(m2 != nullptr);
        Cudd_Quit(m2);
        
        DdManager *m3 = Cudd_Init(0, 10, 256, 512, 0);
        REQUIRE(m3 != nullptr);
        Cudd_Quit(m3);
        
        DdManager *m4 = Cudd_Init(5, 5, 64, 128, 1024*1024);
        REQUIRE(m4 != nullptr);
        Cudd_Quit(m4);
    }
}

// ============================================================================
// Tests for ddResizeTable - BDD table resizing
// ============================================================================

TEST_CASE("ddResizeTable - BDD table resize", "[cuddTable]") {
    SECTION("Resize by creating many variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        REQUIRE(Cudd_ReadSize(manager) == 5);
        
        // Create more variables than initial
        for (int i = 0; i < 30; i++) {
            DdNode *v = Cudd_bddIthVar(manager, i);
            REQUIRE(v != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) >= 30);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Resize with variable map") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set up a variable map
        DdNode *x[5], *y[5];
        for (int i = 0; i < 5; i++) {
            x[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create more variables for mapping
        for (int i = 0; i < 5; i++) {
            y[i] = Cudd_bddIthVar(manager, i + 5);
        }
        
        // Set variable map
        int result = Cudd_SetVarMap(manager, x, y, 5);
        REQUIRE(result == 1);
        
        // Now force resize - map handling should work
        for (int i = 10; i < 25; i++) {
            DdNode *v = Cudd_bddIthVar(manager, i);
            REQUIRE(v != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for edge cases and error handling
// ============================================================================

TEST_CASE("cuddTable - Edge cases and error handling", "[cuddTable]") {
    SECTION("Operations near index limits") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create a high-index variable
        DdNode *v = Cudd_bddIthVar(manager, 100);
        REQUIRE(v != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Timeout handling in unique table") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a very short time limit
        Cudd_SetTimeLimit(manager, 1); // 1 millisecond
        
        // Try to do work - may timeout
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Reset time limit
        Cudd_UnsetTimeLimit(manager);
        
        // Continue with normal operations
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, f);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Max live nodes limit") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set max live nodes
        Cudd_SetMaxLive(manager, 1000);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create some nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for termination callback
// ============================================================================

static int terminationCounter = 0;

static int testTerminationCallback(const void *arg) {
    (void)arg;
    terminationCounter++;
    return (terminationCounter > 1000) ? 1 : 0;  // Terminate after 1000 checks
}

TEST_CASE("cuddTable - Termination callback", "[cuddTable]") {
    SECTION("Register and trigger termination callback") {
        terminationCounter = 0;
        
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Register termination callback
        Cudd_RegisterTerminationCallback(manager, testTerminationCallback, nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Do some work
        for (int i = 0; i < 50; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            if (f != nullptr) {
                Cudd_Ref(f);
                Cudd_RecursiveDeref(manager, f);
            }
        }
        
        // Unregister callback
        Cudd_UnregisterTerminationCallback(manager);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for hooks (pre/post GC hooks)
// ============================================================================

static int gcHookCounter = 0;

static int testPreGCHook(DdManager *dd, const char *str, void *data) {
    (void)dd;
    (void)str;
    (void)data;
    gcHookCounter++;
    return 1;
}

static int testPostGCHook(DdManager *dd, const char *str, void *data) {
    (void)dd;
    (void)str;
    (void)data;
    gcHookCounter++;
    return 1;
}

TEST_CASE("cuddTable - GC hooks", "[cuddTable]") {
    SECTION("Pre and post GC hooks") {
        gcHookCounter = 0;
        
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Add GC hooks
        int result1 = Cudd_AddHook(manager, testPreGCHook, CUDD_PRE_GC_HOOK);
        REQUIRE(result1 == 1);
        
        int result2 = Cudd_AddHook(manager, testPostGCHook, CUDD_POST_GC_HOOK);
        REQUIRE(result2 == 1);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create and destroy nodes to potentially trigger GC
        for (int i = 0; i < 200; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            Cudd_RecursiveDeref(manager, f);
        }
        
        // Remove hooks
        Cudd_RemoveHook(manager, testPreGCHook, CUDD_PRE_GC_HOOK);
        Cudd_RemoveHook(manager, testPostGCHook, CUDD_POST_GC_HOOK);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for death row and node reclamation
// ============================================================================

TEST_CASE("cuddTable - Death row and reclamation", "[cuddTable]") {
    SECTION("Test delayed deref and reclamation") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            nodes.push_back(f);
        }
        
        // Use delayed deref
        for (auto n : nodes) {
            Cudd_DelayedDerefBdd(manager, n);
        }
        
        // Create more nodes to potentially trigger reclamation
        for (int i = 0; i < 50; i++) {
            DdNode *g = Cudd_bddOr(manager, x0, x1);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, g);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for reordering interaction with unique table
// ============================================================================

TEST_CASE("cuddTable - Reordering interaction", "[cuddTable]") {
    SECTION("Enable auto-reordering and create BDDs") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable automatic reordering
        Cudd_AutodynEnable(manager, CUDD_REORDER_SIFT);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create complex BDD to potentially trigger reordering
        DdNode *f = vars[0];
        Cudd_Ref(f);
        
        for (int i = 1; i < 10; i++) {
            DdNode *g = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        Cudd_RecursiveDeref(manager, f);
        
        // Disable reordering
        Cudd_AutodynDisable(manager);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Manual reordering") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create BDD
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, f, vars[2]);
        Cudd_Ref(g);
        
        // Force reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        // Result may be 0 if nothing to reorder
        (void)result;
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for ZDD garbage collection
// ============================================================================

TEST_CASE("cuddTable - ZDD garbage collection", "[cuddTable]") {
    SECTION("Create and destroy ZDD nodes") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        DdNode *z[10];
        for (int i = 0; i < 10; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create and destroy ZDD nodes
        for (int j = 0; j < 5; j++) {
            std::vector<DdNode*> nodes;
            for (int i = 0; i < 50; i++) {
                DdNode *f = Cudd_zddUnion(manager, z[i % 10], z[(i + 1) % 10]);
                if (f != nullptr) {
                    Cudd_Ref(f);
                    nodes.push_back(f);
                }
            }
            
            // Free nodes
            for (auto n : nodes) {
                Cudd_RecursiveDerefZdd(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for mixed BDD/ZDD operations
// ============================================================================

TEST_CASE("cuddTable - Mixed BDD and ZDD operations", "[cuddTable]") {
    SECTION("Create both BDD and ZDD in same manager") {
        DdManager *manager = Cudd_Init(10, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // BDD operations
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // ZDD operations
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *g = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(g);
        
        // Clean up
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDerefZdd(manager, g);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional stress tests for high coverage
// ============================================================================

TEST_CASE("cuddTable - Stress tests", "[cuddTable]") {
    SECTION("High node creation rate") {
        DdManager *manager = Cudd_Init(15, 0, 64, 256, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[15];
        for (int i = 0; i < 15; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create many complex BDDs
        std::vector<DdNode*> results;
        for (int i = 0; i < 100; i++) {
            DdNode *f = vars[i % 15];
            Cudd_Ref(f);
            
            for (int j = 1; j < 5; j++) {
                DdNode *g = Cudd_bddAnd(manager, f, vars[(i + j) % 15]);
                Cudd_Ref(g);
                Cudd_RecursiveDeref(manager, f);
                f = g;
            }
            
            results.push_back(f);
        }
        
        // Clean up
        for (auto n : results) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Repeated table operations") {
        for (int iteration = 0; iteration < 3; iteration++) {
            DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
            REQUIRE(manager != nullptr);
            
            // BDD work
            DdNode *x = Cudd_bddNewVar(manager);
            DdNode *y = Cudd_bddNewVar(manager);
            Cudd_Ref(x);
            Cudd_Ref(y);
            
            DdNode *f = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(f);
            
            // ZDD work  
            DdNode *z0 = Cudd_zddIthVar(manager, 0);
            DdNode *z1 = Cudd_zddIthVar(manager, 1);
            DdNode *g = Cudd_zddUnion(manager, z0, z1);
            Cudd_Ref(g);
            
            Cudd_RecursiveDeref(manager, f);
            Cudd_RecursiveDeref(manager, y);
            Cudd_RecursiveDeref(manager, x);
            Cudd_RecursiveDerefZdd(manager, g);
            
            Cudd_Quit(manager);
        }
    }
}

// ============================================================================
// Additional tests targeting uncovered functions
// ============================================================================

TEST_CASE("cuddTable - cuddRehash tests", "[cuddTable]") {
    SECTION("Force BDD subtable rehashing with many nodes at same level") {
        // Start with small initial slots to trigger rehashing sooner
        DdManager *manager = Cudd_Init(3, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        // Create many unique nodes to force subtable rehashing
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 500; i++) {
            // Create nodes at level 0 by using various combinations
            DdNode *temp = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(temp);
            
            DdNode *f = Cudd_bddOr(manager, temp, x2);
            Cudd_Ref(f);
            nodes.push_back(f);
            
            Cudd_RecursiveDeref(manager, temp);
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Rehash with GC trigger") {
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create and destroy many nodes to accumulate dead nodes
        for (int j = 0; j < 10; j++) {
            std::vector<DdNode*> batch;
            for (int i = 0; i < 200; i++) {
                DdNode *f = Cudd_bddAnd(manager, vars[i % 5], vars[(i + 1) % 5]);
                Cudd_Ref(f);
                batch.push_back(f);
            }
            
            for (auto n : batch) {
                Cudd_RecursiveDeref(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - ddRehashZdd tests", "[cuddTable]") {
    SECTION("Force ZDD subtable rehashing") {
        // Start with small initial slots
        DdManager *manager = Cudd_Init(0, 3, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        // Create many ZDD nodes to force rehashing
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 300; i++) {
            DdNode *f = Cudd_zddUnion(manager, z0, z1);
            Cudd_Ref(f);
            
            DdNode *g = Cudd_zddUnion(manager, f, z2);
            Cudd_Ref(g);
            nodes.push_back(g);
            
            Cudd_RecursiveDerefZdd(manager, f);
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDerefZdd(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddShrinkSubtable tests", "[cuddTable]") {
    SECTION("Trigger shrink by reordering") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create many nodes at specific levels
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 10], vars[(i + 1) % 10]);
            Cudd_Ref(f);
            nodes.push_back(f);
        }
        
        // Trigger reordering which may shrink subtables
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        (void)result;
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddDestroySubtables more tests", "[cuddTable]") {
    SECTION("Create then destroy variables") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables dynamically
        for (int i = 0; i < 5; i++) {
            DdNode *v = Cudd_bddNewVar(manager);
            REQUIRE(v != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 5);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddInsertSubtables more tests", "[cuddTable]") {
    SECTION("Insert at beginning of order") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert variable at level 0
        DdNode *v = Cudd_bddNewVarAtLevel(manager, 0);
        REQUIRE(v != nullptr);
        
        REQUIRE(Cudd_ReadSize(manager) >= 6);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert at middle of order") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert variable at level 5
        DdNode *v = Cudd_bddNewVarAtLevel(manager, 5);
        REQUIRE(v != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert multiple subtables sequentially") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        for (int i = 0; i < 5; i++) {
            DdNode *v = Cudd_bddNewVarAtLevel(manager, i);
            REQUIRE(v != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert with existing tree") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create and set a variable tree
        MtrNode *tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        REQUIRE(tree != nullptr);
        Cudd_SetTree(manager, tree);
        
        // Insert variable - should trigger ddPatchTree
        DdNode *v1 = Cudd_bddNewVarAtLevel(manager, 2);
        REQUIRE(v1 != nullptr);
        
        DdNode *v2 = Cudd_bddNewVarAtLevel(manager, 4);
        REQUIRE(v2 != nullptr);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddUniqueInterIVO tests", "[cuddTable]") {
    SECTION("Test IVO through Cudd_MakeBddFromZddCover") {
        DdManager *manager = Cudd_Init(5, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Create BDD
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Get ZDD isop which uses IVO operations
        DdNode *zdd = NULL;
        DdNode *bdd = Cudd_zddIsop(manager, f, f, &zdd);
        if (bdd != NULL) {
            Cudd_Ref(bdd);
            if (zdd != NULL) {
                Cudd_Ref(zdd);
                Cudd_RecursiveDerefZdd(manager, zdd);
            }
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddZddGetNodeIVO tests", "[cuddTable]") {
    SECTION("Test ZDD IVO through zddIsop") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Initialize ZDD variables from BDD
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        // Create cover
        DdNode *lower = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(lower);
        
        DdNode *upper = Cudd_bddOr(manager, lower, x2);
        Cudd_Ref(upper);
        
        // Get isop
        DdNode *zdd = NULL;
        DdNode *bdd = Cudd_zddIsop(manager, lower, upper, &zdd);
        if (bdd != NULL) {
            Cudd_Ref(bdd);
            if (zdd != NULL) {
                Cudd_Ref(zdd);
                
                // Convert back to BDD which may use IVO
                DdNode *converted = Cudd_MakeBddFromZddCover(manager, zdd);
                if (converted != NULL) {
                    Cudd_Ref(converted);
                    Cudd_RecursiveDeref(manager, converted);
                }
                
                Cudd_RecursiveDerefZdd(manager, zdd);
            }
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_RecursiveDeref(manager, upper);
        Cudd_RecursiveDeref(manager, lower);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddSlowTableGrowth tests", "[cuddTable]") {
    SECTION("Trigger slow growth with memory pressure") {
        // Create manager with very limited memory
        size_t maxMem = 512 * 1024; // 512 KB
        DdManager *manager = Cudd_Init(5, 0, 64, 128, maxMem);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create many nodes to trigger memory pressure
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 200; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 5], vars[(i + 1) % 5]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddAllocNode edge cases", "[cuddTable]") {
    SECTION("Allocation after garbage collection") {
        DdManager *manager = Cudd_Init(5, 0, 64, 128, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // Create and release many nodes
        for (int j = 0; j < 20; j++) {
            std::vector<DdNode*> batch;
            for (int i = 0; i < 100; i++) {
                DdNode *f = Cudd_bddAnd(manager, x0, x1);
                Cudd_Ref(f);
                batch.push_back(f);
            }
            
            for (auto n : batch) {
                Cudd_RecursiveDeref(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Allocation with max live limit") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a reasonable max live limit
        Cudd_SetMaxLive(manager, 500);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddGarbageCollect edge cases", "[cuddTable]") {
    SECTION("GC with ZDD dead nodes") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z[10];
        for (int i = 0; i < 10; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create and release ZDD nodes to create dead nodes
        for (int j = 0; j < 10; j++) {
            std::vector<DdNode*> batch;
            for (int i = 0; i < 50; i++) {
                DdNode *f = Cudd_zddUnion(manager, z[i % 10], z[(i + 1) % 10]);
                Cudd_Ref(f);
                batch.push_back(f);
            }
            
            for (auto n : batch) {
                Cudd_RecursiveDerefZdd(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("GC with both BDD and ZDD nodes") {
        DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x[5];
        for (int i = 0; i < 5; i++) {
            x[i] = Cudd_bddIthVar(manager, i);
        }
        
        DdNode *z[5];
        for (int i = 0; i < 5; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create and release both BDD and ZDD nodes
        for (int j = 0; j < 5; j++) {
            std::vector<DdNode*> bddBatch;
            std::vector<DdNode*> zddBatch;
            
            for (int i = 0; i < 30; i++) {
                DdNode *f = Cudd_bddAnd(manager, x[i % 5], x[(i + 1) % 5]);
                Cudd_Ref(f);
                bddBatch.push_back(f);
                
                DdNode *g = Cudd_zddUnion(manager, z[i % 5], z[(i + 1) % 5]);
                Cudd_Ref(g);
                zddBatch.push_back(g);
            }
            
            for (auto n : bddBatch) {
                Cudd_RecursiveDeref(manager, n);
            }
            for (auto n : zddBatch) {
                Cudd_RecursiveDerefZdd(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("GC with constant nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many constant nodes
        std::vector<DdNode*> constants;
        for (int i = 0; i < 100; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i);
            REQUIRE(c != nullptr);
            constants.push_back(c);
        }
        
        // Create BDD operations
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        for (int i = 0; i < 50; i++) {
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            Cudd_RecursiveDeref(manager, f);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddUniqueInter edge cases", "[cuddTable]") {
    SECTION("Unique table lookup with auto-reordering") {
        DdManager *manager = Cudd_Init(10, 0, 64, 256, 0);
        REQUIRE(manager != nullptr);
        
        // Enable auto-reordering
        Cudd_AutodynEnable(manager, CUDD_REORDER_SIFT);
        Cudd_SetNextReordering(manager, 100);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create many nodes to potentially trigger reordering
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 200; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 10], vars[(i + 1) % 10]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_AutodynDisable(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddUniqueInterZdd edge cases", "[cuddTable]") {
    SECTION("ZDD unique table with auto-reordering") {
        DdManager *manager = Cudd_Init(0, 10, 64, 256, 0);
        REQUIRE(manager != nullptr);
        
        // Enable ZDD auto-reordering
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_SIFT);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create ZDD nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_zddUnion(manager, vars[i % 10], vars[(i + 1) % 10]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDerefZdd(manager, n);
        }
        
        Cudd_AutodynDisableZdd(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - ddResizeTable edge cases", "[cuddTable]") {
    SECTION("Resize with map") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set up variable map
        DdNode *x[5], *y[5];
        for (int i = 0; i < 5; i++) {
            x[i] = Cudd_bddIthVar(manager, i);
            y[i] = Cudd_bddIthVar(manager, i + 5);
        }
        
        int result = Cudd_SetVarMap(manager, x, y, 5);
        REQUIRE(result == 1);
        
        // Force table resize by creating more variables
        for (int i = 10; i < 30; i++) {
            DdNode *v = Cudd_bddIthVar(manager, i);
            REQUIRE(v != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddResizeTableZdd edge cases", "[cuddTable]") {
    SECTION("Resize ZDD table with large index") {
        DdManager *manager = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variable with high index to force resize
        DdNode *z50 = Cudd_zddIthVar(manager, 50);
        REQUIRE(z50 != nullptr);
        
        REQUIRE(Cudd_ReadZddSize(manager) >= 51);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddInitTable edge cases", "[cuddTable]") {
    SECTION("Init with large numbers of variables") {
        DdManager *manager = Cudd_Init(100, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        REQUIRE(Cudd_ReadSize(manager) == 100);
        Cudd_Quit(manager);
    }
    
    SECTION("Init with large numbers of ZDD variables") {
        DdManager *manager = Cudd_Init(0, 100, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        REQUIRE(Cudd_ReadZddSize(manager) == 100);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddFreeTable edge cases", "[cuddTable]") {
    SECTION("Free with hooks still registered") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Add hooks (they should be cleaned up by Quit)
        Cudd_AddHook(manager, testPreGCHook, CUDD_PRE_GC_HOOK);
        Cudd_AddHook(manager, testPostGCHook, CUDD_POST_GC_HOOK);
        
        // Create some work
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_RecursiveDeref(manager, x);
        
        // Quit should clean up hooks
        Cudd_Quit(manager);
    }
    
    SECTION("Free with tree set") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set tree
        MtrNode *tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        Cudd_SetTree(manager, tree);
        
        // Quit should free tree
        Cudd_Quit(manager);
    }
    
    SECTION("Free with ZDD tree set") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set ZDD tree
        MtrNode *tree = Cudd_MakeZddTreeNode(manager, 0, 5, MTR_DEFAULT);
        Cudd_SetZddTree(manager, tree);
        
        // Quit should free ZDD tree
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional tests to trigger cuddRehash specifically
// ============================================================================

TEST_CASE("cuddTable - cuddRehash specific tests", "[cuddTable]") {
    SECTION("Force cuddRehash by disabling GC and creating many nodes") {
        // Start with small slots and disable GC
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Disable garbage collection
        Cudd_DisableGarbageCollection(manager);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create many nodes to exceed maxKeys and trigger rehash
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 500; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 5], vars[(i + 1) % 5]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        // Re-enable GC
        Cudd_EnableGarbageCollection(manager);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Rehash with dead nodes below threshold") {
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Disable GC to prevent it from running during node creation
        Cudd_DisableGarbageCollection(manager);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create nodes without releasing to keep dead count low
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 300; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[i % 5], vars[(i + 1) % 5]);
            Cudd_Ref(f);
            nodes.push_back(f);
            
            DdNode *g = Cudd_bddOr(manager, vars[(i + 2) % 5], f);
            Cudd_Ref(g);
            nodes.push_back(g);
        }
        
        // Clean up
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_EnableGarbageCollection(manager);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to trigger ddRehashZdd
// ============================================================================

TEST_CASE("cuddTable - ddRehashZdd specific tests", "[cuddTable]") {
    SECTION("Force ZDD rehash by disabling GC") {
        DdManager *manager = Cudd_Init(0, 5, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_DisableGarbageCollection(manager);
        
        DdNode *z[5];
        for (int i = 0; i < 5; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create many ZDD nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 400; i++) {
            DdNode *f = Cudd_zddUnion(manager, z[i % 5], z[(i + 1) % 5]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        for (auto n : nodes) {
            Cudd_RecursiveDerefZdd(manager, n);
        }
        
        Cudd_EnableGarbageCollection(manager);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests to trigger cuddShrinkSubtable
// ============================================================================

TEST_CASE("cuddTable - cuddShrinkSubtable specific tests", "[cuddTable]") {
    SECTION("Trigger shrink through window reordering") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create BDD that benefits from reordering
        DdNode *f = vars[0];
        Cudd_Ref(f);
        
        for (int i = 1; i < 10; i++) {
            DdNode *g = Cudd_bddXor(manager, f, vars[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        // Try window reordering which can trigger shrink
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_WINDOW2, 0);
        (void)result;
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Trigger shrink through symm sift") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create symmetric BDD
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        for (int i = 2; i < 8; i += 2) {
            DdNode *pair = Cudd_bddAnd(manager, vars[i], vars[i+1]);
            Cudd_Ref(pair);
            DdNode *g = Cudd_bddXor(manager, f, pair);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, pair);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        // Symmetric sift reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SYMM_SIFT, 0);
        (void)result;
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for shuffle heap which can trigger various table operations  
// ============================================================================

TEST_CASE("cuddTable - Shuffle heap tests", "[cuddTable]") {
    SECTION("Shuffle heap with BDD nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create BDD
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, f, vars[2]);
        Cudd_Ref(g);
        
        // Define new permutation
        int perm[5] = {4, 3, 2, 1, 0}; // Reverse order
        
        int result = Cudd_ShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD shuffle heap") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z[5];
        for (int i = 0; i < 5; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create ZDD
        DdNode *f = Cudd_zddUnion(manager, z[0], z[1]);
        Cudd_Ref(f);
        
        // Define new permutation
        int perm[5] = {2, 4, 1, 3, 0};
        
        int result = Cudd_zddShuffleHeap(manager, perm);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for constant table operations
// ============================================================================

TEST_CASE("cuddTable - Constant table rehash", "[cuddTable]") {
    SECTION("Create many constants to trigger constant table rehash") {
        DdManager *manager = Cudd_Init(0, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Disable GC to allow constant table to grow
        Cudd_DisableGarbageCollection(manager);
        
        // Create many unique constant values
        std::vector<DdNode*> constants;
        for (int i = 0; i < 200; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i + 0.1 * i);
            REQUIRE(c != nullptr);
            constants.push_back(c);
        }
        
        Cudd_EnableGarbageCollection(manager);
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional edge case tests
// ============================================================================

TEST_CASE("cuddTable - Memory pressure tests", "[cuddTable]") {
    SECTION("Operations under memory pressure") {
        // Create manager with very small memory limit
        size_t maxMem = 256 * 1024; // 256 KB
        DdManager *manager = Cudd_Init(3, 0, 32, 64, maxMem);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[3];
        for (int i = 0; i < 3; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Try to create nodes under pressure
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
            if (f != nullptr) {
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Tests for variable binding operations
// ============================================================================

TEST_CASE("cuddTable - Variable binding", "[cuddTable]") {
    SECTION("Bind and unbind variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Bind variable
        int result = Cudd_bddBindVar(manager, 0);
        REQUIRE(result == 1);
        
        // Check if bound
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 1);
        
        // Unbind variable
        result = Cudd_bddUnbindVar(manager, 0);
        REQUIRE(result == 1);
        
        // Check if unbound
        REQUIRE(Cudd_bddVarIsBound(manager, 0) == 0);
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional tests targeting hard-to-cover functions
// ============================================================================

TEST_CASE("cuddTable - More ZDD rehash tests", "[cuddTable]") {
    SECTION("ZDD subtable rehash with small tables and no GC") {
        // Use very small subtable to ensure maxKeys is exceeded quickly
        DdManager *manager = Cudd_Init(0, 10, 8, 64, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_DisableGarbageCollection(manager);
        
        DdNode *z[10];
        for (int i = 0; i < 10; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create many unique ZDD nodes through various operations
        std::vector<DdNode*> nodes;
        
        // First, create pairwise unions
        for (int i = 0; i < 10; i++) {
            for (int j = i + 1; j < 10; j++) {
                DdNode *f = Cudd_zddUnion(manager, z[i], z[j]);
                if (f != nullptr) {
                    Cudd_Ref(f);
                    nodes.push_back(f);
                }
            }
        }
        
        // Then create products (generates more unique nodes)
        for (int i = 0; i < 10; i++) {
            for (int j = i + 1; j < 10; j++) {
                DdNode *f = Cudd_zddProduct(manager, z[i], z[j]);
                if (f != nullptr) {
                    Cudd_Ref(f);
                    nodes.push_back(f);
                }
            }
        }
        
        // Create more complex combinations
        for (size_t i = 0; i < nodes.size() && i < 20; i++) {
            for (size_t j = i + 1; j < nodes.size() && j < 20; j++) {
                DdNode *f = Cudd_zddUnion(manager, nodes[i], nodes[j]);
                if (f != nullptr) {
                    Cudd_Ref(f);
                    nodes.push_back(f);
                }
            }
        }
        
        for (auto n : nodes) {
            Cudd_RecursiveDerefZdd(manager, n);
        }
        
        Cudd_EnableGarbageCollection(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddUniqueConst rehash", "[cuddTable]") {
    SECTION("Force constant table rehash") {
        // Use small constant table size
        DdManager *manager = Cudd_Init(0, 0, 8, 64, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_DisableGarbageCollection(manager);
        
        // Create many unique constants to trigger rehash
        for (int i = 0; i < 300; i++) {
            double val = (double)i + 0.12345 * i;
            DdNode *c = Cudd_addConst(manager, val);
            REQUIRE(c != nullptr);
        }
        
        Cudd_EnableGarbageCollection(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - Complex reordering tests", "[cuddTable]") {
    SECTION("Genetic reordering") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create BDD
        DdNode *f = vars[0];
        Cudd_Ref(f);
        
        for (int i = 1; i < 8; i++) {
            DdNode *g = Cudd_bddXor(manager, f, vars[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        // Genetic reordering
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GENETIC, 0);
        (void)result;
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Annealing reordering") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(f);
        
        for (int i = 2; i < 6; i++) {
            DdNode *g = Cudd_bddOr(manager, f, vars[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_ANNEALING, 0);
        (void)result;
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Linear sift reordering") {
        DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[6];
        for (int i = 0; i < 6; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        DdNode *f = Cudd_bddAnd(manager, vars[0], vars[5]);
        Cudd_Ref(f);
        
        DdNode *g = Cudd_bddOr(manager, vars[1], vars[4]);
        Cudd_Ref(g);
        
        DdNode *h = Cudd_bddXor(manager, f, g);
        Cudd_Ref(h);
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_LINEAR, 0);
        (void)result;
        
        Cudd_RecursiveDeref(manager, h);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - Group sift reordering", "[cuddTable]") {
    SECTION("Group sift with groups") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create groups
        MtrNode *tree = Cudd_MakeTreeNode(manager, 0, 5, MTR_DEFAULT);
        MtrNode *tree2 = Cudd_MakeTreeNode(manager, 5, 5, MTR_DEFAULT);
        Cudd_SetTree(manager, tree);
        (void)tree2;
        
        // Create BDD
        DdNode *f = vars[0];
        Cudd_Ref(f);
        
        for (int i = 1; i < 10; i++) {
            DdNode *g = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_GROUP_SIFT, 0);
        (void)result;
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - ZDD reordering", "[cuddTable]") {
    SECTION("ZDD operations before reordering") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z[5];
        for (int i = 0; i < 5; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create simple ZDD operations
        DdNode *f = Cudd_zddUnion(manager, z[0], z[1]);
        Cudd_Ref(f);
        
        DdNode *g = Cudd_zddUnion(manager, z[2], z[3]);
        Cudd_Ref(g);
        
        DdNode *h = Cudd_zddUnion(manager, f, g);
        Cudd_Ref(h);
        
        // Clean up in order
        Cudd_RecursiveDerefZdd(manager, h);
        Cudd_RecursiveDerefZdd(manager, g);
        Cudd_RecursiveDerefZdd(manager, f);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - Large scale operations", "[cuddTable]") {
    SECTION("Create many BDD variables and nodes") {
        DdManager *manager = Cudd_Init(30, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[30];
        for (int i = 0; i < 30; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create chain of AND operations
        DdNode *f = vars[0];
        Cudd_Ref(f);
        
        for (int i = 1; i < 20; i++) {
            DdNode *g = Cudd_bddAnd(manager, f, vars[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDeref(manager, f);
            f = g;
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Create many ZDD variables and nodes") {
        DdManager *manager = Cudd_Init(0, 30, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z[30];
        for (int i = 0; i < 30; i++) {
            z[i] = Cudd_zddIthVar(manager, i);
        }
        
        // Create chain of union operations
        DdNode *f = z[0];
        Cudd_Ref(f);
        
        for (int i = 1; i < 20; i++) {
            DdNode *g = Cudd_zddUnion(manager, f, z[i]);
            Cudd_Ref(g);
            Cudd_RecursiveDerefZdd(manager, f);
            f = g;
        }
        
        Cudd_RecursiveDerefZdd(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddUniqueInter high coverage", "[cuddTable]") {
    SECTION("Unique inter with small slots") {
        DdManager *manager = Cudd_Init(5, 0, 16, 64, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_DisableGarbageCollection(manager);
        
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create some BDD structures
        std::vector<DdNode*> nodes;
        
        // Simple ANDs - limited to avoid infinite loop
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 5; j++) {
                DdNode *f = Cudd_bddAnd(manager, vars[i], vars[j]);
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        // XORs
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 5; j++) {
                DdNode *f = Cudd_bddXor(manager, vars[i], vars[j]);
                Cudd_Ref(f);
                nodes.push_back(f);
            }
        }
        
        for (auto n : nodes) {
            Cudd_RecursiveDeref(manager, n);
        }
        
        Cudd_EnableGarbageCollection(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddGarbageCollect high coverage", "[cuddTable]") {
    SECTION("GC with many dead nodes in multiple subtables") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(manager, i);
        }
        
        // Create nodes in all subtables
        for (int round = 0; round < 5; round++) {
            std::vector<DdNode*> batch;
            for (int i = 0; i < 10; i++) {
                for (int j = i + 1; j < 10; j++) {
                    DdNode *f = Cudd_bddAnd(manager, vars[i], vars[j]);
                    Cudd_Ref(f);
                    batch.push_back(f);
                }
            }
            
            // Release all to create dead nodes in all subtables
            for (auto n : batch) {
                Cudd_RecursiveDeref(manager, n);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddTable - cuddInitTable high coverage", "[cuddTable]") {
    SECTION("Init with minimum slots") {
        DdManager *manager = Cudd_Init(2, 2, 2, 2, 0);
        REQUIRE(manager != nullptr);
        
        // Verify basic functionality
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *z = Cudd_zddIthVar(manager, 0);
        REQUIRE(x != nullptr);
        REQUIRE(z != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Init with very large variable count") {
        DdManager *manager = Cudd_Init(200, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        REQUIRE(Cudd_ReadSize(manager) == 200);
        Cudd_Quit(manager);
    }
}
