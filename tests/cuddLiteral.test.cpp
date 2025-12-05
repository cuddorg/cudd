#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddLiteral.c
 * 
 * This file contains comprehensive tests for the cuddLiteral module
 * to ensure high code coverage (>90%) and correct functionality.
 * 
 * The module implements Cudd_bddLiteralSetIntersection and its recursive
 * helper function cuddBddLiteralSetIntersectionRecur.
 * 
 * A literal set is represented as a cube (conjunction of literals).
 * The intersection of two literal sets contains the literals that
 * appear in both sets with the same phase.
 */

TEST_CASE("Cudd_bddLiteralSetIntersection - Identical arguments", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Identical constant 1") {
        // f == g case with constants - line 152
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, one, one);
        REQUIRE(result == one);
    }
    
    SECTION("Identical single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // f == g case with variable - line 152
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, x);
        REQUIRE(result == x);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Identical complemented variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        DdNode *notX = Cudd_Not(x);
        
        // f == g case with complemented variable - line 152
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, notX, notX);
        REQUIRE(result == notX);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Identical cube (multiple variables)") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        // Create cube x AND y
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        // f == g case with cube - line 152
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube, cube);
        REQUIRE(result == cube);
        
        Cudd_RecursiveDeref(manager, cube);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Complementary arguments", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    SECTION("Complementary single variable (v and v')") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        DdNode *notX = Cudd_Not(x);
        
        // F == G case (v and v' are complements) - line 162
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, notX);
        REQUIRE(result == one); // Empty intersection represented as 1
        
        // Also test reverse order
        result = Cudd_bddLiteralSetIntersection(manager, notX, x);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Different top variables (topf < topg)", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables - they will be ordered by index
    DdNode *x = Cudd_bddNewVar(manager);  // index 0, topf will be smaller
    DdNode *y = Cudd_bddNewVar(manager);  // index 1, topg will be larger
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Different variables - no overlap") {
        // f = x, g = y - topf < topg, then move down on f
        // Lines 171-180 - move down on f branch
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        // x and y have no common literals, so intersection is 1 (empty set)
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Complemented variables - no overlap") {
        DdNode *notX = Cudd_Not(x);
        
        // f = !x, g = y - topf < topg, move down on f with complement handling
        // Lines 171-180 with comple = 1 case
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, notX, y);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Different top variables (topg < topf)", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    DdNode *x = Cudd_bddNewVar(manager);  // index 0
    DdNode *y = Cudd_bddNewVar(manager);  // index 1
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Different variables - reverse order") {
        // f = y, g = x - topg < topf, so move down on g
        // Lines 181-191 - move down on g branch
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, y, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
    }
    
    SECTION("Complemented g variable") {
        DdNode *notX = Cudd_Not(x);
        
        // f = y, g = !x - topg < topf, move down on g with complement handling
        // Lines 181-191 with comple = 1 for g
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, y, notX);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        Cudd_RecursiveDeref(manager, result);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Same variable, same phase", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Same positive literal in both cubes") {
        // f = x AND y, g = x
        // Both have x in positive phase
        // Lines 229-243: phasef == phaseg, phasef == 1
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube, x);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x); // Intersection contains x
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    SECTION("Same negative literal in both cubes") {
        DdNode *notX = Cudd_Not(x);
        
        // f = !x AND y, g = !x
        // Both have x in negative phase
        // Lines 229-243: phasef == phaseg, phasef == 0
        DdNode *cube = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(cube);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube, notX);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == notX); // Intersection contains !x
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Same variable, different phase", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Positive and negative literals of same variable") {
        DdNode *notX = Cudd_Not(x);
        
        // f = x, g = !x (already tested above for the complementary case)
        // This exercises line 162
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, notX);
        REQUIRE(result == one);
    }
    
    SECTION("Cubes with conflicting literals") {
        DdNode *notX = Cudd_Not(x);
        
        // f = x AND y, g = !x AND y
        // x appears in different phases
        // Line 229: phasef != phaseg, so res = tmp (skip the literal)
        DdNode *cube1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == y); // Only y is common with same phase
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Complex cubes", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);  // index 0
    DdNode *y = Cudd_bddNewVar(manager);  // index 1
    DdNode *z = Cudd_bddNewVar(manager);  // index 2
    DdNode *w = Cudd_bddNewVar(manager);  // index 3
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    Cudd_Ref(w);
    
    SECTION("Multiple overlapping positive literals") {
        // f = x AND y AND z, g = x AND y AND w
        // Common: x AND y
        DdNode *temp1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(temp1);
        DdNode *cube1 = Cudd_bddAnd(manager, temp1, z);
        Cudd_Ref(cube1);
        Cudd_RecursiveDeref(manager, temp1);
        
        temp1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(temp1);
        DdNode *cube2 = Cudd_bddAnd(manager, temp1, w);
        Cudd_Ref(cube2);
        Cudd_RecursiveDeref(manager, temp1);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    SECTION("Mixed positive and negative literals with overlap") {
        // f = x AND !y AND z, g = x AND !y AND w
        // Common: x AND !y
        DdNode *notY = Cudd_Not(y);
        
        DdNode *temp1 = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(temp1);
        DdNode *cube1 = Cudd_bddAnd(manager, temp1, z);
        Cudd_Ref(cube1);
        Cudd_RecursiveDeref(manager, temp1);
        
        temp1 = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(temp1);
        DdNode *cube2 = Cudd_bddAnd(manager, temp1, w);
        Cudd_Ref(cube2);
        Cudd_RecursiveDeref(manager, temp1);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        DdNode *expected = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    SECTION("No overlap at all") {
        // f = x AND y, g = z AND w
        DdNode *cube1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, z, w);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one); // Empty intersection
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    SECTION("Partial overlap with conflicting literals") {
        // f = x AND y AND z, g = x AND !y AND z
        // x and z agree, y conflicts
        DdNode *notY = Cudd_Not(y);
        
        DdNode *temp1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(temp1);
        DdNode *cube1 = Cudd_bddAnd(manager, temp1, z);
        Cudd_Ref(cube1);
        Cudd_RecursiveDeref(manager, temp1);
        
        temp1 = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(temp1);
        DdNode *cube2 = Cudd_bddAnd(manager, temp1, z);
        Cudd_Ref(cube2);
        Cudd_RecursiveDeref(manager, temp1);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Common: x AND z
        DdNode *expected = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_RecursiveDeref(manager, w);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Cache behavior", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Repeated calls should hit cache") {
        // Lines 197-200: cache lookup
        DdNode *cube = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube);
        
        DdNode *result1 = Cudd_bddLiteralSetIntersection(manager, cube, x);
        REQUIRE(result1 != nullptr);
        Cudd_Ref(result1);
        
        // Second call should hit cache
        DdNode *result2 = Cudd_bddLiteralSetIntersection(manager, cube, x);
        REQUIRE(result2 != nullptr);
        Cudd_Ref(result2);
        
        REQUIRE(result1 == result2);
        
        Cudd_RecursiveDeref(manager, result1);
        Cudd_RecursiveDeref(manager, result2);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Empty set (constant 1)", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("Empty set intersected with variable") {
        // f = 1 (empty set), g = x
        // Line 195: when f reaches one after loop
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, one, x);
        REQUIRE(result == one);
    }
    
    SECTION("Variable intersected with empty set") {
        // f = x, g = 1 (empty set)
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, one);
        REQUIRE(result == one);
    }
    
    SECTION("Empty set intersected with empty set") {
        // f = 1, g = 1
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, one, one);
        REQUIRE(result == one);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Complemented cubes", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("All negative literals with overlap") {
        // f = !x AND !y, g = !x AND !z
        // Common: !x
        DdNode *notX = Cudd_Not(x);
        DdNode *notY = Cudd_Not(y);
        DdNode *notZ = Cudd_Not(z);
        
        DdNode *cube1 = Cudd_bddAnd(manager, notX, notY);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, notX, notZ);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == notX);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Coverage for while loop branches", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    
    // Create variables with specific indices to control the while loop
    DdNode *v0 = Cudd_bddNewVar(manager);  // index 0
    DdNode *v1 = Cudd_bddNewVar(manager);  // index 1
    DdNode *v2 = Cudd_bddNewVar(manager);  // index 2
    DdNode *v3 = Cudd_bddNewVar(manager);  // index 3
    Cudd_Ref(v0);
    Cudd_Ref(v1);
    Cudd_Ref(v2);
    Cudd_Ref(v3);
    
    SECTION("Force multiple iterations of while loop - topf < topg") {
        // f = v0 AND v1 AND v2, g = v3
        // Will iterate: v0 (skip) -> v1 (skip) -> v2 (skip) -> reach one
        DdNode *temp = Cudd_bddAnd(manager, v0, v1);
        Cudd_Ref(temp);
        DdNode *cube = Cudd_bddAnd(manager, temp, v2);
        Cudd_Ref(cube);
        Cudd_RecursiveDeref(manager, temp);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube, v3);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    SECTION("Force multiple iterations of while loop - topg < topf") {
        // f = v3, g = v0 AND v1 AND v2
        // Will iterate: v0 (skip) -> v1 (skip) -> v2 (skip) -> reach one
        DdNode *temp = Cudd_bddAnd(manager, v0, v1);
        Cudd_Ref(temp);
        DdNode *cube = Cudd_bddAnd(manager, temp, v2);
        Cudd_Ref(cube);
        Cudd_RecursiveDeref(manager, temp);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, v3, cube);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube);
    }
    
    SECTION("Alternating iterations in while loop") {
        // f = v0 AND v2, g = v1 AND v3
        // Will alternate between topf < topg and topg < topf
        DdNode *cube1 = Cudd_bddAnd(manager, v0, v2);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, v1, v3);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == one); // No overlap
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    Cudd_RecursiveDeref(manager, v0);
    Cudd_RecursiveDeref(manager, v1);
    Cudd_RecursiveDeref(manager, v2);
    Cudd_RecursiveDeref(manager, v3);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Phase detection coverage", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("phasef = 1, phaseg = 1 (both positive)") {
        // Both cubes have x in positive phase
        // Lines 231-237: phasef == phaseg, phasef == 1
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, x);
        REQUIRE(result == x);
    }
    
    SECTION("phasef = 0, phaseg = 0 (both negative)") {
        // Both cubes have x in negative phase
        // Lines 231-237: phasef == phaseg, phasef == 0
        DdNode *notX = Cudd_Not(x);
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, notX, notX);
        REQUIRE(result == notX);
    }
    
    SECTION("phasef = 1, phaseg = 0 (positive vs negative)") {
        // Line 229-230: phasef != phaseg, res = tmp
        DdNode *notX = Cudd_Not(x);
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, notX);
        REQUIRE(result == one); // Complementary - no common literals
    }
    
    SECTION("phasef = 0, phaseg = 1 (negative vs positive)") {
        // Line 229-230: phasef != phaseg, res = tmp
        DdNode *notX = Cudd_Not(x);
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, notX, x);
        REQUIRE(result == one); // Complementary - no common literals
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Deep recursion", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    // Create many variables
    const int NUM_VARS = 10;
    DdNode *vars[NUM_VARS];
    for (int i = 0; i < NUM_VARS; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Large cube intersection") {
        // Build cube1 = v0 AND v1 AND v2 AND v3 AND v4
        DdNode *cube1 = Cudd_ReadOne(manager);
        Cudd_Ref(cube1);
        for (int i = 0; i < 5; i++) {
            DdNode *temp = Cudd_bddAnd(manager, cube1, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, cube1);
            cube1 = temp;
        }
        
        // Build cube2 = v0 AND v1 AND v5 AND v6 AND v7
        DdNode *cube2 = Cudd_ReadOne(manager);
        Cudd_Ref(cube2);
        DdNode *temp = Cudd_bddAnd(manager, cube2, vars[0]);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(manager, cube2);
        cube2 = temp;
        
        temp = Cudd_bddAnd(manager, cube2, vars[1]);
        Cudd_Ref(temp);
        Cudd_RecursiveDeref(manager, cube2);
        cube2 = temp;
        
        for (int i = 5; i < 8; i++) {
            temp = Cudd_bddAnd(manager, cube2, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, cube2);
            cube2 = temp;
        }
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        
        // Expected: v0 AND v1
        DdNode *expected = Cudd_bddAnd(manager, vars[0], vars[1]);
        Cudd_Ref(expected);
        REQUIRE(result == expected);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, expected);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    // Clean up
    for (int i = 0; i < NUM_VARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Single variable cubes", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    DdNode *notX = Cudd_Not(x);
    DdNode *notY = Cudd_Not(y);
    
    SECTION("x intersect x = x") {
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, x);
        REQUIRE(result == x);
    }
    
    SECTION("x intersect !x = 1 (empty)") {
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, notX);
        REQUIRE(result == one);
    }
    
    SECTION("!x intersect !x = !x") {
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, notX, notX);
        REQUIRE(result == notX);
    }
    
    SECTION("x intersect y = 1 (empty)") {
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, x, y);
        REQUIRE(result == one);
    }
    
    SECTION("!x intersect !y = 1 (empty)") {
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, notX, notY);
        REQUIRE(result == one);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Stress test with many variables", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    const int NUM_VARS = 20;
    DdNode *vars[NUM_VARS];
    for (int i = 0; i < NUM_VARS; i++) {
        vars[i] = Cudd_bddNewVar(manager);
        Cudd_Ref(vars[i]);
    }
    
    SECTION("Large disjoint cubes") {
        // cube1 = v0 AND v1 AND ... AND v9
        // cube2 = v10 AND v11 AND ... AND v19
        DdNode *cube1 = Cudd_ReadOne(manager);
        Cudd_Ref(cube1);
        for (int i = 0; i < 10; i++) {
            DdNode *temp = Cudd_bddAnd(manager, cube1, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, cube1);
            cube1 = temp;
        }
        
        DdNode *cube2 = Cudd_ReadOne(manager);
        Cudd_Ref(cube2);
        for (int i = 10; i < 20; i++) {
            DdNode *temp = Cudd_bddAnd(manager, cube2, vars[i]);
            Cudd_Ref(temp);
            Cudd_RecursiveDeref(manager, cube2);
            cube2 = temp;
        }
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == Cudd_ReadOne(manager)); // No overlap
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    // Clean up
    for (int i = 0; i < NUM_VARS; i++) {
        Cudd_RecursiveDeref(manager, vars[i]);
    }
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Edge cases for else branches", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Test fc == zero path (line 175-178)") {
        // This tests the branch where cuddT(F) after complement is zero
        // and we need to take the else branch
        DdNode *notX = Cudd_Not(x);
        
        // When we have !x in the cube, the then-branch leads to zero
        // so we follow the else branch
        DdNode *cube1 = Cudd_bddAnd(manager, notX, y);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, notX, z);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == notX);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    SECTION("Test gc == zero path (line 185-188)") {
        // Similar to above but for the g cube
        DdNode *notY = Cudd_Not(y);
        
        DdNode *cube1 = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, notY, z);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == z);
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddLiteralSetIntersection - Coverage for fc/gc complement handling", "[cuddLiteral]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Complemented cube handling in recursive phase detection") {
        // Testing lines 205-222 with different combinations
        // f is not complemented, fc will be cuddT(F)
        // g is complemented, gc will be Cudd_Not(cuddT(G))
        DdNode *notY = Cudd_Not(y);
        
        DdNode *cube1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(cube1);
        DdNode *cube2 = Cudd_bddAnd(manager, x, notY);
        Cudd_Ref(cube2);
        
        DdNode *result = Cudd_bddLiteralSetIntersection(manager, cube1, cube2);
        REQUIRE(result != nullptr);
        Cudd_Ref(result);
        REQUIRE(result == x); // x is common, y conflicts
        
        Cudd_RecursiveDeref(manager, result);
        Cudd_RecursiveDeref(manager, cube1);
        Cudd_RecursiveDeref(manager, cube2);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}
