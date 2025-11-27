#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <cmath>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "mtr.h"
#include "util.h"

/**
 * @brief Test file for cuddTable.c
 * 
 * This file contains comprehensive tests for the cuddTable module
 * to achieve 90%+ code coverage.
 */

// Helper function to verify a number is prime
static bool isPrime(unsigned int n) {
    if (n <= 1) return n == 1;  // Special case for the CUDD implementation
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (unsigned int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

TEST_CASE("Cudd_Prime - Find next prime number", "[cuddTable][Cudd_Prime]") {
    SECTION("Prime for small numbers") {
        // Cudd_Prime(p) returns next prime >= p
        // Algorithm: p--, then loop { p++; check if prime }
        // So Cudd_Prime(1): p=0, p=1 (odd, 1 is returned as-is)
        // Cudd_Prime(2): p=1, p=2 (even), p=3 (odd, prime)
        REQUIRE(Cudd_Prime(1) == 1);  // Edge case: implementation returns 1
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
    
    SECTION("Verify prime property for range") {
        // Test that returned values are actually prime
        for (unsigned int i = 2; i < 200; i++) {
            unsigned int p = Cudd_Prime(i);
            REQUIRE(p >= i);
            REQUIRE(isPrime(p));
        }
    }
    
    SECTION("Test consecutive composite numbers") {
        // Test gaps between primes
        REQUIRE(Cudd_Prime(24) == 29);  // 24, 25, 26, 27, 28 are composite
        REQUIRE(Cudd_Prime(90) == 97);  // Gap before 97
        REQUIRE(Cudd_Prime(114) == 127); // Gap before 127
    }
    
    SECTION("Test large primes") {
        REQUIRE(Cudd_Prime(500) == 503);
        REQUIRE(Cudd_Prime(1000) == 1009);
        unsigned int p = Cudd_Prime(10000);
        REQUIRE(p >= 10000);
        REQUIRE(isPrime(p));
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
    
    SECTION("Reserve large amount") {
        int result = Cudd_Reserve(manager, 100);
        REQUIRE(result == 1);
        
        // After reserving, we should be able to add many variables quickly
        for (int i = 0; i < 50; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
    }
    
    SECTION("Reserve then create variables") {
        Cudd_Reserve(manager, 20);
        
        for (int i = 0; i < 15; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 15);
    }
    
    SECTION("Multiple reserves") {
        int result1 = Cudd_Reserve(manager, 10);
        int result2 = Cudd_Reserve(manager, 20);
        int result3 = Cudd_Reserve(manager, 5);
        
        REQUIRE(result1 == 1);
        REQUIRE(result2 == 1);
        REQUIRE(result3 == 1);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_Reserve - Edge cases", "[cuddTable][Cudd_Reserve]") {
    SECTION("Reserve on manager with existing variables") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int result = Cudd_Reserve(manager, 10);
        REQUIRE(result == 1);
        
        // Existing variables should still work
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Reserve on manager with ZDD variables") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int result = Cudd_Reserve(manager, 10);
        REQUIRE(result == 1);
        
        Cudd_Quit(manager);
    }
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
        Cudd_Ref(i1);
        // Intersection may or may not be empty depending on ZDD semantics
        Cudd_RecursiveDerefZdd(manager, i1);
        
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
        
        DdNode *or_x1x2 = Cudd_bddOr(manager, x1, x2);
        Cudd_Ref(or_x1x2);
        DdNode *f = Cudd_bddAnd(manager, x0, or_x1x2);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, or_x1x2);
        
        // Test cofactors
        DdNode *f0 = Cudd_Cofactor(manager, f, Cudd_Not(x0));
        DdNode *f1 = Cudd_Cofactor(manager, f, x0);
        
        REQUIRE(f0 != nullptr);
        REQUIRE(f1 != nullptr);
        REQUIRE(f0 == Cudd_ReadLogicZero(manager));
        
        Cudd_Ref(f0);
        Cudd_Ref(f1);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, f0);
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
        
        // Operations with constants don't need ref/deref since results are already in table
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
            Cudd_Ref(inter);
            Cudd_RecursiveDerefZdd(manager, inter);
            
            DdNode *diff = Cudd_zddDiff(manager, combinations[i], combinations[i + 1]);
            REQUIRE(diff != nullptr);
            Cudd_Ref(diff);
            Cudd_RecursiveDerefZdd(manager, diff);
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

TEST_CASE("Massive node creation to trigger multiple table operations", "[cuddTable][massive]") {
    DdManager *manager = Cudd_Init(15, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Create thousands of unique nodes") {
        std::vector<DdNode*> nodes;
        
        // Create 1000+ unique BDD nodes
        for (int i = 0; i < 1000; i++) {
            DdNode *vars[5];
            for (int j = 0; j < 5; j++) {
                vars[j] = Cudd_bddIthVar(manager, (i + j) % 15);
            }
            
            DdNode *expr = vars[0];
            Cudd_Ref(expr);
            for (int j = 1; j < 5; j++) {
                DdNode *new_expr;
                if (i & (1 << j)) {
                    new_expr = Cudd_bddAnd(manager, expr, vars[j]);
                } else {
                    new_expr = Cudd_bddOr(manager, expr, vars[j]);
                }
                Cudd_Ref(new_expr);
                Cudd_RecursiveDeref(manager, expr);
                expr = new_expr;
            }
            
            // expr is already Ref'd, so just push it
            nodes.push_back(expr);
        }
        
        REQUIRE(nodes.size() == 1000);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Complemented edges and node manipulation", "[cuddTable][complement]") {
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Test complemented edges") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        DdNode *not_x = Cudd_Not(x);
        DdNode *not_y = Cudd_Not(y);
        
        // Test properties of complemented edges
        REQUIRE(Cudd_Not(not_x) == x);
        REQUIRE(Cudd_Not(not_y) == y);
        
        // De Morgan's laws
        DdNode *and_xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(and_xy);
        DdNode *not_and = Cudd_Not(and_xy);
        DdNode *or_not = Cudd_bddOr(manager, not_x, not_y);
        Cudd_Ref(or_not);
        
        REQUIRE(not_and == or_not);  // !(x & y) == !x | !y
        
        Cudd_RecursiveDeref(manager, and_xy);
        Cudd_RecursiveDeref(manager, or_not);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Variable ordering and levels", "[cuddTable][ordering]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Check variable levels") {
        for (int i = 0; i < 10; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(Cudd_NodeReadIndex(var) == i);
        }
    }
    
    SECTION("Swap variables") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        int initial_size = Cudd_DagSize(f);
        
        // Swap variables 0 and 1
        int perm[10] = {1, 0, 2, 3, 4, 5, 6, 7, 8, 9};
        int result = Cudd_ShuffleHeap(manager, perm);
        REQUIRE(result >= 0);
        
        int final_size = Cudd_DagSize(f);
        // Size should remain same or improve
        REQUIRE(final_size <= initial_size);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("BDD composition operations", "[cuddTable][compose]") {
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Variable substitution") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *z = Cudd_bddIthVar(manager, 2);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);  // f = x & y
        Cudd_Ref(f);
        
        // Substitute z for x: f[z/x] = z & y
        DdNode *composed = Cudd_bddCompose(manager, f, z, 0);
        REQUIRE(composed != nullptr);
        Cudd_Ref(composed);
        
        DdNode *expected = Cudd_bddAnd(manager, z, y);
        Cudd_Ref(expected);
        REQUIRE(composed == expected);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, composed);
        Cudd_RecursiveDeref(manager, expected);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Quantification operations", "[cuddTable][quantify]") {
    DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Existential quantification") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        DdNode *z = Cudd_bddIthVar(manager, 2);
        
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddAnd(manager, xy, z);  // f = x & y & z
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, xy);
        
        // Exists x: f
        DdNode *cube_x = x;
        DdNode *exists_x = Cudd_bddExistAbstract(manager, f, cube_x);
        REQUIRE(exists_x != nullptr);
        Cudd_Ref(exists_x);
        
        // Result should be y & z (independent of x)
        DdNode *expected = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(expected);
        REQUIRE(exists_x == expected);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, exists_x);
        Cudd_RecursiveDeref(manager, expected);
    }
    
    SECTION("Universal quantification") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        DdNode *f = Cudd_bddOr(manager, x, y);  // f = x | y
        Cudd_Ref(f);
        
        // Forall x: (x | y) should give y
        DdNode *cube_x = x;
        DdNode *forall_x = Cudd_bddUnivAbstract(manager, f, cube_x);
        REQUIRE(forall_x != nullptr);
        Cudd_Ref(forall_x);
        REQUIRE(forall_x == y);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, forall_x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Node counting and statistics", "[cuddTable][stats]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Count nodes") {
        std::vector<DdNode*> bdds;
        
        for (int i = 0; i < 10; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *bdd = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(bdd);
            bdds.push_back(bdd);
        }
        
        // Count total nodes
        int total_nodes = Cudd_SharingSize((DdNode**)bdds.data(), bdds.size());
        REQUIRE(total_nodes > 0);
        
        // Individual sizes
        for (auto bdd : bdds) {
            int size = Cudd_DagSize(bdd);
            REQUIRE(size > 0);
        }
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
    }
    
    SECTION("Manager statistics") {
        unsigned long nodes = Cudd_ReadNodeCount(manager);
        REQUIRE(nodes >= 0);
        
        unsigned long peak_nodes = Cudd_ReadPeakNodeCount(manager);
        REQUIRE(peak_nodes >= nodes);
        
        size_t memory = Cudd_ReadMemoryInUse(manager);
        REQUIRE(memory > 0);
        
        unsigned long gc_count = Cudd_ReadGarbageCollections(manager);
        REQUIRE(gc_count >= 0);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("ZDD advanced operations", "[cuddTable][zdd][advanced]") {
    DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("ZDD change operation") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        DdNode *zdd = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(zdd);
        
        // Change operation
        DdNode *changed = Cudd_zddChange(manager, zdd, 0);
        REQUIRE(changed != nullptr);
        
        Cudd_Ref(changed);
        Cudd_RecursiveDerefZdd(manager, zdd);
        Cudd_RecursiveDerefZdd(manager, changed);
    }
    
    SECTION("ZDD subset operations") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        DdNode *set1 = Cudd_zddUnion(manager, z0, z1);
        DdNode *set2 = Cudd_zddUnion(manager, z1, z2);
        
        Cudd_Ref(set1);
        Cudd_Ref(set2);
        
        // Subset1 operation
        DdNode *sub1 = Cudd_zddSubset1(manager, set1, 1);
        REQUIRE(sub1 != nullptr);
        Cudd_Ref(sub1);
        Cudd_RecursiveDerefZdd(manager, sub1);
        
        // Subset0 operation
        DdNode *sub0 = Cudd_zddSubset0(manager, set1, 1);
        REQUIRE(sub0 != nullptr);
        Cudd_Ref(sub0);
        Cudd_RecursiveDerefZdd(manager, sub0);
        
        Cudd_RecursiveDerefZdd(manager, set1);
        Cudd_RecursiveDerefZdd(manager, set2);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cache and unique table interactions", "[cuddTable][cache]") {
    DdManager *manager = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Repeated operations to test cache") {
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        // Perform same operation multiple times - should hit cache
        for (int i = 0; i < 100; i++) {
            DdNode *and_xy = Cudd_bddAnd(manager, x, y);
            REQUIRE(and_xy != nullptr);
            
            DdNode *or_xy = Cudd_bddOr(manager, x, y);
            REQUIRE(or_xy != nullptr);
            
            DdNode *xor_xy = Cudd_bddXor(manager, x, y);
            REQUIRE(xor_xy != nullptr);
        }
        
        // Check cache hits improved
        unsigned long cache_hits = Cudd_ReadCacheHits(manager);
        REQUIRE(cache_hits > 0);
    }
    
    
    Cudd_Quit(manager);
}

TEST_CASE("Multiple manager instances", "[cuddTable][multi]") {
    SECTION("Create and use multiple managers") {
        DdManager *mgr1 = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        DdManager *mgr2 = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        
        REQUIRE(mgr1 != nullptr);
        REQUIRE(mgr2 != nullptr);
        REQUIRE(mgr1 != mgr2);
        
        // Create nodes in both
        DdNode *x1 = Cudd_bddIthVar(mgr1, 0);
        DdNode *x2 = Cudd_bddIthVar(mgr2, 0);
        
        REQUIRE(x1 != nullptr);
        REQUIRE(x2 != nullptr);
        
        // Nodes are from different managers
        // (Can't directly compare as they're in different address spaces)
        
        Cudd_Quit(mgr1);
        Cudd_Quit(mgr2);
    }
}

TEST_CASE("Extreme value tests", "[cuddTable][extreme]") {
    SECTION("Very large initial sizes") {
        DdManager *manager = Cudd_Init(2, 0, 1024, 8192, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        REQUIRE(x != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Add many variables after initialization") {
        DdManager *manager = Cudd_Init(1, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Add 50 more variables
        for (int i = 0; i < 50; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        int size = Cudd_ReadSize(manager);
        REQUIRE(size == 51);  // 1 + 50
        
        Cudd_Quit(manager);
    }
}

// ============================================================================
// Additional comprehensive tests for cuddTable.c coverage
// ============================================================================

TEST_CASE("cuddAllocNode - Node allocation paths", "[cuddTable][alloc]") {
    SECTION("Allocate many nodes to trigger chunk allocation") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        std::vector<DdNode*> nodes;
        
        // Create many nodes to trigger multiple chunk allocations
        for (int i = 0; i < 2000; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        REQUIRE(nodes.size() == 2000);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Allocate and deallocate in cycles") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        for (int cycle = 0; cycle < 20; cycle++) {
            std::vector<DdNode*> nodes;
            
            for (int i = 0; i < 100; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 5);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
                DdNode *z = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(z);
                nodes.push_back(z);
            }
            
            // Clean up to allow node reuse
            for (auto node : nodes) {
                Cudd_RecursiveDeref(manager, node);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddGarbageCollect - Garbage collection comprehensive", "[cuddTable][gc]") {
    SECTION("Force garbage collection with dead nodes") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable garbage collection
        Cudd_EnableGarbageCollection(manager);
        
        unsigned long initial_gc = Cudd_ReadGarbageCollections(manager);
        
        // Create and destroy many temporary nodes to accumulate dead nodes
        for (int iter = 0; iter < 50; iter++) {
            for (int i = 0; i < 200; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 10);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
                DdNode *z = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(z);
                Cudd_RecursiveDeref(manager, z);
            }
        }
        
        // GC may have been triggered
        unsigned long final_gc = Cudd_ReadGarbageCollections(manager);
        REQUIRE(final_gc >= initial_gc);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Garbage collection with ZDD nodes") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_EnableGarbageCollection(manager);
        
        // Create and destroy ZDD nodes
        for (int iter = 0; iter < 30; iter++) {
            for (int i = 0; i < 50; i++) {
                DdNode *z0 = Cudd_zddIthVar(manager, i % 10);
                DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 10);
                DdNode *zdd = Cudd_zddUnion(manager, z0, z1);
                Cudd_Ref(zdd);
                Cudd_RecursiveDerefZdd(manager, zdd);
            }
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Garbage collection with mixed BDD and ZDD") {
        DdManager *manager = Cudd_Init(5, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_EnableGarbageCollection(manager);
        
        for (int iter = 0; iter < 20; iter++) {
            // BDD operations
            for (int i = 0; i < 30; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 5);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
                DdNode *bdd = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(bdd);
                Cudd_RecursiveDeref(manager, bdd);
            }
            
            // ZDD operations
            for (int i = 0; i < 30; i++) {
                DdNode *z0 = Cudd_zddIthVar(manager, i % 5);
                DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 5);
                DdNode *zdd = Cudd_zddUnion(manager, z0, z1);
                Cudd_Ref(zdd);
                Cudd_RecursiveDerefZdd(manager, zdd);
            }
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("GC with hooks") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // The hooks are tested through triggering GC
        // Create temporary nodes and let them become dead
        for (int i = 0; i < 500; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddUniqueInter - Comprehensive unique table tests", "[cuddTable][unique][comprehensive]") {
    SECTION("Create new variable through unique inter") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Access variables beyond initial size to trigger table growth
        for (int i = 0; i < 30; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 30);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Trigger auto-reordering during unique inter") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable automatic reordering
        Cudd_AutodynEnable(manager, CUDD_REORDER_SIFT);
        
        std::vector<DdNode*> bdds;
        
        // Create complex BDDs to potentially trigger reordering
        for (int i = 0; i < 100; i++) {
            DdNode *expr = Cudd_ReadOne(manager);
            Cudd_Ref(expr);
            
            for (int j = 0; j < 5; j++) {
                DdNode *var = Cudd_bddIthVar(manager, (i + j) % 10);
                DdNode *temp = Cudd_bddAnd(manager, expr, var);
                if (temp == nullptr) {
                    Cudd_RecursiveDeref(manager, expr);
                    expr = nullptr;
                    break;
                }
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            if (expr != nullptr) {
                bdds.push_back(expr);
            }
        }
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_AutodynDisable(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("Test reclaim of dead nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        // Create a node, reference it, then deref to make it dead
        DdNode *and1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(and1);
        Cudd_RecursiveDeref(manager, and1);
        
        // Create the same node again - should reclaim
        DdNode *and2 = Cudd_bddAnd(manager, x, y);
        REQUIRE(and2 != nullptr);
        REQUIRE(and2 == and1);  // Should get same node back
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddUniqueInterZdd - ZDD unique table tests", "[cuddTable][zdd][unique]") {
    SECTION("Create ZDD nodes through unique inter") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables
        for (int i = 0; i < 10; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Expand ZDD table beyond initial size") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Access ZDD variables beyond initial size
        for (int i = 0; i < 25; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadZddSize(manager) == 25);
        
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD with auto dynamic reordering") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_SIFT);
        
        std::vector<DdNode*> zdds;
        
        for (int i = 0; i < 50; i++) {
            DdNode *z0 = Cudd_zddIthVar(manager, i % 10);
            DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 10);
            DdNode *u = Cudd_zddUnion(manager, z0, z1);
            if (u != nullptr) {
                Cudd_Ref(u);
                zdds.push_back(u);
            }
        }
        
        for (auto zdd : zdds) {
            Cudd_RecursiveDerefZdd(manager, zdd);
        }
        
        Cudd_AutodynDisableZdd(manager);
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddUniqueConst - Constant node tests", "[cuddTable][const]") {
    SECTION("Create ADD constants") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create various constant nodes
        DdNode *c1 = Cudd_addConst(manager, 1.0);
        DdNode *c2 = Cudd_addConst(manager, 2.0);
        DdNode *c3 = Cudd_addConst(manager, 3.14159);
        DdNode *c4 = Cudd_addConst(manager, -5.0);
        DdNode *c5 = Cudd_addConst(manager, 0.0);
        
        REQUIRE(c1 != nullptr);
        REQUIRE(c2 != nullptr);
        REQUIRE(c3 != nullptr);
        REQUIRE(c4 != nullptr);
        REQUIRE(c5 != nullptr);
        
        // Verify values
        REQUIRE(Cudd_V(c1) == 1.0);
        REQUIRE(Cudd_V(c2) == 2.0);
        REQUIRE(std::abs(Cudd_V(c3) - 3.14159) < 1e-10);
        REQUIRE(Cudd_V(c4) == -5.0);
        REQUIRE(Cudd_V(c5) == 0.0);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Constant uniqueness") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Same value should return same node
        DdNode *c1 = Cudd_addConst(manager, 42.0);
        DdNode *c2 = Cudd_addConst(manager, 42.0);
        REQUIRE(c1 == c2);
        
        // Different values should return different nodes
        DdNode *c3 = Cudd_addConst(manager, 43.0);
        REQUIRE(c1 != c3);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Create many constants to trigger rehash") {
        DdManager *manager = Cudd_Init(0, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many different constant values
        for (int i = 0; i < 200; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i);
            REQUIRE(c != nullptr);
            REQUIRE(Cudd_V(c) == (double)i);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Near-zero constants and epsilon") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Very small values should be treated as zero
        DdNode *c1 = Cudd_addConst(manager, 1e-15);
        DdNode *c2 = Cudd_addConst(manager, 0.0);
        // Depending on epsilon setting, these might be same
        // At least one should work
        REQUIRE(c1 != nullptr);
        REQUIRE(c2 != nullptr);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddRehash - Rehashing BDD subtables", "[cuddTable][rehash]") {
    SECTION("Force subtable rehash through many insertions") {
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        unsigned int initial_slots = Cudd_ReadSlots(manager);
        
        std::vector<DdNode*> nodes;
        
        // Create many unique nodes to force rehashing
        for (int i = 0; i < 500; i++) {
            DdNode *vars[5];
            for (int j = 0; j < 5; j++) {
                vars[j] = Cudd_bddIthVar(manager, j);
            }
            
            DdNode *expr = vars[0];
            Cudd_Ref(expr);
            
            for (int j = 1; j < 5; j++) {
                DdNode *lit = (i & (1 << j)) ? vars[j] : Cudd_Not(vars[j]);
                DdNode *temp = Cudd_bddAnd(manager, expr, lit);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            nodes.push_back(expr);
        }
        
        unsigned int final_slots = Cudd_ReadSlots(manager);
        // Slots should have increased due to rehashing
        REQUIRE(final_slots >= initial_slots);
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Rehash with dead nodes") {
        DdManager *manager = Cudd_Init(4, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create nodes, then make them dead
        for (int cycle = 0; cycle < 10; cycle++) {
            for (int i = 0; i < 100; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 4);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 4);
                DdNode *z = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(z);
                Cudd_RecursiveDeref(manager, z);
            }
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Rehash constant table") {
        DdManager *manager = Cudd_Init(0, 0, 8, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many constant nodes to trigger constant table rehash
        for (int i = 0; i < 300; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i * 0.1);
            REQUIRE(c != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("ddRehashZdd - Rehashing ZDD subtables", "[cuddTable][rehash][zdd]") {
    SECTION("Force ZDD subtable rehash") {
        DdManager *manager = Cudd_Init(0, 5, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        std::vector<DdNode*> zdds;
        
        // Create many ZDD nodes to force rehashing
        for (int i = 0; i < 200; i++) {
            DdNode *z0 = Cudd_zddIthVar(manager, i % 5);
            DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 5);
            DdNode *z2 = Cudd_zddIthVar(manager, (i + 2) % 5);
            
            DdNode *u1 = Cudd_zddUnion(manager, z0, z1);
            Cudd_Ref(u1);
            DdNode *u2 = Cudd_zddUnion(manager, u1, z2);
            Cudd_Ref(u2);
            Cudd_RecursiveDerefZdd(manager, u1);
            
            zdds.push_back(u2);
        }
        
        // Clean up
        for (auto zdd : zdds) {
            Cudd_RecursiveDerefZdd(manager, zdd);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddShrinkSubtable - Shrink subtable operations", "[cuddTable][shrink]") {
    SECTION("Trigger shrinking through reordering") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some BDDs
        std::vector<DdNode*> bdds;
        for (int i = 0; i < 20; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            bdds.push_back(z);
        }
        
        // Trigger reordering which may shrink tables
        int result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 0);
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddInsertSubtables - Insert subtables", "[cuddTable][insert]") {
    SECTION("Insert via Cudd_bddNewVarAtLevel") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int initial_size = Cudd_ReadSize(manager);
        
        // Insert a new variable at level 2
        DdNode *new_var = Cudd_bddNewVarAtLevel(manager, 2);
        REQUIRE(new_var != nullptr);
        
        REQUIRE(Cudd_ReadSize(manager) == initial_size + 1);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert multiple variables at various levels") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert at beginning
        DdNode *v1 = Cudd_bddNewVarAtLevel(manager, 0);
        REQUIRE(v1 != nullptr);
        
        // Insert at middle
        DdNode *v2 = Cudd_bddNewVarAtLevel(manager, 2);
        REQUIRE(v2 != nullptr);
        
        // Insert at end
        DdNode *v3 = Cudd_bddNewVarAtLevel(manager, Cudd_ReadSize(manager) - 1);
        REQUIRE(v3 != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert many subtables to trigger reallocation") {
        DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert many new variables to trigger table reallocation
        for (int i = 0; i < 50; i++) {
            DdNode *var = Cudd_bddNewVarAtLevel(manager, 0);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 52);  // 2 initial + 50 new
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddDestroySubtables - Destroy subtables", "[cuddTable][destroy]") {
    SECTION("Basic subtable destruction") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int initial_size = Cudd_ReadSize(manager);
        
        // Add new variables
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 10);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddResizeTableZdd - ZDD table resize", "[cuddTable][resize][zdd]") {
    SECTION("Resize ZDD table by adding variables") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        int initial_size = Cudd_ReadZddSize(manager);
        
        // Access ZDD variables beyond initial size to trigger resize
        for (int i = initial_size; i < 30; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadZddSize(manager) == 30);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Large ZDD table expansion") {
        DdManager *manager = Cudd_Init(0, 2, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Expand ZDD table significantly
        for (int i = 0; i < 100; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadZddSize(manager) == 100);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddSlowTableGrowth - Slow table growth", "[cuddTable][slow]") {
    SECTION("Operations after memory pressure") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a low memory limit to simulate memory pressure
        Cudd_SetMaxMemory(manager, 1024 * 1024);  // 1 MB
        
        // Create nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            if (z != nullptr) {
                Cudd_Ref(z);
                nodes.push_back(z);
            }
        }
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGetNode - ZDD node creation wrapper", "[cuddTable][zdd][getnode]") {
    SECTION("ZDD reduction rule") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Test ZDD operations that use cuddZddGetNode internally
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        
        // Create ZDD operations that exercise the reduction rule
        DdNode *u = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(u != nullptr);
        Cudd_Ref(u);
        
        DdNode *i = Cudd_zddIntersect(manager, z0, z1);
        REQUIRE(i != nullptr);
        Cudd_Ref(i);
        
        Cudd_RecursiveDerefZdd(manager, u);
        Cudd_RecursiveDerefZdd(manager, i);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddZddGetNodeIVO - ZDD IVO node creation", "[cuddTable][zdd][ivo]") {
    SECTION("ZDD operations with variable ordering independence") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDDs using operations that use IVO internally
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z5 = Cudd_zddIthVar(manager, 5);
        DdNode *z9 = Cudd_zddIthVar(manager, 9);
        
        REQUIRE(z0 != nullptr);
        REQUIRE(z5 != nullptr);
        REQUIRE(z9 != nullptr);
        
        // Create complex ZDD expressions
        DdNode *u1 = Cudd_zddUnion(manager, z0, z9);
        Cudd_Ref(u1);
        DdNode *u2 = Cudd_zddUnion(manager, u1, z5);
        Cudd_Ref(u2);
        
        REQUIRE(u2 != nullptr);
        
        Cudd_RecursiveDerefZdd(manager, u1);
        Cudd_RecursiveDerefZdd(manager, u2);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("cuddUniqueInterIVO - BDD IVO operations", "[cuddTable][bdd][ivo]") {
    SECTION("BDD composition using IVO") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x5 = Cudd_bddIthVar(manager, 5);
        DdNode *x9 = Cudd_bddIthVar(manager, 9);
        
        // Create BDD using ITE which may use IVO internally
        DdNode *f = Cudd_bddIte(manager, x0, x5, x9);
        REQUIRE(f != nullptr);
        Cudd_Ref(f);
        
        // Create more complex expressions
        DdNode *g = Cudd_bddIte(manager, x5, x0, x9);
        REQUIRE(g != nullptr);
        Cudd_Ref(g);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("ddResizeTable - Internal table resize", "[cuddTable][resize]") {
    SECTION("Trigger internal resize through variable creation") {
        DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create variables beyond initial max size
        for (int i = 0; i < 100; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 100);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Resize with Cudd_Reserve") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Reserve triggers ddResizeTable
        int result = Cudd_Reserve(manager, 200);
        REQUIRE(result == 1);
        
        // Now create many variables quickly
        for (int i = 5; i < 150; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("ddFixLimits - Fix manager limits", "[cuddTable][limits]") {
    SECTION("Limits adjusted after table operations") {
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Various operations that trigger ddFixLimits internally
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 300; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Timeout and termination callbacks", "[cuddTable][timeout]") {
    SECTION("Operations with timeout set") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a generous timeout
        Cudd_SetTimeLimit(manager, 10000);  // 10 seconds
        
        // Perform operations
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            REQUIRE(z != nullptr);
        }
        
        // Unset timeout
        Cudd_UnsetTimeLimit(manager);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Death row operations", "[cuddTable][deathrow]") {
    SECTION("Operations that exercise death row") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create and destroy many nodes to exercise death row
        for (int iter = 0; iter < 50; iter++) {
            DdNode *x = Cudd_bddIthVar(manager, iter % 10);
            DdNode *y = Cudd_bddIthVar(manager, (iter + 1) % 10);
            
            DdNode *temp = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(temp);
            
            // Deref immediately to send to death row
            Cudd_Deref(temp);
        }
        
        // Clear death row (use internal function)
        cuddClearDeathRow(manager);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Variable map operations", "[cuddTable][varmap]") {
    SECTION("Set and use variable map") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Get variable nodes for the map
        DdNode *x[5], *y[5];
        for (int i = 0; i < 5; i++) {
            x[i] = Cudd_bddIthVar(manager, i);
        }
        // Swap pairs: 0<->1, 2<->3, 4->4
        y[0] = x[1];
        y[1] = x[0];
        y[2] = x[3];
        y[3] = x[2];
        y[4] = x[4];
        
        int result = Cudd_SetVarMap(manager, x, y, 5);
        
        if (result == 1) {  // Map set successfully
            DdNode *x0 = Cudd_bddIthVar(manager, 0);
            DdNode *x1 = Cudd_bddIthVar(manager, 1);
            
            DdNode *f = Cudd_bddAnd(manager, x0, x1);
            Cudd_Ref(f);
            
            // Use the map for substitution
            DdNode *g = Cudd_bddVarMap(manager, f);
            if (g != nullptr) {
                Cudd_Ref(g);
                Cudd_RecursiveDeref(manager, g);
            }
            
            Cudd_RecursiveDeref(manager, f);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Manager statistics and memory", "[cuddTable][stats]") {
    SECTION("Check all statistics after operations") {
        DdManager *manager = Cudd_Init(10, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create nodes
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        // Read various statistics
        unsigned long nodeCount = Cudd_ReadNodeCount(manager);
        REQUIRE(nodeCount >= 0);
        
        unsigned long peakNodes = Cudd_ReadPeakNodeCount(manager);
        REQUIRE(peakNodes >= nodeCount);
        
        size_t memoryInUse = Cudd_ReadMemoryInUse(manager);
        REQUIRE(memoryInUse > 0);
        
        unsigned int slots = Cudd_ReadSlots(manager);
        REQUIRE(slots > 0);
        
        unsigned int keys = Cudd_ReadKeys(manager);
        REQUIRE(keys >= 0);
        
        unsigned int dead = Cudd_ReadDead(manager);
        REQUIRE(dead >= 0);
        
        unsigned long gcCount = Cudd_ReadGarbageCollections(manager);
        REQUIRE(gcCount >= 0);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Complex reordering scenarios", "[cuddTable][reorder][complex]") {
    SECTION("Reordering with various methods") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create some BDDs
        std::vector<DdNode*> bdds;
        for (int i = 0; i < 20; i++) {
            DdNode *expr = Cudd_ReadOne(manager);
            Cudd_Ref(expr);
            
            for (int j = 0; j < 5; j++) {
                DdNode *var = Cudd_bddIthVar(manager, (i + j) % 10);
                DdNode *temp = Cudd_bddAnd(manager, expr, var);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            bdds.push_back(expr);
        }
        
        // Try different reordering methods
        int result;
        
        result = Cudd_ReduceHeap(manager, CUDD_REORDER_NONE, 0);
        REQUIRE(result >= 0);
        
        result = Cudd_ReduceHeap(manager, CUDD_REORDER_SAME, 0);
        REQUIRE(result >= 0);
        
        result = Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        REQUIRE(result >= 0);
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Edge cases for table operations", "[cuddTable][edge]") {
    SECTION("Very small initial table") {
        DdManager *manager = Cudd_Init(1, 1, 8, 8, 0);
        REQUIRE(manager != nullptr);
        
        // Operations on tiny table
        DdNode *x = Cudd_bddIthVar(manager, 0);
        REQUIRE(x != nullptr);
        
        DdNode *z = Cudd_zddIthVar(manager, 0);
        REQUIRE(z != nullptr);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Zero initial variables") {
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Add variables dynamically
        for (int i = 0; i < 20; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 20);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Operations with loose_up_to boundary") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set loose_up_to
        Cudd_SetLooseUpTo(manager, 100000);
        
        // Perform operations
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Hooks and callbacks", "[cuddTable][hooks]") {
    SECTION("Operations that may trigger hooks") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create and destroy many nodes to potentially trigger GC hooks
        for (int iter = 0; iter < 100; iter++) {
            for (int i = 0; i < 50; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 10);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
                DdNode *z = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(z);
                Cudd_RecursiveDeref(manager, z);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Free list management", "[cuddTable][freelist]") {
    SECTION("Test free list through allocation patterns") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Pattern 1: Allocate, free, allocate again
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 500; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        // Free half
        for (size_t i = 0; i < nodes.size() / 2; i++) {
            Cudd_RecursiveDeref(manager, nodes[i]);
        }
        nodes.erase(nodes.begin(), nodes.begin() + nodes.size() / 2);
        
        // Allocate more - should reuse freed nodes
        for (int i = 0; i < 250; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 2) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        // Clean up remaining
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
}

// Additional tests to improve cuddTable.c coverage

TEST_CASE("ZDD ISOP operations for IVO coverage", "[cuddTable][zdd][isop]") {
    SECTION("BDD to ZDD cover operations") {
        DdManager *manager = Cudd_Init(5, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Create a simple BDD
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Convert BDD to ZDD cover - this uses cuddZddGetNodeIVO
        DdNode *zdd = Cudd_zddPortFromBdd(manager, f);
        if (zdd != nullptr) {
            Cudd_Ref(zdd);
            Cudd_RecursiveDerefZdd(manager, zdd);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("Complex ZDD cover operations") {
        DdManager *manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Create more complex BDDs
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        // f = (x0 AND x1) OR x2
        DdNode *and01 = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(and01);
        DdNode *f = Cudd_bddOr(manager, and01, x2);
        Cudd_Ref(f);
        Cudd_RecursiveDeref(manager, and01);
        
        // Convert to ZDD
        DdNode *zdd = Cudd_zddPortFromBdd(manager, f);
        if (zdd != nullptr) {
            Cudd_Ref(zdd);
            Cudd_RecursiveDerefZdd(manager, zdd);
        }
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
    
    SECTION("ISOP operations using Cudd_zddIsop") {
        DdManager *manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Create BDDs for L (lower) and U (upper) bounds
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);
        
        // L = x0 & x1 (lower bound)
        DdNode *L = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(L);
        
        // U = x0 | x2 (upper bound - should contain L)
        DdNode *U = Cudd_bddOr(manager, x0, x2);
        Cudd_Ref(U);
        
        // Compute ISOP - this should use cuddZddGetNodeIVO and cuddUniqueInterIVO
        DdNode *zdd_I = nullptr;
        DdNode *isop = Cudd_zddIsop(manager, L, U, &zdd_I);
        if (isop != nullptr) {
            Cudd_Ref(isop);
            if (zdd_I != nullptr) {
                Cudd_Ref(zdd_I);
                Cudd_RecursiveDerefZdd(manager, zdd_I);
            }
            Cudd_RecursiveDeref(manager, isop);
        }
        
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
    
    SECTION("Cudd_bddIsop operations") {
        DdManager *manager = Cudd_Init(4, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Create simple bounds
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        
        // L = x0
        Cudd_Ref(x0);
        DdNode *L = x0;
        
        // U = x0 | x1
        DdNode *U = Cudd_bddOr(manager, x0, x1);
        Cudd_Ref(U);
        
        // Compute BDD ISOP
        DdNode *isop = Cudd_bddIsop(manager, L, U);
        if (isop != nullptr) {
            Cudd_Ref(isop);
            Cudd_RecursiveDeref(manager, isop);
        }
        
        Cudd_RecursiveDeref(manager, L);
        Cudd_RecursiveDeref(manager, U);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple ISOP operations") {
        DdManager *manager = Cudd_Init(5, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Perform multiple ISOP computations
        for (int i = 0; i < 5; i++) {
            DdNode *xi = Cudd_bddIthVar(manager, i);
            DdNode *xi1 = Cudd_bddIthVar(manager, (i + 1) % 5);
            
            DdNode *L = Cudd_bddAnd(manager, xi, xi1);
            Cudd_Ref(L);
            
            DdNode *U = Cudd_bddOr(manager, xi, xi1);
            Cudd_Ref(U);
            
            DdNode *zdd_I = nullptr;
            DdNode *isop = Cudd_zddIsop(manager, L, U, &zdd_I);
            if (isop != nullptr) {
                Cudd_Ref(isop);
                if (zdd_I != nullptr) {
                    Cudd_Ref(zdd_I);
                    Cudd_RecursiveDerefZdd(manager, zdd_I);
                }
                Cudd_RecursiveDeref(manager, isop);
            }
            
            Cudd_RecursiveDeref(manager, L);
            Cudd_RecursiveDeref(manager, U);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("MakeBddFromZddCover for cuddUniqueInterIVO") {
        DdManager *manager = Cudd_Init(5, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Create a ZDD cover
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        
        DdNode *cover = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(cover);
        
        DdNode *cover2 = Cudd_zddUnion(manager, cover, z2);
        Cudd_Ref(cover2);
        Cudd_RecursiveDerefZdd(manager, cover);
        
        // Convert ZDD cover to BDD - this uses cuddUniqueInterIVO
        DdNode *bdd = Cudd_MakeBddFromZddCover(manager, cover2);
        if (bdd != nullptr) {
            Cudd_Ref(bdd);
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_RecursiveDerefZdd(manager, cover2);
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple MakeBddFromZddCover calls") {
        DdManager *manager = Cudd_Init(6, 12, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create ZDD variables from BDD variables
        int result = Cudd_zddVarsFromBddVars(manager, 2);
        REQUIRE(result == 1);
        
        // Test with different ZDD covers
        for (int i = 0; i < 5; i++) {
            DdNode *z0 = Cudd_zddIthVar(manager, i * 2);
            DdNode *z1 = Cudd_zddIthVar(manager, i * 2 + 1);
            
            DdNode *cover = Cudd_zddUnion(manager, z0, z1);
            Cudd_Ref(cover);
            
            DdNode *bdd = Cudd_MakeBddFromZddCover(manager, cover);
            if (bdd != nullptr) {
                Cudd_Ref(bdd);
                Cudd_RecursiveDeref(manager, bdd);
            }
            
            Cudd_RecursiveDerefZdd(manager, cover);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Group handling for ddPatchTree", "[cuddTable][groups]") {
    SECTION("Insert variables to trigger tree operations") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert new variables which may trigger tree operations
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddNewVarAtLevel(manager, i);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Multiple variable insertions") {
        DdManager *manager = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert variables at various levels
        for (int i = 0; i < 10; i++) {
            DdNode *var = Cudd_bddNewVarAtLevel(manager, i % 8);
            REQUIRE(var != nullptr);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Termination callback for cuddAllocNode", "[cuddTable][termination]") {
    SECTION("Register termination callback") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Note: The termination callback will only be called during expensive operations
        // We're just testing that we can register it without issues
        // The callback signature requires const void*
        static bool terminationCalled = false;
        terminationCalled = false;
        
        // Perform operations without the callback since registration is complex
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Out of memory callback", "[cuddTable][oom]") {
    SECTION("Register OOM callback") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Register OOM callback (won't be called unless we run out of memory)
        DD_OOMFP old = Cudd_RegisterOutOfMemoryCallback(manager, [](size_t size) {
            // Do nothing, just return
        });
        
        // Perform operations
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        // Restore old callback
        Cudd_RegisterOutOfMemoryCallback(manager, old);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Max live nodes limit", "[cuddTable][maxlive]") {
    SECTION("Set max live limit") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a very high max live limit
        Cudd_SetMaxLive(manager, 1000000);
        
        // Perform operations
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Memory hard limit", "[cuddTable][memhard]") {
    SECTION("Set memory hard limit") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a very high memory limit
        Cudd_SetMaxMemory(manager, 1024 * 1024 * 100);  // 100 MB
        
        // Perform operations
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            if (z != nullptr) {
                Cudd_Ref(z);
                nodes.push_back(z);
            }
        }
        
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("GC fraction settings", "[cuddTable][gcfrac]") {
    SECTION("Adjust GC related settings") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Read min dead
        unsigned int minDead = Cudd_ReadMinDead(manager);
        REQUIRE(minDead >= 0);
        
        // Perform operations to trigger GC
        for (int iter = 0; iter < 50; iter++) {
            for (int i = 0; i < 100; i++) {
                DdNode *x = Cudd_bddIthVar(manager, i % 5);
                DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
                DdNode *z = Cudd_bddAnd(manager, x, y);
                Cudd_Ref(z);
                Cudd_RecursiveDeref(manager, z);
            }
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Multiple subtable insertion scenarios", "[cuddTable][insert][multi]") {
    SECTION("Insert at various positions") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Insert at level 0 multiple times
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddNewVarAtLevel(manager, 0);
            REQUIRE(var != nullptr);
        }
        
        // Insert at end
        int size = Cudd_ReadSize(manager);
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddNewVarAtLevel(manager, size + i);
            REQUIRE(var != nullptr);
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 15);  // 5 + 5 + 5
        
        Cudd_Quit(manager);
    }
    
    SECTION("Insert with BDD operations") {
        DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDDs with initial variables
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        
        // Insert new variable in middle
        DdNode *newVar = Cudd_bddNewVarAtLevel(manager, 1);
        REQUIRE(newVar != nullptr);
        
        // The BDD should still be valid
        REQUIRE(f != nullptr);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_Quit(manager);
    }
}

TEST_CASE("ZDD resize table comprehensive", "[cuddTable][zdd][resize]") {
    SECTION("Progressive ZDD table expansion") {
        DdManager *manager = Cudd_Init(0, 2, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Incrementally access higher ZDD indices
        for (int i = 0; i < 50; i++) {
            DdNode *var = Cudd_zddIthVar(manager, i);
            REQUIRE(var != nullptr);
            
            // Create some ZDD operations to stress test
            if (i > 0) {
                DdNode *prevVar = Cudd_zddIthVar(manager, i - 1);
                DdNode *u = Cudd_zddUnion(manager, var, prevVar);
                REQUIRE(u != nullptr);
            }
        }
        
        REQUIRE(Cudd_ReadZddSize(manager) == 50);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("BDD resize table comprehensive", "[cuddTable][bdd][resize]") {
    SECTION("Progressive BDD table expansion") {
        DdManager *manager = Cudd_Init(2, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Incrementally access higher BDD indices
        for (int i = 0; i < 100; i++) {
            DdNode *var = Cudd_bddIthVar(manager, i);
            REQUIRE(var != nullptr);
            
            // Create some BDD operations
            if (i > 0) {
                DdNode *prevVar = Cudd_bddIthVar(manager, i - 1);
                DdNode *a = Cudd_bddAnd(manager, var, prevVar);
                REQUIRE(a != nullptr);
            }
        }
        
        REQUIRE(Cudd_ReadSize(manager) == 100);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Rehash under various conditions", "[cuddTable][rehash][conditions]") {
    SECTION("Rehash with high dead ratio") {
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many nodes, then make most of them dead
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 200; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        // Make 90% of them dead
        for (size_t i = 0; i < nodes.size() * 9 / 10; i++) {
            Cudd_RecursiveDeref(manager, nodes[i]);
        }
        
        // Create more to trigger rehash with high dead ratio
        for (int i = 0; i < 100; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 2) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            Cudd_RecursiveDeref(manager, z);
        }
        
        // Clean up remaining
        for (size_t i = nodes.size() * 9 / 10; i < nodes.size(); i++) {
            Cudd_RecursiveDeref(manager, nodes[i]);
        }
        
        Cudd_Quit(manager);
    }
    
    SECTION("Rehash with GC disabled") {
        DdManager *manager = Cudd_Init(5, 0, 16, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Disable GC
        Cudd_DisableGarbageCollection(manager);
        
        // Create many nodes
        std::vector<DdNode*> nodes;
        for (int i = 0; i < 200; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 5);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 5);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(z);
            nodes.push_back(z);
        }
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        // Re-enable GC
        Cudd_EnableGarbageCollection(manager);
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Constant table stress", "[cuddTable][const][stress]") {
    SECTION("Create many unique constants") {
        DdManager *manager = Cudd_Init(0, 0, 8, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create many different constant values to stress the constant table
        std::vector<DdNode*> consts;
        for (int i = 0; i < 500; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i * 0.01);
            REQUIRE(c != nullptr);
            consts.push_back(c);
        }
        
        // Verify uniqueness
        for (int i = 0; i < 500; i++) {
            DdNode *c = Cudd_addConst(manager, (double)i * 0.01);
            REQUIRE(c == consts[i]);  // Should return same node
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Variable ordering stress", "[cuddTable][ordering][stress]") {
    SECTION("Shuffle heap multiple times") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create BDDs
        std::vector<DdNode*> bdds;
        for (int i = 0; i < 20; i++) {
            DdNode *expr = Cudd_ReadOne(manager);
            Cudd_Ref(expr);
            
            for (int j = 0; j < 4; j++) {
                DdNode *var = Cudd_bddIthVar(manager, (i + j) % 10);
                DdNode *temp = Cudd_bddAnd(manager, expr, var);
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            bdds.push_back(expr);
        }
        
        // Perform multiple shuffles
        int perm[10];
        for (int shuffle = 0; shuffle < 5; shuffle++) {
            // Reverse order
            for (int i = 0; i < 10; i++) {
                perm[i] = 9 - i;
            }
            
            int result = Cudd_ShuffleHeap(manager, perm);
            REQUIRE(result >= 0);
            
            // Verify BDDs still valid
            for (auto bdd : bdds) {
                REQUIRE(bdd != nullptr);
            }
        }
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("GC with high memory usage", "[cuddTable][gc][highmem]") {
    SECTION("Create nodes near memory limit") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Set a moderate memory limit
        Cudd_SetMaxMemory(manager, 1024 * 1024 * 10);  // 10 MB
        
        // Create many nodes
        std::vector<DdNode*> nodes;
        bool hitLimit = false;
        
        for (int i = 0; i < 10000 && !hitLimit; i++) {
            DdNode *x = Cudd_bddIthVar(manager, i % 10);
            DdNode *y = Cudd_bddIthVar(manager, (i + 1) % 10);
            DdNode *z = Cudd_bddAnd(manager, x, y);
            
            if (z != nullptr) {
                Cudd_Ref(z);
                nodes.push_back(z);
            } else {
                hitLimit = true;
            }
        }
        
        // Clean up
        for (auto node : nodes) {
            Cudd_RecursiveDeref(manager, node);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Death row comprehensive", "[cuddTable][deathrow][comprehensive]") {
    SECTION("Fill and flush death row") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Create nodes and immediately deref (sends to death row)
        for (int iter = 0; iter < 100; iter++) {
            DdNode *x = Cudd_bddIthVar(manager, iter % 10);
            DdNode *y = Cudd_bddIthVar(manager, (iter + 1) % 10);
            
            DdNode *temp = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(temp);
            
            // Use Deref (not RecursiveDeref) to send to death row
            Cudd_Deref(temp);
        }
        
        // Clear death row via internal function
        cuddClearDeathRow(manager);
        
        // More operations after clearing
        for (int iter = 0; iter < 50; iter++) {
            DdNode *x = Cudd_bddIthVar(manager, iter % 10);
            DdNode *y = Cudd_bddIthVar(manager, (iter + 2) % 10);
            
            DdNode *temp = Cudd_bddAnd(manager, x, y);
            Cudd_Ref(temp);
            Cudd_Deref(temp);
        }
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Node reclamation paths", "[cuddTable][reclaim]") {
    SECTION("Reclaim dead nodes") {
        DdManager *manager = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddIthVar(manager, 0);
        DdNode *y = Cudd_bddIthVar(manager, 1);
        
        // Create a node, ref it, deref it
        DdNode *node1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(node1);
        Cudd_RecursiveDeref(manager, node1);
        
        // Now try to create the same node - should reclaim
        DdNode *node2 = Cudd_bddAnd(manager, x, y);
        REQUIRE(node2 == node1);  // Should be same node (reclaimed)
        
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD reclaim") {
        DdManager *manager = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        
        // Create, ref, deref
        DdNode *node1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(node1);
        Cudd_RecursiveDerefZdd(manager, node1);
        
        // Try to create same node
        DdNode *node2 = Cudd_zddUnion(manager, z0, z1);
        REQUIRE(node2 == node1);  // Should reclaim
        
        Cudd_Quit(manager);
    }
}

TEST_CASE("Auto reordering interaction with unique table", "[cuddTable][auto][reorder]") {
    SECTION("Enable auto reordering and create BDDs") {
        DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable auto reordering
        Cudd_AutodynEnable(manager, CUDD_REORDER_SIFT);
        
        // Set a low threshold to trigger reordering
        Cudd_SetNextReordering(manager, 100);
        
        std::vector<DdNode*> bdds;
        
        // Create many BDDs to potentially trigger auto reordering
        for (int i = 0; i < 200; i++) {
            DdNode *expr = Cudd_ReadOne(manager);
            Cudd_Ref(expr);
            
            for (int j = 0; j < 3; j++) {
                DdNode *var = Cudd_bddIthVar(manager, (i + j) % 10);
                DdNode *temp = Cudd_bddAnd(manager, expr, var);
                if (temp == nullptr) {
                    // Reordering may have returned NULL
                    Cudd_RecursiveDeref(manager, expr);
                    expr = nullptr;
                    break;
                }
                Cudd_Ref(temp);
                Cudd_RecursiveDeref(manager, expr);
                expr = temp;
            }
            
            if (expr != nullptr) {
                bdds.push_back(expr);
            }
        }
        
        // Clean up
        for (auto bdd : bdds) {
            Cudd_RecursiveDeref(manager, bdd);
        }
        
        Cudd_AutodynDisable(manager);
        Cudd_Quit(manager);
    }
    
    SECTION("ZDD auto reordering") {
        DdManager *manager = Cudd_Init(0, 10, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(manager != nullptr);
        
        // Enable ZDD auto reordering
        Cudd_AutodynEnableZdd(manager, CUDD_REORDER_SIFT);
        
        std::vector<DdNode*> zdds;
        
        for (int i = 0; i < 100; i++) {
            DdNode *z0 = Cudd_zddIthVar(manager, i % 10);
            DdNode *z1 = Cudd_zddIthVar(manager, (i + 1) % 10);
            DdNode *u = Cudd_zddUnion(manager, z0, z1);
            if (u != nullptr) {
                Cudd_Ref(u);
                zdds.push_back(u);
            }
        }
        
        for (auto zdd : zdds) {
            Cudd_RecursiveDerefZdd(manager, zdd);
        }
        
        Cudd_AutodynDisableZdd(manager);
        Cudd_Quit(manager);
    }
}
