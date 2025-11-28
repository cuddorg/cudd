#include <catch2/catch_test_macros.hpp>
#include "cudd/cudd.h"
#include "cuddInt.h"  // For CUDD_CONST_INDEX

#include "util.h"
#include <cstdio>

/**
 * @brief Comprehensive test file for cuddAPI.c targeting 90% coverage
 */

// Test hook function for hook tests
static int testHookFunction(DdManager *dd, const char *str, void *data) {
    (void)dd; (void)str; (void)data;
    return 1;
}

// ============================================================================
// Variable Creation Functions
// ============================================================================

TEST_CASE("Cudd_addNewVar - ADD variable creation", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Create new ADD variable") {
        DdNode *var = Cudd_addNewVar(dd);
        REQUIRE(var != nullptr);
        REQUIRE(Cudd_ReadSize(dd) == 1);
    }
    
    SECTION("Create multiple ADD variables") {
        for (int i = 0; i < 5; i++) {
            DdNode *var = Cudd_addNewVar(dd);
            REQUIRE(var != nullptr);
        }
        REQUIRE(Cudd_ReadSize(dd) == 5);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addNewVarAtLevel - ADD variable at level", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Create variable at level >= size") {
        DdNode *var = Cudd_addNewVarAtLevel(dd, 10);
        REQUIRE(var != nullptr);
    }
    
    SECTION("Create variable at level < size") {
        DdNode *var = Cudd_addNewVarAtLevel(dd, 1);
        REQUIRE(var != nullptr);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_bddNewVar - BDD variable creation", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *var = Cudd_bddNewVar(dd);
    REQUIRE(var != nullptr);
    REQUIRE(Cudd_ReadSize(dd) == 1);
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_bddNewVarAtLevel - BDD variable at level", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Level >= size") {
        DdNode *var = Cudd_bddNewVarAtLevel(dd, 10);
        REQUIRE(var != nullptr);
    }
    
    SECTION("Level < size") {
        DdNode *var = Cudd_bddNewVarAtLevel(dd, 1);
        REQUIRE(var != nullptr);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_bddIsVar - Check if node is variable", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *var = Cudd_bddIthVar(dd, 0);
    REQUIRE(Cudd_bddIsVar(dd, var) == 1);
    
    DdNode *one = Cudd_ReadOne(dd);
    REQUIRE(Cudd_bddIsVar(dd, one) == 0);
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_addIthVar - ADD i-th variable", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *var = Cudd_addIthVar(dd, 5);
    REQUIRE(var != nullptr);
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_bddIthVar - BDD i-th variable", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Existing variable") {
        DdNode *var = Cudd_bddIthVar(dd, 2);
        REQUIRE(var != nullptr);
    }
    
    SECTION("New variable") {
        DdNode *var = Cudd_bddIthVar(dd, 10);
        REQUIRE(var != nullptr);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_zddIthVar - ZDD i-th variable", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 5, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *var = Cudd_zddIthVar(dd, 2);
    REQUIRE(var != nullptr);
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_zddVarsFromBddVars - Create ZDD vars from BDD vars", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Multiplicity 1") {
        int result = Cudd_zddVarsFromBddVars(dd, 1);
        REQUIRE(result == 1);
    }
    
    SECTION("Multiplicity 2") {
        int result = Cudd_zddVarsFromBddVars(dd, 2);
        REQUIRE(result == 1);
    }
    
    SECTION("Invalid multiplicity") {
        int result = Cudd_zddVarsFromBddVars(dd, 0);
        REQUIRE(result == 0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_ReadMaxIndex - Maximum index", "[cuddAPI]") {
    unsigned int maxIdx = Cudd_ReadMaxIndex();
    REQUIRE(maxIdx > 0);
}

// ============================================================================
// Constant Functions
// ============================================================================

TEST_CASE("Cudd_addConst - ADD constant", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *c = Cudd_addConst(dd, 3.14);
    REQUIRE(c != nullptr);
    REQUIRE(Cudd_V(c) == 3.14);
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_IsConstant and Cudd_IsNonConstant", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    REQUIRE(Cudd_IsConstant(one) == 1);
    
    DdNode *var = Cudd_bddIthVar(dd, 0);
    REQUIRE(Cudd_IsConstant(var) == 0);
    REQUIRE(Cudd_IsNonConstant(var) == 1);
    
    Cudd_Quit(dd);
}

TEST_CASE("Cudd_T, Cudd_E, Cudd_V - Node accessors", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(2, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *var = Cudd_bddIthVar(dd, 0);
    DdNode *t = Cudd_T(var);
    DdNode *e = Cudd_E(var);
    REQUIRE(t != nullptr);
    REQUIRE(e != nullptr);
    
    DdNode *one = Cudd_ReadOne(dd);
    CUDD_VALUE_TYPE val = Cudd_V(one);
    REQUIRE(val == 1.0);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Time Management Functions
// ============================================================================

TEST_CASE("Time management functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("ReadStartTime and SetStartTime") {
        unsigned long st = Cudd_ReadStartTime(dd);
        Cudd_SetStartTime(dd, st + 100);
        REQUIRE(Cudd_ReadStartTime(dd) == st + 100);
    }
    
    SECTION("ResetStartTime") {
        Cudd_ResetStartTime(dd);
        unsigned long st = Cudd_ReadStartTime(dd);
        REQUIRE(st >= 0);
    }
    
    SECTION("ReadElapsedTime") {
        unsigned long elapsed = Cudd_ReadElapsedTime(dd);
        REQUIRE(elapsed >= 0);
    }
    
    SECTION("Time limit functions") {
        unsigned long old = Cudd_SetTimeLimit(dd, 5000);
        REQUIRE(Cudd_ReadTimeLimit(dd) == 5000);
        REQUIRE(Cudd_TimeLimited(dd) == 1);
        
        Cudd_IncreaseTimeLimit(dd, 1000);
        REQUIRE(Cudd_ReadTimeLimit(dd) == 6000);
        
        Cudd_UnsetTimeLimit(dd);
        REQUIRE(Cudd_TimeLimited(dd) == 0);
        
        Cudd_IncreaseTimeLimit(dd, 1000);
        REQUIRE(Cudd_ReadTimeLimit(dd) == 1000);
    }
    
    SECTION("UpdateTimeLimit") {
        Cudd_SetTimeLimit(dd, 10000);
        Cudd_UpdateTimeLimit(dd);
        REQUIRE(Cudd_ReadTimeLimit(dd) <= 10000);
        
        Cudd_UnsetTimeLimit(dd);
        Cudd_UpdateTimeLimit(dd);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Callback Functions
// ============================================================================

TEST_CASE("Callback registration functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Termination callback") {
        Cudd_RegisterTerminationCallback(dd, nullptr, nullptr);
        Cudd_UnregisterTerminationCallback(dd);
    }
    
    SECTION("Out of memory callback") {
        DD_OOMFP old = Cudd_RegisterOutOfMemoryCallback(dd, Cudd_OutOfMemSilent);
        REQUIRE(old != nullptr);
        Cudd_UnregisterOutOfMemoryCallback(dd);
    }
    
    SECTION("Timeout handler") {
        Cudd_RegisterTimeoutHandler(dd, nullptr, nullptr);
        void *arg;
        DD_TOHFP handler = Cudd_ReadTimeoutHandler(dd, &arg);
        REQUIRE(handler == nullptr);
        
        handler = Cudd_ReadTimeoutHandler(dd, nullptr);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Reordering Functions
// ============================================================================

TEST_CASE("Reordering control functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("AutodynEnable/Disable") {
        Cudd_AutodynEnable(dd, CUDD_REORDER_SIFT);
        Cudd_ReorderingType method;
        int status = Cudd_ReorderingStatus(dd, &method);
        REQUIRE(status == 1);
        REQUIRE(method == CUDD_REORDER_SIFT);
        
        Cudd_AutodynEnable(dd, CUDD_REORDER_SAME);
        
        Cudd_AutodynDisable(dd);
        status = Cudd_ReorderingStatus(dd, &method);
        REQUIRE(status == 0);
        
        status = Cudd_ReorderingStatus(dd, nullptr);
    }
    
    SECTION("ZDD AutodynEnable/Disable") {
        Cudd_AutodynEnableZdd(dd, CUDD_REORDER_SIFT);
        Cudd_ReorderingType method;
        int status = Cudd_ReorderingStatusZdd(dd, &method);
        REQUIRE(status == 1);
        
        Cudd_AutodynEnableZdd(dd, CUDD_REORDER_SAME);
        
        Cudd_AutodynDisableZdd(dd);
        status = Cudd_ReorderingStatusZdd(dd, &method);
        REQUIRE(status == 0);
    }
    
    SECTION("Realignment functions") {
        REQUIRE(Cudd_zddRealignmentEnabled(dd) == 0);
        Cudd_zddRealignEnable(dd);
        REQUIRE(Cudd_zddRealignmentEnabled(dd) == 1);
        Cudd_zddRealignDisable(dd);
        REQUIRE(Cudd_zddRealignmentEnabled(dd) == 0);
        
        REQUIRE(Cudd_bddRealignmentEnabled(dd) == 0);
        Cudd_bddRealignEnable(dd);
        REQUIRE(Cudd_bddRealignmentEnabled(dd) == 1);
        Cudd_bddRealignDisable(dd);
        REQUIRE(Cudd_bddRealignmentEnabled(dd) == 0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Read Constant Functions
// ============================================================================

TEST_CASE("Read constant functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    REQUIRE(Cudd_ReadOne(dd) != nullptr);
    REQUIRE(Cudd_ReadZero(dd) != nullptr);
    REQUIRE(Cudd_ReadLogicZero(dd) != nullptr);
    REQUIRE(Cudd_ReadPlusInfinity(dd) != nullptr);
    REQUIRE(Cudd_ReadMinusInfinity(dd) != nullptr);
    REQUIRE(Cudd_ReadBackground(dd) != nullptr);
    
    SECTION("ReadZddOne") {
        DdNode *zddOne = Cudd_ReadZddOne(dd, 0);
        REQUIRE(zddOne != nullptr);
        
        zddOne = Cudd_ReadZddOne(dd, -1);
        REQUIRE(zddOne == nullptr);
        
        zddOne = Cudd_ReadZddOne(dd, 10);
        REQUIRE(zddOne != nullptr);
    }
    
    SECTION("SetBackground") {
        DdNode *zero = Cudd_ReadZero(dd);
        Cudd_SetBackground(dd, zero);
        REQUIRE(Cudd_ReadBackground(dd) == zero);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Cache Functions
// ============================================================================

TEST_CASE("Cache functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    REQUIRE(Cudd_ReadCacheSlots(dd) > 0);
    REQUIRE(Cudd_ReadCacheUsedSlots(dd) >= 0.0);
    REQUIRE(Cudd_ReadCacheLookUps(dd) >= 0.0);
    REQUIRE(Cudd_ReadCacheHits(dd) >= 0.0);
    
    unsigned int minHit = Cudd_ReadMinHit(dd);
    Cudd_SetMinHit(dd, 30);
    REQUIRE(Cudd_ReadMinHit(dd) == 30);
    
    unsigned int maxCache = Cudd_ReadMaxCache(dd);
    REQUIRE(maxCache > 0);
    
    unsigned int maxCacheHard = Cudd_ReadMaxCacheHard(dd);
    Cudd_SetMaxCacheHard(dd, 10000);
    REQUIRE(Cudd_ReadMaxCacheHard(dd) == 10000);
    Cudd_SetMaxCacheHard(dd, 0);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Manager Info Functions
// ============================================================================

TEST_CASE("Manager info read functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    REQUIRE(Cudd_ReadSize(dd) == 5);
    REQUIRE(Cudd_ReadZddSize(dd) == 3);
    REQUIRE(Cudd_ReadSlots(dd) > 0);
    REQUIRE(Cudd_ReadUsedSlots(dd) >= 0.0);
    REQUIRE(Cudd_ExpectedUsedSlots(dd) >= 0.0);
    REQUIRE(Cudd_ReadKeys(dd) > 0);
    REQUIRE(Cudd_ReadDead(dd) >= 0);
    REQUIRE(Cudd_ReadMinDead(dd) >= 0);
    REQUIRE(Cudd_ReadReorderings(dd) >= 0);
    REQUIRE(Cudd_ReadMaxReorderings(dd) > 0);
    REQUIRE(Cudd_ReadReorderingTime(dd) >= 0);
    REQUIRE(Cudd_ReadGarbageCollections(dd) >= 0);
    REQUIRE(Cudd_ReadGarbageCollectionTime(dd) >= 0);
    REQUIRE(Cudd_ReadRecursiveCalls(dd) != 0);
    REQUIRE(Cudd_ReadNodesFreed(dd) != 0);
    REQUIRE(Cudd_ReadNodesDropped(dd) != 0);
    REQUIRE(Cudd_ReadUniqueLookUps(dd) != 0);
    REQUIRE(Cudd_ReadUniqueLinks(dd) != 0);
    REQUIRE(Cudd_ReadMemoryInUse(dd) > 0);
    REQUIRE(Cudd_ReadPeakNodeCount(dd) > 0);
    REQUIRE(Cudd_ReadPeakLiveNodeCount(dd) > 0);
    REQUIRE(Cudd_ReadNodeCount(dd) >= 0);
    REQUIRE(Cudd_zddReadNodeCount(dd) >= 0);
    REQUIRE(Cudd_ReadSwapSteps(dd) != 0);
    
    Cudd_SetMaxReorderings(dd, 100);
    REQUIRE(Cudd_ReadMaxReorderings(dd) == 100);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Sift Parameters
// ============================================================================

TEST_CASE("Sift parameter functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int smv = Cudd_ReadSiftMaxVar(dd);
    Cudd_SetSiftMaxVar(dd, 100);
    REQUIRE(Cudd_ReadSiftMaxVar(dd) == 100);
    
    int sms = Cudd_ReadSiftMaxSwap(dd);
    Cudd_SetSiftMaxSwap(dd, 200);
    REQUIRE(Cudd_ReadSiftMaxSwap(dd) == 200);
    
    double mg = Cudd_ReadMaxGrowth(dd);
    Cudd_SetMaxGrowth(dd, 1.5);
    REQUIRE(Cudd_ReadMaxGrowth(dd) == 1.5);
    
    double mga = Cudd_ReadMaxGrowthAlternate(dd);
    Cudd_SetMaxGrowthAlternate(dd, 1.2);
    REQUIRE(Cudd_ReadMaxGrowthAlternate(dd) == 1.2);
    
    int cycle = Cudd_ReadReorderingCycle(dd);
    Cudd_SetReorderingCycle(dd, 5);
    REQUIRE(Cudd_ReadReorderingCycle(dd) == 5);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Loose Up To Functions
// ============================================================================

TEST_CASE("LooseUpTo functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    unsigned int lut = Cudd_ReadLooseUpTo(dd);
    Cudd_SetLooseUpTo(dd, 50000);
    REQUIRE(Cudd_ReadLooseUpTo(dd) == 50000);
    Cudd_SetLooseUpTo(dd, 0);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Tree Functions
// ============================================================================


// ============================================================================
// Permutation Functions
// ============================================================================

TEST_CASE("Permutation functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("ReadPerm") {
        int perm = Cudd_ReadPerm(dd, 0);
        REQUIRE(perm >= 0);
        
        perm = Cudd_ReadPerm(dd, CUDD_CONST_INDEX);
        REQUIRE(perm == CUDD_CONST_INDEX);
        
        perm = Cudd_ReadPerm(dd, -1);
        REQUIRE(perm == -1);
        
        perm = Cudd_ReadPerm(dd, 100);
        REQUIRE(perm == -1);
    }
    
    SECTION("ReadPermZdd") {
        int perm = Cudd_ReadPermZdd(dd, 0);
        REQUIRE(perm >= 0);
        
        perm = Cudd_ReadPermZdd(dd, CUDD_CONST_INDEX);
        REQUIRE(perm == CUDD_CONST_INDEX);
        
        perm = Cudd_ReadPermZdd(dd, -1);
        REQUIRE(perm == -1);
        
        perm = Cudd_ReadPermZdd(dd, 100);
        REQUIRE(perm == -1);
    }
    
    SECTION("ReadInvPerm") {
        int inv = Cudd_ReadInvPerm(dd, 0);
        REQUIRE(inv >= 0);
        
        inv = Cudd_ReadInvPerm(dd, CUDD_CONST_INDEX);
        REQUIRE(inv == CUDD_CONST_INDEX);
        
        inv = Cudd_ReadInvPerm(dd, -1);
        REQUIRE(inv == -1);
        
        inv = Cudd_ReadInvPerm(dd, 100);
        REQUIRE(inv == -1);
    }
    
    SECTION("ReadInvPermZdd") {
        int inv = Cudd_ReadInvPermZdd(dd, 0);
        REQUIRE(inv >= 0);
        
        inv = Cudd_ReadInvPermZdd(dd, CUDD_CONST_INDEX);
        REQUIRE(inv == CUDD_CONST_INDEX);
        
        inv = Cudd_ReadInvPermZdd(dd, -1);
        REQUIRE(inv == -1);
        
        inv = Cudd_ReadInvPermZdd(dd, 100);
        REQUIRE(inv == -1);
    }
    
    SECTION("NodeReadIndex") {
        DdNode *var = Cudd_bddIthVar(dd, 2);
        unsigned int idx = Cudd_NodeReadIndex(var);
        REQUIRE(idx == 2);
    }
    
    SECTION("ReadVars") {
        DdNode *var = Cudd_ReadVars(dd, 0);
        REQUIRE(var != nullptr);
        
        var = Cudd_ReadVars(dd, -1);
        REQUIRE(var == nullptr);
        
        var = Cudd_ReadVars(dd, 100);
        REQUIRE(var == nullptr);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Epsilon Functions
// ============================================================================

TEST_CASE("Epsilon functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    CUDD_VALUE_TYPE ep = Cudd_ReadEpsilon(dd);
    Cudd_SetEpsilon(dd, 0.001);
    REQUIRE(Cudd_ReadEpsilon(dd) == 0.001);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Groupcheck Functions
// ============================================================================

TEST_CASE("Groupcheck functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    Cudd_AggregationType gc = Cudd_ReadGroupcheck(dd);
    Cudd_SetGroupcheck(dd, CUDD_GROUP_CHECK5);
    REQUIRE(Cudd_ReadGroupcheck(dd) == CUDD_GROUP_CHECK5);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Garbage Collection Functions
// ============================================================================

TEST_CASE("Garbage collection functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    REQUIRE(Cudd_GarbageCollectionEnabled(dd) == 1);
    Cudd_DisableGarbageCollection(dd);
    REQUIRE(Cudd_GarbageCollectionEnabled(dd) == 0);
    Cudd_EnableGarbageCollection(dd);
    REQUIRE(Cudd_GarbageCollectionEnabled(dd) == 1);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Dead Counting Functions
// ============================================================================

TEST_CASE("Dead counting functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    Cudd_TurnOnCountDead(dd);
    REQUIRE(Cudd_DeadAreCounted(dd) == 1);
    Cudd_TurnOffCountDead(dd);
    REQUIRE(Cudd_DeadAreCounted(dd) == 0);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Recomb Functions
// ============================================================================

TEST_CASE("Recomb functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int recomb = Cudd_ReadRecomb(dd);
    Cudd_SetRecomb(dd, 5);
    REQUIRE(Cudd_ReadRecomb(dd) == 5);
    
    int symm = Cudd_ReadSymmviolation(dd);
    Cudd_SetSymmviolation(dd, 10);
    REQUIRE(Cudd_ReadSymmviolation(dd) == 10);
    
    int arc = Cudd_ReadArcviolation(dd);
    Cudd_SetArcviolation(dd, 15);
    REQUIRE(Cudd_ReadArcviolation(dd) == 15);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Population Size Functions
// ============================================================================

TEST_CASE("Population size functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int pop = Cudd_ReadPopulationSize(dd);
    Cudd_SetPopulationSize(dd, 50);
    REQUIRE(Cudd_ReadPopulationSize(dd) == 50);
    
    int xov = Cudd_ReadNumberXovers(dd);
    Cudd_SetNumberXovers(dd, 30);
    REQUIRE(Cudd_ReadNumberXovers(dd) == 30);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Order Randomization Functions
// ============================================================================

TEST_CASE("Order randomization functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    unsigned int rand = Cudd_ReadOrderRandomization(dd);
    Cudd_SetOrderRandomization(dd, 5);
    REQUIRE(Cudd_ReadOrderRandomization(dd) == 5);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Hook Functions
// ============================================================================

TEST_CASE("Hook functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Add and remove hooks") {
        int result = Cudd_AddHook(dd, testHookFunction, CUDD_PRE_GC_HOOK);
        REQUIRE(result == 1);
        
        result = Cudd_AddHook(dd, testHookFunction, CUDD_PRE_GC_HOOK);
        REQUIRE(result == 2);
        
        REQUIRE(Cudd_IsInHook(dd, testHookFunction, CUDD_PRE_GC_HOOK) == 1);
        
        result = Cudd_RemoveHook(dd, testHookFunction, CUDD_PRE_GC_HOOK);
        REQUIRE(result == 1);
        
        REQUIRE(Cudd_IsInHook(dd, testHookFunction, CUDD_PRE_GC_HOOK) == 0);
        
        result = Cudd_RemoveHook(dd, testHookFunction, CUDD_PRE_GC_HOOK);
        REQUIRE(result == 0);
    }
    
    SECTION("All hook types") {
        Cudd_AddHook(dd, testHookFunction, CUDD_POST_GC_HOOK);
        REQUIRE(Cudd_IsInHook(dd, testHookFunction, CUDD_POST_GC_HOOK) == 1);
        Cudd_RemoveHook(dd, testHookFunction, CUDD_POST_GC_HOOK);
        
        Cudd_AddHook(dd, testHookFunction, CUDD_PRE_REORDERING_HOOK);
        REQUIRE(Cudd_IsInHook(dd, testHookFunction, CUDD_PRE_REORDERING_HOOK) == 1);
        Cudd_RemoveHook(dd, testHookFunction, CUDD_PRE_REORDERING_HOOK);
        
        Cudd_AddHook(dd, testHookFunction, CUDD_POST_REORDERING_HOOK);
        REQUIRE(Cudd_IsInHook(dd, testHookFunction, CUDD_POST_REORDERING_HOOK) == 1);
        Cudd_RemoveHook(dd, testHookFunction, CUDD_POST_REORDERING_HOOK);
    }
    
    SECTION("Invalid hook type") {
        int result = Cudd_AddHook(dd, testHookFunction, (Cudd_HookType)99);
        REQUIRE(result == 0);
        
        result = Cudd_RemoveHook(dd, testHookFunction, (Cudd_HookType)99);
        REQUIRE(result == 0);
        
        result = Cudd_IsInHook(dd, testHookFunction, (Cudd_HookType)99);
        REQUIRE(result == 0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Reordering Reporting Functions
// ============================================================================

TEST_CASE("Reordering reporting functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Enable/Disable reordering reporting") {
        int result = Cudd_EnableReorderingReporting(dd);
        REQUIRE(result == 1);
        REQUIRE(Cudd_ReorderingReporting(dd) == 1);
        
        result = Cudd_DisableReorderingReporting(dd);
        REQUIRE(result == 1);
        REQUIRE(Cudd_ReorderingReporting(dd) == 0);
    }
    
    SECTION("Enable/Disable ordering monitoring") {
        int result = Cudd_EnableOrderingMonitoring(dd);
        REQUIRE(result == 1);
        REQUIRE(Cudd_OrderingMonitoring(dd) == 1);
        
        result = Cudd_DisableOrderingMonitoring(dd);
        REQUIRE(result == 1);
        REQUIRE(Cudd_OrderingMonitoring(dd) == 0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Application Hook Functions
// ============================================================================

TEST_CASE("Application hook functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int data = 42;
    Cudd_SetApplicationHook(dd, &data);
    void *hook = Cudd_ReadApplicationHook(dd);
    REQUIRE(hook == &data);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Error Code Functions
// ============================================================================

TEST_CASE("Error code functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    Cudd_ErrorType err = Cudd_ReadErrorCode(dd);
    REQUIRE(err == CUDD_NO_ERROR);
    
    Cudd_ClearErrorCode(dd);
    err = Cudd_ReadErrorCode(dd);
    REQUIRE(err == CUDD_NO_ERROR);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Out of Memory Handler
// ============================================================================

TEST_CASE("Out of memory handler", "[cuddAPI]") {
    DD_OOMFP old = Cudd_InstallOutOfMemoryHandler(Cudd_OutOfMemSilent);
    REQUIRE(old != nullptr);
    Cudd_InstallOutOfMemoryHandler(old);
}

// ============================================================================
// Stdio Functions
// ============================================================================

TEST_CASE("Stdio functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    FILE *out = Cudd_ReadStdout(dd);
    REQUIRE(out != nullptr);
    Cudd_SetStdout(dd, stdout);
    REQUIRE(Cudd_ReadStdout(dd) == stdout);
    
    FILE *err = Cudd_ReadStderr(dd);
    REQUIRE(err != nullptr);
    Cudd_SetStderr(dd, stderr);
    REQUIRE(Cudd_ReadStderr(dd) == stderr);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Next Reordering Functions
// ============================================================================

TEST_CASE("Next reordering functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    unsigned int next = Cudd_ReadNextReordering(dd);
    Cudd_SetNextReordering(dd, 10000);
    REQUIRE(Cudd_ReadNextReordering(dd) == 10000);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Max Live and Max Memory Functions
// ============================================================================

TEST_CASE("Max live and max memory functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    unsigned int maxLive = Cudd_ReadMaxLive(dd);
    Cudd_SetMaxLive(dd, 100000);
    REQUIRE(Cudd_ReadMaxLive(dd) == 100000);
    
    size_t maxMem = Cudd_ReadMaxMemory(dd);
    size_t oldMem = Cudd_SetMaxMemory(dd, 1024*1024*100);
    REQUIRE(Cudd_ReadMaxMemory(dd) == 1024*1024*100);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Variable Binding Functions
// ============================================================================

TEST_CASE("Variable binding functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Bind and unbind") {
        int result = Cudd_bddBindVar(dd, 0);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddVarIsBound(dd, 0) == 1);
        
        result = Cudd_bddUnbindVar(dd, 0);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddVarIsBound(dd, 0) == 0);
    }
    
    SECTION("Invalid index") {
        int result = Cudd_bddBindVar(dd, 100);
        REQUIRE(result == 0);
        
        result = Cudd_bddUnbindVar(dd, 100);
        REQUIRE(result == 0);
        
        result = Cudd_bddVarIsBound(dd, 100);
        REQUIRE(result == 0);
        
        result = Cudd_bddBindVar(dd, -1);
        REQUIRE(result == 0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Variable Type Functions
// ============================================================================

TEST_CASE("Variable type functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Set and check PI var") {
        int result = Cudd_bddSetPiVar(dd, 0);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddIsPiVar(dd, 0) == 1);
        REQUIRE(Cudd_bddIsPsVar(dd, 0) == 0);
        REQUIRE(Cudd_bddIsNsVar(dd, 0) == 0);
    }
    
    SECTION("Set and check PS var") {
        int result = Cudd_bddSetPsVar(dd, 1);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddIsPsVar(dd, 1) == 1);
    }
    
    SECTION("Set and check NS var") {
        int result = Cudd_bddSetNsVar(dd, 2);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddIsNsVar(dd, 2) == 1);
    }
    
    SECTION("Invalid index") {
        REQUIRE(Cudd_bddSetPiVar(dd, 100) == 0);
        REQUIRE(Cudd_bddSetPsVar(dd, 100) == 0);
        REQUIRE(Cudd_bddSetNsVar(dd, 100) == 0);
        REQUIRE(Cudd_bddIsPiVar(dd, 100) == -1);
        REQUIRE(Cudd_bddIsPsVar(dd, 100) == -1);
        REQUIRE(Cudd_bddIsNsVar(dd, 100) == -1);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Pair Index Functions
// ============================================================================

TEST_CASE("Pair index functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int result = Cudd_bddSetPairIndex(dd, 0, 1);
    REQUIRE(result == 1);
    REQUIRE(Cudd_bddReadPairIndex(dd, 0) == 1);
    
    result = Cudd_bddSetPairIndex(dd, 100, 1);
    REQUIRE(result == 0);
    
    int idx = Cudd_bddReadPairIndex(dd, 100);
    REQUIRE(idx == -1);
    
    Cudd_Quit(dd);
}

// ============================================================================
// Variable Grouping Functions
// ============================================================================

TEST_CASE("Variable grouping functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Set var to be grouped") {
        int result = Cudd_bddSetVarToBeGrouped(dd, 0);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddIsVarToBeGrouped(dd, 0) != 0);
        
        result = Cudd_bddResetVarToBeGrouped(dd, 0);
        REQUIRE(result == 1);
    }
    
    SECTION("Set var hard group") {
        int result = Cudd_bddSetVarHardGroup(dd, 1);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddIsVarHardGroup(dd, 1) == 1);
    }
    
    SECTION("Set var to be ungrouped") {
        int result = Cudd_bddSetVarToBeUngrouped(dd, 2);
        REQUIRE(result == 1);
        REQUIRE(Cudd_bddIsVarToBeUngrouped(dd, 2) == 1);
    }
    
    SECTION("Invalid index") {
        REQUIRE(Cudd_bddSetVarToBeGrouped(dd, 100) == 0);
        REQUIRE(Cudd_bddSetVarHardGroup(dd, 100) == 0);
        REQUIRE(Cudd_bddResetVarToBeGrouped(dd, 100) == 0);
        REQUIRE(Cudd_bddSetVarToBeUngrouped(dd, 100) == 0);
        REQUIRE(Cudd_bddIsVarToBeGrouped(dd, 100) == -1);
        REQUIRE(Cudd_bddIsVarToBeUngrouped(dd, 100) == -1);
        REQUIRE(Cudd_bddIsVarHardGroup(dd, 100) == -1);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// PrintInfo Function
// ============================================================================

TEST_CASE("PrintInfo function", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    FILE *fp = fopen("/dev/null", "w");
    REQUIRE(fp != nullptr);
    
    int result = Cudd_PrintInfo(dd, fp);
    REQUIRE(result == 1);
    
    fclose(fp);
    Cudd_Quit(dd);
}

// ============================================================================
// StdPreReordHook and StdPostReordHook Functions
// ============================================================================

TEST_CASE("Standard reorder hook functions", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    FILE *fp = fopen("/dev/null", "w");
    REQUIRE(fp != nullptr);
    Cudd_SetStdout(dd, fp);
    
    int result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_SIFT);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "ZDD", (void*)(uintptr_t)CUDD_REORDER_SIFT_CONVERGE);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_RANDOM);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_SYMM_SIFT);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_LAZY_SIFT);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_GROUP_SIFT);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_WINDOW2);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_ANNEALING);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_GENETIC);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_LINEAR);
    REQUIRE(result == 1);
    
    result = Cudd_StdPreReordHook(dd, "BDD", (void*)(uintptr_t)CUDD_REORDER_EXACT);
    REQUIRE(result == 1);
    
    unsigned long startTime = util_cpu_time();
    result = Cudd_StdPostReordHook(dd, "BDD", (void*)(uintptr_t)startTime);
    REQUIRE(result == 1);
    
    result = Cudd_StdPostReordHook(dd, "ZDD", (void*)(uintptr_t)startTime);
    REQUIRE(result == 1);
    
    fclose(fp);
    Cudd_SetStdout(dd, stdout);
    Cudd_Quit(dd);
}

// ============================================================================
// PrintGroupedOrder Function
// ============================================================================

TEST_CASE("PrintGroupedOrder function", "[cuddAPI]") {
    DdManager *dd = Cudd_Init(5, 3, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    FILE *fp = fopen("/dev/null", "w");
    REQUIRE(fp != nullptr);
    Cudd_SetStdout(dd, fp);
    
    int result = Cudd_PrintGroupedOrder(dd, "BDD", nullptr);
    REQUIRE(result == 1);
    
    result = Cudd_PrintGroupedOrder(dd, "ZDD", nullptr);
    REQUIRE(result == 1);
    
    fclose(fp);
    Cudd_SetStdout(dd, stdout);
    Cudd_Quit(dd);
}
