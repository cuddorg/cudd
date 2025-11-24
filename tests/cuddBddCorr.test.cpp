#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"

/**
 * @brief Test file for cuddBddCorr.c
 * 
 * This file contains comprehensive tests for the cuddBddCorr module
 * to achieve 100% code coverage and ensure correct functionality.
 * 
 * The module computes correlation between BDDs:
 * - Cudd_bddCorrelation: correlation of f and g (fraction of minterms in EXNOR)
 * - Cudd_bddCorrelationWeights: correlation with input probabilities
 */

TEST_CASE("Cudd_bddCorrelation - Terminal cases", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    SECTION("Identical functions have correlation 1.0") {
        // f == g => correlation = 1.0
        double corr = Cudd_bddCorrelation(manager, one, one);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(1.0, 0.0001));
        
        corr = Cudd_bddCorrelation(manager, zero, zero);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(1.0, 0.0001));
    }
    
    SECTION("Complementary functions have correlation 0.0") {
        // f == g' => correlation = 0.0
        double corr = Cudd_bddCorrelation(manager, one, zero);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.0, 0.0001));
        
        corr = Cudd_bddCorrelation(manager, zero, one);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.0, 0.0001));
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelation - Single variable cases", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    
    SECTION("Variable with itself") {
        // x correlated with x = 1.0
        double corr = Cudd_bddCorrelation(manager, x, x);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(1.0, 0.0001));
    }
    
    SECTION("Variable with its complement") {
        // x correlated with !x = 0.0
        double corr = Cudd_bddCorrelation(manager, x, Cudd_Not(x));
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.0, 0.0001));
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelation - Two variable cases", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Independent variables") {
        // x and y are independent, correlation = 0.5
        double corr = Cudd_bddCorrelation(manager, x, y);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.5, 0.0001));
    }
    
    SECTION("AND operation") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Correlation of x AND y with x
        double corr = Cudd_bddCorrelation(manager, f, x);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.75, 0.0001));
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("OR operation") {
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        // Correlation of x OR y with x
        double corr = Cudd_bddCorrelation(manager, f, x);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.75, 0.0001));
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("XOR operation") {
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        // x XOR y with x - correlation should be 0.5
        double corr = Cudd_bddCorrelation(manager, f, x);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.5, 0.0001));
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("XNOR operation") {
        DdNode *f = Cudd_bddXnor(manager, x, y);
        Cudd_Ref(f);
        
        // x XNOR y with x - correlation should be 0.5
        double corr = Cudd_bddCorrelation(manager, f, x);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.5, 0.0001));
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelation - Standardization logic", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("Correlation is symmetric") {
        // Correlation(f, g) = Correlation(g, f)
        double corr1 = Cudd_bddCorrelation(manager, x, y);
        double corr2 = Cudd_bddCorrelation(manager, y, x);
        REQUIRE_THAT(corr1, Catch::Matchers::WithinRel(corr2, 0.0001));
    }
    
    SECTION("Correlation with complements") {
        // Correlation(f', g') = Correlation(f, g)
        double corr1 = Cudd_bddCorrelation(manager, x, y);
        double corr2 = Cudd_bddCorrelation(manager, Cudd_Not(x), Cudd_Not(y));
        REQUIRE_THAT(corr1, Catch::Matchers::WithinRel(corr2, 0.0001));
    }
    
    SECTION("Mixed complement cases") {
        // Test with one complemented, one not
        double corr1 = Cudd_bddCorrelation(manager, Cudd_Not(x), y);
        double corr2 = Cudd_bddCorrelation(manager, x, Cudd_Not(y));
        REQUIRE_THAT(corr1, Catch::Matchers::WithinRel(corr2, 0.0001));
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelation - Hash table caching", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Complex function triggers caching") {
        // Create a more complex function that will trigger recursive calls
        // and hash table usage
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddOr(manager, f1, z);
        Cudd_Ref(f2);
        
        DdNode *g1 = Cudd_bddOr(manager, x, y);
        Cudd_Ref(g1);
        DdNode *g2 = Cudd_bddAnd(manager, g1, z);
        Cudd_Ref(g2);
        
        // Compute correlation - this will exercise hash table lookups
        double corr = Cudd_bddCorrelation(manager, f2, g2);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        // Compute again to test caching at top level
        double corr2 = Cudd_bddCorrelation(manager, f2, g2);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(corr2, 0.0001));
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, g1);
        Cudd_RecursiveDeref(manager, g2);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelation - Different variable orderings", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    SECTION("Functions with different top variables") {
        // f has x as top variable, g has y as top variable
        DdNode *f = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        // This tests the case where topF <= topG and topG <= topF branches
        double corr = Cudd_bddCorrelation(manager, f, g);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    SECTION("One function is deeper") {
        // f = x, g = y AND z (g has deeper structure)
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        double corr = Cudd_bddCorrelation(manager, x, g);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, g);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Terminal cases", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *one = Cudd_ReadOne(manager);
    DdNode *zero = Cudd_Not(one);
    
    double prob[10] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    
    SECTION("Identical functions have correlation 1.0") {
        double corr = Cudd_bddCorrelationWeights(manager, one, one, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(1.0, 0.0001));
        
        corr = Cudd_bddCorrelationWeights(manager, zero, zero, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(1.0, 0.0001));
    }
    
    SECTION("Complementary functions have correlation 0.0") {
        double corr = Cudd_bddCorrelationWeights(manager, one, zero, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.0, 0.0001));
        
        corr = Cudd_bddCorrelationWeights(manager, zero, one, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.0, 0.0001));
    }
    
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Weighted correlation with equal probabilities", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    double prob[10] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    
    SECTION("With equal probabilities, weighted correlation matches unweighted") {
        double corr_weighted = Cudd_bddCorrelationWeights(manager, x, y, prob);
        double corr_unweighted = Cudd_bddCorrelation(manager, x, y);
        REQUIRE_THAT(corr_weighted, Catch::Matchers::WithinRel(corr_unweighted, 0.0001));
    }
    
    SECTION("Variable with itself") {
        double corr = Cudd_bddCorrelationWeights(manager, x, x, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(1.0, 0.0001));
    }
    
    SECTION("Variable with its complement") {
        double corr = Cudd_bddCorrelationWeights(manager, x, Cudd_Not(x), prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.0, 0.0001));
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Biased probabilities", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    SECTION("High probability for first variable") {
        double prob[10] = {0.9, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        double corr = Cudd_bddCorrelationWeights(manager, f, x, prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Low probability for first variable") {
        double prob[10] = {0.1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        double corr = Cudd_bddCorrelationWeights(manager, f, x, prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Different probabilities for both variables") {
        double prob[10] = {0.7, 0.3, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, x, y);
        Cudd_Ref(g);
        
        double corr = Cudd_bddCorrelationWeights(manager, f, g, prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Correlation with constant gives probability", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *one = Cudd_ReadOne(manager);
    Cudd_Ref(x);
    
    SECTION("Correlation with constant one gives probability of function") {
        double prob[10] = {0.7, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        
        // Correlation of x with constant 1 should give prob[0] = 0.7
        double corr = Cudd_bddCorrelationWeights(manager, x, one, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.7, 0.0001));
    }
    
    SECTION("Different probability") {
        double prob[10] = {0.3, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        
        // Correlation of x with constant 1 should give prob[0] = 0.3
        double corr = Cudd_bddCorrelationWeights(manager, x, one, prob);
        REQUIRE_THAT(corr, Catch::Matchers::WithinRel(0.3, 0.0001));
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Standardization logic", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    double prob[10] = {0.6, 0.4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    
    SECTION("Correlation is symmetric") {
        double corr1 = Cudd_bddCorrelationWeights(manager, x, y, prob);
        double corr2 = Cudd_bddCorrelationWeights(manager, y, x, prob);
        REQUIRE_THAT(corr1, Catch::Matchers::WithinRel(corr2, 0.0001));
    }
    
    SECTION("Correlation with complements") {
        double corr1 = Cudd_bddCorrelationWeights(manager, x, y, prob);
        double corr2 = Cudd_bddCorrelationWeights(manager, Cudd_Not(x), Cudd_Not(y), prob);
        REQUIRE_THAT(corr1, Catch::Matchers::WithinRel(corr2, 0.0001));
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Complex functions with caching", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    DdNode *z = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    Cudd_Ref(z);
    
    double prob[10] = {0.6, 0.5, 0.4, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    
    SECTION("Complex nested function") {
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f1);
        DdNode *f2 = Cudd_bddOr(manager, f1, z);
        Cudd_Ref(f2);
        
        DdNode *g1 = Cudd_bddOr(manager, x, y);
        Cudd_Ref(g1);
        DdNode *g2 = Cudd_bddAnd(manager, g1, z);
        Cudd_Ref(g2);
        
        double corr = Cudd_bddCorrelationWeights(manager, f2, g2, prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, g1);
        Cudd_RecursiveDeref(manager, g2);
    }
    
    SECTION("Functions with different top variables") {
        DdNode *f = Cudd_bddAnd(manager, x, z);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, y, z);
        Cudd_Ref(g);
        
        double corr = Cudd_bddCorrelationWeights(manager, f, g, prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_RecursiveDeref(manager, z);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_bddCorrelationWeights - Complement handling in recursive calls", "[cuddBddCorr]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    DdNode *x = Cudd_bddNewVar(manager);
    DdNode *y = Cudd_bddNewVar(manager);
    Cudd_Ref(x);
    Cudd_Ref(y);
    
    double prob[10] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    
    SECTION("Complemented second function") {
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        // Test with g being complemented - tests the "g != G" branch
        double corr = Cudd_bddCorrelationWeights(manager, x, Cudd_Not(y), prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
    }
    
    SECTION("Both functions involve complements") {
        DdNode *f = Cudd_bddOr(manager, x, Cudd_Not(y));
        Cudd_Ref(f);
        DdNode *g = Cudd_bddAnd(manager, Cudd_Not(x), y);
        Cudd_Ref(g);
        
        double corr = Cudd_bddCorrelationWeights(manager, f, g, prob);
        REQUIRE(corr >= 0.0);
        REQUIRE(corr <= 1.0);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, g);
    }
    
    Cudd_RecursiveDeref(manager, x);
    Cudd_RecursiveDeref(manager, y);
    Cudd_Quit(manager);
}
