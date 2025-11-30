#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

/**
 * @brief Comprehensive test file for cuddApa.c targeting 90% coverage
 * 
 * This file contains tests for all Arbitrary Precision Arithmetic (APA)
 * functions in the CUDD library.
 */

// ============================================================================
// Cudd_ApaNumberOfDigits Tests
// ============================================================================

TEST_CASE("Cudd_ApaNumberOfDigits - Basic digit calculation", "[cuddApa]") {
    SECTION("Exact multiple of DD_APA_BITS (32)") {
        // 32 binary digits = 1 digit (32 bits per digit)
        int digits = Cudd_ApaNumberOfDigits(32);
        REQUIRE(digits == 1);
        
        // 64 binary digits = 2 digits
        digits = Cudd_ApaNumberOfDigits(64);
        REQUIRE(digits == 2);
    }
    
    SECTION("Non-exact multiple") {
        // 33 binary digits should round up to 2 digits
        int digits = Cudd_ApaNumberOfDigits(33);
        REQUIRE(digits == 2);
        
        // 1 binary digit should give 1 digit
        digits = Cudd_ApaNumberOfDigits(1);
        REQUIRE(digits == 1);
        
        // 65 binary digits should give 3 digits
        digits = Cudd_ApaNumberOfDigits(65);
        REQUIRE(digits == 3);
    }
    
    SECTION("Large values") {
        // 100 binary digits
        int digits = Cudd_ApaNumberOfDigits(100);
        REQUIRE(digits == 4);
        
        // 128 binary digits
        digits = Cudd_ApaNumberOfDigits(128);
        REQUIRE(digits == 4);
    }
}

// ============================================================================
// Cudd_NewApaNumber and Cudd_FreeApaNumber Tests
// ============================================================================

TEST_CASE("Cudd_NewApaNumber and Cudd_FreeApaNumber - Allocation", "[cuddApa]") {
    SECTION("Allocate single digit") {
        DdApaNumber num = Cudd_NewApaNumber(1);
        REQUIRE(num != nullptr);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Allocate multiple digits") {
        DdApaNumber num = Cudd_NewApaNumber(5);
        REQUIRE(num != nullptr);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Allocate larger number") {
        DdApaNumber num = Cudd_NewApaNumber(100);
        REQUIRE(num != nullptr);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaCopy Tests
// ============================================================================

TEST_CASE("Cudd_ApaCopy - Copy operations", "[cuddApa]") {
    SECTION("Copy single digit") {
        DdApaNumber src = Cudd_NewApaNumber(1);
        DdApaNumber dest = Cudd_NewApaNumber(1);
        src[0] = 12345;
        
        Cudd_ApaCopy(1, src, dest);
        REQUIRE(dest[0] == 12345);
        
        Cudd_FreeApaNumber(src);
        Cudd_FreeApaNumber(dest);
    }
    
    SECTION("Copy multiple digits") {
        int digits = 3;
        DdApaNumber src = Cudd_NewApaNumber(digits);
        DdApaNumber dest = Cudd_NewApaNumber(digits);
        
        src[0] = 111;
        src[1] = 222;
        src[2] = 333;
        
        Cudd_ApaCopy(digits, src, dest);
        
        REQUIRE(dest[0] == 111);
        REQUIRE(dest[1] == 222);
        REQUIRE(dest[2] == 333);
        
        Cudd_FreeApaNumber(src);
        Cudd_FreeApaNumber(dest);
    }
}

// ============================================================================
// Cudd_ApaAdd Tests
// ============================================================================

TEST_CASE("Cudd_ApaAdd - Addition operations", "[cuddApa]") {
    SECTION("Simple addition without carry") {
        int digits = 1;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber sum = Cudd_NewApaNumber(digits);
        
        a[0] = 100;
        b[0] = 200;
        
        DdApaDigit carry = Cudd_ApaAdd(digits, a, b, sum);
        REQUIRE(sum[0] == 300);
        REQUIRE(carry == 0);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(sum);
    }
    
    SECTION("Addition with carry") {
        int digits = 1;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber sum = Cudd_NewApaNumber(digits);
        
        // Use maximum values to cause overflow/carry
        a[0] = 0xFFFFFFFF;
        b[0] = 1;
        
        DdApaDigit carry = Cudd_ApaAdd(digits, a, b, sum);
        REQUIRE(sum[0] == 0);
        REQUIRE(carry == 1);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(sum);
    }
    
    SECTION("Multi-digit addition") {
        int digits = 2;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber sum = Cudd_NewApaNumber(digits);
        
        a[0] = 1;
        a[1] = 0xFFFFFFFF;
        b[0] = 0;
        b[1] = 1;
        
        DdApaDigit carry = Cudd_ApaAdd(digits, a, b, sum);
        REQUIRE(sum[0] == 2);
        REQUIRE(sum[1] == 0);
        REQUIRE(carry == 0);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(sum);
    }
}

// ============================================================================
// Cudd_ApaSubtract Tests
// ============================================================================

TEST_CASE("Cudd_ApaSubtract - Subtraction operations", "[cuddApa]") {
    SECTION("Simple subtraction without borrow") {
        int digits = 1;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber diff = Cudd_NewApaNumber(digits);
        
        a[0] = 300;
        b[0] = 100;
        
        DdApaDigit borrow = Cudd_ApaSubtract(digits, a, b, diff);
        REQUIRE(diff[0] == 200);
        REQUIRE(borrow == 0);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(diff);
    }
    
    SECTION("Subtraction with borrow") {
        int digits = 1;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber diff = Cudd_NewApaNumber(digits);
        
        a[0] = 100;
        b[0] = 200;
        
        DdApaDigit borrow = Cudd_ApaSubtract(digits, a, b, diff);
        // When a < b, we get a borrow. The function returns 
        // DD_MSDIGIT(partial) - 1, where partial starts at DD_APA_BASE.
        // Result: 0xFFFFFFFF (which represents -1 or a borrow situation)
        REQUIRE(borrow == 0xFFFFFFFF);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(diff);
    }
    
    SECTION("Multi-digit subtraction") {
        int digits = 2;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber diff = Cudd_NewApaNumber(digits);
        
        a[0] = 2;
        a[1] = 0;
        b[0] = 0;
        b[1] = 1;
        
        DdApaDigit borrow = Cudd_ApaSubtract(digits, a, b, diff);
        REQUIRE(diff[0] == 1);
        REQUIRE(diff[1] == 0xFFFFFFFF);
        REQUIRE(borrow == 0);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(diff);
    }
}

// ============================================================================
// Cudd_ApaShortDivision Tests
// ============================================================================

TEST_CASE("Cudd_ApaShortDivision - Division by digit", "[cuddApa]") {
    SECTION("Simple division") {
        int digits = 1;
        DdApaNumber dividend = Cudd_NewApaNumber(digits);
        DdApaNumber quotient = Cudd_NewApaNumber(digits);
        
        dividend[0] = 100;
        DdApaDigit remainder = Cudd_ApaShortDivision(digits, dividend, 7, quotient);
        
        REQUIRE(quotient[0] == 14);
        REQUIRE(remainder == 2);
        
        Cudd_FreeApaNumber(dividend);
        Cudd_FreeApaNumber(quotient);
    }
    
    SECTION("Division by 10") {
        int digits = 1;
        DdApaNumber dividend = Cudd_NewApaNumber(digits);
        DdApaNumber quotient = Cudd_NewApaNumber(digits);
        
        dividend[0] = 12345;
        DdApaDigit remainder = Cudd_ApaShortDivision(digits, dividend, 10, quotient);
        
        REQUIRE(quotient[0] == 1234);
        REQUIRE(remainder == 5);
        
        Cudd_FreeApaNumber(dividend);
        Cudd_FreeApaNumber(quotient);
    }
    
    SECTION("Multi-digit division") {
        int digits = 2;
        DdApaNumber dividend = Cudd_NewApaNumber(digits);
        DdApaNumber quotient = Cudd_NewApaNumber(digits);
        
        dividend[0] = 0;
        dividend[1] = 100;
        DdApaDigit remainder = Cudd_ApaShortDivision(digits, dividend, 10, quotient);
        
        REQUIRE(quotient[1] == 10);
        REQUIRE(remainder == 0);
        
        Cudd_FreeApaNumber(dividend);
        Cudd_FreeApaNumber(quotient);
    }
}

// ============================================================================
// Cudd_ApaIntDivision Tests
// ============================================================================

TEST_CASE("Cudd_ApaIntDivision - Division by unsigned int", "[cuddApa]") {
    SECTION("Simple division") {
        int digits = 1;
        DdApaNumber dividend = Cudd_NewApaNumber(digits);
        DdApaNumber quotient = Cudd_NewApaNumber(digits);
        
        dividend[0] = 100;
        unsigned int remainder = Cudd_ApaIntDivision(digits, dividend, 7, quotient);
        
        REQUIRE(quotient[0] == 14);
        REQUIRE(remainder == 2);
        
        Cudd_FreeApaNumber(dividend);
        Cudd_FreeApaNumber(quotient);
    }
    
    SECTION("Larger divisor") {
        int digits = 1;
        DdApaNumber dividend = Cudd_NewApaNumber(digits);
        DdApaNumber quotient = Cudd_NewApaNumber(digits);
        
        dividend[0] = 1000000;
        unsigned int remainder = Cudd_ApaIntDivision(digits, dividend, 1000, quotient);
        
        REQUIRE(quotient[0] == 1000);
        REQUIRE(remainder == 0);
        
        Cudd_FreeApaNumber(dividend);
        Cudd_FreeApaNumber(quotient);
    }
    
    SECTION("Multi-digit division by int") {
        int digits = 2;
        DdApaNumber dividend = Cudd_NewApaNumber(digits);
        DdApaNumber quotient = Cudd_NewApaNumber(digits);
        
        dividend[0] = 1;
        dividend[1] = 0;
        unsigned int remainder = Cudd_ApaIntDivision(digits, dividend, 100, quotient);
        
        // Result should be 2^32 / 100 with remainder
        REQUIRE(quotient[0] == 0);
        
        Cudd_FreeApaNumber(dividend);
        Cudd_FreeApaNumber(quotient);
    }
}

// ============================================================================
// Cudd_ApaShiftRight Tests
// ============================================================================

TEST_CASE("Cudd_ApaShiftRight - Right shift operations", "[cuddApa]") {
    SECTION("Simple shift with in=0") {
        int digits = 1;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        
        a[0] = 8;  // Binary: 1000
        Cudd_ApaShiftRight(digits, 0, a, b);
        
        REQUIRE(b[0] == 4);  // Binary: 0100
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
    }
    
    SECTION("Shift with in=1") {
        int digits = 1;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        
        a[0] = 8;
        Cudd_ApaShiftRight(digits, 1, a, b);
        
        // MSB should be 1 now
        REQUIRE(b[0] == (4 | 0x80000000));
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
    }
    
    SECTION("Multi-digit shift") {
        int digits = 2;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        
        a[0] = 1;  // High digit
        a[1] = 0;  // Low digit
        
        Cudd_ApaShiftRight(digits, 0, a, b);
        
        // LSB of high digit should propagate to MSB of low digit
        REQUIRE(b[1] == 0x80000000);
        REQUIRE(b[0] == 0);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
    }
}

// ============================================================================
// Cudd_ApaSetToLiteral Tests
// ============================================================================

TEST_CASE("Cudd_ApaSetToLiteral - Set to literal value", "[cuddApa]") {
    SECTION("Single digit") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        Cudd_ApaSetToLiteral(digits, num, 42);
        REQUIRE(num[0] == 42);
        
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Multiple digits - clears high order") {
        int digits = 3;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        // Set all to non-zero first
        num[0] = 111;
        num[1] = 222;
        num[2] = 333;
        
        Cudd_ApaSetToLiteral(digits, num, 999);
        
        REQUIRE(num[0] == 0);
        REQUIRE(num[1] == 0);
        REQUIRE(num[2] == 999);
        
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Set to zero") {
        int digits = 2;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        Cudd_ApaSetToLiteral(digits, num, 0);
        
        REQUIRE(num[0] == 0);
        REQUIRE(num[1] == 0);
        
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaPowerOfTwo Tests
// ============================================================================

TEST_CASE("Cudd_ApaPowerOfTwo - Power of two", "[cuddApa]") {
    SECTION("Power 0 (2^0 = 1)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        Cudd_ApaPowerOfTwo(digits, num, 0);
        REQUIRE(num[0] == 1);
        
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Power 1 (2^1 = 2)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        Cudd_ApaPowerOfTwo(digits, num, 1);
        REQUIRE(num[0] == 2);
        
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Power 31 (2^31)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        Cudd_ApaPowerOfTwo(digits, num, 31);
        REQUIRE(num[0] == 0x80000000);
        
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Power 32 (2^32) - needs 2 digits") {
        int digits = 2;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        
        Cudd_ApaPowerOfTwo(digits, num, 32);
        REQUIRE(num[0] == 1);
        REQUIRE(num[1] == 0);
        
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Power too large for digits - sets to 0") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0xFFFFFFFF;  // Pre-set to non-zero
        
        Cudd_ApaPowerOfTwo(digits, num, 100);  // Too large
        REQUIRE(num[0] == 0);
        
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaCompare Tests
// ============================================================================

TEST_CASE("Cudd_ApaCompare - Comparison operations", "[cuddApa]") {
    SECTION("Equal numbers") {
        int digits = 2;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 100;
        first[1] = 200;
        second[0] = 100;
        second[1] = 200;
        
        int result = Cudd_ApaCompare(digits, first, digits, second);
        REQUIRE(result == 0);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("First larger") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 200;
        second[0] = 100;
        
        int result = Cudd_ApaCompare(digits, first, digits, second);
        REQUIRE(result == 1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Second larger") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 100;
        second[0] = 200;
        
        int result = Cudd_ApaCompare(digits, first, digits, second);
        REQUIRE(result == -1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Different digit counts - first has more significant digits") {
        int digitsFirst = 2;
        int digitsSecond = 1;
        DdApaNumber first = Cudd_NewApaNumber(digitsFirst);
        DdApaNumber second = Cudd_NewApaNumber(digitsSecond);
        
        first[0] = 1;
        first[1] = 0;
        second[0] = 0xFFFFFFFF;
        
        int result = Cudd_ApaCompare(digitsFirst, first, digitsSecond, second);
        REQUIRE(result == 1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Leading zeros - effectively equal") {
        int digitsFirst = 3;
        int digitsSecond = 2;
        DdApaNumber first = Cudd_NewApaNumber(digitsFirst);
        DdApaNumber second = Cudd_NewApaNumber(digitsSecond);
        
        first[0] = 0;
        first[1] = 1;
        first[2] = 2;
        second[0] = 1;
        second[1] = 2;
        
        int result = Cudd_ApaCompare(digitsFirst, first, digitsSecond, second);
        REQUIRE(result == 0);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Both zero") {
        int digits = 2;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 0;
        first[1] = 0;
        second[0] = 0;
        second[1] = 0;
        
        int result = Cudd_ApaCompare(digits, first, digits, second);
        REQUIRE(result == 0);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
}

// ============================================================================
// Cudd_ApaCompareRatios Tests
// ============================================================================

TEST_CASE("Cudd_ApaCompareRatios - Compare ratios", "[cuddApa]") {
    SECTION("Equal ratios") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 100;
        second[0] = 200;
        
        // 100/2 vs 200/4 both equal 50
        int result = Cudd_ApaCompareRatios(digits, first, 2, digits, second, 4);
        REQUIRE(result == 0);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("First ratio larger") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 100;
        second[0] = 100;
        
        // 100/2 = 50 vs 100/3 = 33.33
        int result = Cudd_ApaCompareRatios(digits, first, 2, digits, second, 3);
        REQUIRE(result == 1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Second ratio larger") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 100;
        second[0] = 100;
        
        // 100/3 = 33.33 vs 100/2 = 50
        int result = Cudd_ApaCompareRatios(digits, first, 3, digits, second, 2);
        REQUIRE(result == -1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Ratio comparison with remainders making difference") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        // 7/3 = 2.333... vs 11/5 = 2.2
        first[0] = 7;
        second[0] = 11;
        
        int result = Cudd_ApaCompareRatios(digits, first, 3, digits, second, 5);
        REQUIRE(result == 1);  // 2.333 > 2.2
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
}

// ============================================================================
// Cudd_ApaPrintHex Tests
// ============================================================================

TEST_CASE("Cudd_ApaPrintHex - Hexadecimal printing", "[cuddApa]") {
    SECTION("Print single digit") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0x12345678;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintHex(fp, digits, num);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "12345678") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print multiple digits") {
        int digits = 2;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0xABCD;
        num[1] = 0xEF01;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintHex(fp, digits, num);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "0000abcd0000ef01") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print zero") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintHex(fp, digits, num);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "00000000") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaPrintDecimal Tests
// ============================================================================

TEST_CASE("Cudd_ApaPrintDecimal - Decimal printing", "[cuddApa]") {
    SECTION("Print single digit value") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 12345;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintDecimal(fp, digits, num);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "12345") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print zero") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintDecimal(fp, digits, num);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "0") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print larger value") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 1000000000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintDecimal(fp, digits, num);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "1000000000") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaStringDecimal Tests
// ============================================================================

TEST_CASE("Cudd_ApaStringDecimal - Decimal string conversion", "[cuddApa]") {
    SECTION("Convert simple number") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 12345;
        
        char *str = Cudd_ApaStringDecimal(digits, num);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "12345") == 0);
        
        free(str);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Convert zero") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0;
        
        char *str = Cudd_ApaStringDecimal(digits, num);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "0") == 0);
        
        free(str);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Convert max 32-bit value") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0xFFFFFFFF;
        
        char *str = Cudd_ApaStringDecimal(digits, num);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "4294967295") == 0);
        
        free(str);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaPrintExponential Tests
// ============================================================================

TEST_CASE("Cudd_ApaPrintExponential - Exponential printing", "[cuddApa]") {
    SECTION("Print as integer (precision >= digits needed)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 12345;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 10);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "12345") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print with exponent (precision < digits needed)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 123456789;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 3);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        // Should be something like "1.23e+08"
        REQUIRE(strstr(buffer, "e+") != nullptr);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print with rounding up") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 99999999;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strstr(buffer, "e+") != nullptr);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Print zero") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 0;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 5);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Rounding with decimal[last] = 5 (round to even)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 12500;  // Should round to 12 or 13 depending on even rule
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Rounding with decimal[last] > 5") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 12700;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Rounding with decimal[last] < 5") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 12400;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Cudd_ApaCountMinterm Tests
// ============================================================================

TEST_CASE("Cudd_ApaCountMinterm - Count minterms using APA", "[cuddApa]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Count minterms of constant one") {
        DdNode *one = Cudd_ReadOne(dd);
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, one, 3, &digits);
        REQUIRE(count != nullptr);
        
        // For 3 variables, constant one has 2^3 = 8 minterms
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "8") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
    }
    
    SECTION("Count minterms of constant zero") {
        DdNode *zero = Cudd_ReadLogicZero(dd);
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, zero, 3, &digits);
        REQUIRE(count != nullptr);
        
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "0") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
    }
    
    SECTION("Count minterms of single variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, var, 3, &digits);
        REQUIRE(count != nullptr);
        
        // For 3 variables, single variable has 2^2 = 4 minterms
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "4") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, var);
    }
    
    SECTION("Count minterms of complemented variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        DdNode *notVar = Cudd_Not(var);
        Cudd_Ref(notVar);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, notVar, 3, &digits);
        REQUIRE(count != nullptr);
        
        // Complemented single variable also has 2^2 = 4 minterms
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "4") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, notVar);
    }
    
    SECTION("Count minterms of AND of two variables") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *andNode = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andNode);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, andNode, 3, &digits);
        REQUIRE(count != nullptr);
        
        // AND of two variables with 3 total: 2^1 = 2 minterms
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "2") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, andNode);
    }
    
    SECTION("Count minterms with many variables") {
        // Create a more complex BDD
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *temp = Cudd_bddOr(dd, var0, var1);
        Cudd_Ref(temp);
        DdNode *orNode = Cudd_bddOr(dd, temp, var2);
        Cudd_Ref(orNode);
        Cudd_RecursiveDeref(dd, temp);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, orNode, 3, &digits);
        REQUIRE(count != nullptr);
        
        // OR of 3 variables: 2^3 - 1 = 7 minterms (all except 000)
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "7") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, orNode);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_ApaPrintMinterm Tests
// ============================================================================

TEST_CASE("Cudd_ApaPrintMinterm - Print minterm count", "[cuddApa]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Print minterm count of constant one") {
        DdNode *one = Cudd_ReadOne(dd);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintMinterm(fp, dd, one, 4);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        // Should be "16\n" for 4 variables
        REQUIRE(strstr(buffer, "16") != nullptr);
        
        fclose(fp);
    }
    
    SECTION("Print minterm count of single variable") {
        DdNode *var = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintMinterm(fp, dd, var, 4);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(dd, var);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_ApaPrintMintermExp Tests
// ============================================================================

TEST_CASE("Cudd_ApaPrintMintermExp - Print minterm count exponential", "[cuddApa]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Print exponential minterm count") {
        DdNode *one = Cudd_ReadOne(dd);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintMintermExp(fp, dd, one, 10, 5);
        REQUIRE(result == 1);
        
        fclose(fp);
    }
    
    SECTION("Print exponential minterm count of complex BDD") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *andNode = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(andNode);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintMintermExp(fp, dd, andNode, 5, 3);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(dd, andNode);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Cudd_ApaPrintDensity Tests
// ============================================================================

TEST_CASE("Cudd_ApaPrintDensity - Print density", "[cuddApa]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Print density of constant one") {
        DdNode *one = Cudd_ReadOne(dd);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintDensity(fp, dd, one, 4);
        REQUIRE(result == 1);
        
        fclose(fp);
    }
    
    SECTION("Print density of complex BDD") {
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        
        DdNode *temp1 = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(temp1);
        DdNode *orNode = Cudd_bddOr(dd, temp1, var2);
        Cudd_Ref(orNode);
        Cudd_RecursiveDeref(dd, temp1);
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintDensity(fp, dd, orNode, 5);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_RecursiveDeref(dd, orNode);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Additional edge case tests for better coverage
// ============================================================================

TEST_CASE("cuddApa - Edge cases for additional coverage", "[cuddApa]") {
    SECTION("ApaCompare with different digit counts - second larger") {
        int digitsFirst = 1;
        int digitsSecond = 2;
        DdApaNumber first = Cudd_NewApaNumber(digitsFirst);
        DdApaNumber second = Cudd_NewApaNumber(digitsSecond);
        
        first[0] = 0xFFFFFFFF;
        second[0] = 1;
        second[1] = 0;
        
        int result = Cudd_ApaCompare(digitsFirst, first, digitsSecond, second);
        REQUIRE(result == -1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("ApaCompare same significant digits - second larger") {
        int digits = 2;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        first[0] = 1;
        first[1] = 100;
        second[0] = 1;
        second[1] = 200;
        
        int result = Cudd_ApaCompare(digits, first, digits, second);
        REQUIRE(result == -1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("ApaPrintExponential - trailing zeros removal") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 100000000;  // Has trailing zeros when divided
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 3);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("ApaPrintExponential - decimal[last] == 5 with odd previous digit") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 15000 -> in precision 2: 1.5 -> rounds to 1.6 (odd previous)
        num[0] = 15000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("ApaPrintExponential - decimal[last] == 5 with even previous digit") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 25000 -> in precision 2: 2.5 -> rounds to 2.6 (even previous, but there might be trailing digits)
        num[0] = 25000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Tests for internal function coverage (cuddApaCountMintermAux paths)
// ============================================================================

TEST_CASE("cuddApa - Internal function coverage", "[cuddApa]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Count minterms with complemented else branch") {
        // Create a BDD with complemented edges
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        
        // XOR creates complemented edges
        DdNode *xorNode = Cudd_bddXor(dd, var0, var1);
        Cudd_Ref(xorNode);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, xorNode, 4, &digits);
        REQUIRE(count != nullptr);
        
        // XOR of 2 vars with 4 total vars: half of 16 = 8 minterms
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "8") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, xorNode);
    }
    
    SECTION("Count minterms with shared nodes (caching)") {
        // Create BDDs that share nodes to test caching
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        
        // (v0 AND v1) OR (v0 AND v2) - shares v0
        DdNode *and1 = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(and1);
        DdNode *and2 = Cudd_bddAnd(dd, var0, var2);
        Cudd_Ref(and2);
        DdNode *orNode = Cudd_bddOr(dd, and1, and2);
        Cudd_Ref(orNode);
        
        Cudd_RecursiveDeref(dd, and1);
        Cudd_RecursiveDeref(dd, and2);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, orNode, 4, &digits);
        REQUIRE(count != nullptr);
        
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, orNode);
    }
    
    SECTION("Count minterms of background/zero node") {
        DdNode *background = Cudd_ReadBackground(dd);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, background, 5, &digits);
        REQUIRE(count != nullptr);
        
        // Background node (zero ADD) should have 0 minterms
        char *str = Cudd_ApaStringDecimal(digits, count);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "0") == 0);
        
        free(str);
        Cudd_FreeApaNumber(count);
    }
    
    SECTION("Count minterms with large variable count") {
        // Test with many variables to exercise more code paths
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        Cudd_Ref(var0);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, var0, 50, &digits);
        REQUIRE(count != nullptr);
        
        // Single variable in 50 variables: 2^49 minterms
        REQUIRE(digits > 1);  // Should need multiple digits
        
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, var0);
    }
    
    Cudd_Quit(dd);
}

// ============================================================================
// Test rounding edge cases in ApaPrintExponential
// ============================================================================

TEST_CASE("cuddApa - ApaPrintExponential rounding edge cases", "[cuddApa]") {
    SECTION("Rounding causes carry propagation") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 99999 with precision 2 might cause carry
        num[0] = 99999;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Last digit 5 with non-zero trailing digits") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 15001 - the 5 followed by non-zero
        num[0] = 15001;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Last == decimalDigits (no truncation needed)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // Small number where all digits fit
        num[0] = 123;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        // Large precision - should print as integer
        int result = Cudd_ApaPrintExponential(fp, digits, num, 20);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "123") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}

// ============================================================================
// Additional tests for 90% coverage
// ============================================================================

TEST_CASE("cuddApa - ApaCompareRatios remainder path", "[cuddApa]") {
    SECTION("Same integer part but second remainder larger") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        // 10/3 = 3.33... vs 10/4 = 2.5
        // After integer division: 3 vs 2, so result is 1
        // But we need same integer part...
        // 8/3 = 2.66... vs 9/4 = 2.25 -> first > second
        first[0] = 8;
        second[0] = 9;
        
        int result = Cudd_ApaCompareRatios(digits, first, 3, digits, second, 4);
        REQUIRE(result == 1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
    
    SECTION("Same integer part, first remainder smaller - tests return(-1)") {
        int digits = 1;
        DdApaNumber first = Cudd_NewApaNumber(digits);
        DdApaNumber second = Cudd_NewApaNumber(digits);
        
        // We need numbers where integer quotients are equal but remainders differ
        // 9/4 = 2.25 vs 8/3 = 2.66... -> second is larger
        // Let's verify: 9/4 = 2, remainder 1, so 1/4 = 0.25
        // 8/3 = 2, remainder 2, so 2/3 = 0.666...
        first[0] = 9;
        second[0] = 8;
        
        int result = Cudd_ApaCompareRatios(digits, first, 4, digits, second, 3);
        REQUIRE(result == -1);
        
        Cudd_FreeApaNumber(first);
        Cudd_FreeApaNumber(second);
    }
}

TEST_CASE("cuddApa - ApaPrintExponential detailed coverage", "[cuddApa]") {
    SECTION("last == decimalDigits case") {
        // Need a small number with precision >= decimalDigits
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        num[0] = 5;  // Small number
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        // Large precision ensures last == decimalDigits (no truncation)
        int result = Cudd_ApaPrintExponential(fp, digits, num, 100);
        REQUIRE(result == 1);
        
        rewind(fp);
        char buffer[100];
        fgets(buffer, sizeof(buffer), fp);
        REQUIRE(strcmp(buffer, "5") == 0);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("decimal[last] == 5 with nonZero trailing (carry=1)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // For precision 2, we need decimal[2]=5 with non-zero after
        // 12500001 -> digits: 1,2,5,0,0,0,0,1
        // first=0, precision=2, last=2, decimal[2]=5
        // Check i=3..7 for nonzero: decimal[7]=1 is nonzero -> carry=1
        num[0] = 12500001;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("decimal[last-1] odd with decimal[last]=5 and no trailing nonzero") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // For precision 2, need decimal[2]=5, decimal[1] is odd, no nonzero after 
        // 13500000 -> 1,3,5,0,0,0,0,0 -> decimal[2]=5, decimal[1]=3 (odd)
        // Check i=3..N for nonzero: all zeros -> nonZero=FALSE
        // Then check decimal[last-1]&1 = decimal[1]&1 = 3&1 = 1 -> odd -> carry=1
        num[0] = 13500000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}

TEST_CASE("cuddApa - cuddApaCountMintermAux paths", "[cuddApa]") {
    DdManager *dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(dd != nullptr);
    
    SECTION("Test ref==1 path in cuddApaCountMintermAux") {
        // Create a BDD where nodes have ref count 1
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        DdNode *var2 = Cudd_bddIthVar(dd, 2);
        
        // Create complex BDD with intermediate nodes
        DdNode *temp1 = Cudd_bddAnd(dd, var0, var1);
        Cudd_Ref(temp1);
        DdNode *temp2 = Cudd_bddAnd(dd, temp1, var2);
        Cudd_Ref(temp2);
        Cudd_RecursiveDeref(dd, temp1);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, temp2, 5, &digits);
        REQUIRE(count != nullptr);
        
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, temp2);
    }
    
    SECTION("Test single reference node minterm count") {
        // Create a brand new BDD with no shared references
        DdNode *var0 = Cudd_bddIthVar(dd, 0);
        DdNode *var1 = Cudd_bddIthVar(dd, 1);
        
        // The ITE creates a unique structure
        DdNode *ite = Cudd_bddIte(dd, var0, var1, Cudd_Not(var1));
        Cudd_Ref(ite);
        
        int digits;
        DdApaNumber count = Cudd_ApaCountMinterm(dd, ite, 4, &digits);
        REQUIRE(count != nullptr);
        
        Cudd_FreeApaNumber(count);
        Cudd_RecursiveDeref(dd, ite);
    }
    
    Cudd_Quit(dd);
}

TEST_CASE("cuddApa - Multi-digit arithmetic", "[cuddApa]") {
    SECTION("Multi-digit addition with propagating carry") {
        int digits = 3;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber sum = Cudd_NewApaNumber(digits);
        
        // Set all digits to max to cause cascading carry
        a[0] = 0xFFFFFFFF;
        a[1] = 0xFFFFFFFF;
        a[2] = 0xFFFFFFFF;
        b[0] = 0;
        b[1] = 0;
        b[2] = 1;
        
        DdApaDigit carry = Cudd_ApaAdd(digits, a, b, sum);
        // 0xFFFFFFFF...FFF + 1 = 0x100...000
        REQUIRE(sum[0] == 0);
        REQUIRE(sum[1] == 0);
        REQUIRE(sum[2] == 0);
        REQUIRE(carry == 1);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(sum);
    }
    
    SECTION("Multi-digit subtraction with cascading borrow") {
        int digits = 3;
        DdApaNumber a = Cudd_NewApaNumber(digits);
        DdApaNumber b = Cudd_NewApaNumber(digits);
        DdApaNumber diff = Cudd_NewApaNumber(digits);
        
        // 0x1 00000000 00000000 - 1 = 0x0 FFFFFFFF FFFFFFFF
        a[0] = 1;
        a[1] = 0;
        a[2] = 0;
        b[0] = 0;
        b[1] = 0;
        b[2] = 1;
        
        DdApaDigit borrow = Cudd_ApaSubtract(digits, a, b, diff);
        REQUIRE(diff[0] == 0);
        REQUIRE(diff[1] == 0xFFFFFFFF);
        REQUIRE(diff[2] == 0xFFFFFFFF);
        REQUIRE(borrow == 0);
        
        Cudd_FreeApaNumber(a);
        Cudd_FreeApaNumber(b);
        Cudd_FreeApaNumber(diff);
    }
}

TEST_CASE("cuddApa - ApaPrintExponential comprehensive", "[cuddApa]") {
    SECTION("Trailing zeros removal") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // Number with trailing zeros in decimal
        num[0] = 1230000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 4);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("decimal[last] < 5 (carry=0)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 124 with precision 2 -> 12, decimal[2]=4 < 5, carry=0
        num[0] = 124000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("decimal[last] > 5 (carry=1)") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 126 with precision 2 -> 13 (rounded up)
        num[0] = 126000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
    
    SECTION("Carry propagation to first digit") {
        int digits = 1;
        DdApaNumber num = Cudd_NewApaNumber(digits);
        // 996 with precision 2 -> should round to 100
        num[0] = 996000;
        
        FILE *fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        int result = Cudd_ApaPrintExponential(fp, digits, num, 2);
        REQUIRE(result == 1);
        
        fclose(fp);
        Cudd_FreeApaNumber(num);
    }
}
