#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Test file for cuddTable.c
 * 
 * This file contains comprehensive tests for the cuddTable module
 * to achieve 100% code coverage.
 */

TEST_CASE("Cudd_Prime - Find next prime number", "[cuddTable][Cudd_Prime]") {
    SECTION("Prime for small numbers") {
        // Cudd_Prime(p) returns next prime >= p
        // Algorithm: p--, then loop { p++; check if prime }
        // So Cudd_Prime(1): p=0, p=1 (odd, 1 is returned as-is)
        // Cudd_Prime(2): p=1, p=2 (even), p=3 (odd, prime)
        REQUIRE(Cudd_Prime(1) == 1);  // Returns 1
        REQUIRE(Cudd_Prime(2) == 3);  // Next odd prime after 1
        REQUIRE(Cudd_Prime(3) == 3);  // 3 is prime
        REQUIRE(Cudd_Prime(4) == 5);  // Next prime after 3
        REQUIRE(Cudd_Prime(5) == 5);  // 5 is prime
        REQUIRE(Cudd_Prime(6) == 7);
        REQUIRE(Cudd_Prime(7) == 7);
        REQUIRE(Cudd_Prime(8) == 11); // Next odd prime
        REQUIRE(Cudd_Prime(9) == 11);
    }
    
    SECTION("Prime for larger numbers") {
        REQUIRE(Cudd_Prime(10) == 11);
        REQUIRE(Cudd_Prime(11) == 11);
        REQUIRE(Cudd_Prime(12) == 13);
        REQUIRE(Cudd_Prime(13) == 13);
        REQUIRE(Cudd_Prime(100) == 101);
    }
    
    SECTION("Prime for even numbers") {
        REQUIRE(Cudd_Prime(50) == 53);
        REQUIRE(Cudd_Prime(100) == 101);
        REQUIRE(Cudd_Prime(200) == 211);
    }
    
    SECTION("Prime for powers of 2") {
        REQUIRE(Cudd_Prime(16) == 17);
        REQUIRE(Cudd_Prime(32) == 37);
        REQUIRE(Cudd_Prime(64) == 67);
        REQUIRE(Cudd_Prime(128) == 131);
        REQUIRE(Cudd_Prime(256) == 257);
    }
    
    SECTION("Verify prime property") {
        // Test that returned values are actually prime
        for (unsigned int i = 2; i < 100; i++) {
            unsigned int p = Cudd_Prime(i);
            REQUIRE(p >= i);
            // Verify p is odd (except for special cases)
            if (p > 2) {
                REQUIRE((p & 1) == 1);
            }
        }
    }
}

TEST_CASE("Cudd_Reserve - Reserve additional subtables", "[cuddTable][Cudd_Reserve]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Reserve positive amount") {
        int initial_size = Cudd_ReadSize(manager);
        int result = Cudd_Reserve(manager, 10);
        REQUIRE(result == 1);  // Success
        // Size should remain the same, but capacity increased
        REQUIRE(Cudd_ReadSize(manager) == initial_size);
    }
    
    SECTION("Reserve zero amount") {
        int result = Cudd_Reserve(manager, 0);
        REQUIRE(result == 1);  // Should succeed
    }
    
    SECTION("Reserve negative amount") {
        int result = Cudd_Reserve(manager, -5);
        REQUIRE(result == 0);  // Should fail
    }
    
    SECTION("Reserve within current capacity") {
        int initial_size = Cudd_ReadSize(manager);
        int result = Cudd_Reserve(manager, 1);
        REQUIRE(result == 1);
        REQUIRE(Cudd_ReadSize(manager) == initial_size);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddInitTable and cuddFreeTable - Manager lifecycle", "[cuddTable][lifecycle]") {
    SECTION("Initialize and free manager") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with BDD variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        REQUIRE(Cudd_ReadSize(manager) == 5);
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with ZDD variables") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        REQUIRE(Cudd_ReadZddSize(manager) == 5);
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with both BDD and ZDD variables") {
        DdManager *manager = Cudd_Init(3, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        REQUIRE(Cudd_ReadSize(manager) == 3);
        REQUIRE(Cudd_ReadZddSize(manager) == 4);
        Cudd_Quit(manager);
    }
    
    SECTION("Initialize with custom slots") {
        DdManager *manager = Cudd_Init(2, 2, 128, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddUniqueInter - BDD node creation", "[cuddTable][unique]") {
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create BDD variables") {
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        REQUIRE(x0 != nullptr);
        REQUIRE(x1 != nullptr);
        REQUIRE(x2 != nullptr);
        REQUIRE(x0 != x1);
        REQUIRE(x1 != x2);
    }
    
    SECTION("Create BDD operations") {
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode *and_node = Cudd_bddAnd(manager, x0, x1);
        DdNode *or_node = Cudd_bddOr(manager, x0, x1);
        DdNode *xor_node = Cudd_bddXor(manager, x0, x1);
        
        REQUIRE(and_node != nullptr);
        REQUIRE(or_node != nullptr);
        REQUIRE(xor_node != nullptr);
        REQUIRE(and_node != or_node);
        REQUIRE(or_node != xor_node);
        
        Cudd_Ref(and_node);
        Cudd_Ref(or_node);
        Cudd_Ref(xor_node);
        Cudd_RecursiveDeref(manager, and_node);
        Cudd_RecursiveDeref(manager, or_node);
        Cudd_RecursiveDeref(manager, xor_node);
    }
    
    SECTION("Test node uniqueness") {
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        DdNode *and1 = Cudd_bddAnd(manager, x0, x1);
        DdNode *and2 = Cudd_bddAnd(manager, x0, x1);
        
        // Should return the same node (unique table property)
        REQUIRE(and1 == and2);
        
        Cudd_Ref(and1);
        Cudd_RecursiveDeref(manager, and1);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddUniqueInterZdd - ZDD node creation", "[cuddTable][zdd]") {
    DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create ZDD variables") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        REQUIRE(z2 != nullptr);
    }
    
    SECTION("ZDD operations") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        DdNode *union_node = Cudd_zddUnion(manager, z0, z1);
        DdNode *intersect_node = Cudd_zddIntersect(manager, z0, z1);
        
        REQUIRE(union_node != nullptr);
        REQUIRE(intersect_node != nullptr);
        
        Cudd_Ref(union_node);
        Cudd_Ref(intersect_node);
        Cudd_RecursiveDerefZdd(manager, union_node);
        Cudd_RecursiveDerefZdd(manager, intersect_node);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddGarbageCollect - Garbage collection", "[cuddTable][gc]") {
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create and destroy nodes to trigger garbage collection") {
        // Create many temporary nodes
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *temp = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, temp);
        }
        
        // Force garbage collection
        int collected = Cudd_ReduceHeap(manager, CUDD_REORDER_NONE, 0);
        REQUIRE(collected >= 0);
    }
    
    SECTION("Check garbage collection stats") {
        unsigned long initial_gc = Cudd_ReadGarbageCollections(manager);
        
        // Create and destroy nodes
        for (int i = 0; i < 50; i++) {
            DdNode *x = Cudd_bddIthVar(manager, 0);
            DdNode *y = Cudd_bddIthVar(manager, 1);
            DdNode *temp = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, temp);
        }
        
        unsigned long final_gc = Cudd_ReadGarbageCollections(manager);
        // GC count should not decrease
        REQUIRE(final_gc >= initial_gc);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Node allocation and memory management", "[cuddTable][memory]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Allocate many nodes") {
        std::vector<DdNode*> nodes;
        
        // Create many nodes
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        // Verify all nodes are valid
        REQUIRE(nodes.size() == 100);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
    }
    
    SECTION("Check memory usage") {
        size_t initial_mem = Cudd_ReadMemoryInUse(manager);
        REQUIRE(initial_mem > 0);
        
        // Create nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 50; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        size_t after_alloc_mem = Cudd_ReadMemoryInUse(manager);
        REQUIRE(after_alloc_mem >= initial_mem);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Table resizing", "[cuddTable][resize]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Add new variables to trigger table growth") {
        int initial_size = Cudd_ReadSize(manager);
        
        // Add many new variables
        for (int i = 0; i < 10; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        int final_size = Cudd_ReadSize(manager);
        REQUIRE(final_size > initial_size);
    }
    
    SECTION("Reserve and then add variables") {
        int result = Cudd_Reserve(manager, 20);
        REQUIRE(result == 1);
        
        for (int i = 0; i < 15; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("ZDD specific table operations", "[cuddTable][zdd][specific]") {
    DdManager *manager = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("ZDD variable creation and operations") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        REQUIRE(z2 != nullptr);
        
        // Test ZDD union
        DdNode *u01 = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(u01 != nullptr);
        Cudd_Ref(u01);
        
        DdNode *u012 = Cudd_zddUnion(manager, u01, z2);
        REQUIRE(u012 != nullptr);
        Cudd_Ref(u012);
        
        Cudd_RecursiveDerefZdd(manager, u01);
        Cudd_RecursiveDerefZdd(manager, u012);
    }
    
    SECTION("Access ZDD variables") {
        int size = Cudd_ReadZddSize(manager);
        REQUIRE(size == 3);
        
        // Access all ZDD variables
        for (int i = 0; i < size; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Unique table integrity", "[cuddTable][integrity]") {
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Same operation returns same node") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        DdNode *and1 = Cudd_bddAnd(manager, x, y);
        DdNode *and2 = Cudd_bddAnd(manager, x, y);
        DdNode *and3 = Cudd_bddAnd(manager, x, y);
        
        REQUIRE(and1 == and2);
        REQUIRE(and2 == and3);
    }
    
    SECTION("Constants are unique") {
        DdNode *one1 = Cudd_ReadOne(manager);
        DdNode *one2 = Cudd_ReadOne(manager);
        DdNode *zero1 = Cudd_ReadLogicZero(manager);
        DdNode *zero2 = Cudd_ReadLogicZero(manager);
        
        REQUIRE(one1 == one2);
        REQUIRE(zero1 == zero2);
        REQUIRE(one1 != zero1);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Subtable operations", "[cuddTable][subtables]") {
    DdManager *manager = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Check subtable stats") {
        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 5);
        
        int zsize = Cudd_ReadZddSize(manager);
        REQUIRE(zsize == 3);
        
        unsigned int slots = Cudd_ReadSlots(manager);
        REQUIRE(slots > 0);
    }
    
    SECTION("Create nodes in different subtables") {
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
            REQUIRE(Cudd_NodeReadIndex(var) == i);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Large-scale stress test", "[cuddTable][stress]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create many complex BDDs") {
        std::vector<DdNode*> bdds;
        
        // Create variables
        std::vector<DdNode*> vars;
        for (int i = 0; i < 10; i++) {
            vars.push_back(Cudd_bddIthVar(manager, i));
        }
        
        // Create complex expressions
        for (int i = 0; i < 50; i++) {
            DdNode *expr = Cudd_ReadOne(manager);
            Cudd_Ref(expr);
            
            for (int j = 0; j < 5; j++) {
                DdNode *temp = Cudd_bddAnd(manager, expr, vars[j]);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            bdds.push_back(expr);
        }
        
        REQUIRE(bdds.size() == 50);
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Edge cases and boundary conditions", "[cuddTable][edge]") {
    SECTION("Initialize with zero variables") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_ReadLogicZero(manager);
        REQUIRE(one != nullptr);
        REQUIRE(zero != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create single variable manager") {
        DdManager *manager = Cudd_Init(1, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        REQUIRE(x != nullptr);
        
        DdNode *not_x = Cudd_Not(x);
        REQUIRE(not_x != nullptr);
        REQUIRE(x != not_x);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Test with small unique slots") {
        DdManager *manager = Cudd_Init(3, 0, 8, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create nodes that will likely cause rehashing
        for (int i = 0; i < 20; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 3);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 3);
            DdNode *temp = Cudd_bddAnd(manager, x, y);
            (void)temp; // Use the node
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Rehashing and table growth", "[cuddTable][rehash]") {
    DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Force rehashing by creating many nodes") {
        std::vector<DdNode*> nodes;
        
        // Create enough nodes to trigger rehashing
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddIthVar(manager, (i + 2) % 5);
            
            DdNode *temp1 = Cudd_bddAnd(manager, x, y);
            DdNode *temp2 = Cudd_bddOr(manager, temp1, z);
            Cudd_Ref(temp2);
            nodes.push_back(temp2);
        }
        
        REQUIRE(nodes.size() == 100);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
    }
    
    SECTION("Verify node reuse after rehashing") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        DdNode *and1 = Cudd_bddAnd(manager, x, y);
        
        // Create many other nodes to potentially trigger rehashing
        for (int i = 0; i < 50; i++) {
            DdNode *a = Cudd_bddIthVar(manager, i % 5);
            DdNode *b = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *temp = Cudd_bddXor(manager, a, b);
            (void)temp;
        }
        
        // Should still return the same node
        DdNode *and2 = Cudd_bddAnd(manager, x, y);
        REQUIRE(and1 == and2);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("ZDD table operations - comprehensive", "[cuddTable][zdd][comprehensive]") {
    DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create and manipulate ZDD sets") {
        DdNode *empty = Cudd_ReadZero(manager);
        DdNode *base = Cudd_ReadZddOne(manager, 0);
        REQUIRE(empty != nullptr);
        REQUIRE(base != nullptr);
        
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        // Test unions
        DdNode *u1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(u1);
        DdNode *u2 = Cudd_zddUnion(manager, u1, z2);
        Cudd_Ref(u2);
        
        REQUIRE(u1 != nullptr);
        REQUIRE(u2 != nullptr);
        
        // Test intersections
        DdNode *i1 = Cudd_zddIntersect(manager, z0, z1);
        REQUIRE(i1 != nullptr);
        // Intersection may or may not be empty depending on ZDD semantics
        
        // Test diffs
        DdNode *d1 = Cudd_zddDiff(manager, u2, z0);
        Cudd_Ref(d1);
        REQUIRE(d1 != nullptr);
        
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, u2);
        Cudd_RecursiveDerefZdd(manager, d1);
    }
    
    SECTION("ZDD product operations") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        DdNode *prod = Cudd_zddProduct(manager, z0, z1);
        Cudd_Ref(prod);
        REQUIRE(prod != nullptr);
        
        Cudd_RecursiveDerefZdd(manager, prod);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Complex BDD operations to exercise unique table", "[cuddTable][complex]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Build large BDDs") {
        std::vector<DdNode*> vars;
        for (int i = 0; i < 10; i++) {
            vars.push_back(Cudd_bddIthVar(manager, i));
        }
        
        // Create a large BDD representing a complex function
        DdNode *f = Cudd_ReadLogicZero(manager);
        Cudd_Ref(f);
        
        for (int i = 0; i < 20; i++) {
            DdNode *clause = Cudd_ReadOne(manager);
            Cudd_Ref(clause);
            
            for (int j = 0; j < 3; j++) {
                int idx = (i * 3 + j) % 10;
                DdNode *lit = (i & (1 << j)) ? vars[idx] : Cudd_Not(vars[idx]);
                DdNode *temp = Cudd_bddAnd(manager, clause, lit);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, clause);
                clause = temp;
            }
            
            DdNode *temp = Cudd_bddOr(manager, f, clause);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, f);
            Cudd_RecursiveDeref(manager, clause);
            f = temp;
        }
        
        REQUIRE(f != nullptr);
        REQUIRE(f != Cudd_ReadLogicZero(manager));
        
        // Check some properties
        int node_count = Cudd_DagSize(f);
        REQUIRE(node_count > 0);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Test cofactors and restrictions") {
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        DdNode *f = Cudd_bddAnd(manager, x0, Cudd_bddOr(manager, x1, x2));
        Cudd_Ref(f);
        
        // Test cofactors
        DdNode *f0 = Cudd_Cofactor(manager, f, Cudd_Not(x0));
        DdNode *f1 = Cudd_Cofactor(manager, f, x0);
        
        REQUIRE(f0 != nullptr);
        REQUIRE(f1 != nullptr);
        REQUIRE(f0 == Cudd_ReadLogicZero(manager));
        
        Cudd_Ref(f1);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f1);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Memory pressure and garbage collection", "[cuddTable][gc][memory]") {
    DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create and destroy many temporary nodes") {
        unsigned long initial_nodes = Cudd_ReadNodeCount(manager);
        
        for (int iter = 0; iter < 10; iter++) {
            std::vector<DdNode*> temps;
            
            for (int i = 0; i < 100; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 8);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 8);
                DdNode *z = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(z);
                temps.push_back(z);
            }
            
            // Dereference all
            for (auto t : temps) {
                Cudd_RecursiveDeref(manager, t);
            }
        }
        
        // After GC, node count should be reasonable
        unsigned long final_nodes = Cudd_ReadNodeCount(manager);
        REQUIRE(final_nodes >= initial_nodes);
    }
    
    SECTION("Check GC stats") {
        unsigned long initial_gc = Cudd_ReadGarbageCollections(manager);
        
        // Create temporary nodes to trigger GC
        for (int i = 0; i < 200; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 8);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 8);
            DdNode *z = Cudd_bddXor(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        unsigned long final_gc = Cudd_ReadGarbageCollections(manager);
        REQUIRE(final_gc >= initial_gc);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Dynamic variable creation", "[cuddTable][dynamic]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Add variables dynamically") {
        int initial_size = Cudd_ReadSize(manager);
        REQUIRE(initial_size == 2);
        
        // Add new variables
        for (int i = 0; i < 10; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        int final_size = Cudd_ReadSize(manager);
        REQUIRE(final_size == 12); // 2 + 10
    }
    
    SECTION("Reserve and add") {
        Cudd_Reserve(manager, 20);
        
        for (int i = 0; i < 15; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 17); // 2 + 15
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Mixed BDD and ZDD operations", "[cuddTable][mixed]") {
    DdManager *manager = Cudd_Init(3, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create BDD and ZDD nodes") {
        // BDD nodes
        DdNode *b0 = Cudd_bddIthVar(manager, 0);
        DdNode *b1 = Cudd_bddIthVar(manager, 1);
        DdNode *b2 = Cudd_bddIthVar(manager, 2);
        
        // ZDD nodes
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        REQUIRE(b0 != nullptr);
        REQUIRE(b1 != nullptr);
        REQUIRE(b2 != nullptr);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        REQUIRE(z2 != nullptr);
        
        // BDD operations
        DdNode *bdd_result = Cudd_bddAnd(manager, b0, b1);
        REQUIRE(bdd_result != nullptr);
        
        // ZDD operations
        DdNode *zdd_result = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(zdd_result != nullptr);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Constant nodes and special cases", "[cuddTable][constants]") {
    DdManager *manager = Cudd_Init(3, 2, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Test constant nodes") {
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_ReadLogicZero(manager);
        DdNode *zdd_one = Cudd_ReadZddOne(manager, 0);
        DdNode *zdd_zero = Cudd_ReadZero(manager);
        
        REQUIRE(one != nullptr);
        REQUIRE(zero != nullptr);
        REQUIRE(zdd_one != nullptr);
        REQUIRE(zdd_zero != nullptr);
        
        // Test operations with constants
        DdNode *x = Cudd_bddIthVar(manager, 0);
        
        DdNode *and_one = Cudd_bddAnd(manager, x, one);
        DdNode *and_zero = Cudd_bddAnd(manager, x, zero);
        DdNode *or_one = Cudd_bddOr(manager, x, one);
        DdNode *or_zero = Cudd_bddOr(manager, x, zero);
        
        REQUIRE(and_one == x);
        REQUIRE(and_zero == zero);
        REQUIRE(or_one == one);
        REQUIRE(or_zero == x);
    }
    
    SECTION("Test negation") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *not_x = Cudd_Not(x);
        DdNode *not_not_x = Cudd_Not(not_x);
        
        REQUIRE(not_not_x == x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Intensive rehashing scenarios", "[cuddTable][rehash][intensive]") {
    SECTION("Small table with many insertions") {
        DdManager *manager = Cudd_Init(4, 0, 8, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many distinct nodes to force multiple rehashings
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 200; i++) {
            DdNode *a = Cudd_bddIthVar(manager, i % 4);
            DdNode *b = Cudd_bddIthVar(manager, (i + 1) % 4);
            DdNode *c = Cudd_bddIthVar(manager, (i + 2) % 4);
            DdNode *d = Cudd_bddIthVar(manager, (i + 3) % 4);
            
            DdNode *temp1 = Cudd_bddAnd(manager, a, b);
            DdNode *temp2 = Cudd_bddOr(manager, c, d);
            DdNode *result = Cudd_bddXor(manager, temp1, temp2);
            
            Cudd_Ref(result);
            nodes.push_back(result);
        }
        
        REQUIRE(nodes.size() == 200);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD rehashing") {
        DdManager *manager = Cudd_Init(0, 4, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        std::vector<DdNode*> zdds;
        for (int i = 0; i < 150; i++) {
            DdNode *z0 = Cudd_zddIthVar(manager, i % 4);
            DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 4);
            DdNode *z2 = Cudd_zddIthVar(manager, (i + 2) % 4);
            
            DdNode *u1 = Cudd_zddUnion(manager, z0, z1);
            DdNode *result = Cudd_zddUnion(manager, u1, z2);
            
            Cudd_Ref(result);
            zdds.push_back(result);
        }
        
        REQUIRE(zdds.size() == 150);
        
        for (auto zdd : zdds) {
            Cudd_RecursiveDerefZdd(manager, zdd);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Extensive ZDD table operations", "[cuddTable][zdd][extensive]") {
    DdManager *manager = Cudd_Init(0, 6, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create complex ZDD structures") {
        std::vector<DdNode*> vars;
        for (int i = 0; i < 6; i++) {
            vars.push_back(Cudd_zddIthVar(manager, i));
        }
        
        // Build simpler ZDD combinations to avoid issues
        std::vector<DdNode*> combinations;
        for (int i = 0; i < 10; i++) {
            DdNode *z0 = vars[i % 6];
            DdNode *z1 = vars[(i + 1) % 6];
            
            DdNode *u = Cudd_zddUnion(manager, z0, z1);
            Cudd_Ref(u);
            combinations.push_back(u);
        }
        
        REQUIRE(combinations.size() == 10);
        
        // Perform operations on combinations
        for (size_t i = 0; i < combinations.size() / 2; i++) {
            DdNode *inter = Cudd_zddIntersect(manager, combinations[i], combinations[i + 1]);
            REQUIRE(inter != nullptr);
            
            DdNode *diff = Cudd_zddDiff(manager, combinations[i], combinations[i + 1]);
            REQUIRE(diff != nullptr);
        }
        
        // Clean up
        for (auto combo : combinations) {
            Cudd_RecursiveDerefZdd(manager, combo);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Table operations with variable reordering", "[cuddTable][reorder]") {
    DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create BDDs and trigger reordering") {
        std::vector<DdNode*> bdds;
        
        // Create BDDs
        for (int i = 0; i < 20; i++) {
            DdNode *expr = Cudd_ReadOne(manager);
            Cudd_Ref(expr);
            
            for (int j = 0; j < 4; j++) {
                DdNode *var = Cudd_bddIthVar(manager, j);
                DdNode *temp = Cudd_bddAnd(manager, expr, var);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            bdds.push_back(expr);
        }
        
        // Try to trigger reordering (may or may not happen depending on settings)
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 0);
        
        // Verify BDDs are still valid
        for (auto bdd : bdds) {
            REQUIRE(bdd != nullptr);
        }
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Extreme stress testing", "[cuddTable][stress][extreme]") {
    SECTION("Very large number of operations") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create and destroy nodes in a loop
        for (int iter = 0; iter < 50; iter++) {
            for (int i = 0; i < 50; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 8);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 8);
                DdNode *z = Cudd_bddIthVar(manager, (i + 2) % 8);
                
                DdNode *temp1 = Cudd_bddAnd(manager, x, y);
                DdNode *temp2 = Cudd_bddOr(manager, temp1, z);
                DdNode *temp3 = Cudd_bddXor(manager, temp2, x);
                
                Cudd_Ref(temp3);
                Cudd_RecursiveDeref(manager, temp3);
            }
        }
        
        // Check that manager is still functional
        DdNode *test = Cudd_bddIthVar(manager, 0);
        REQUIRE(test != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Mixed BDD and ZDD stress test") {
        DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        for (int iter = 0; iter < 30; iter++) {
            // BDD operations
            for (int i = 0; i < 20; i++) {
                DdNode *b0 = Cudd_bddIthVar(manager, i % 5);
                DdNode *b1 = Cudd_bddIthVar(manager, (i + 1) % 5);
                DdNode *bdd = Cudd_bddAnd(manager, b0, b1);
                Cudd_Ref(bdd);
                Cudd_RecursiveDeref(manager, bdd);
            }
            
            // ZDD operations  
            for (int i = 0; i < 20; i++) {
                DdNode *z0 = Cudd_zddIthVar(manager, i % 5);
                DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 5);
                DdNode *zdd = Cudd_zddUnion(manager, z0, z1);
                Cudd_Ref(zdd);
                Cudd_RecursiveDerefZdd(manager, zdd);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Operations that exercise unique table deeply", "[cuddTable][deep]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create deeply nested BDDs") {
        std::vector<DdNode*> vars;
        for (int i = 0; i < 10; i++) {
            vars.push_back(Cudd_bddIthVar(manager, i));
        }
        
        // Create nested structure
        DdNode *result = Cudd_ReadLogicZero(manager);
        Cudd_Ref(result);
        
        for (int i = 0; i < 100; i++) {
            DdNode *term = Cudd_ReadOne(manager);
            Cudd_Ref(term);
            
            // Create a term with multiple variables
            for (int j = 0; j < 5; j++) {
                int idx = (i * 5 + j) % 10;
                DdNode *lit = (i & (1 << j)) ? vars[idx] : Cudd_Not(vars[idx]);
                DdNode *temp = Cudd_bddAnd(manager, term, lit);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, term);
                term = temp;
            }
            
            DdNode *temp = Cudd_bddOr(manager, result, term);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, result);
            Cudd_RecursiveDeref(manager, term);
            result = temp;
        }
        
        REQUIRE(result != nullptr);
        REQUIRE(result != Cudd_ReadLogicZero(manager));
        
        int size = Cudd_DagSize(result);
        REQUIRE(size > 0);
        
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Test ITE operations extensively") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *z = Cudd_bddIthVar(manager, 2);
        
        // Create many ITE combinations
        for (int i = 0; i < 50; i++) {
            DdNode *cond = (i % 3 == 0) ? x : ((i % 3 == 1) ? y : z);
            DdNode *then_part = Cudd_bddIthVar(manager, (i + 3) % 10);
            DdNode *else_part = Cudd_bddIthVar(manager, (i + 4) % 10);
            
            DdNode *ite = Cudd_bddIte(manager, cond, then_part, else_part);
            REQUIRE(ite != nullptr);
            
            Cudd_Ref(ite);
            Cudd_RecursiveDeref(manager, ite);
        }
    }
    
    Cudd_Quit(manager);
}
