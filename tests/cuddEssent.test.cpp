#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <cstring>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddEssent.c
 * 
 * This file contains comprehensive tests for the cuddEssent module
 * to ensure 100% code coverage and correct functionality.
 * 
 * Tests cover:
 * - Cudd_FindEssential: Finding essential variables
 * - Cudd_bddIsVarEssential: Checking if a variable is essential
 * - Cudd_FindTwoLiteralClauses: Finding one- and two-literal clauses
 * - Cudd_ReadIthClause: Reading individual clauses
 * - Cudd_PrintTwoLiteralClauses: Printing clauses
 * - Cudd_tlcInfoFree: Freeing TLC info structures
 */

TEST_CASE("Cudd_FindEssential - Basic tests with constants", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Essential variables of constant 1") {
        DdNode *ess = Cudd_FindEssential(manager, one);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // No essential variables
    }
    
    SECTION("Essential variables of constant 0") {
        DdNode *ess = Cudd_FindEssential(manager, zero);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // No essential variables
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindEssential - Single variable functions", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("Function x has x as essential variable (positive)") {
        DdNode *ess = Cudd_FindEssential(manager, x);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == x);  // x is essential
    }
    
    SECTION("Function !x has x as essential variable (negative)") {
        DdNode *notx = Cudd_Not(x);
        DdNode *ess = Cudd_FindEssential(manager, notx);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == notx);  // !x is essential
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindEssential - Two variable functions", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Function x AND y has essential variables") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        // For f = x AND y: when f=1, both x=1 and y=1 must hold
        // Therefore both x and y are essential in positive phase
        REQUIRE(ess != one);  // Has essential variables (both x and y)
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Function x OR y has no essential variables") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // No essential variables
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Function x AND !y has essential variables") {
        DdNode *f = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess != one);  // Has essential variables (x=1 and y=0)
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindEssential - Complex functions with essential variables", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Function x with cofactor 1 has x as essential") {
        // f = x
        DdNode *f = x;
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == x);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Function (x AND y) OR (!x AND z) - no essential variables") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *notx_z = Cudd_bddAnd(manager, Cudd_Not(x), z);
        Cudd_Ref(notx_z);
        DdNode *f = Cudd_bddOr(manager, xy, notx_z);
        Cudd_Ref(f);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // No essential variables
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, notx_z);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIsVarEssential - Basic tests", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    int xid = Cudd_NodeReadIndex(x);
    
    SECTION("Variable x is essential in positive phase for function x") {
        int result = Cudd_bddIsVarEssential(manager, x, xid, 1);
        REQUIRE(result == 1);
    }
    
    SECTION("Variable x is not essential in negative phase for function x") {
        int result = Cudd_bddIsVarEssential(manager, x, xid, 0);
        REQUIRE(result == 0);
    }
    
    SECTION("Variable x is essential in negative phase for function !x") {
        DdNode *notx = Cudd_Not(x);
        int result = Cudd_bddIsVarEssential(manager, notx, xid, 0);
        REQUIRE(result == 1);
    }
    
    SECTION("Variable x is not essential in positive phase for function !x") {
        DdNode *notx = Cudd_Not(x);
        int result = Cudd_bddIsVarEssential(manager, notx, xid, 1);
        REQUIRE(result == 0);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIsVarEssential - Two variable functions", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    int xid = Cudd_NodeReadIndex(x);
    int yid = Cudd_NodeReadIndex(y);
    
    SECTION("Both variables are essential in x AND y with positive phase") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        // For f = x AND y, if f=1 then x=1 and y=1, so both are essential with positive phase
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 1) == 1);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 1) == 1);
        // But they're not essential with negative phase
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 0) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 0) == 0);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("No variable is essential in x OR y") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        // For f = x OR y, f=1 doesn't force any specific variable assignment
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 1) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 0) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 1) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 0) == 0);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Constants", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Constant 1 has empty clause set") {
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, one);
        REQUIRE(tlc != nullptr);
        // Check that there are no clauses by trying to read the first one
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);  // No clauses
        Cudd_tlcInfoFree(tlc);
    }
    
    SECTION("Constant 0 has empty clause set") {
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, zero);
        REQUIRE(tlc != nullptr);
        // Check that there are no clauses by trying to read the first one
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);  // No clauses
        Cudd_tlcInfoFree(tlc);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Single variable", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    int xid = Cudd_NodeReadIndex(x);
    
    SECTION("Variable x has one-literal clause +x") {
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, x);
        REQUIRE(tlc != nullptr);
        
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        REQUIRE(var1 == (unsigned)xid);
        REQUIRE(phase1 == 0);  // positive phase
        
        // Check that there's no second clause
        result = Cudd_ReadIthClause(tlc, 1, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);
        
        Cudd_tlcInfoFree(tlc);
    }
    
    SECTION("Variable !x has one-literal clause -x") {
        DdNode *notx = Cudd_Not(x);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, notx);
        REQUIRE(tlc != nullptr);
        
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        REQUIRE(var1 == (unsigned)xid);
        REQUIRE(phase1 == 1);  // negative phase
        
        // Check that there's no second clause
        result = Cudd_ReadIthClause(tlc, 1, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);
        
        Cudd_tlcInfoFree(tlc);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Two variables", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("x OR y generates two-literal clauses") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("x AND y generates two-literal clauses") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Complex functions", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("(x AND y) OR z generates clauses") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("x XOR y generates clauses") {
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_ReadIthClause - Boundary conditions", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, x);
    REQUIRE(tlc != nullptr);
    
    unsigned var1, var2;
    int phase1, phase2;
    
    SECTION("Read valid clause at index 0") {
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
    }
    
    SECTION("Read clause with negative index") {
        int result = Cudd_ReadIthClause(tlc, -1, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);
    }
    
    SECTION("Read clause with out-of-range index") {
        // Find how many clauses exist by reading until we fail
        int i = 0;
        while (Cudd_ReadIthClause(tlc, i, &var1, &var2, &phase1, &phase2) == 1) {
            i++;
        }
        // Now try to read one past the end
        int result = Cudd_ReadIthClause(tlc, i, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);
    }
    
    SECTION("Read clause with NULL tlc") {
        int result = Cudd_ReadIthClause(nullptr, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);
    }
    
    Cudd_tlcInfoFree(tlc);
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_PrintTwoLiteralClauses - Basic printing", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Print clauses for x OR y without names") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        // Create a temporary file for output
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, f, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Print clauses for x OR y with names") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        char *names[2];
        names[0] = (char *)"var_x";
        names[1] = (char *)"var_y";
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, f, names, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Print clauses with NULL file pointer (uses manager->out)") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, f, nullptr, nullptr);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Print clauses for single variable without names") {
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, x, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
    }
    
    SECTION("Print clauses for single variable with names") {
        char *names[1];
        names[0] = (char *)"x";
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, x, names, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_tlcInfoFree - Memory management", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("Free valid TLC info") {
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, x);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        // If we reach here without crash, the test passes
        REQUIRE(true);
    }
    
    SECTION("Free TLC info from constant") {
        DdNode *one = Cudd_ReadOne(manager);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, one);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        REQUIRE(true);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindEssential - Reordering stress test", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Enable dynamic reordering
    Cudd_AutodynEnable(manager, CUDD_REORDER_SAME);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("FindEssential with potential reordering") {
        DdNode *f = Cudd_bddOr(manager, x, Cudd_bddAnd(manager, y, z));
        Cudd_Ref(f);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Various BDD patterns", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Negated AND: !(x AND y)") {
        DdNode *f = Cudd_Not(Cudd_bddAnd(manager, x, y));
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Three-way OR: x OR y OR z") {
        DdNode *xy = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // x OR y OR z is a single three-literal clause, not representable as 
        // a set of one- or two-literal clauses, so the result is empty
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 0);  // No two-literal clauses
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("ITE: if x then y else z") {
        DdNode *f = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // TLC should exist (may or may not have clauses depending on function)
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Implication: x -> y (equivalent to !x OR y)") {
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

// Additional tests to improve coverage of different code paths
TEST_CASE("Cudd_FindEssential - Complex branch coverage", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    DdNode *w = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    Cudd_Ref(w);
    
    SECTION("ITE with then=1, else=non-constant: x ? 1 : y") {
        // This creates: ITE(x, 1, y) = x OR y
        DdNode *f = Cudd_bddIte(manager, x, one, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with then=0, else=non-constant: x ? 0 : y") {
        // This creates: ITE(x, 0, y) = !x AND y
        DdNode *zero = Cudd_Not(one);
        DdNode *f = Cudd_bddIte(manager, x, zero, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with then=non-constant, else=0: x ? y : 0") {
        // This creates: ITE(x, y, 0) = x AND y
        DdNode *zero = Cudd_Not(one);
        DdNode *f = Cudd_bddIte(manager, x, y, zero);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess != one);  // Has essential variables
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with then=non-constant, else=1: x ? y : 1") {
        // This creates: ITE(x, y, 1) = !x OR y = x -> y
        DdNode *f = Cudd_bddIte(manager, x, y, one);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with both branches non-constant: x ? y : z") {
        DdNode *f = Cudd_bddIte(manager, x, y, z);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Nested ITE: x ? (y AND z) : (w OR z)") {
        DdNode *yz = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(yz);
        DdNode *wz = Cudd_bddOr(manager, w, z);
        Cudd_Ref(wz);
        DdNode *f = Cudd_bddIte(manager, x, yz, wz);
        Cudd_Ref(f);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, wz);
        Cudd_RecursiveDeref(manager, yz);
    }
    
    SECTION("Complemented function: !(x AND y AND z)") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *xyz = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(xyz);
        DdNode *f = Cudd_Not(xyz);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        
        Cudd_RecursiveDeref(manager, xyz);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("XOR function: x XOR y") {
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // XOR has no essential variables
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("XNOR function: x XNOR y") {
        DdNode *f = Cudd_bddXnor(manager, x, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // XNOR has no essential variables
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, w);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Comprehensive coverage", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("ITE with then=1, else=var: x ? 1 : y") {
        DdNode *f = Cudd_bddIte(manager, x, one, y);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with then=0, else=var: x ? 0 : y") {
        DdNode *f = Cudd_bddIte(manager, x, zero, y);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with then=var, else=0: x ? y : 0") {
        DdNode *f = Cudd_bddIte(manager, x, y, zero);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("ITE with then=var, else=1: x ? y : 1") {
        DdNode *f = Cudd_bddIte(manager, x, y, one);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Nested ITE: x ? (y AND z) : 1") {
        DdNode *yz = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddIte(manager, x, yz, one);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
    }
    
    SECTION("Nested ITE: x ? 1 : (y AND z)") {
        DdNode *yz = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddIte(manager, x, one, yz);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        Cudd_tlcInfoFree(tlc);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
    }
    
    SECTION("Three-way AND: x AND y AND z") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Complemented single variable: !x") {
        DdNode *notx = Cudd_Not(x);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, notx);
        REQUIRE(tlc != nullptr);
        
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        REQUIRE(phase1 == 1);  // Negative phase
        
        Cudd_tlcInfoFree(tlc);
    }
    
    SECTION("Complex formula: (x AND y) OR (!x AND z)") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *notx_z = Cudd_bddAnd(manager, Cudd_Not(x), z);
        Cudd_Ref(notx_z);
        DdNode *f = Cudd_bddOr(manager, xy, notx_z);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, notx_z);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_PrintTwoLiteralClauses - More coverage", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Print complex formula") {
        DdNode *f = Cudd_bddAnd(manager, Cudd_bddOr(manager, x, y), z);
        Cudd_Ref(f);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, f, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Print with variable names for multi-literal formula") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        char *names[3];
        names[0] = (char *)"x";
        names[1] = (char *)"y";
        names[2] = (char *)"z";
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_PrintTwoLiteralClauses(manager, f, names, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddIsVarEssential - Extended tests", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    int xid = Cudd_NodeReadIndex(x);
    int yid = Cudd_NodeReadIndex(y);
    int zid = Cudd_NodeReadIndex(z);
    
    SECTION("Essential in three-way AND") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(f);
        
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 1) == 1);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 1) == 1);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, zid, 1) == 1);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Essential in implication x -> y") {
        DdNode *f = Cudd_bddOr(manager, Cudd_Not(x), y);
        Cudd_Ref(f);
        
        // x -> y means if f=1, we can't deduce anything specific about x or y alone
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 1) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 1) == 0);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Essential in x AND !y") {
        DdNode *f = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(f);
        
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 1) == 1);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 0) == 1);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 0) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 1) == 0);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindEssential - Cache and special cases", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    DdNode *w = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    Cudd_Ref(w);
    
    SECTION("Cache hit by calling FindEssential twice") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // First call - populates cache
        DdNode *ess1 = Cudd_FindEssential(manager, f);
        REQUIRE(ess1 != nullptr);
        
        // Second call - should hit cache
        DdNode *ess2 = Cudd_FindEssential(manager, f);
        REQUIRE(ess2 != nullptr);
        REQUIRE(ess1 == ess2);  // Should be the same result
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Complex nested formula to exercise all branches") {
        // Create a formula that exercises different branches
        // (x AND y) OR ((z AND w) OR (!z AND !w))
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *nz_nw = Cudd_bddAnd(manager, Cudd_Not(z), Cudd_Not(w));
        Cudd_Ref(nz_nw);
        DdNode *zw_or_nznw = Cudd_bddOr(manager, zw, nz_nw);
        Cudd_Ref(zw_or_nznw);
        DdNode *f = Cudd_bddOr(manager, xy, zw_or_nznw);
        Cudd_Ref(f);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        
        // Call again to hit cache
        DdNode *ess2 = Cudd_FindEssential(manager, f);
        REQUIRE(ess2 == ess);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw_or_nznw);
        Cudd_RecursiveDeref(manager, nz_nw);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Formula where essT == one (line 537)") {
        // Create a formula where the then branch has no essential variables
        // (x AND (y OR z))
        DdNode *yz = Cudd_bddOr(manager, y, z);
        Cudd_Ref(yz);
        DdNode *f = Cudd_bddAnd(manager, x, yz);
        Cudd_Ref(f);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yz);
    }
    
    SECTION("Four variable formula") {
        // (x AND y) OR (z AND w)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *f = Cudd_bddOr(manager, xy, zw);
        Cudd_Ref(f);
        
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, w);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FindTwoLiteralClauses - Cache and complex formulas", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    DdNode *w = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    Cudd_Ref(w);
    
    SECTION("Four variable formula: (x AND y) OR (z AND w)") {
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *f = Cudd_bddOr(manager, xy, zw);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Complex nested formula") {
        // ((x OR y) AND z) OR w
        DdNode *xy = Cudd_bddOr(manager, x, y);
        Cudd_Ref(xy);
        DdNode *xyz = Cudd_bddAnd(manager, xy, z);
        Cudd_Ref(xyz);
        DdNode *f = Cudd_bddOr(manager, xyz, w);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xyz);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    SECTION("Deeply nested formula") {
        // (x AND (y AND (z OR w)))
        DdNode *zw = Cudd_bddOr(manager, z, w);
        Cudd_Ref(zw);
        DdNode *yzw = Cudd_bddAnd(manager, y, zw);
        Cudd_Ref(yzw);
        DdNode *f = Cudd_bddAnd(manager, x, yzw);
        Cudd_Ref(f);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, yzw);
        Cudd_RecursiveDeref(manager, zw);
    }
    
    SECTION("Complemented complex formula") {
        // !((x AND y) OR (z AND w))
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *zw = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(zw);
        DdNode *xyzw = Cudd_bddOr(manager, xy, zw);
        Cudd_Ref(xyzw);
        DdNode *f = Cudd_Not(xyzw);
        
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, xyzw);
        Cudd_RecursiveDeref(manager, zw);
        Cudd_RecursiveDeref(manager, xy);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, w);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_ReadIthClause - Additional edge cases", "[cuddEssent]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    unsigned var1, var2;
    int phase1, phase2;
    
    SECTION("Read multiple clauses from complex formula") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdTlcInfo *tlc = Cudd_FindTwoLiteralClauses(manager, f);
        REQUIRE(tlc != nullptr);
        
        // Try reading multiple valid clauses
        int count = 0;
        while (Cudd_ReadIthClause(tlc, count, &var1, &var2, &phase1, &phase2)) {
            count++;
            if (count > 100) break; // Safety limit
        }
        REQUIRE(count > 0);  // Should have at least one clause
        
        Cudd_tlcInfoFree(tlc);
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}
