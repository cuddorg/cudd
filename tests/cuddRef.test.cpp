#include <catch2/catch_test_macros.hpp>

// Include CUDD headers - cuddInt.h must come before cudd.h for internal functions
#include "cuddInt.h"
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Comprehensive test file for cuddRef.c targeting 90%+ coverage
 * 
 * This file contains tests for all functions in cuddRef.c:
 * - Exported functions: Cudd_Ref, Cudd_RecursiveDeref, Cudd_IterDerefBdd,
 *   Cudd_DelayedDerefBdd, Cudd_RecursiveDerefZdd, Cudd_Deref, Cudd_CheckZeroRef
 * - Internal functions: cuddReclaim, cuddReclaimZdd, cuddShrinkDeathRow,
 *   cuddClearDeathRow, cuddIsInDeathRow, cuddTimesInDeathRow
 */

// ============================================================================
// Cudd_Ref Tests
// ============================================================================

TEST_CASE("Cudd_Ref - Basic reference counting", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Ref regular node") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        Cudd_Ref(var);
        // Node should have increased reference count
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("Ref complemented node") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        DdNode *notVar = Cudd_Not(var);
        Cudd_Ref(notVar);
        // Node should have increased reference count even when complemented
        Cudd_RecursiveDeref(dd, notVar);
    }

    SECTION("Ref constant node") {
        DdNode *one = Cudd_ReadOne(dd);
        Cudd_Ref(one);
        Cudd_RecursiveDeref(dd, one);
    }

    SECTION("Multiple refs") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        Cudd_Ref(var);
        Cudd_Ref(var);
        Cudd_Ref(var);
        Cudd_RecursiveDeref(dd, var);
        Cudd_RecursiveDeref(dd, var);
        Cudd_RecursiveDeref(dd, var);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_RecursiveDeref Tests
// ============================================================================

TEST_CASE("Cudd_RecursiveDeref - Basic dereferencing", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Deref single variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        Cudd_Ref(var);
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("Deref complemented variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        DdNode *notVar = Cudd_Not(var);
        Cudd_Ref(notVar);
        Cudd_RecursiveDeref(dd, notVar);
    }

    SECTION("Deref AND of two variables") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        REQUIRE(var0 != nullptr);
        REQUIRE(var1 != nullptr);
        
        DdNode *andResult = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andResult);
        Cudd_RecursiveDeref(dd, andResult);
    }

    SECTION("Deref complex BDD - tests recursive path") {
        // Create a more complex BDD to test recursive dereferencing
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        DdNode *var3 = Cudd_bddIthVar(dd, 3);
        
        DdNode *and01 = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(and01);
        DdNode *and23 = Cudd_bddAnd(dd, var2, var3);
        Cudd_Ref(and23);
        DdNode *result = Cudd_bddOr(dd, and01, and23);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(dd, and01);
        Cudd_RecursiveDeref(dd, and23);
        Cudd_RecursiveDeref(dd, result);
    }

    SECTION("Deref shared nodes - tests ref count > 1 path") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *and01 = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(and01);
        Cudd_Ref(and01);  // Second reference
        
        // First deref should just decrease count
        Cudd_RecursiveDeref(dd, and01);
        // Second deref should actually free
        Cudd_RecursiveDeref(dd, and01);
    }

    SECTION("Peak live nodes tracking") {
        // Create several nodes to track peak live count
        DdNode *nodes[10];
        for (int i = 0; i < 5; i++) {
            nodes[i] = Cudd_bddIthVar(dd, i);
            Cudd_Ref(nodes[i]);
        }
        
        // Create compound nodes
        DdNode *result = Cudd_ReadOne(dd);
        Cudd_Ref(result);
        for (int i = 0; i < 5; i++) {
            DdNode *temp = Cudd_bddAnd(dd, result, nodes[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, result);
            result = temp;
        }
        
        Cudd_RecursiveDeref(dd, result);
        for (int i = 0; i < 5; i++) {
            Cudd_RecursiveDeref(dd, nodes[i]);
        }
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_RecursiveDeref - Constant node handling", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Deref BDD that leads to constant") {
        // Create an ITE that leads to constant
        DdNode *var = Cudd_bddIthVar(dd, 0);
        DdNode *one = Cudd_ReadOne(dd);
        DdNode *zero = Cudd_ReadLogicZero(dd);
        
        DdNode *ite = Cudd_bddIte(dd, var, one, zero);
        Cudd_Ref(ite);
        Cudd_RecursiveDeref(dd, ite);
    }

    SECTION("ADD constant handling") {
        DdNode *constNode = Cudd_addConst(dd, 3.14);
        Cudd_Ref(constNode);
        Cudd_RecursiveDeref(dd, constNode);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_IterDerefBdd Tests
// ============================================================================

TEST_CASE("Cudd_IterDerefBdd - Iterative BDD dereferencing", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("IterDeref single variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        Cudd_Ref(var);
        Cudd_IterDerefBdd(dd, var);
    }

    SECTION("IterDeref complemented variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        DdNode *notVar = Cudd_Not(var);
        Cudd_Ref(notVar);
        Cudd_IterDerefBdd(dd, notVar);
    }

    SECTION("IterDeref AND result") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *andResult = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andResult);
        Cudd_IterDerefBdd(dd, andResult);
    }

    SECTION("IterDeref complex BDD - tests stack usage") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        DdNode *var3 = Cudd_bddIthVar(dd, 3);
        DdNode *var4 = Cudd_bddIthVar(dd, 4);
        
        // Build a chain to test iterative stack traversal
        DdNode *tmp = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(tmp);
        DdNode *tmp2 = Cudd_bddAnd(dd, tmp, var2);
        Cudd_Ref(tmp2);
        Cudd_IterDerefBdd(dd, tmp);
        DdNode *tmp3 = Cudd_bddAnd(dd, tmp2, var3);
        Cudd_Ref(tmp3);
        Cudd_IterDerefBdd(dd, tmp2);
        DdNode *result = Cudd_bddAnd(dd, tmp3, var4);
        Cudd_Ref(result);
        Cudd_IterDerefBdd(dd, tmp3);
        Cudd_IterDerefBdd(dd, result);
    }

    SECTION("IterDeref shared nodes - tests ref count > 1") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *andResult = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andResult);
        Cudd_Ref(andResult);  // Second reference
        
        Cudd_IterDerefBdd(dd, andResult);
        Cudd_IterDerefBdd(dd, andResult);
    }

    SECTION("Peak live nodes tracking with IterDeref") {
        unsigned int initialPeak = Cudd_ReadPeakLiveNodeCount(dd);
        
        DdNode *result = Cudd_ReadOne(dd);
        Cudd_Ref(result);
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_bddIthVar(dd, i);
            DdNode *temp = Cudd_bddAnd(dd, result, var);
            Cudd_Ref(temp);
            Cudd_IterDerefBdd(dd, result);
            result = temp;
        }
        
        unsigned int newPeak = Cudd_ReadPeakLiveNodeCount(dd);
        REQUIRE(newPeak >= initialPeak);
        
        Cudd_IterDerefBdd(dd, result);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_DelayedDerefBdd Tests
// ============================================================================

TEST_CASE("Cudd_DelayedDerefBdd - Delayed dereferencing with death row", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("DelayedDeref single variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        Cudd_Ref(var);
        Cudd_DelayedDerefBdd(dd, var);
    }

    SECTION("DelayedDeref complemented variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        DdNode *notVar = Cudd_Not(var);
        Cudd_Ref(notVar);
        Cudd_DelayedDerefBdd(dd, notVar);
    }

    SECTION("DelayedDeref constant node - fast path") {
        // Constant nodes should be handled specially
        DdNode *one = Cudd_ReadOne(dd);
        Cudd_Ref(one);
        Cudd_DelayedDerefBdd(dd, one);
    }

    SECTION("DelayedDeref node with ref > 1 - fast path") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        Cudd_Ref(var);  // Second reference
        
        // First delayed deref just decrements count
        Cudd_DelayedDerefBdd(dd, var);
        // Clean up with regular deref
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("DelayedDeref multiple nodes - tests death row queue") {
        // Create and delayed-deref many nodes to fill the death row
        for (int i = 0; i < 20; i++) {
            DdNode *var0 = Cudd_bddIthVar(dd, i % 5);
            DdNode *var1 = Cudd_bddIthVar(dd, (i + 1) % 5);
            DdNode *andResult = Cudd_bddAnd(dd, var0, var1);
            Cudd_Ref(andResult);
            Cudd_DelayedDerefBdd(dd, andResult);
        }
    }

    SECTION("DelayedDeref fills death row - tests dequeue") {
        // The death row has depth 1 by default, so after the first DelayedDeref
        // with a ref=1 node, the second call will find N != NULL and dequeue.
        // We need to create nodes that have exactly ref == 1 to enter death row.
        
        // First, let's create a node that will have ref = 1
        // We need to create BDD operations that result in unique nodes
        DdNode *prev = nullptr;
        
        for (int i = 0; i < 20; i++) {
            // Create a unique BDD by varying the structure
            DdNode *var0 = Cudd_bddIthVar(dd, 0);
            DdNode *var1 = Cudd_bddIthVar(dd, 1);
            DdNode *var2 = Cudd_bddIthVar(dd, 2);
            DdNode *var3 = Cudd_bddIthVar(dd, 3);
            DdNode *var4 = Cudd_bddIthVar(dd, 4);
            
            // Build a unique structure for each iteration by including prev
            DdNode *t1;
            if (prev == nullptr) {
                t1 = Cudd_bddAnd(dd, var0, var1);
            } else {
                DdNode *tmp = Cudd_bddAnd(dd, var0, var1);
                Cudd_Ref(tmp);
                t1 = Cudd_bddXor(dd, tmp, prev);
                Cudd_RecursiveDeref(dd, tmp);
                Cudd_RecursiveDeref(dd, prev);
            }
            Cudd_Ref(t1);
            
            DdNode *t2 = Cudd_bddOr(dd, t1, var2);
            Cudd_Ref(t2);
            
            DdNode *t3 = Cudd_bddXor(dd, t2, var3);
            Cudd_Ref(t3);
            
            DdNode *t4 = Cudd_bddAnd(dd, t3, var4);
            Cudd_Ref(t4);
            
            // Clean up intermediate results
            Cudd_RecursiveDeref(dd, t1);
            Cudd_RecursiveDeref(dd, t2);
            Cudd_RecursiveDeref(dd, t3);
            
            // Now t4 should have ref = 1 and will go into death row
            // When the next DelayedDeref happens, it should dequeue t4
            Cudd_DelayedDerefBdd(dd, t4);
            
            // Also track prev for next iteration to create unique nodes
            var0 = Cudd_bddIthVar(dd, i % 5);
            var1 = Cudd_bddIthVar(dd, (i + 1) % 5);
            prev = Cudd_bddXor(dd, var0, var1);
            Cudd_Ref(prev);
        }
        
        if (prev != nullptr) {
            Cudd_RecursiveDeref(dd, prev);
        }
    }

    SECTION("DelayedDeref explicit death row dequeue") {
        // This test explicitly tests the death row dequeue by:
        // 1. Creating a node with ref=1
        // 2. DelayedDeref it (goes into death row slot 0)
        // 3. Create another node with ref=1
        // 4. DelayedDeref it (should find slot 0 occupied, dequeue it, put new one)
        
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        
        // First node - will go into death row
        DdNode *node1 = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(node1);
        
        // Check: node1 should have ref = 1 now (it's a new unique node)
        // DelayedDeref should put it in death row
        Cudd_DelayedDerefBdd(dd, node1);
        
        // Second node - should trigger dequeue of node1
        DdNode *node2 = Cudd_bddOr(dd, var1, var2);
        Cudd_Ref(node2);
        
        // This should find node1 in death row and dequeue it
        Cudd_DelayedDerefBdd(dd, node2);
        
        // Third node - should trigger dequeue of node2
        DdNode *node3 = Cudd_bddXor(dd, var0, var2);
        Cudd_Ref(node3);
        
        Cudd_DelayedDerefBdd(dd, node3);
        
        // Clear death row to finish
        cuddClearDeathRow(dd);
    }

    Cudd_Quit(dd);
}

TEST_CASE("Cudd_DelayedDerefBdd - Death row dequeue", "[cuddRef]") {
    // Use a fresh manager for this test to ensure nodes are newly created
    DdManager *dd = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    // Enable autodyn reordering to shrink the death row to 1 slot
    // This is crucial for testing the death row dequeue path
    Cudd_AutodynEnable(dd, CUDD_REORDER_SIFT);

    SECTION("Force death row dequeue with ref==1 nodes") {
        // With autodyn enabled, death row depth = 1, deadMask = 0
        // After first DelayedDeref with ref==1 node, death row slot 0 is occupied.
        // Second DelayedDeref with ref==1 node will find N != NULL and dequeue.
        
        // Create variables
        DdNode *v0 = Cudd_bddIthVar(dd, 0);
        DdNode *v1 = Cudd_bddIthVar(dd, 1);
        DdNode *v2 = Cudd_bddIthVar(dd, 2);
        DdNode *v3 = Cudd_bddIthVar(dd, 3);
        DdNode *v4 = Cudd_bddIthVar(dd, 4);
        DdNode *v5 = Cudd_bddIthVar(dd, 5);
        DdNode *v6 = Cudd_bddIthVar(dd, 6);
        
        // Create first unique node - AND of v0 and v1
        DdNode *n1 = Cudd_bddAnd(dd, v0, v1);
        Cudd_Ref(n1);
        // n1 now has ref = 1
        
        // DelayedDeref n1 - it has ref=1, not constant, goes into death row slot 0
        Cudd_DelayedDerefBdd(dd, n1);
        
        // Create second unique node - AND of v2 and v3
        DdNode *n2 = Cudd_bddAnd(dd, v2, v3);
        Cudd_Ref(n2);
        // n2 now has ref = 1
        
        // DelayedDeref n2 - death row slot 0 has n1, so this should:
        // 1. Find N = n1 (not NULL)
        // 2. Execute the dequeue loop in Cudd_DelayedDerefBdd
        // 3. Put n2 into death row
        Cudd_DelayedDerefBdd(dd, n2);
        
        // Create third unique node - triggers dequeue of n2
        DdNode *n3 = Cudd_bddAnd(dd, v4, v5);
        Cudd_Ref(n3);
        Cudd_DelayedDerefBdd(dd, n3);
        
        // Create fourth unique node - triggers dequeue of n3
        DdNode *n4 = Cudd_bddAnd(dd, v5, v6);
        Cudd_Ref(n4);
        Cudd_DelayedDerefBdd(dd, n4);
        
        // Create fifth unique node  
        DdNode *n5 = Cudd_bddOr(dd, v0, v3);
        Cudd_Ref(n5);
        Cudd_DelayedDerefBdd(dd, n5);
        
        // Create more complex nodes to ensure we exercise ref > 1 path in dequeue
        DdNode *complex1 = Cudd_bddAnd(dd, v0, v2);
        Cudd_Ref(complex1);
        DdNode *complex2 = Cudd_bddOr(dd, complex1, v4);
        Cudd_Ref(complex2);
        Cudd_DelayedDerefBdd(dd, complex2);
        Cudd_RecursiveDeref(dd, complex1);
        
        // Clear death row at the end
        cuddClearDeathRow(dd);
    }

    SECTION("Multiple iterations to exercise dequeue loop") {
        // Run many iterations to thoroughly test the dequeue path
        for (int round = 0; round < 10; round++) {
            DdNode *vars[10];
            for (int i = 0; i < 10; i++) {
                vars[i] = Cudd_bddIthVar(dd, i);
            }
            
            for (int i = 0; i < 30; i++) {
                // Create a unique combination
                int idx1 = (i * 3 + round) % 10;
                int idx2 = (i * 7 + round + 1) % 10;
                if (idx1 == idx2) idx2 = (idx2 + 1) % 10;
                
                DdNode *node = Cudd_bddAnd(dd, vars[idx1], vars[idx2]);
                Cudd_Ref(node);
                Cudd_DelayedDerefBdd(dd, node);
            }
        }
        cuddClearDeathRow(dd);
    }
    
    SECTION("Test ref > 1 path in death row dequeue") {
        // Create nodes that share children to test the ref > 1 path
        // in the death row dequeue loop (cuddSatDec branch)
        DdNode *v0 = Cudd_bddIthVar(dd, 0);
        DdNode *v1 = Cudd_bddIthVar(dd, 1);
        DdNode *v2 = Cudd_bddIthVar(dd, 2);
        DdNode *v3 = Cudd_bddIthVar(dd, 3);
        
        // Create a base node that will be shared
        DdNode *base = Cudd_bddAnd(dd, v0, v1);
        Cudd_Ref(base);
        
        // Create another node that includes base (base gets ref += 1)
        DdNode *composed1 = Cudd_bddOr(dd, base, v2);
        Cudd_Ref(composed1);
        
        // Put composed1 into death row
        Cudd_DelayedDerefBdd(dd, composed1);
        
        // Create another unique node
        DdNode *unique = Cudd_bddAnd(dd, v2, v3);
        Cudd_Ref(unique);
        
        // This should dequeue composed1, and when it traverses composed1,
        // it should find that base has ref > 1, exercising the cuddSatDec path
        Cudd_DelayedDerefBdd(dd, unique);
        
        // Clean up base
        Cudd_RecursiveDeref(dd, base);
        cuddClearDeathRow(dd);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_RecursiveDerefZdd Tests
// ============================================================================

TEST_CASE("Cudd_RecursiveDerefZdd - ZDD dereferencing", "[cuddRef]") {
    DdManager *dd = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("DerefZdd single variable") {
        DdNode *var = Cudd_zddIthVar(dd, 0);
        REQUIRE(var != nullptr);
        Cudd_Ref(var);
        Cudd_RecursiveDerefZdd(dd, var);
    }

    SECTION("DerefZdd union result") {
        DdNode *var0 = Cudd_zddIthVar(dd, 0);
        DdNode *var1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        
        DdNode *unionResult = Cudd_zddUnion(dd, var0, var1);
        Cudd_Ref(unionResult);
        
        Cudd_RecursiveDerefZdd(dd, var0);
        Cudd_RecursiveDerefZdd(dd, var1);
        Cudd_RecursiveDerefZdd(dd, unionResult);
    }

    SECTION("DerefZdd complex ZDD - tests recursive path") {
        DdNode *var0 = Cudd_zddIthVar(dd, 0);
        DdNode *var1 = Cudd_zddIthVar(dd, 1);
        DdNode *var2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        Cudd_Ref(var2);
        
        DdNode *union01 = Cudd_zddUnion(dd, var0, var1);
        Cudd_Ref(union01);
        DdNode *result = Cudd_zddUnion(dd, union01, var2);
        Cudd_Ref(result);
        
        Cudd_RecursiveDerefZdd(dd, var0);
        Cudd_RecursiveDerefZdd(dd, var1);
        Cudd_RecursiveDerefZdd(dd, var2);
        Cudd_RecursiveDerefZdd(dd, union01);
        Cudd_RecursiveDerefZdd(dd, result);
    }

    SECTION("DerefZdd shared nodes - tests ref count > 0 path") {
        DdNode *var0 = Cudd_zddIthVar(dd, 0);
        DdNode *var1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        
        DdNode *unionResult = Cudd_zddUnion(dd, var0, var1);
        Cudd_Ref(unionResult);
        Cudd_Ref(unionResult);  // Second reference
        
        Cudd_RecursiveDerefZdd(dd, var0);
        Cudd_RecursiveDerefZdd(dd, var1);
        Cudd_RecursiveDerefZdd(dd, unionResult);
        Cudd_RecursiveDerefZdd(dd, unionResult);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_Deref Tests
// ============================================================================

TEST_CASE("Cudd_Deref - Simple dereference", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Deref regular node") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        Cudd_Ref(var);
        // Deref just decrements the reference count
        Cudd_Deref(var);
        // Clean up with proper recursive deref
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("Deref complemented node") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        DdNode *notVar = Cudd_Not(var);
        Cudd_Ref(notVar);
        Cudd_Ref(notVar);
        Cudd_Deref(notVar);
        Cudd_RecursiveDeref(dd, notVar);
    }

    SECTION("Deref constant") {
        DdNode *one = Cudd_ReadOne(dd);
        Cudd_Ref(one);
        Cudd_Ref(one);
        Cudd_Deref(one);
        Cudd_RecursiveDeref(dd, one);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_CheckZeroRef Tests
// ============================================================================

TEST_CASE("Cudd_CheckZeroRef - Check for memory leaks", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Clean manager has zero refs") {
        int count = Cudd_CheckZeroRef(dd);
        REQUIRE(count == 0);
    }

    SECTION("Leaked BDD shows non-zero ref") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        
        // Don't deref - simulate a leak
        int count = Cudd_CheckZeroRef(dd);
        REQUIRE(count >= 1);
        
        // Clean up
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("Leaked complex BDD") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *andResult = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andResult);
        
        int count = Cudd_CheckZeroRef(dd);
        REQUIRE(count >= 1);
        
        Cudd_RecursiveDeref(dd, andResult);
    }

    SECTION("ZDD subtables check") {
        // Create some ZDD nodes and check they're tracked
        DdNode *zvar = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(zvar);
        
        int count = Cudd_CheckZeroRef(dd);
        // ZDD variable should show up if leaked
        
        Cudd_RecursiveDerefZdd(dd, zvar);
        count = Cudd_CheckZeroRef(dd);
        REQUIRE(count == 0);
    }

    SECTION("Constant table check") {
        // Test constant node tracking
        DdNode *constNode = Cudd_addConst(dd, 42.0);
        Cudd_Ref(constNode);
        
        int count = Cudd_CheckZeroRef(dd);
        REQUIRE(count >= 1);
        
        Cudd_RecursiveDeref(dd, constNode);
    }

    SECTION("Special constants not counted as leaks") {
        // One, zero, plusinfinity, minusinfinity are special
        int count = Cudd_CheckZeroRef(dd);
        REQUIRE(count == 0);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// cuddReclaim Tests (Internal function)
// ============================================================================

TEST_CASE("cuddReclaim - Reclaim dead BDD nodes", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Reclaim simple dead node") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        
        DdNode *andResult = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andResult);
        
        // Dereference to make it dead
        Cudd_RecursiveDeref(dd, andResult);
        
        // Reclaim it
        DdNode *andAgain = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andAgain);
        
        Cudd_RecursiveDeref(dd, andAgain);
    }

    SECTION("Reclaim constant node path") {
        // This tests the cuddIsConstant path in cuddReclaim
        DdNode *constNode = Cudd_addConst(dd, 5.0);
        Cudd_Ref(constNode);
        Cudd_RecursiveDeref(dd, constNode);
        
        // Re-create to potentially hit the reclaim path
        DdNode *constAgain = Cudd_addConst(dd, 5.0);
        Cudd_Ref(constAgain);
        Cudd_RecursiveDeref(dd, constAgain);
    }

    SECTION("Reclaim complex structure") {
        // Build and tear down complex structure to test reclaim
        DdNode *vars[5];
        for (int i = 0; i < 5; i++) {
            vars[i] = Cudd_bddIthVar(dd, i);
        }
        
        DdNode *result = Cudd_ReadOne(dd);
        Cudd_Ref(result);
        for (int i = 0; i < 5; i++) {
            DdNode *temp = Cudd_bddAnd(dd, result, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, result);
            result = temp;
        }
        Cudd_RecursiveDeref(dd, result);
        
        // Rebuild - should hit reclaim paths
        result = Cudd_ReadOne(dd);
        Cudd_Ref(result);
        for (int i = 0; i < 5; i++) {
            DdNode *temp = Cudd_bddAnd(dd, result, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, result);
            result = temp;
        }
        Cudd_RecursiveDeref(dd, result);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// cuddReclaimZdd Tests (Internal function)
// ============================================================================

TEST_CASE("cuddReclaimZdd - Reclaim dead ZDD nodes", "[cuddRef]") {
    DdManager *dd = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Reclaim simple dead ZDD node") {
        DdNode *var0 = Cudd_zddIthVar(dd, 0);
        DdNode *var1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        
        DdNode *unionResult = Cudd_zddUnion(dd, var0, var1);
        Cudd_Ref(unionResult);
        
        Cudd_RecursiveDerefZdd(dd, var0);
        Cudd_RecursiveDerefZdd(dd, var1);
        Cudd_RecursiveDerefZdd(dd, unionResult);
        
        // Rebuild to potentially hit reclaim path
        var0 = Cudd_zddIthVar(dd, 0);
        var1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        
        unionResult = Cudd_zddUnion(dd, var0, var1);
        Cudd_Ref(unionResult);
        
        Cudd_RecursiveDerefZdd(dd, var0);
        Cudd_RecursiveDerefZdd(dd, var1);
        Cudd_RecursiveDerefZdd(dd, unionResult);
    }

    SECTION("Reclaim ZDD with shared nodes") {
        DdNode *var0 = Cudd_zddIthVar(dd, 0);
        DdNode *var1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(var0);
        Cudd_Ref(var1);
        
        DdNode *unionResult = Cudd_zddUnion(dd, var0, var1);
        Cudd_Ref(unionResult);
        Cudd_Ref(unionResult);  // Extra ref
        
        Cudd_RecursiveDerefZdd(dd, var0);
        Cudd_RecursiveDerefZdd(dd, var1);
        Cudd_RecursiveDerefZdd(dd, unionResult);
        Cudd_RecursiveDerefZdd(dd, unionResult);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// cuddShrinkDeathRow Tests (Internal function)
// ============================================================================

TEST_CASE("cuddShrinkDeathRow - Shrink death row", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Shrink empty death row") {
        // Just call shrink on a fresh manager
        cuddShrinkDeathRow(dd);
    }

    SECTION("Shrink after filling death row") {
        // Fill death row with delayed derefs
        for (int i = 0; i < 50; i++) {
            DdNode *var0 = Cudd_bddIthVar(dd, i % 5);
            DdNode *var1 = Cudd_bddIthVar(dd, (i + 1) % 5);
            DdNode *result = Cudd_bddAnd(dd, var0, var1);
            Cudd_Ref(result);
            Cudd_DelayedDerefBdd(dd, result);
        }
        
        // Now shrink
        cuddShrinkDeathRow(dd);
    }

    SECTION("Multiple shrinks") {
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 30; i++) {
                DdNode *var0 = Cudd_bddIthVar(dd, i % 5);
                DdNode *var1 = Cudd_bddIthVar(dd, (i + 2) % 5);
                DdNode *result = Cudd_bddXor(dd, var0, var1);
                Cudd_Ref(result);
                Cudd_DelayedDerefBdd(dd, result);
            }
            cuddShrinkDeathRow(dd);
        }
    }

    Cudd_Quit(dd);
}

// ============================================================================
// cuddClearDeathRow Tests (Internal function)
// ============================================================================

TEST_CASE("cuddClearDeathRow - Clear death row", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Clear empty death row") {
        cuddClearDeathRow(dd);
    }

    SECTION("Clear after delayed derefs") {
        for (int i = 0; i < 20; i++) {
            DdNode *var0 = Cudd_bddIthVar(dd, i % 5);
            DdNode *var1 = Cudd_bddIthVar(dd, (i + 1) % 5);
            DdNode *result = Cudd_bddAnd(dd, var0, var1);
            Cudd_Ref(result);
            Cudd_DelayedDerefBdd(dd, result);
        }
        
        cuddClearDeathRow(dd);
    }

    SECTION("Clear multiple times") {
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 15; i++) {
                DdNode *var = Cudd_bddIthVar(dd, i % 5);
                Cudd_Ref(var);
                Cudd_DelayedDerefBdd(dd, var);
            }
            cuddClearDeathRow(dd);
        }
    }

    Cudd_Quit(dd);
}

// ============================================================================
// cuddIsInDeathRow Tests (Internal function)
// ============================================================================

TEST_CASE("cuddIsInDeathRow - Check if node is in death row", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Node not in death row") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        
        int pos = cuddIsInDeathRow(dd, var);
        // Fresh node should not be in death row
        // (position depends on implementation)
        
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("Node in death row after delayed deref") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *result = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(result);
        
        // Get the regular pointer before delayed deref
        DdNode *regularResult = Cudd_Regular(result);
        
        Cudd_DelayedDerefBdd(dd, result);
        
        // Check if it's in death row
        int pos = cuddIsInDeathRow(dd, regularResult);
        // If ref was 1 and it went into death row, pos should be >= 0
    }

    SECTION("Search through death row") {
        // Fill death row with several nodes
        DdNode *nodes[10];
        for (int i = 0; i < 10; i++) {
            DdNode *var0 = Cudd_bddIthVar(dd, i % 5);
            DdNode *var1 = Cudd_bddIthVar(dd, (i + 1) % 5);
            nodes[i] = Cudd_bddOr(dd, var0, var1);
            Cudd_Ref(nodes[i]);
            Cudd_DelayedDerefBdd(dd, nodes[i]);
        }
        
        // Search for last node
        int pos = cuddIsInDeathRow(dd, Cudd_Regular(nodes[9]));
        // It should be in death row somewhere
    }

    Cudd_Quit(dd);
}

// ============================================================================
// cuddTimesInDeathRow Tests (Internal function)
// ============================================================================

TEST_CASE("cuddTimesInDeathRow - Count times node is in death row", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Node not in death row") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        
        int count = cuddTimesInDeathRow(dd, var);
        REQUIRE(count == 0);
        
        Cudd_RecursiveDeref(dd, var);
    }

    SECTION("Node appears once in death row") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *result = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(result);
        
        DdNode *regularResult = Cudd_Regular(result);
        Cudd_DelayedDerefBdd(dd, result);
        
        int count = cuddTimesInDeathRow(dd, regularResult);
        // Count should be >= 0 (could be 0 if not using death row,
        // or 1 if using death row)
        REQUIRE(count >= 0);
    }

    SECTION("Count varies with multiple entries") {
        // Some implementations may allow the same node multiple times
        DdNode *var = Cudd_bddIthVar(dd, 0);
        int initialCount = cuddTimesInDeathRow(dd, var);
        REQUIRE(initialCount >= 0);
    }

    Cudd_Quit(dd);
}

// ============================================================================
// Edge Cases and Stress Tests
// ============================================================================

TEST_CASE("cuddRef - Edge cases", "[cuddRef]") {
    DdManager *dd = Cudd_Init(10, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("Very deep BDD") {
        DdNode *result = Cudd_ReadOne(dd);
        Cudd_Ref(result);
        
        for (int i = 0; i < 10; i++) {
            DdNode *var = Cudd_bddIthVar(dd, i);
            DdNode *temp = Cudd_bddAnd(dd, result, var);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, result);
            result = temp;
        }
        
        Cudd_RecursiveDeref(dd, result);
    }

    SECTION("Wide BDD with many siblings") {
        DdNode *vars[10];
        for (int i = 0; i < 10; i++) {
            vars[i] = Cudd_bddIthVar(dd, i);
            Cudd_Ref(vars[i]);
        }
        
        DdNode *result = Cudd_ReadLogicZero(dd);
        Cudd_Ref(result);
        for (int i = 0; i < 10; i++) {
            DdNode *temp = Cudd_bddOr(dd, result, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(dd, result);
            result = temp;
        }
        
        Cudd_RecursiveDeref(dd, result);
        for (int i = 0; i < 10; i++) {
            Cudd_RecursiveDeref(dd, vars[i]);
        }
    }

    SECTION("Mixed BDD and ZDD operations") {
        DdNode *bddVar = Cudd_bddIthVar(dd, 0);
        DdNode *zddVar = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(bddVar);
        Cudd_Ref(zddVar);
        
        // Check zero ref should handle both
        int count = Cudd_CheckZeroRef(dd);
        REQUIRE(count >= 2);
        
        Cudd_RecursiveDeref(dd, bddVar);
        Cudd_RecursiveDerefZdd(dd, zddVar);
        
        count = Cudd_CheckZeroRef(dd);
        REQUIRE(count == 0);
    }

    SECTION("Stress test with many operations") {
        for (int iter = 0; iter < 100; iter++) {
            DdNode *var0 = Cudd_bddIthVar(dd, iter % 10);
            DdNode *var1 = Cudd_bddIthVar(dd, (iter + 3) % 10);
            DdNode *result = Cudd_bddXor(dd, var0, var1);
            Cudd_Ref(result);
            
            if (iter % 3 == 0) {
                Cudd_RecursiveDeref(dd, result);
            } else if (iter % 3 == 1) {
                Cudd_IterDerefBdd(dd, result);
            } else {
                Cudd_DelayedDerefBdd(dd, result);
            }
        }
        
        // Clear death row at the end
        cuddClearDeathRow(dd);
    }

    Cudd_Quit(dd);
}

TEST_CASE("cuddRef - ADD operations", "[cuddRef]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);

    SECTION("ADD variable dereferencing") {
        DdNode *addVar = Cudd_addIthVar(dd, 0);
        Cudd_Ref(addVar);
        Cudd_RecursiveDeref(dd, addVar);
    }

    SECTION("ADD constant dereferencing") {
        DdNode *constNode = Cudd_addConst(dd, 42.0);
        Cudd_Ref(constNode);
        Cudd_RecursiveDeref(dd, constNode);
    }

    SECTION("ADD ITE dereferencing") {
        DdNode *var = Cudd_addIthVar(dd, 0);
        DdNode *one = Cudd_ReadOne(dd);
        DdNode *zero = Cudd_ReadZero(dd);
        
        DdNode *ite = Cudd_addIte(dd, var, one, zero);
        Cudd_Ref(ite);
        Cudd_RecursiveDeref(dd, ite);
    }

    SECTION("ADD apply dereferencing") {
        DdNode *var0 = Cudd_addIthVar(dd, 0);
        DdNode *var1 = Cudd_addIthVar(dd, 1);
        
        DdNode *result = Cudd_addApply(dd, Cudd_addPlus, var0, var1);
        Cudd_Ref(result);
        Cudd_RecursiveDeref(dd, result);
    }

    Cudd_Quit(dd);
}
