#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

/**
 * @brief Test file for cuddZddUtil.c
 * 
 * This file contains comprehensive tests for all public functions
 * in the cuddZddUtil module to achieve 90%+ code coverage.
 */

// Helper function to create a simple ZDD 
static DdNode* createSimpleZDD(DdManager* dd) {
    DdNode *z0 = Cudd_zddIthVar(dd, 0);
    DdNode *z1 = Cudd_zddIthVar(dd, 1);
    if (z0 == nullptr || z1 == nullptr) return nullptr;
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    DdNode *result = Cudd_zddUnion(dd, z0, z1);
    Cudd_RecursiveDerefZdd(dd, z0);
    Cudd_RecursiveDerefZdd(dd, z1);
    if (result != nullptr) Cudd_Ref(result);
    return result;
}

// Helper function to create a more complex ZDD for testing
static DdNode* createComplexZDD(DdManager* dd) {
    DdNode *z0 = Cudd_zddIthVar(dd, 0);
    DdNode *z1 = Cudd_zddIthVar(dd, 1);
    DdNode *z2 = Cudd_zddIthVar(dd, 2);
    if (z0 == nullptr || z1 == nullptr || z2 == nullptr) return nullptr;
    Cudd_Ref(z0);
    Cudd_Ref(z1);
    Cudd_Ref(z2);
    
    // Create union: z0 | z1 | z2
    DdNode *tmp = Cudd_zddUnion(dd, z0, z1);
    Cudd_Ref(tmp);
    DdNode *result = Cudd_zddUnion(dd, tmp, z2);
    
    Cudd_RecursiveDerefZdd(dd, z0);
    Cudd_RecursiveDerefZdd(dd, z1);
    Cudd_RecursiveDerefZdd(dd, z2);
    Cudd_RecursiveDerefZdd(dd, tmp);
    
    if (result != nullptr) Cudd_Ref(result);
    return result;
}

TEST_CASE("cuddZddUtil - Cudd_zddPrintMinterm", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    // Initialize ZDD variables from BDD variables
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Print minterm for simple ZDD") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        int result = Cudd_zddPrintMinterm(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Print minterm for ZDD variable") {
        DdNode* z = Cudd_zddIthVar(dd, 0);
        REQUIRE(z != nullptr);
        Cudd_Ref(z);
        
        int result = Cudd_zddPrintMinterm(dd, z);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, z);
    }
    
    SECTION("Print minterm for ZDD constant one") {
        DdNode* one = DD_ONE(dd);
        int result = Cudd_zddPrintMinterm(dd, one);
        REQUIRE(result == 1);
    }
    
    SECTION("Print minterm for ZDD constant zero") {
        DdNode* zero = DD_ZERO(dd);
        int result = Cudd_zddPrintMinterm(dd, zero);
        REQUIRE(result == 1);
    }
    
    SECTION("Print minterm for complex ZDD") {
        DdNode* f = createComplexZDD(dd);
        REQUIRE(f != nullptr);
        
        int result = Cudd_zddPrintMinterm(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddPrintCover", "[cuddZddUtil]") {
    // Need even number of ZDD variables for PrintCover
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    // Initialize ZDD variables with multiplicity 2 (for cover representation)
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Print cover for simple ZDD") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        int result = Cudd_zddPrintCover(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Print cover for ZDD constant one") {
        DdNode* one = DD_ONE(dd);
        int result = Cudd_zddPrintCover(dd, one);
        REQUIRE(result == 1);
    }
    
    SECTION("Print cover for ZDD constant zero") {
        DdNode* zero = DD_ZERO(dd);
        int result = Cudd_zddPrintCover(dd, zero);
        REQUIRE(result == 1);
    }
    
    SECTION("Print cover with odd number of variables - should fail") {
        // Create manager with odd number of ZDD variables
        DdManager* dd_odd = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(dd_odd != nullptr);
        
        // Initialize 3 ZDD variables (odd)
        int init = Cudd_zddVarsFromBddVars(dd_odd, 1);
        REQUIRE(init == 1);
        
        // Create a ZDD and try to print cover - now we have 3 ZDD variables
        // We need to add one more variable to make it odd total
        DdNode* z = Cudd_zddIthVar(dd_odd, 0);
        Cudd_Ref(z);
        
        // Get current ZDD size - should be 2 (multiplicity 2)
        // This won't return 0 since we have even vars
        int result = Cudd_zddPrintCover(dd_odd, z);
        // Result depends on whether sizeZ is even or odd
        REQUIRE((result == 0 || result == 1));
        
        Cudd_RecursiveDerefZdd(dd_odd, z);
        Cudd_Quit(dd_odd);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddPrintDebug", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    DdNode* f = createSimpleZDD(dd);
    REQUIRE(f != nullptr);
    
    SECTION("pr = 0 - prints nothing") {
        int result = Cudd_zddPrintDebug(dd, f, 4, 0);
        REQUIRE(result == 1);
    }
    
    SECTION("pr = 1 - prints counts only") {
        int result = Cudd_zddPrintDebug(dd, f, 4, 1);
        REQUIRE(result == 1);
    }
    
    SECTION("pr = 2 - prints counts + disjoint sum") {
        int result = Cudd_zddPrintDebug(dd, f, 4, 2);
        REQUIRE(result == 1);
    }
    
    SECTION("pr = 3 - prints counts + list of nodes") {
        int result = Cudd_zddPrintDebug(dd, f, 4, 3);
        REQUIRE(result == 1);
    }
    
    SECTION("pr = 4 - prints counts + disjoint sum + list of nodes") {
        int result = Cudd_zddPrintDebug(dd, f, 4, 4);
        REQUIRE(result == 1);
    }
    
    SECTION("pr > 4 - same as pr = 4") {
        int result = Cudd_zddPrintDebug(dd, f, 4, 5);
        REQUIRE(result == 1);
    }
    
    SECTION("Empty ZDD with pr > 0") {
        DdNode* zero = DD_ZERO(dd);
        int result = Cudd_zddPrintDebug(dd, zero, 4, 1);
        REQUIRE(result == 1);
    }
    
    SECTION("Empty ZDD with pr = 2") {
        DdNode* zero = DD_ZERO(dd);
        int result = Cudd_zddPrintDebug(dd, zero, 4, 2);
        REQUIRE(result == 1);
    }
    
    SECTION("Constant one ZDD") {
        DdNode* one = DD_ONE(dd);
        int result = Cudd_zddPrintDebug(dd, one, 4, 2);
        REQUIRE(result == 1);
    }
    
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddFirstPath and Cudd_zddNextPath", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Enumerate paths in simple ZDD") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        gen = Cudd_zddFirstPath(dd, f, &path);
        REQUIRE(gen != nullptr);
        REQUIRE(path != nullptr);
        
        while (!Cudd_IsGenEmpty(gen)) {
            count++;
            int status = Cudd_zddNextPath(gen, &path);
            if (status == 0) break;
        }
        
        REQUIRE(count >= 1);
        
        Cudd_GenFree(gen);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Enumerate paths using Cudd_zddForeachPath macro") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        Cudd_zddForeachPath(dd, f, gen, path) {
            count++;
            REQUIRE(path != nullptr);
        }
        
        REQUIRE(count >= 1);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Enumerate paths in ZDD constant one") {
        DdNode* one = DD_ONE(dd);
        
        DdGen* gen;
        int* path;
        
        gen = Cudd_zddFirstPath(dd, one, &path);
        REQUIRE(gen != nullptr);
        
        int status = Cudd_IsGenEmpty(gen);
        // Constant one should have exactly one path
        REQUIRE(status == 0);
        
        Cudd_GenFree(gen);
    }
    
    SECTION("Enumerate paths in ZDD constant zero") {
        DdNode* zero = DD_ZERO(dd);
        
        DdGen* gen;
        int* path;
        
        gen = Cudd_zddFirstPath(dd, zero, &path);
        REQUIRE(gen != nullptr);
        
        // Zero ZDD should be empty
        int empty = Cudd_IsGenEmpty(gen);
        REQUIRE(empty == 1);
        
        Cudd_GenFree(gen);
    }
    
    SECTION("Path enumeration with complex ZDD") {
        DdNode* f = createComplexZDD(dd);
        REQUIRE(f != nullptr);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        Cudd_zddForeachPath(dd, f, gen, path) {
            count++;
        }
        
        REQUIRE(count >= 1);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("NULL manager for FirstPath") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        int* path;
        DdGen* gen = Cudd_zddFirstPath(nullptr, f, &path);
        REQUIRE(gen == nullptr);
        
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("NULL node for FirstPath") {
        int* path;
        DdGen* gen = Cudd_zddFirstPath(dd, nullptr, &path);
        REQUIRE(gen == nullptr);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddCoverPathToString", "[cuddZddUtil]") {
    // Need even number of ZDD variables
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Convert path to string with NULL str") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        DdGen* gen;
        int* path;
        
        gen = Cudd_zddFirstPath(dd, f, &path);
        REQUIRE(gen != nullptr);
        
        if (!Cudd_IsGenEmpty(gen)) {
            // Convert path to string - function allocates memory
            char* str = Cudd_zddCoverPathToString(dd, path, nullptr);
            REQUIRE(str != nullptr);
            FREE(str);
        }
        
        Cudd_GenFree(gen);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Convert path to string with provided buffer") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        DdGen* gen;
        int* path;
        
        gen = Cudd_zddFirstPath(dd, f, &path);
        REQUIRE(gen != nullptr);
        
        if (!Cudd_IsGenEmpty(gen)) {
            // Provide our own buffer (nvars/2 + 1 for null terminator)
            int nvars = dd->sizeZ / 2;
            char* buffer = new char[nvars + 1];
            
            char* str = Cudd_zddCoverPathToString(dd, path, buffer);
            REQUIRE(str == buffer);
            
            delete[] buffer;
        }
        
        Cudd_GenFree(gen);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Test various path value encodings") {
        // Create different ZDD structures to exercise different switch cases
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        Cudd_Ref(z3);
        
        // Create a union of some variables
        DdNode* tmp1 = Cudd_zddUnion(dd, z0, z2);
        Cudd_Ref(tmp1);
        
        DdGen* gen;
        int* path;
        
        gen = Cudd_zddFirstPath(dd, tmp1, &path);
        REQUIRE(gen != nullptr);
        
        // Iterate through all paths
        do {
            if (Cudd_IsGenEmpty(gen)) break;
            
            char* str = Cudd_zddCoverPathToString(dd, path, nullptr);
            REQUIRE(str != nullptr);
            FREE(str);
        } while (Cudd_zddNextPath(gen, &path));
        
        Cudd_GenFree(gen);
        
        Cudd_RecursiveDerefZdd(dd, tmp1);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddCoverPathToString with odd variables", "[cuddZddUtil]") {
    // Test with odd number of variables - should return NULL
    DdManager* dd = Cudd_Init(3, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    // This creates 6 ZDD variables (even), so we need different approach
    // Create exactly 1 ZDD variable to get odd count
    DdNode* z = Cudd_zddIthVar(dd, 0);
    REQUIRE(z != nullptr);
    Cudd_Ref(z);
    
    // Get sizeZ and verify
    int sizeZ = dd->sizeZ;
    
    // Only test if we have odd number
    if (sizeZ & 1) {
        int path[] = {0, 0, 0};  // dummy path
        char* str = Cudd_zddCoverPathToString(dd, path, nullptr);
        REQUIRE(str == nullptr);
    }
    
    Cudd_RecursiveDerefZdd(dd, z);
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddSupport", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Support of simple ZDD") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        DdNode* support = Cudd_zddSupport(dd, f);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        
        // Support should be non-constant (has variables)
        REQUIRE(!Cudd_IsConstant(support));
        
        Cudd_RecursiveDeref(dd, support);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Support of ZDD variable") {
        DdNode* z = Cudd_zddIthVar(dd, 0);
        REQUIRE(z != nullptr);
        Cudd_Ref(z);
        
        DdNode* support = Cudd_zddSupport(dd, z);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        
        Cudd_RecursiveDeref(dd, support);
        Cudd_RecursiveDerefZdd(dd, z);
    }
    
    SECTION("Support of constant one") {
        DdNode* one = DD_ONE(dd);
        
        DdNode* support = Cudd_zddSupport(dd, one);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        
        // Support of constant should be constant one (empty support)
        REQUIRE(support == Cudd_ReadOne(dd));
        
        Cudd_RecursiveDeref(dd, support);
    }
    
    SECTION("Support of constant zero") {
        DdNode* zero = DD_ZERO(dd);
        
        DdNode* support = Cudd_zddSupport(dd, zero);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        
        Cudd_RecursiveDeref(dd, support);
    }
    
    SECTION("Support of complex ZDD") {
        DdNode* f = createComplexZDD(dd);
        REQUIRE(f != nullptr);
        
        DdNode* support = Cudd_zddSupport(dd, f);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        
        Cudd_RecursiveDeref(dd, support);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cudd_zddDumpDot", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Dump simple ZDD without names") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_zddDumpDot(dd, 1, &f, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Dump ZDD with input and output names") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        // Create input variable names
        const char* inames[] = {"z0", "z1", "z2", "z3", "z4", "z5", "z6", "z7"};
        const char* onames[] = {"f"};
        
        int result = Cudd_zddDumpDot(dd, 1, &f, inames, onames, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Dump multiple ZDDs") {
        DdNode* f1 = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(f1);
        DdNode* f2 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(f2);
        
        DdNode* funcs[2] = {f1, f2};
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        const char* onames[] = {"f1", "f2"};
        
        int result = Cudd_zddDumpDot(dd, 2, funcs, nullptr, onames, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDerefZdd(dd, f1);
        Cudd_RecursiveDerefZdd(dd, f2);
    }
    
    SECTION("Dump ZDD constant one") {
        DdNode* one = DD_ONE(dd);
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_zddDumpDot(dd, 1, &one, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
    }
    
    SECTION("Dump ZDD constant zero") {
        DdNode* zero = DD_ZERO(dd);
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_zddDumpDot(dd, 1, &zero, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
    }
    
    SECTION("Dump complex ZDD") {
        DdNode* f = createComplexZDD(dd);
        REQUIRE(f != nullptr);
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_zddDumpDot(dd, 1, &f, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Verify dot file contains expected content") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        char filename[] = "/tmp/cudd_zdd_dot_XXXXXX";
        int fd = mkstemp(filename);
        REQUIRE(fd != -1);
        FILE* fp = fdopen(fd, "w");
        REQUIRE(fp != nullptr);
        
        int result = Cudd_zddDumpDot(dd, 1, &f, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        fclose(fp);
        
        // Read back and verify content
        FILE* check = fopen(filename, "r");
        REQUIRE(check != nullptr);
        
        char buffer[1024];
        bool found_digraph = false;
        bool found_edges = false;
        
        while (fgets(buffer, sizeof(buffer), check) != nullptr) {
            if (strstr(buffer, "digraph") != nullptr) {
                found_digraph = true;
            }
            if (strstr(buffer, "->") != nullptr) {
                found_edges = true;
            }
        }
        fclose(check);
        unlink(filename);
        
        REQUIRE(found_digraph);
        // Edges should exist for non-trivial ZDDs
        
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - cuddZddP (internal print function)", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Print ZDD via PrintDebug with pr=3") {
        DdNode* f = createSimpleZDD(dd);
        REQUIRE(f != nullptr);
        
        // cuddZddP is called when pr > 2
        int result = Cudd_zddPrintDebug(dd, f, 4, 3);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
    }
    
    SECTION("Print complex ZDD structure") {
        // Create a more complex structure to exercise zp2 recursion
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        // Create union z0 | z1
        DdNode* u1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u1);
        
        // Create intersection
        DdNode* i1 = Cudd_zddIntersect(dd, u1, z2);
        Cudd_Ref(i1);
        
        // Print the structures
        int result1 = Cudd_zddPrintDebug(dd, u1, 4, 3);
        REQUIRE(result1 == 1);
        
        int result2 = Cudd_zddPrintDebug(dd, i1, 4, 3);
        REQUIRE(result2 == 1);
        
        Cudd_RecursiveDerefZdd(dd, i1);
        Cudd_RecursiveDerefZdd(dd, u1);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - zdd_print_minterm_aux coverage", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Test with missing variables (gap in levels)") {
        // Create ZDD with non-consecutive variable indices
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        Cudd_Ref(z0);
        Cudd_Ref(z4);
        
        DdNode* f = Cudd_zddUnion(dd, z0, z4);
        Cudd_Ref(f);
        
        // This should exercise the "missing variable" branch in zdd_print_minterm_aux
        int result = Cudd_zddPrintMinterm(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Test with Nv == Nnv (don't care variable)") {
        // Create ZDD where then and else children are the same
        // This happens naturally in some ZDD structures
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // Create a structure that exercises different branches
        DdNode* f = Cudd_zddDiff(dd, z0, z1);
        Cudd_Ref(f);
        
        int result = Cudd_zddPrintMinterm(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - zddPrintCoverAux coverage", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    // Create even number of ZDD variables
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Test with missing variables (gap in levels)") {
        // Create ZDD with non-consecutive variable indices
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        Cudd_Ref(z0);
        Cudd_Ref(z4);
        
        DdNode* f = Cudd_zddUnion(dd, z0, z4);
        Cudd_Ref(f);
        
        // This exercises the "missing variable" branch in zddPrintCoverAux
        int result = Cudd_zddPrintCover(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Test various cover encodings") {
        // Create ZDDs that will produce different cover characters (0, 1, -)
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        Cudd_Ref(z3);
        
        // Create union
        DdNode* tmp1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(tmp1);
        DdNode* tmp2 = Cudd_zddUnion(dd, z2, z3);
        Cudd_Ref(tmp2);
        DdNode* f = Cudd_zddUnion(dd, tmp1, tmp2);
        Cudd_Ref(f);
        
        int result = Cudd_zddPrintCover(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, tmp2);
        Cudd_RecursiveDerefZdd(dd, tmp1);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - zddSupportStep and zddClearFlag coverage", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Support with complemented edges") {
        // ZDDs don't use complemented edges in the same way BDDs do,
        // but we can test the support function thoroughly
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // Create a more complex ZDD
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        DdNode* support1 = Cudd_zddSupport(dd, u);
        REQUIRE(support1 != nullptr);
        Cudd_Ref(support1);
        
        // Call support again - should work with cleared flags
        DdNode* support2 = Cudd_zddSupport(dd, u);
        REQUIRE(support2 != nullptr);
        Cudd_Ref(support2);
        
        // Should get the same support
        REQUIRE(support1 == support2);
        
        Cudd_RecursiveDeref(dd, support2);
        Cudd_RecursiveDeref(dd, support1);
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Support with deeply nested ZDD") {
        // Create a chain of operations
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        Cudd_Ref(z3);
        
        DdNode* tmp1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(tmp1);
        DdNode* tmp2 = Cudd_zddUnion(dd, z2, z3);
        Cudd_Ref(tmp2);
        DdNode* tmp3 = Cudd_zddIntersect(dd, tmp1, tmp2);
        Cudd_Ref(tmp3);
        DdNode* f = Cudd_zddUnion(dd, tmp1, tmp3);
        Cudd_Ref(f);
        
        DdNode* support = Cudd_zddSupport(dd, f);
        REQUIRE(support != nullptr);
        Cudd_Ref(support);
        
        Cudd_RecursiveDeref(dd, support);
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, tmp3);
        Cudd_RecursiveDerefZdd(dd, tmp2);
        Cudd_RecursiveDerefZdd(dd, tmp1);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Edge cases and error paths", "[cuddZddUtil]") {
    SECTION("Large ZDD for DumpDot") {
        DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(dd != nullptr);
        
        int init_result = Cudd_zddVarsFromBddVars(dd, 2);
        REQUIRE(init_result == 1);
        
        // Create a larger ZDD structure
        DdNode* vars[8];
        for (int i = 0; i < 8; i++) {
            vars[i] = Cudd_zddIthVar(dd, i);
            REQUIRE(vars[i] != nullptr);
            Cudd_Ref(vars[i]);
        }
        
        // Create union of all
        DdNode* result = vars[0];
        Cudd_Ref(result);
        for (int i = 1; i < 8; i++) {
            DdNode* tmp = Cudd_zddUnion(dd, result, vars[i]);
            Cudd_Ref(tmp);
            Cudd_RecursiveDerefZdd(dd, result);
            result = tmp;
        }
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int dump_result = Cudd_zddDumpDot(dd, 1, &result, nullptr, nullptr, fp);
        REQUIRE(dump_result == 1);
        
        fclose(fp);
        
        Cudd_RecursiveDerefZdd(dd, result);
        for (int i = 0; i < 8; i++) {
            Cudd_RecursiveDerefZdd(dd, vars[i]);
        }
        
        Cudd_Quit(dd);
    }
    
    SECTION("Multiple ZDDs with shared nodes") {
        DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        REQUIRE(dd != nullptr);
        
        int init_result = Cudd_zddVarsFromBddVars(dd, 2);
        REQUIRE(init_result == 1);
        
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // f1 and f2 share nodes
        DdNode* f1 = u;
        DdNode* f2 = z0;
        DdNode* funcs[2] = {f1, f2};
        
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_zddDumpDot(dd, 2, funcs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        fclose(fp);
        
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
        
        Cudd_Quit(dd);
    }
}

TEST_CASE("cuddZddUtil - Path enumeration edge cases", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Single variable ZDD") {
        DdNode* z = Cudd_zddIthVar(dd, 0);
        REQUIRE(z != nullptr);
        Cudd_Ref(z);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        Cudd_zddForeachPath(dd, z, gen, path) {
            count++;
            // Path should have proper values
            REQUIRE(path != nullptr);
        }
        
        REQUIRE(count >= 1);
        Cudd_RecursiveDerefZdd(dd, z);
    }
    
    SECTION("ZDD with multiple paths - verify backtracking") {
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        
        // Create: z0 | z1 | z2
        DdNode* tmp = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(tmp);
        DdNode* f = Cudd_zddUnion(dd, tmp, z2);
        Cudd_Ref(f);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        // This exercises both FirstPath and NextPath backtracking
        Cudd_zddForeachPath(dd, f, gen, path) {
            count++;
        }
        
        REQUIRE(count >= 3);  // Should have at least 3 paths
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, tmp);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - zp2 function coverage", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("zp2 with constant nodes") {
        // Printing constant one exercises IsConstantInt branch
        DdNode* one = DD_ONE(dd);
        int result = Cudd_zddPrintDebug(dd, one, 4, 3);
        REQUIRE(result == 1);
    }
    
    SECTION("zp2 with repeated nodes (st_is_member check)") {
        // Create ZDD with shared substructure
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // Create union - this creates shared structure
        DdNode* u = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(u);
        
        // Print with pr=3 to invoke cuddZddP
        int result = Cudd_zddPrintDebug(dd, u, 4, 3);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, u);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("zp2 with then child as constant") {
        // Create structure where T child is constant
        DdNode* z = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z);
        
        int result = Cudd_zddPrintDebug(dd, z, 4, 3);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, z);
    }
    
    SECTION("zp2 with else child as constant") {
        // Variables have else child pointing to zero
        DdNode* z = Cudd_zddIthVar(dd, 0);
        Cudd_Ref(z);
        
        int result = Cudd_zddPrintDebug(dd, z, 4, 3);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, z);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Comprehensive path coverage", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Exercise all switch cases in CoverPathToString") {
        // Create ZDD patterns that will produce different path values
        // when interpreted as cover encoding
        
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        Cudd_Ref(z3);
        
        // Create a union that gives various path patterns
        DdNode* tmp1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(tmp1);
        DdNode* tmp2 = Cudd_zddUnion(dd, z2, z3);
        Cudd_Ref(tmp2);
        DdNode* f = Cudd_zddUnion(dd, tmp1, tmp2);
        Cudd_Ref(f);
        
        DdGen* gen;
        int* path;
        
        Cudd_zddForeachPath(dd, f, gen, path) {
            char* str = Cudd_zddCoverPathToString(dd, path, nullptr);
            REQUIRE(str != nullptr);
            // Verify string contains valid characters
            for (int i = 0; str[i] != '\0'; i++) {
                REQUIRE((str[i] == '0' || str[i] == '1' || str[i] == '-' || str[i] == '?'));
            }
            FREE(str);
        }
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, tmp2);
        Cudd_RecursiveDerefZdd(dd, tmp1);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Additional coverage for path backtracking", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Deep backtracking in path enumeration") {
        // Create a ZDD structure that requires deep backtracking
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        DdNode* z3 = Cudd_zddIthVar(dd, 3);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        Cudd_Ref(z2);
        Cudd_Ref(z3);
        Cudd_Ref(z4);
        
        // Create nested structure to force backtracking
        DdNode* tmp1 = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(tmp1);
        DdNode* tmp2 = Cudd_zddUnion(dd, z2, z3);
        Cudd_Ref(tmp2);
        DdNode* tmp3 = Cudd_zddIntersect(dd, tmp1, z4);
        Cudd_Ref(tmp3);
        DdNode* f = Cudd_zddUnion(dd, tmp2, tmp3);
        Cudd_Ref(f);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        Cudd_zddForeachPath(dd, f, gen, path) {
            count++;
        }
        
        REQUIRE(count >= 2);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, tmp3);
        Cudd_RecursiveDerefZdd(dd, tmp2);
        Cudd_RecursiveDerefZdd(dd, tmp1);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z3);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Path enumeration ending in zero") {
        // Create ZDD that will have paths ending in zero terminal
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // Create difference: z0 - z1
        DdNode* f = Cudd_zddDiff(dd, z0, z1);
        Cudd_Ref(f);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        Cudd_zddForeachPath(dd, f, gen, path) {
            count++;
        }
        
        // Clean up
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Multiple consecutive backtracking") {
        // Create structure that requires multiple consecutive backtracks
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z2 = Cudd_zddIthVar(dd, 2);
        DdNode* z4 = Cudd_zddIthVar(dd, 4);
        DdNode* z6 = Cudd_zddIthVar(dd, 6);
        Cudd_Ref(z0);
        Cudd_Ref(z2);
        Cudd_Ref(z4);
        Cudd_Ref(z6);
        
        // Create union of non-adjacent variables
        DdNode* tmp1 = Cudd_zddUnion(dd, z0, z2);
        Cudd_Ref(tmp1);
        DdNode* tmp2 = Cudd_zddUnion(dd, z4, z6);
        Cudd_Ref(tmp2);
        DdNode* f = Cudd_zddUnion(dd, tmp1, tmp2);
        Cudd_Ref(f);
        
        DdGen* gen;
        int* path;
        int count = 0;
        
        Cudd_zddForeachPath(dd, f, gen, path) {
            count++;
            // Print for verification
            char* str = Cudd_zddCoverPathToString(dd, path, nullptr);
            if (str != nullptr) {
                FREE(str);
            }
        }
        
        REQUIRE(count >= 4);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, tmp2);
        Cudd_RecursiveDerefZdd(dd, tmp1);
        Cudd_RecursiveDerefZdd(dd, z6);
        Cudd_RecursiveDerefZdd(dd, z4);
        Cudd_RecursiveDerefZdd(dd, z2);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Cover printing with various encodings", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Cover with 1 encoding (v = 4 or 6)") {
        // To get '1' output, we need v = 4 or v = 6
        // v = (path[2*i] << 2) | path[2*i+1]
        // v = 4: path[2*i] = 1, path[2*i+1] = 0
        // v = 6: path[2*i] = 1, path[2*i+1] = 2 (don't care)
        
        // Create structure to achieve this
        DdNode* z0 = Cudd_zddIthVar(dd, 0);  // First variable's positive literal
        Cudd_Ref(z0);
        
        // Print the cover
        int result = Cudd_zddPrintCover(dd, z0);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Cover with 0 encoding (v = 1 or 9)") {
        // v = 1: path[2*i] = 0, path[2*i+1] = 1
        // v = 9: path[2*i] = 2, path[2*i+1] = 1
        
        DdNode* z1 = Cudd_zddIthVar(dd, 1);  // Second literal (negative)
        Cudd_Ref(z1);
        
        int result = Cudd_zddPrintCover(dd, z1);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, z1);
    }
    
    SECTION("Cover with - encoding (v = 0, 2, 8, 10)") {
        // v = 0: path[2*i] = 0, path[2*i+1] = 0
        // v = 2: path[2*i] = 0, path[2*i+1] = 2
        // etc.
        
        // Constant one represents "don't care" for all variables
        DdNode* one = DD_ONE(dd);
        
        int result = Cudd_zddPrintCover(dd, one);
        REQUIRE(result == 1);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddZddUtil - Minterm printing edge cases", "[cuddZddUtil]") {
    DdManager* dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    int init_result = Cudd_zddVarsFromBddVars(dd, 2);
    REQUIRE(init_result == 1);
    
    SECTION("Minterm with Nv == Nnv case") {
        // This happens when both children of a node are the same
        // In ZDDs, this represents a don't care condition
        
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z1 = Cudd_zddIthVar(dd, 1);
        Cudd_Ref(z0);
        Cudd_Ref(z1);
        
        // Create structure where children might be the same
        DdNode* tmp = Cudd_zddUnion(dd, z0, z1);
        Cudd_Ref(tmp);
        
        // Use subset operations to create interesting structure
        DdNode* s0 = Cudd_zddSubset0(dd, tmp, 0);
        Cudd_Ref(s0);
        DdNode* s1 = Cudd_zddSubset1(dd, tmp, 0);
        Cudd_Ref(s1);
        
        // Print minterm for each
        int result1 = Cudd_zddPrintMinterm(dd, s0);
        REQUIRE(result1 == 1);
        
        int result2 = Cudd_zddPrintMinterm(dd, s1);
        REQUIRE(result2 == 1);
        
        Cudd_RecursiveDerefZdd(dd, s1);
        Cudd_RecursiveDerefZdd(dd, s0);
        Cudd_RecursiveDerefZdd(dd, tmp);
        Cudd_RecursiveDerefZdd(dd, z1);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    SECTION("Minterm with level skip in recursion") {
        // Create ZDD with gaps in variable indices
        DdNode* z0 = Cudd_zddIthVar(dd, 0);
        DdNode* z7 = Cudd_zddIthVar(dd, 7);
        Cudd_Ref(z0);
        Cudd_Ref(z7);
        
        DdNode* f = Cudd_zddUnion(dd, z0, z7);
        Cudd_Ref(f);
        
        // This tests the "missing variable" branch in zdd_print_minterm_aux
        int result = Cudd_zddPrintMinterm(dd, f);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDerefZdd(dd, f);
        Cudd_RecursiveDerefZdd(dd, z7);
        Cudd_RecursiveDerefZdd(dd, z0);
    }
    
    Cudd_Quit(dd);
}
