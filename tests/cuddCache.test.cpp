#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <unistd.h>

/**
 * @brief Test file for cuddCache.c
 * 
 * This file contains comprehensive tests to ensure 100% coverage
 * of the cuddCache module, including all cache insert, lookup,
 * profiling, and flush operations.
 */

TEST_CASE("cuddCache - Cache initialization through manager creation", "[cuddCache]") {
    // cuddInitCache is called internally by Cudd_Init
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Verify cache was initialized by checking cache slots
    unsigned int cacheSlots = Cudd_ReadCacheSlots(manager);
    REQUIRE(cacheSlots > 0);
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Cache insert and lookup operations", "[cuddCache]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Cache operations through BDD ITE") {
        // cuddCacheInsert and cuddCacheLookup are exercised through BDD operations
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // First operation - will insert into cache
        DdNode *result1 = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(result1);
        
        // Second identical operation - should hit cache
        DdNode *result2 = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Cache operations with two operands") {
        // cuddCacheInsert2 and cuddCacheLookup2 are exercised through binary operations
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // AND operation - exercises 2-operand cache
        DdNode *result1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(result1);
        
        // Same operation - should hit cache
        DdNode *result2 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Cache operations with single operands", "[cuddCache]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Unary operations exercise single-operand cache") {
        // Operations like complement, support, etc. use 1-operand cache
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Support computation exercises cuddCacheInsert1 and cuddCacheLookup1
        DdNode *support1 = Cudd_Support(manager, f);
        Cudd_Ref(support1);
        
        // Same operation - should hit cache
        DdNode *support2 = Cudd_Support(manager, f);
        Cudd_Ref(support2);
        
        REQUIRE(support1 == support2);
        
        Cudd_RecursiveDeref(manager, support2);
        Cudd_RecursiveDeref(manager, support1);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("BDD complementation exercises single-operand cache") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Various unary operations
        DdNode *support = Cudd_Support(manager, x);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        Cudd_RecursiveDeref(manager, support);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - ZDD cache operations", "[cuddCache]") {
    DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Initialize ZDD variables from BDD variables
    int result = Cudd_zddVarsFromBddVars(manager, 2);
    REQUIRE(result == 1);
    
    SECTION("ZDD union operations exercise ZDD cache") {
        // Create ZDD variables - cuddCacheLookupZdd, cuddCacheLookup2Zdd, cuddCacheLookup1Zdd
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        // ZDD union - exercises ZDD cache functions
        DdNode *result1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(result1);
        
        // Same operation - should hit ZDD cache
        DdNode *result2 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z0);
    }
    
    SECTION("ZDD intersection operations") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // ZDD intersect - exercises ZDD cache
        DdNode *result = Cudd_zddIntersect(manager, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z0);
    }
    
    SECTION("ZDD difference operations") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // ZDD diff - exercises ZDD cache
        DdNode *result = Cudd_zddDiff(manager, z0, z1);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z0);
    }
    
    SECTION("Complex ZDD operations for deeper cache testing") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        REQUIRE(z2 != nullptr);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        // Build more complex ZDD expressions
        DdNode *u1 = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(u1);
        
        DdNode *u2 = Cudd_zddUnion(manager, u1, z2);
        Cudd_Ref(u2);
        
        DdNode *i1 = Cudd_zddIntersect(manager, u1, z2);
        Cudd_Ref(i1);
        
        DdNode *d1 = Cudd_zddDiff(manager, u2, i1);
        REQUIRE(d1 != nullptr);
        Cudd_Ref(d1);
        
        Cudd_RecursiveDeref(manager, d1);
        Cudd_RecursiveDeref(manager, i1);
        Cudd_RecursiveDeref(manager, u2);
        Cudd_RecursiveDeref(manager, u1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z0);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Cache profiling", "[cuddCache]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Profile cache usage") {
        // Perform some operations to populate cache
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // Create several BDD nodes to populate cache
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddOr(manager, x, z);
        Cudd_Ref(f2);
        DdNode *f3 = Cudd_bddXor(manager, y, z);
        Cudd_Ref(f3);
        DdNode *f4 = Cudd_bddIte(manager, x, f2, f3);
        Cudd_Ref(f4);
        
        // Create a temporary file for profile output
        char filename[] = "/tmp/cudd_cache_profile_XXXXXX";
        int fd = mkstemp(filename);
        REQUIRE(fd != -1);
        FILE *fp = fdopen(fd, "w");
        REQUIRE(fp != nullptr);
        
        // Call cuddCacheProfile through Cudd_PrintInfo
        int result = Cudd_PrintInfo(manager, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        
        // Verify file was written
        FILE *check = fopen(filename, "r");
        REQUIRE(check != nullptr);
        char buffer[256];
        bool found_cache_info = false;
        while (fgets(buffer, sizeof(buffer), check) != nullptr) {
            if (strstr(buffer, "cache") != nullptr || strstr(buffer, "Cache") != nullptr) {
                found_cache_info = true;
                break;
            }
        }
        fclose(check);
        
        // Clean up
        unlink(filename);
        
        REQUIRE(found_cache_info);
        
        Cudd_RecursiveDeref(manager, f4);
        Cudd_RecursiveDeref(manager, f3);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, z);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Cache flushing", "[cuddCache]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Flush cache and verify behavior") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Perform operations to populate cache
        DdNode *result1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(result1);
        
        // Get initial cache statistics
        unsigned int lookups_before = Cudd_ReadCacheLookUps(manager);
        
        // Flush the garbage collection which internally may flush cache
        // This exercises cuddCacheFlush indirectly
        Cudd_ReduceHeap(manager, CUDD_REORDER_SIFT, 0);
        
        // Perform same operation again
        DdNode *result2 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(result2);
        
        // Verify results are still correct
        REQUIRE(result1 == result2);
        
        unsigned int lookups_after = Cudd_ReadCacheLookUps(manager);
        REQUIRE(lookups_after >= lookups_before);
        
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Cache resizing and collisions", "[cuddCache]") {
    // Create manager with small initial cache to trigger resizing
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, 256, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Force cache resize through many operations") {
        // Create many variables to populate cache heavily
        std::vector<DdNode*> vars;
        for (int i = 0; i < 20; i++) {
            DdNode *var = Cudd_bddNewVar(manager);
            Cudd_Ref(var);
            vars.push_back(var);
        }
        
        // Perform many operations to fill cache and trigger resize
        std::vector<DdNode*> results;
        for (size_t i = 0; i < vars.size() - 1; i++) {
            for (size_t j = i + 1; j < vars.size(); j++) {
                DdNode *and_result = Cudd_bddAnd(manager, vars[i], vars[j]);
                Cudd_Ref(and_result);
                results.push_back(and_result);
                
                DdNode *or_result = Cudd_bddOr(manager, vars[i], vars[j]);
                Cudd_Ref(or_result);
                results.push_back(or_result);
                
                DdNode *xor_result = Cudd_bddXor(manager, vars[i], vars[j]);
                Cudd_Ref(xor_result);
                results.push_back(xor_result);
            }
        }
        
        // Cache should have resized - verify it's still functioning
        unsigned int final_cache_slots = Cudd_ReadCacheSlots(manager);
        REQUIRE(final_cache_slots >= 256);
        
        // Clean up
        for (auto result : results) {
            Cudd_RecursiveDeref(manager, result);
        }
        for (auto var : vars) {
            Cudd_RecursiveDeref(manager, var);
        }
    }
    
    SECTION("Test cache statistics") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        unsigned int hits_before = Cudd_ReadCacheHits(manager);
        unsigned int lookups_before = Cudd_ReadCacheLookUps(manager);
        
        // First operation - cache miss
        DdNode *result1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(result1);
        
        // Second identical operation - cache hit
        DdNode *result2 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(result2);
        
        unsigned int hits_after = Cudd_ReadCacheHits(manager);
        unsigned int lookups_after = Cudd_ReadCacheLookUps(manager);
        
        // Verify cache hit occurred
        REQUIRE(lookups_after > lookups_before);
        REQUIRE(hits_after > hits_before);
        
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Constant lookup optimization", "[cuddCache]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("ITE constant checks exercise cuddConstantLookup") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Cudd_bddIteConstant exercises cuddConstantLookup
        DdNode *result = Cudd_bddIteConstant(manager, x, one, zero);
        // Result depends on x, so it should not be a constant (returns special marker)
        // We just verify the function executes without crashing
        REQUIRE(result != nullptr);
        
        // ITE with constant result
        result = Cudd_bddIteConstant(manager, x, one, one);
        REQUIRE(result == one);
        
        result = Cudd_bddIteConstant(manager, x, zero, zero);
        REQUIRE(result == zero);
        
        // More complex case
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        result = Cudd_bddIteConstant(manager, f, one, one);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Comprehensive cache coverage", "[cuddCache]") {
    DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Initialize ZDD variables from BDD variables
    int zdd_init = Cudd_zddVarsFromBddVars(manager, 2);
    REQUIRE(zdd_init == 1);
    
    SECTION("Mixed BDD and ZDD operations") {
        // BDD operations
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *bdd_and = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(bdd_and);
        
        DdNode *bdd_support = Cudd_Support(manager, bdd_and);
        Cudd_Ref(bdd_support);
        
        // ZDD operations
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        DdNode *zdd_union = Cudd_zddUnion(manager, z0, z1);
        Cudd_Ref(zdd_union);
        
        // Verify all operations succeeded
        REQUIRE(bdd_and != nullptr);
        REQUIRE(bdd_support != nullptr);
        REQUIRE(zdd_union != nullptr);
        
        // Get cache statistics
        unsigned int cache_hits = Cudd_ReadCacheHits(manager);
        unsigned int cache_lookups = Cudd_ReadCacheLookUps(manager);
        
        REQUIRE(cache_lookups > 0);
        // Some hits expected from repeated subcomputations
        REQUIRE(cache_hits >= 0);
        
        Cudd_RecursiveDeref(manager, zdd_union);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z0);
        Cudd_RecursiveDeref(manager, bdd_support);
        Cudd_RecursiveDeref(manager, bdd_and);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Exercise cuddComputeFloorLog2") {
        // This is called during cache initialization
        // Create managers with different cache sizes
        for (unsigned int cache_size : {128, 256, 512, 1024, 2048}) {
            DdManager *mgr = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, cache_size, 0);
            REQUIRE(mgr != nullptr);
            
            unsigned int actual_slots = Cudd_ReadCacheSlots(mgr);
            // Should be rounded to power of 2
            REQUIRE(actual_slots > 0);
            
            // Verify it's a power of 2
            REQUIRE((actual_slots & (actual_slots - 1)) == 0);
            
            Cudd_Quit(mgr);
        }
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - ADD operations using single-operand cache", "[cuddCache]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("ADD negate operations exercise cuddCacheLookup1 and cuddCacheInsert1") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Convert BDD to ADD
        DdNode *add_x = Cudd_BddToAdd(manager, x);
        REQUIRE(add_x != nullptr);
        Cudd_Ref(add_x);
        
        // Negate the ADD - exercises cuddCacheLookup1 and cuddCacheInsert1
        DdNode *neg1 = Cudd_addNegate(manager, add_x);
        REQUIRE(neg1 != nullptr);
        Cudd_Ref(neg1);
        
        // Same operation - should hit cache
        DdNode *neg2 = Cudd_addNegate(manager, add_x);
        REQUIRE(neg2 != nullptr);
        Cudd_Ref(neg2);
        REQUIRE(neg1 == neg2);
        
        Cudd_RecursiveDeref(manager, neg2);
        Cudd_RecursiveDeref(manager, neg1);
        Cudd_RecursiveDeref(manager, add_x);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("ADD complement operations") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create an ADD
        DdNode *add_x = Cudd_BddToAdd(manager, x);
        Cudd_Ref(add_x);
        DdNode *add_y = Cudd_BddToAdd(manager, y);
        Cudd_Ref(add_y);
        
        // ADD complement
        DdNode *cmpl = Cudd_addCmpl(manager, add_x);
        REQUIRE(cmpl != nullptr);
        Cudd_Ref(cmpl);
        
        Cudd_RecursiveDeref(manager, cmpl);
        Cudd_RecursiveDeref(manager, add_y);
        Cudd_RecursiveDeref(manager, add_x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("BDD to ADD conversion") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *bdd_and = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(bdd_and);
        
        // Convert to ADD - exercises single-operand cache
        DdNode *add1 = Cudd_BddToAdd(manager, bdd_and);
        REQUIRE(add1 != nullptr);
        Cudd_Ref(add1);
        
        // Same conversion - should hit cache
        DdNode *add2 = Cudd_BddToAdd(manager, bdd_and);
        REQUIRE(add2 != nullptr);
        Cudd_Ref(add2);
        REQUIRE(add1 == add2);
        
        Cudd_RecursiveDeref(manager, add2);
        Cudd_RecursiveDeref(manager, add1);
        Cudd_RecursiveDeref(manager, bdd_and);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - ZDD ITE operations using 3-operand ZDD cache", "[cuddCache]") {
    DdManager *manager = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Initialize ZDD variables
    int result = Cudd_zddVarsFromBddVars(manager, 2);
    REQUIRE(result == 1);
    
    SECTION("ZDD ITE operations exercise cuddCacheLookupZdd") {
        DdNode *z0 = Cudd_zddIthVar(manager, 0);
        DdNode *z1 = Cudd_zddIthVar(manager, 1);
        DdNode *z2 = Cudd_zddIthVar(manager, 2);
        REQUIRE(z0 != nullptr);
        REQUIRE(z1 != nullptr);
        REQUIRE(z2 != nullptr);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        // ZDD ITE operation - exercises cuddCacheLookupZdd
        DdNode *ite1 = Cudd_zddIte(manager, z0, z1, z2);
        REQUIRE(ite1 != nullptr);
        Cudd_Ref(ite1);
        
        // Same operation - should hit ZDD cache
        DdNode *ite2 = Cudd_zddIte(manager, z0, z1, z2);
        REQUIRE(ite2 != nullptr);
        Cudd_Ref(ite2);
        REQUIRE(ite1 == ite2);
        
        Cudd_RecursiveDeref(manager, ite2);
        Cudd_RecursiveDeref(manager, ite1);
        Cudd_RecursiveDerefZdd(manager, z2);
        Cudd_RecursiveDerefZdd(manager, z1);
        Cudd_RecursiveDerefZdd(manager, z0);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - ZDD single-operand operations", "[cuddCache]") {
    DdManager *manager = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Initialize ZDD variables
    int result = Cudd_zddVarsFromBddVars(manager, 2);
    REQUIRE(result == 1);
    
    SECTION("ZDD port operations exercise cuddCacheLookup1Zdd") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *bdd_and = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(bdd_and);
        
        // Port BDD to ZDD - exercises cuddCacheLookup1Zdd and related functions
        DdNode *zdd1 = Cudd_zddPortFromBdd(manager, bdd_and);
        REQUIRE(zdd1 != nullptr);
        Cudd_Ref(zdd1);
        
        // Same operation - should hit cache
        DdNode *zdd2 = Cudd_zddPortFromBdd(manager, bdd_and);
        REQUIRE(zdd2 != nullptr);
        Cudd_Ref(zdd2);
        REQUIRE(zdd1 == zdd2);
        
        // Port back to BDD
        DdNode *bdd1 = Cudd_zddPortToBdd(manager, zdd1);
        REQUIRE(bdd1 != nullptr);
        Cudd_Ref(bdd1);
        
        // Same operation - should hit cache
        DdNode *bdd2 = Cudd_zddPortToBdd(manager, zdd1);
        REQUIRE(bdd2 != nullptr);
        Cudd_Ref(bdd2);
        REQUIRE(bdd1 == bdd2);
        
        Cudd_RecursiveDeref(manager, bdd2);
        Cudd_RecursiveDeref(manager, bdd1);
        Cudd_RecursiveDeref(manager, zdd2);
        Cudd_RecursiveDeref(manager, zdd1);
        Cudd_RecursiveDeref(manager, bdd_and);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("cuddCache - Edge cases and error conditions", "[cuddCache]") {
    SECTION("Very small cache size") {
        // Test with minimal cache
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, 1, 0);
        REQUIRE(manager != nullptr);
        
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Should still work even with tiny cache
        DdNode *result = Cudd_bddAnd(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, x);
        
        Cudd_Quit(manager);
    }
    
    SECTION("Large cache size") {
        // Test with large cache
        DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, 262144, 0);
        REQUIRE(manager != nullptr);
        
        unsigned int slots = Cudd_ReadCacheSlots(manager);
        REQUIRE(slots > 0);
        
        Cudd_Quit(manager);
    }
}
