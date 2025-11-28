#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

/**
 * @brief Test file for cuddRead.c
 * 
 * This file contains comprehensive tests for the cuddRead module
 * to achieve high code coverage for both Cudd_addRead and Cudd_bddRead functions.
 */

// Helper function to create a temporary file with specific content
static FILE* create_temp_file_with_content(const char* content) {
    char temp_name[] = "/tmp/cudd_read_test_XXXXXX";
    int fd = mkstemp(temp_name);
    if (fd == -1) return nullptr;
    FILE* fp = fdopen(fd, "w+");
    if (fp) {
        unlink(temp_name); // Delete on close
        fputs(content, fp);
        rewind(fp);
    }
    return fp;
}

// =============================================================================
// Tests for Cudd_addRead
// =============================================================================

TEST_CASE("Cudd_addRead - Basic matrix reading", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Read simple 2x2 matrix with one entry") {
        const char* content = "2 2\n0 0 1.5\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        REQUIRE(m == 2);
        REQUIRE(n == 2);
        REQUIRE(nx == 1);
        REQUIRE(ny == 1);
        REQUIRE(x != nullptr);
        REQUIRE(y != nullptr);
        REQUIRE(xn != nullptr);
        REQUIRE(yn != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read 4x4 matrix with multiple entries") {
        const char* content = "4 4\n0 0 1.0\n1 1 2.0\n2 2 3.0\n3 3 4.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        REQUIRE(m == 4);
        REQUIRE(n == 4);
        REQUIRE(nx == 2);
        REQUIRE(ny == 2);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read matrix with existing variables") {
        // First create some variables
        DdNode *var0 = Cudd_addIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *var1 = Cudd_addIthVar(manager, 1);
        Cudd_Ref(var1);
        
        DdNode **x = (DdNode **)ALLOC(DdNode*, 1);
        DdNode **xn = (DdNode **)ALLOC(DdNode*, 1);
        DdNode **y = (DdNode **)ALLOC(DdNode*, 1);
        DdNode **yn = (DdNode **)ALLOC(DdNode*, 1);
        x[0] = var0;
        xn[0] = Cudd_addCmpl(manager, var0);
        Cudd_Ref(xn[0]);
        y[0] = var1;
        yn[0] = Cudd_addCmpl(manager, var1);
        Cudd_Ref(yn[0]);
        
        const char* content = "2 2\n0 1 5.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        int nx = 1, ny = 1, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read empty matrix (no entries after dimensions)") {
        const char* content = "2 2\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read larger matrix 8x8") {
        const char* content = "8 8\n0 0 1.0\n7 7 2.0\n3 4 3.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(m == 8);
        REQUIRE(n == 8);
        REQUIRE(nx == 3);
        REQUIRE(ny == 3);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read 1x1 matrix (edge case)") {
        const char* content = "1 1\n0 0 42.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(m == 1);
        REQUIRE(n == 1);
        REQUIRE(nx == 0);
        REQUIRE(ny == 0);
        
        Cudd_RecursiveDeref(manager, E);
        fclose(fp);
    }
    
    SECTION("Read non-square matrix 4x8") {
        const char* content = "4 8\n0 0 1.0\n3 7 2.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(m == 4);
        REQUIRE(n == 8);
        REQUIRE(nx == 2);
        REQUIRE(ny == 3);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addRead - Error handling", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("EOF at start") {
        const char* content = "";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        fclose(fp);
    }
    
    SECTION("Invalid format at start (single number)") {
        const char* content = "5\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        fclose(fp);
    }
    
    SECTION("Invalid format in matrix entries (not 3 values)") {
        const char* content = "2 2\n0 0\n";  // Missing value
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Row index out of range (u >= m)") {
        const char* content = "2 2\n2 0 1.0\n";  // Row 2 is out of range for 2x2
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Column index out of range (v >= n)") {
        const char* content = "2 2\n0 2 1.0\n";  // Column 2 is out of range for 2x2
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Negative row index (u < 0)") {
        const char* content = "2 2\n-1 0 1.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Negative column index (v < 0)") {
        const char* content = "2 2\n0 -1 1.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Invalid format with text instead of number") {
        const char* content = "abc def\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addRead - Path coverage", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Test different bit patterns in row/column indices") {
        // Test various bit patterns to cover both branches of u & 1 and v & 1
        const char* content = "8 8\n0 0 1.0\n1 0 2.0\n0 1 3.0\n1 1 4.0\n7 7 5.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Test with different step values") {
        const char* content = "4 4\n0 0 1.0\n3 3 2.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        // Use step 1 instead of step 2
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 1, 10, 1);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Test with different base indices") {
        const char* content = "2 2\n0 0 1.0\n1 1 2.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        // Use higher base indices
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 10, 2, 20, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }

    SECTION("Test when lnx <= *nx (no x realloc needed)") {
        // Pre-allocate arrays with initialized variables
        // For a 2x2 matrix, we need 1 row var and 1 col var
        // We'll create arrays that are already large enough (2 slots for lnx=1)
        DdNode **x = (DdNode **)ALLOC(DdNode*, 2);
        DdNode **xn = (DdNode **)ALLOC(DdNode*, 2);
        DdNode **y = (DdNode **)ALLOC(DdNode*, 2);
        DdNode **yn = (DdNode **)ALLOC(DdNode*, 2);
        
        // Create initial variables at the expected indices
        // bx=0, sx=2 means indices 0, 2, 4, etc.
        // by=1, sy=2 means indices 1, 3, 5, etc.
        
        // Create x[0] at index 0
        x[0] = Cudd_addIthVar(manager, 0);
        Cudd_Ref(x[0]);
        xn[0] = Cudd_addCmpl(manager, x[0]);
        Cudd_Ref(xn[0]);
        
        // Create y[0] at index 1
        y[0] = Cudd_addIthVar(manager, 1);
        Cudd_Ref(y[0]);
        yn[0] = Cudd_addCmpl(manager, y[0]);
        Cudd_Ref(yn[0]);
        
        const char* content = "2 2\n0 0 1.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        int nx = 1, ny = 1, m = 0, n = 0;  // Already have 1 of each variable
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(nx == 1);  // Should remain 1 since 2x2 needs only 1 bit
        REQUIRE(ny == 1);
        
        Cudd_RecursiveDeref(manager, E);
        // The function used our pre-existing variables, deref them
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

// =============================================================================
// Tests for Cudd_bddRead
// =============================================================================

TEST_CASE("Cudd_bddRead - Basic graph reading", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Read simple 2x2 graph with one arc") {
        const char* content = "2 2\n0 0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        REQUIRE(m == 2);
        REQUIRE(n == 2);
        REQUIRE(nx == 1);
        REQUIRE(ny == 1);
        REQUIRE(x != nullptr);
        REQUIRE(y != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Read 4x4 graph with multiple arcs") {
        const char* content = "4 4\n0 0\n1 1\n2 2\n3 3\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        REQUIRE(m == 4);
        REQUIRE(n == 4);
        REQUIRE(nx == 2);
        REQUIRE(ny == 2);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Read empty graph (no arcs)") {
        const char* content = "2 2\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Read 8x8 graph") {
        const char* content = "8 8\n0 0\n7 7\n3 4\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(m == 8);
        REQUIRE(n == 8);
        REQUIRE(nx == 3);
        REQUIRE(ny == 3);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Read 1x1 graph (edge case)") {
        const char* content = "1 1\n0 0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(m == 1);
        REQUIRE(n == 1);
        REQUIRE(nx == 0);
        REQUIRE(ny == 0);
        
        Cudd_RecursiveDeref(manager, E);
        fclose(fp);
    }
    
    SECTION("Read non-square graph 4x8") {
        const char* content = "4 8\n0 0\n3 7\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(m == 4);
        REQUIRE(n == 8);
        REQUIRE(nx == 2);
        REQUIRE(ny == 3);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Read with existing variables") {
        // First create some variables
        DdNode *var0 = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(var0);
        DdNode *var1 = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(var1);
        
        DdNode **x = (DdNode **)ALLOC(DdNode*, 1);
        DdNode **y = (DdNode **)ALLOC(DdNode*, 1);
        x[0] = var0;
        y[0] = var1;
        
        const char* content = "2 2\n0 1\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        int nx = 1, ny = 1, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddRead - Error handling", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("EOF at start") {
        const char* content = "";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        fclose(fp);
    }
    
    SECTION("Invalid format at start (single number)") {
        const char* content = "5\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        fclose(fp);
    }
    
    SECTION("Invalid format in arc entries (not 2 values)") {
        const char* content = "2 2\n0\n";  // Missing second value
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Row index out of range (u >= m)") {
        const char* content = "2 2\n2 0\n";  // Row 2 is out of range for 2x2
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Column index out of range (v >= n)") {
        const char* content = "2 2\n0 2\n";  // Column 2 is out of range for 2x2
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Negative row index (u < 0)") {
        const char* content = "2 2\n-1 0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Negative column index (v < 0)") {
        const char* content = "2 2\n0 -1\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        // Cleanup
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Invalid format with text") {
        const char* content = "abc def\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 0);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddRead - Path coverage", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Test different bit patterns in row/column indices") {
        // Test various bit patterns to cover both branches of u & 1 and v & 1
        const char* content = "8 8\n0 0\n1 0\n0 1\n1 1\n7 7\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Test with different step values") {
        const char* content = "4 4\n0 0\n3 3\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        // Use step 1
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 1, 10, 1);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Test with different base indices") {
        const char* content = "2 2\n0 0\n1 1\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        // Use higher base indices
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 10, 2, 20, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Test when lnx <= *nx (no x realloc needed)") {
        // Pre-allocate arrays with initialized variables
        // For a 2x2 matrix, we need 1 row var and 1 col var
        DdNode **x = (DdNode **)ALLOC(DdNode*, 2);
        DdNode **y = (DdNode **)ALLOC(DdNode*, 2);
        
        // Create initial variables at the expected indices
        // bx=0, sx=2 means indices 0, 2, 4, etc.
        // by=1, sy=2 means indices 1, 3, 5, etc.
        x[0] = Cudd_bddIthVar(manager, 0);
        Cudd_Ref(x[0]);
        y[0] = Cudd_bddIthVar(manager, 1);
        Cudd_Ref(y[0]);
        
        const char* content = "2 2\n0 0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        int nx = 1, ny = 1, m = 0, n = 0;  // Already have 1 of each variable
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(nx == 1);  // Should remain 1 since 2x2 needs only 1 bit
        REQUIRE(ny == 1);
        
        Cudd_RecursiveDeref(manager, E);
        // The function used our pre-existing variables, deref them
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Test all bits set in minterm building") {
        // Use indices that have all bits set to 1
        const char* content = "16 16\n15 15\n";  // Binary: 1111, 1111
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Test alternating bits in minterm building") {
        // Use indices with alternating bits: 0101 = 5, 1010 = 10
        const char* content = "16 16\n5 10\n10 5\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_addRead - Complete entry reading", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Read multiple entries with different values") {
        const char* content = "4 4\n0 0 1.0\n0 1 2.0\n1 0 3.0\n1 1 4.0\n2 2 5.0\n3 3 6.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read with negative values") {
        const char* content = "2 2\n0 0 -1.5\n1 1 -2.5\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read with zero values") {
        const char* content = "2 2\n0 0 0.0\n1 1 0.0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    SECTION("Read large values") {
        const char* content = "2 2\n0 0 1e10\n1 1 1e-10\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_addRead(fp, manager, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
            Cudd_RecursiveDeref(manager, xn[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
            Cudd_RecursiveDeref(manager, yn[i]);
        }
        FREE(x);
        FREE(y);
        FREE(xn);
        FREE(yn);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddRead - Complete arc reading", "[cuddRead]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Read multiple arcs creating full graph") {
        const char* content = "4 4\n0 0\n0 1\n0 2\n0 3\n1 0\n1 1\n1 2\n1 3\n2 0\n2 1\n2 2\n2 3\n3 0\n3 1\n3 2\n3 3\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        REQUIRE(E != nullptr);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    SECTION("Read sparse graph") {
        const char* content = "8 8\n0 7\n7 0\n";
        FILE *fp = create_temp_file_with_content(content);
        REQUIRE(fp != nullptr);
        
        DdNode *E = nullptr;
        DdNode **x = nullptr, **y = nullptr;
        int nx = 0, ny = 0, m = 0, n = 0;
        
        int result = Cudd_bddRead(fp, manager, &E, &x, &y, &nx, &ny, &m, &n, 0, 2, 1, 2);
        REQUIRE(result == 1);
        
        Cudd_RecursiveDeref(manager, E);
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(manager, x[i]);
        }
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(manager, y[i]);
        }
        FREE(x);
        FREE(y);
        fclose(fp);
    }
    
    Cudd_Quit(manager);
}
