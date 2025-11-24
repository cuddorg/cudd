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
    
    SECTION("Function x AND y has no essential variables") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // No essential variables
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
    
    SECTION("Function x AND !y has no essential variables") {
        DdNode *f = Cudd_bddAnd(manager, x, Cudd_Not(y));
        Cudd_Ref(f);
        DdNode *ess = Cudd_FindEssential(manager, f);
        REQUIRE(ess != nullptr);
        REQUIRE(ess == one);  // No essential variables
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
    
    SECTION("No variable is essential in x AND y") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 1) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, xid, 0) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 1) == 0);
        REQUIRE(Cudd_bddIsVarEssential(manager, f, yid, 0) == 0);
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("No variable is essential in x OR y") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
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
        // Verify at least one clause exists
        unsigned var1, var2;
        int phase1, phase2;
        int result = Cudd_ReadIthClause(tlc, 0, &var1, &var2, &phase1, &phase2);
        REQUIRE(result == 1);
        
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
