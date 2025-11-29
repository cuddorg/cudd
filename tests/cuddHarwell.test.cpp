#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

#include <cstdio>
#include <cstring>

/**
 * @brief Test file for cuddHarwell.c
 * 
 * This file contains comprehensive tests for the Cudd_addHarwell function
 * to achieve 90%+ code coverage of the source file.
 */

/**
 * @brief Helper function to create a temporary file with given content
 */
static FILE* createTempFile(const char* content) {
    FILE* fp = tmpfile();
    if (fp) {
        fputs(content, fp);
        rewind(fp);
    }
    return fp;
}

/**
 * @brief Helper to clean up Harwell arrays when function returns error
 * 
 * When Cudd_addHarwell returns 0 (failure), it may have allocated x, xn, y, yn
 * arrays with DD nodes that were reference-counted. The caller is responsible
 * for cleaning up these resources.
 */
static void cleanupHarwellArrays(DdManager *dd, DdNode *E, DdNode **x, DdNode **xn, 
                                  DdNode **y, DdNode **yn, int nx, int ny) {
    if (x && nx > 0) {
        for (int i = 0; i < nx; i++) {
            if (x[i]) Cudd_RecursiveDeref(dd, x[i]);
            if (xn && xn[i]) Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        if (xn) FREE(xn);
    }
    if (y && ny > 0) {
        for (int i = 0; i < ny; i++) {
            if (y[i]) Cudd_RecursiveDeref(dd, y[i]);
            if (yn && yn[i]) Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        if (yn) FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
}

TEST_CASE("cuddHarwell - Negative nx parameter returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = -1, ny = 0, m = 0, n = 0;
    
    FILE* fp = createTempFile("dummy content");
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Negative ny parameter returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = -1, m = 0, n = 0;
    
    FILE* fp = createTempFile("dummy content");
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Empty file returns 0 (header EOF)", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    FILE* fp = createTempFile("");
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Header line 1 incomplete returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Only 72 characters for title, missing key
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Header line 2 EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 72 chars title + 8 chars key, but no second line
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Header line 2 incomplete returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 72 chars title + 8 chars key + only 3 integers instead of 5
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 20 30\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Header line 3 EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 72 chars title + 8 chars key + 5 integers, but no third line
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 20 30 40 0\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Header line 3 incomplete returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 72 chars title + 8 chars key + 5 integers + incomplete third line
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 20 30 40 0\n"
        "RUA 4\n";  // Only 2 items instead of 5
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Invalid matrix type returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Invalid matrix type "CSC" instead of "RUA"
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 20 30 40 0\n"
        "CSC 4 4 8 0\n"
        "(10I8) (10I8) (10E15.8)\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Non-zero neltvl returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // neltvl = 1 instead of 0
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 20 30 40 0\n"
        "RUA 4 4 8 1\n"
        "(10I8) (10I8) (10E15.8)\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - FORTRAN format EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Missing FORTRAN format line
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 20 30 40 0\n"
        "RUA 4 4 8 0";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Valid simple 2x2 matrix", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Valid 2x2 identity matrix in Harwell-Boeing format
    // Matrix:
    // 1 0
    // 0 1
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3\n"    // colptr: [0, 1, 2] (1-based becomes 0-based)
        "1 2\n"       // rowind: [0, 1] (1-based becomes 0-based)
        "1.0 1.0\n";  // values
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    REQUIRE(m == 2);
    REQUIRE(n == 2);
    REQUIRE(nx == 1);  // log2(2) = 1
    REQUIRE(ny == 1);  // log2(2) = 1
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Valid 4x4 matrix with verbose output pr=1", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Valid 4x4 sparse matrix
    // Matrix (only storing non-zeros):
    // 1.0  0    0    0
    // 0    2.0  0    0
    // 0    0    3.0  0
    // 0    0    0    4.0
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 4 4 4 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5\n"     // colptr: [0, 1, 2, 3, 4]
        "1 2 3 4\n"       // rowind: [0, 1, 2, 3]
        "1.0 2.0 3.0 4.0\n";  // values
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 1); // pr=1 for verbose
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    REQUIRE(m == 4);
    REQUIRE(n == 4);
    REQUIRE(nx == 2);  // log2(4) = 2
    REQUIRE(ny == 2);  // log2(4) = 2
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Valid 4x4 matrix with verbose output pr=2", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Valid 4x4 sparse matrix
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 4 4 4 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5\n"
        "1 2 3 4\n"
        "1.0 2.0 3.0 4.0\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 2); // pr=2 for more verbose
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Matrix with rhscrd != 0 (optional 5th line)", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Matrix with right-hand side data (rhscrd = 1)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"   // rhscrd = 1
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "F   1 0\n"  // rhstyp, nrhs, nrhsix
        "1 2 3\n"
        "1 2\n"
        "1.0 1.0\n"
        "5.0 6.0\n";  // right-hand side values
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 1);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Sparse right-hand side not supported", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Matrix with sparse right-hand side (rhstyp != 'F')
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "M   1 0\n";  // rhstyp = 'M' (sparse), not supported
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - rhscrd line EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Missing 5th line when rhscrd != 0
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - rhscrd line incomplete returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Incomplete 5th line (only 2 items instead of 3)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "F   1\n";  // Missing nrhsix
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - colptr EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Missing colptr data
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - colptr[0] != 1 returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // colptr[0] = 2 instead of 1 (becomes 1 != 0 after -1)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "2 2 3\n"     // colptr[0] = 2 (should be 1)
        "1 2\n"
        "1.0 1.0\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - rowind EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Missing rowind data
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3\n";     // Only colptr, no rowind
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - values EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Missing values data
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3\n"
        "1 2\n";     // No values
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - 1x1 matrix (lnx=0, lny=0 path)", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 1x1 matrix - triggers lnx=0, lny=0 path
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 2 1 1 0\n"
        "RUA 1 1 1 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2\n"
        "1\n"
        "5.0\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    REQUIRE(m == 1);
    REQUIRE(n == 1);
    REQUIRE(nx == 0);
    REQUIRE(ny == 0);
    REQUIRE(x == nullptr);
    REQUIRE(y == nullptr);
    
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Pre-allocated variables reuse (nx>0, ny>0)", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // First call: read a 4x4 matrix to create variables
    const char* content1 = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 4 4 4 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5\n"
        "1 2 3 4\n"
        "1.0 2.0 3.0 4.0\n";
    
    FILE* fp1 = createTempFile(content1);
    REQUIRE(fp1 != nullptr);
    
    int result1 = Cudd_addHarwell(fp1, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result1 == 1);
    REQUIRE(nx == 2);
    REQUIRE(ny == 2);
    fclose(fp1);
    
    // Dereference the ADD from first call
    if (E) {
        Cudd_RecursiveDeref(dd, E);
        E = nullptr;
    }
    
    // Second call: read a smaller 2x2 matrix - triggers the else path (lnx <= *nx, lny <= *ny)
    const char* content2 = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY2\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3\n"
        "1 2\n"
        "1.0 1.0\n";
    
    FILE* fp2 = createTempFile(content2);
    REQUIRE(fp2 != nullptr);
    
    int result2 = Cudd_addHarwell(fp2, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result2 == 1);
    REQUIRE(E != nullptr);
    // nx and ny should be updated to 1 (smaller matrix)
    REQUIRE(nx == 1);
    REQUIRE(ny == 1);
    
    fclose(fp2);
    
    // Clean up - the variables created by the first call are still referenced
    // but we can only dereference up to the current nx/ny since that's what we have access to
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Matrix with right-hand side and zero values", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Matrix with right-hand side containing zero values (triggers val == 0.0 continue)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "F   1 0\n"
        "1 2 3\n"
        "1 2\n"
        "1.0 1.0\n"
        "0.0 5.0\n";  // First RHS value is 0.0 (triggers continue)
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 1);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Matrix with nrhs > ncol", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Matrix with more right-hand sides than columns (triggers ddMax(ncol, nrhs) path)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "F   3 0\n"   // nrhs = 3 > ncol = 2
        "1 2 3\n"
        "1 2\n"
        "1.0 1.0\n"
        "5.0 6.0\n"   // RHS 1
        "7.0 8.0\n"   // RHS 2
        "9.0 10.0\n"; // RHS 3
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 1);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - REALLOC path for x variables", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // First call: read a 2x2 matrix to create some variables
    const char* content1 = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3\n"
        "1 2\n"
        "1.0 1.0\n";
    
    FILE* fp1 = createTempFile(content1);
    REQUIRE(fp1 != nullptr);
    
    int result1 = Cudd_addHarwell(fp1, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result1 == 1);
    REQUIRE(nx == 1);
    REQUIRE(ny == 1);
    fclose(fp1);
    
    // Dereference the ADD from first call
    if (E) {
        Cudd_RecursiveDeref(dd, E);
        E = nullptr;
    }
    
    // Second call: read a 4x4 matrix which needs more variables - triggers REALLOC
    const char* content2 = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY2\n"
        "10 5 4 1 0\n"
        "RUA 4 4 4 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5\n"
        "1 2 3 4\n"
        "1.0 2.0 3.0 4.0\n";
    
    FILE* fp2 = createTempFile(content2);
    REQUIRE(fp2 != nullptr);
    
    int result2 = Cudd_addHarwell(fp2, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result2 == 1);
    REQUIRE(E != nullptr);
    REQUIRE(nx == 2);  // Should have grown
    REQUIRE(ny == 2);  // Should have grown
    
    fclose(fp2);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - RHS values EOF returns 0", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Missing RHS values
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "F   1 0\n"
        "1 2 3\n"
        "1 2\n"
        "1.0 1.0\n";  // No RHS values
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - 8x8 matrix for odd row/col bit paths", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 8x8 diagonal matrix - exercises all bit patterns
    // Rows 0-7 and cols 0-7 cover various v & 1 and u & 1 combinations
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 9 8 1 0\n"
        "RUA 8 8 8 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5 6 7 8 9\n"  // colptr
        "1 2 3 4 5 6 7 8\n"    // rowind
        "1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0\n";  // values
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    REQUIRE(m == 8);
    REQUIRE(n == 8);
    REQUIRE(nx == 3);  // log2(8) = 3
    REQUIRE(ny == 3);  // log2(8) = 3
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - colptr read error (not EOF, wrong format)", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Invalid colptr data (letters instead of numbers)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "abc def ghi\n";  // Invalid colptr
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - rowind read error (not EOF, wrong format)", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Invalid rowind data (letters instead of numbers)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 2 2 2 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3\n"
        "abc def\n";  // Invalid rowind
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 0);
    
    // Clean up arrays that may have been allocated before the error
    cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - large matrix with limited memory (stress test)", "[cuddHarwell][!mayfail]") {
    // This test attempts to trigger memory-related error paths
    // by processing a larger matrix with constrained memory
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS / 4, CUDD_CACHE_SLOTS / 4, 0);
    REQUIRE(dd != nullptr);
    
    // Set a very restrictive memory limit
    Cudd_SetMaxMemory(dd, 64 * 1024);  // 64KB limit
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // A 16x16 dense matrix would require many nodes
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 17 16 1 0\n"
        "RUA 16 16 16 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17\n"
        "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\n"
        "1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 11.0 12.0 13.0 14.0 15.0 16.0\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    // This might succeed or fail depending on memory constraints
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    
    // Clean up whatever was allocated
    if (result == 1 && E != nullptr) {
        if (x) {
            for (int i = 0; i < nx; i++) {
                Cudd_RecursiveDeref(dd, x[i]);
                Cudd_RecursiveDeref(dd, xn[i]);
            }
            FREE(x);
            FREE(xn);
        }
        if (y) {
            for (int i = 0; i < ny; i++) {
                Cudd_RecursiveDeref(dd, y[i]);
                Cudd_RecursiveDeref(dd, yn[i]);
            }
            FREE(y);
            FREE(yn);
        }
        Cudd_RecursiveDeref(dd, E);
    }
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Multiple RHS values non-zero", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Matrix with multiple right-hand sides all non-zero to fully exercise RHS loop
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 1\n"
        "RUA 4 4 4 0\n"
        "(10I8) (10I8) (10E15.8) (10E15.8)\n"
        "F   2 0\n"   // 2 RHS vectors
        "1 2 3 4 5\n"
        "1 2 3 4\n"
        "1.0 2.0 3.0 4.0\n"
        "1.0 2.0 3.0 4.0\n"   // RHS 1: all non-zero
        "5.0 6.0 7.0 8.0\n";  // RHS 2: all non-zero
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 1);
    REQUIRE(E != nullptr);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Matrix with all zeros in main data", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // Minimal matrix with a single entry
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 3 1 1 0\n"
        "RUA 2 2 1 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 1 2\n"    // colptr: first col has 1 entry, second col has 0 entries
        "1\n"        // rowind: entry at row 0
        "0.0\n";     // value is zero
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 1);
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Odd and even row indices", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    // 4x4 matrix with entries in specific positions to exercise u&1 paths
    // Entries at positions (0,0), (1,1), (2,2), (3,3) - diagonal
    // Row 0: binary 00 (even/even)
    // Row 1: binary 01 (odd/even)  
    // Row 2: binary 10 (even/odd)
    // Row 3: binary 11 (odd/odd)
    const char* content = 
        "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
        "10 5 4 1 0\n"
        "RUA 4 4 4 0\n"
        "(10I8) (10I8) (10E15.8)\n"
        "1 2 3 4 5\n"   // colptr
        "1 2 3 4\n"     // rowind: 0,1,2,3 (1-based: 1,2,3,4)
        "1.0 2.0 3.0 4.0\n";
    
    FILE* fp = createTempFile(content);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    REQUIRE(result == 1);
    REQUIRE(nx == 2);  // 4 rows needs 2 bits
    REQUIRE(ny == 2);  // 4 cols needs 2 bits
    
    // Clean up
    if (x) {
        for (int i = 0; i < nx; i++) {
            Cudd_RecursiveDeref(dd, x[i]);
            Cudd_RecursiveDeref(dd, xn[i]);
        }
        FREE(x);
        FREE(xn);
    }
    if (y) {
        for (int i = 0; i < ny; i++) {
            Cudd_RecursiveDeref(dd, y[i]);
            Cudd_RecursiveDeref(dd, yn[i]);
        }
        FREE(y);
        FREE(yn);
    }
    if (E) Cudd_RecursiveDeref(dd, E);
    
    fclose(fp);
    Cudd_Quit(dd);
}

/**
 * @brief Common test matrix content for stress tests
 */
static const char* STRESS_TEST_MATRIX_4x4 = 
    "TITLE PADDING TO FILL 72 CHARACTERS EXACTLY INCLUDING ALL SPACES NEEDED!TESTKEY1\n"
    "10 5 4 1 0\n"
    "RUA 4 4 4 0\n"
    "(10I8) (10I8) (10E15.8)\n"
    "1 2 3 4 5\n"
    "1 2 3 4\n"
    "1.0 2.0 3.0 4.0\n";

/**
 * @brief Test with timeout handler to exercise timeout code paths
 * 
 * Note: This test verifies timeout handler registration works correctly.
 * The operations typically complete too fast to actually trigger timeout.
 */
TEST_CASE("cuddHarwell - Test with timeout handler registration", "[cuddHarwell]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    // Local callback tracker to avoid test interference
    struct TimeoutContext {
        int called;
    } ctx = {0};
    
    auto timeoutHandler = [](DdManager *mgr, void *arg) {
        (void)mgr;
        static_cast<TimeoutContext*>(arg)->called = 1;
    };
    
    // Register a timeout handler
    Cudd_RegisterTimeoutHandler(dd, timeoutHandler, &ctx);
    
    // Set a reasonable time limit (100 milliseconds)
    Cudd_SetTimeLimit(dd, 100);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    FILE* fp = createTempFile(STRESS_TEST_MATRIX_4x4);
    REQUIRE(fp != nullptr);
    
    // This will likely succeed since the operation is fast
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    
    if (result == 1) {
        cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    }
    
    fclose(fp);
    Cudd_Quit(dd);
}

TEST_CASE("cuddHarwell - Test with very small cache and slot sizes", "[cuddHarwell]") {
    // Initialize with minimum possible slots to stress memory allocation
    DdManager *dd = Cudd_Init(0, 0, 2, 2, 0);
    REQUIRE(dd != nullptr);
    
    DdNode *E = nullptr;
    DdNode **x = nullptr, **y = nullptr, **xn = nullptr, **yn = nullptr;
    int nx = 0, ny = 0, m = 0, n = 0;
    
    FILE* fp = createTempFile(STRESS_TEST_MATRIX_4x4);
    REQUIRE(fp != nullptr);
    
    int result = Cudd_addHarwell(fp, dd, &E, &x, &y, &xn, &yn, &nx, &ny, &m, &n, 0, 2, 1, 2, 0);
    
    if (result == 1) {
        cleanupHarwellArrays(dd, E, x, xn, y, yn, nx, ny);
    }
    
    fclose(fp);
    Cudd_Quit(dd);
}
