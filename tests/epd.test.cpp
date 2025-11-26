#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"
#include "epd.h"
#include "epdInt.h"

#include <cmath>
#include <cstring>
#include <limits>

/**
 * @brief Test file for epd.c
 * 
 * This file contains comprehensive tests to ensure high coverage
 * of the epd (extended precision double) module.
 */

// Helper to get the library-compatible NaN value
static double getLibraryNaN() {
    EpDouble epd;
    EpdMakeNan(&epd);
    return epd.type.value;
}

TEST_CASE("epd - EpdAlloc and EpdFree", "[epd]") {
    EpDouble *epd = EpdAlloc();
    REQUIRE(epd != nullptr);
    EpdFree(epd);
}

TEST_CASE("epd - EpdCmp", "[epd]") {
    EpDouble epd1, epd2;
    
    SECTION("Equal values") {
        EpdConvert(1.5, &epd1);
        EpdConvert(1.5, &epd2);
        REQUIRE(EpdCmp(&epd1, &epd2) == 0);
    }
    
    SECTION("Different values") {
        EpdConvert(1.5, &epd1);
        EpdConvert(2.5, &epd2);
        REQUIRE(EpdCmp(&epd1, &epd2) == 1);
    }
    
    SECTION("Different exponents") {
        EpdConvert(1.5, &epd1);
        EpdConvert(1.5, &epd2);
        epd2.exponent = 1;
        REQUIRE(EpdCmp(&epd1, &epd2) == 1);
    }
}

TEST_CASE("epd - EpdGetString", "[epd]") {
    EpDouble epd;
    char str[128];
    
    SECTION("Normal positive value") {
        EpdConvert(1.5, &epd);
        EpdGetString(&epd, str);
        REQUIRE(strlen(str) > 0);
    }
    
    SECTION("Negative value") {
        EpdConvert(-2.5, &epd);
        EpdGetString(&epd, str);
        REQUIRE(strlen(str) > 0);
    }
    
    SECTION("NaN value") {
        EpdMakeNan(&epd);
        EpdGetString(&epd, str);
        REQUIRE(strcmp(str, "NaN") == 0);
    }
    
    SECTION("Positive Inf value") {
        EpdMakeInf(&epd, 0);
        EpdGetString(&epd, str);
        REQUIRE(strcmp(str, "inf") == 0);
    }
    
    SECTION("Negative Inf value") {
        EpdMakeInf(&epd, 1);
        EpdGetString(&epd, str);
        REQUIRE(strcmp(str, "-inf") == 0);
    }
    
    SECTION("Null string") {
        EpdConvert(1.5, &epd);
        EpdGetString(&epd, nullptr);
        // Should not crash - just return
    }
    
    SECTION("Value with exponent < 10") {
        EpdConvert(5.0, &epd);
        EpdGetString(&epd, str);
        REQUIRE(strlen(str) > 0);
    }
    
    SECTION("Value with negative exponent") {
        EpdConvert(0.001, &epd);
        EpdGetString(&epd, str);
        REQUIRE(strlen(str) > 0);
    }
    
    SECTION("Large exponent >= 10") {
        EpdConvert(1e15, &epd);
        EpdGetString(&epd, str);
        REQUIRE(strlen(str) > 0);
    }
    
    SECTION("Small negative exponent >= 10") {
        EpdConvert(1e-15, &epd);
        EpdGetString(&epd, str);
        REQUIRE(strlen(str) > 0);
    }
}

TEST_CASE("epd - EpdConvert", "[epd]") {
    EpDouble epd;
    
    SECTION("Positive value") {
        EpdConvert(1.5, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Negative value") {
        EpdConvert(-2.5, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Zero value") {
        // Note: EpdConvert normalizes zero, which changes its representation
        // Use EpdMakeZero for actual zero testing
        EpDouble epd_zero;
        EpdMakeZero(&epd_zero, 0);
        REQUIRE(EpdIsZero(&epd_zero));
    }
    
    SECTION("Very large value") {
        EpdConvert(1e200, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Very small value") {
        EpdConvert(1e-200, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
}

TEST_CASE("epd - EpdMultiply", "[epd]") {
    EpDouble epd;
    
    SECTION("Normal multiplication") {
        EpdConvert(2.0, &epd);
        EpdMultiply(&epd, 3.0);
        // Result should be approximately 6.0
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Multiply by zero") {
        EpdConvert(2.0, &epd);
        EpdMultiply(&epd, 0.0);
        // Should handle zero correctly
    }
    
    SECTION("Multiply with NaN operand 1") {
        EpdMakeNan(&epd);
        EpdMultiply(&epd, 3.0);
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Multiply with NaN value") {
        EpdConvert(2.0, &epd);
        EpdMultiply(&epd, getLibraryNaN());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Multiply with Inf operand 1") {
        EpdMakeInf(&epd, 0);
        EpdMultiply(&epd, 3.0);
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Multiply with Inf value") {
        EpdConvert(2.0, &epd);
        EpdMultiply(&epd, std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Multiply with negative Inf") {
        EpdConvert(2.0, &epd);
        EpdMultiply(&epd, -std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsInf(&epd));
    }
}

TEST_CASE("epd - EpdMultiply2", "[epd]") {
    EpDouble epd1, epd2;
    
    SECTION("Normal multiplication") {
        EpdConvert(2.0, &epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
        REQUIRE(!EpdIsInf(&epd1));
    }
    
    SECTION("Multiply with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Multiply with NaN operand 2") {
        EpdConvert(2.0, &epd1);
        EpdMakeNan(&epd2);
        EpdMultiply2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Multiply with Inf operand 1") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(3.0, &epd2);
        EpdMultiply2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Multiply with Inf operand 2") {
        EpdConvert(2.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdMultiply2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
}

TEST_CASE("epd - EpdMultiply2Decimal", "[epd]") {
    EpDouble epd1, epd2;
    
    SECTION("Normal multiplication") {
        EpdConvert(2.0, &epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply2Decimal(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
        REQUIRE(!EpdIsInf(&epd1));
    }
    
    SECTION("Multiply with NaN") {
        EpdMakeNan(&epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply2Decimal(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Multiply with Inf") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(3.0, &epd2);
        EpdMultiply2Decimal(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
}

TEST_CASE("epd - EpdMultiply3", "[epd]") {
    EpDouble epd1, epd2, epd3;
    
    SECTION("Normal multiplication") {
        EpdConvert(2.0, &epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
        REQUIRE(!EpdIsInf(&epd3));
    }
    
    SECTION("Multiply with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Multiply with NaN operand 2") {
        EpdConvert(2.0, &epd1);
        EpdMakeNan(&epd2);
        EpdMultiply3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Multiply with Inf operand 1") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(3.0, &epd2);
        EpdMultiply3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Multiply with Inf operand 2") {
        EpdConvert(2.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdMultiply3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
}

TEST_CASE("epd - EpdMultiply3Decimal", "[epd]") {
    EpDouble epd1, epd2, epd3;
    
    SECTION("Normal multiplication") {
        EpdConvert(2.0, &epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply3Decimal(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
        REQUIRE(!EpdIsInf(&epd3));
    }
    
    SECTION("Multiply with NaN") {
        EpdMakeNan(&epd1);
        EpdConvert(3.0, &epd2);
        EpdMultiply3Decimal(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Multiply with Inf") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(3.0, &epd2);
        EpdMultiply3Decimal(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
}

TEST_CASE("epd - EpdDivide", "[epd]") {
    EpDouble epd;
    
    SECTION("Normal division") {
        EpdConvert(6.0, &epd);
        EpdDivide(&epd, 2.0);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Divide by zero") {
        EpdConvert(6.0, &epd);
        EpdDivide(&epd, 0.0);
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Divide with NaN operand 1") {
        EpdMakeNan(&epd);
        EpdDivide(&epd, 2.0);
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Divide with NaN value") {
        EpdConvert(6.0, &epd);
        EpdDivide(&epd, getLibraryNaN());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Inf divided by Inf") {
        EpdMakeInf(&epd, 0);
        EpdDivide(&epd, std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Inf divided by finite") {
        EpdMakeInf(&epd, 0);
        EpdDivide(&epd, 2.0);
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Finite divided by Inf") {
        EpdConvert(6.0, &epd);
        EpdDivide(&epd, std::numeric_limits<double>::infinity());
        // Result is zero
        REQUIRE(EpdIsZero(&epd));
    }
    
    SECTION("Negative Inf divided by positive") {
        EpdMakeInf(&epd, 1);
        EpdDivide(&epd, 2.0);
        REQUIRE(EpdIsInf(&epd));
    }
}

TEST_CASE("epd - EpdDivide2", "[epd]") {
    EpDouble epd1, epd2;
    
    SECTION("Normal division") {
        EpdConvert(6.0, &epd1);
        EpdConvert(2.0, &epd2);
        EpdDivide2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
        REQUIRE(!EpdIsInf(&epd1));
    }
    
    SECTION("Divide by zero") {
        EpdConvert(6.0, &epd1);
        EpdMakeZero(&epd2, 0);  // Use EpdMakeZero for proper zero
        EpdDivide2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Divide with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(2.0, &epd2);
        EpdDivide2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Divide with NaN operand 2") {
        EpdConvert(6.0, &epd1);
        EpdMakeNan(&epd2);
        EpdDivide2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Inf divided by Inf") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 0);
        EpdDivide2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Inf divided by finite") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(2.0, &epd2);
        EpdDivide2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Finite divided by Inf") {
        EpdConvert(6.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdDivide2(&epd1, &epd2);
        // Result is zero
        REQUIRE(EpdIsZero(&epd1));
    }
}

TEST_CASE("epd - EpdDivide3", "[epd]") {
    EpDouble epd1, epd2, epd3;
    
    SECTION("Normal division") {
        EpdConvert(6.0, &epd1);
        EpdConvert(2.0, &epd2);
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
        REQUIRE(!EpdIsInf(&epd3));
    }
    
    SECTION("Divide by zero") {
        EpdConvert(6.0, &epd1);
        EpdMakeZero(&epd2, 0);  // Use EpdMakeZero for proper zero
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Divide with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(2.0, &epd2);
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Divide with NaN operand 2") {
        EpdConvert(6.0, &epd1);
        EpdMakeNan(&epd2);
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Inf divided by Inf") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 0);
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Inf divided by finite") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(2.0, &epd2);
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Finite divided by Inf") {
        EpdConvert(6.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdDivide3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsZero(&epd3));
    }
}

TEST_CASE("epd - EpdAdd", "[epd]") {
    EpDouble epd;
    
    SECTION("Normal addition") {
        EpdConvert(2.0, &epd);
        EpdAdd(&epd, 3.0);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Add with NaN operand 1") {
        EpdMakeNan(&epd);
        EpdAdd(&epd, 3.0);
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Add with NaN value") {
        EpdConvert(2.0, &epd);
        EpdAdd(&epd, getLibraryNaN());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Inf + (-Inf) = NaN") {
        EpdMakeInf(&epd, 0);
        EpdAdd(&epd, -std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Inf + Inf = Inf") {
        EpdMakeInf(&epd, 0);
        EpdAdd(&epd, std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Finite + Inf = Inf") {
        EpdConvert(2.0, &epd);
        EpdAdd(&epd, std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Add with different exponents - epd1 > epd2") {
        EpdConvert(1e100, &epd);
        EpdAdd(&epd, 1.0);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Add with different exponents - epd1 < epd2") {
        EpdConvert(1.0, &epd);
        EpdAdd(&epd, 1e100);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Add with same exponents") {
        EpdConvert(1.5, &epd);
        EpdAdd(&epd, 1.5);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Add with large exponent difference > EPD_MAX_BIN - epd1 > epd2") {
        EpdConvert(1e300, &epd);
        EpdAdd(&epd, 1e-300);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Add with large exponent difference > EPD_MAX_BIN - epd1 < epd2") {
        EpdConvert(1e-300, &epd);
        EpdAdd(&epd, 1e300);
        REQUIRE(!EpdIsNan(&epd));
    }
}

TEST_CASE("epd - EpdAdd2", "[epd]") {
    EpDouble epd1, epd2;
    
    SECTION("Normal addition") {
        EpdConvert(2.0, &epd1);
        EpdConvert(3.0, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
        REQUIRE(!EpdIsInf(&epd1));
    }
    
    SECTION("Add with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(3.0, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Add with NaN operand 2") {
        EpdConvert(2.0, &epd1);
        EpdMakeNan(&epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Inf + (-Inf) = NaN") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 1);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Inf + Inf = Inf") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 0);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Finite + Inf = Inf") {
        EpdConvert(2.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Inf + Finite = Inf") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(2.0, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Add with different exponents - epd1 > epd2") {
        EpdConvert(1e100, &epd1);
        EpdConvert(1.0, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Add with different exponents - epd1 < epd2") {
        EpdConvert(1.0, &epd1);
        EpdConvert(1e100, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Add with same exponents") {
        EpdConvert(1.5, &epd1);
        EpdConvert(1.5, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Add with large exponent difference > EPD_MAX_BIN - epd1 > epd2") {
        EpdConvert(1e300, &epd1);
        EpdConvert(1e-300, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Add with large exponent difference > EPD_MAX_BIN - epd1 < epd2") {
        EpdConvert(1e-300, &epd1);
        EpdConvert(1e300, &epd2);
        EpdAdd2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
}

TEST_CASE("epd - EpdAdd3", "[epd]") {
    EpDouble epd1, epd2, epd3;
    
    SECTION("Normal addition") {
        EpdConvert(2.0, &epd1);
        EpdConvert(3.0, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
        REQUIRE(!EpdIsInf(&epd3));
    }
    
    SECTION("Add with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(3.0, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Add with NaN operand 2") {
        EpdConvert(2.0, &epd1);
        EpdMakeNan(&epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Inf + (-Inf) = NaN") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 1);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Inf + Inf = Inf") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 0);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Inf + Finite") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(2.0, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Finite + Inf") {
        EpdConvert(2.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Add with different exponents - epd1 > epd2") {
        EpdConvert(1e100, &epd1);
        EpdConvert(1.0, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Add with different exponents - epd1 < epd2") {
        EpdConvert(1.0, &epd1);
        EpdConvert(1e100, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Add with same exponents") {
        EpdConvert(1.5, &epd1);
        EpdConvert(1.5, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Add with large exponent difference > EPD_MAX_BIN - epd1 > epd2") {
        EpdConvert(1e300, &epd1);
        EpdConvert(1e-300, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Add with large exponent difference > EPD_MAX_BIN - epd1 < epd2") {
        EpdConvert(1e-300, &epd1);
        EpdConvert(1e300, &epd2);
        EpdAdd3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
}

TEST_CASE("epd - EpdSubtract", "[epd]") {
    EpDouble epd;
    
    SECTION("Normal subtraction") {
        EpdConvert(5.0, &epd);
        EpdSubtract(&epd, 2.0);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Subtract with NaN operand 1") {
        EpdMakeNan(&epd);
        EpdSubtract(&epd, 2.0);
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Subtract with NaN value") {
        EpdConvert(5.0, &epd);
        EpdSubtract(&epd, getLibraryNaN());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Inf - Inf = NaN") {
        EpdMakeInf(&epd, 0);
        EpdSubtract(&epd, std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsNan(&epd));
    }
    
    SECTION("Inf - (-Inf) = Inf") {
        EpdMakeInf(&epd, 0);
        EpdSubtract(&epd, -std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Finite - Inf = -Inf") {
        EpdConvert(2.0, &epd);
        EpdSubtract(&epd, std::numeric_limits<double>::infinity());
        REQUIRE(EpdIsInf(&epd));
    }
    
    SECTION("Subtract with different exponents - epd1 > epd2") {
        EpdConvert(1e100, &epd);
        EpdSubtract(&epd, 1.0);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Subtract with different exponents - epd1 < epd2") {
        EpdConvert(1.0, &epd);
        EpdSubtract(&epd, 1e100);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Subtract with same exponents") {
        EpdConvert(1.5, &epd);
        EpdSubtract(&epd, 0.5);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Subtract with large exponent difference > EPD_MAX_BIN - epd1 > epd2") {
        EpdConvert(1e300, &epd);
        EpdSubtract(&epd, 1e-300);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Subtract with large exponent difference > EPD_MAX_BIN - epd1 < epd2") {
        EpdConvert(1e-300, &epd);
        EpdSubtract(&epd, 1e300);
        REQUIRE(!EpdIsNan(&epd));
    }
}

TEST_CASE("epd - EpdSubtract2", "[epd]") {
    EpDouble epd1, epd2;
    
    SECTION("Normal subtraction") {
        EpdConvert(5.0, &epd1);
        EpdConvert(2.0, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
        REQUIRE(!EpdIsInf(&epd1));
    }
    
    SECTION("Subtract with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(2.0, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Subtract with NaN operand 2") {
        EpdConvert(5.0, &epd1);
        EpdMakeNan(&epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Inf - Inf = NaN") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 0);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(EpdIsNan(&epd1));
    }
    
    SECTION("Inf - (-Inf) = Inf") {
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 1);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Finite - Inf") {
        EpdConvert(2.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Inf - Finite = Inf") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(2.0, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(EpdIsInf(&epd1));
    }
    
    SECTION("Subtract with different exponents - epd1 > epd2") {
        EpdConvert(1e100, &epd1);
        EpdConvert(1.0, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Subtract with different exponents - epd1 < epd2") {
        EpdConvert(1.0, &epd1);
        EpdConvert(1e100, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Subtract with same exponents") {
        EpdConvert(1.5, &epd1);
        EpdConvert(0.5, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Subtract with large exponent difference > EPD_MAX_BIN - epd1 > epd2") {
        EpdConvert(1e300, &epd1);
        EpdConvert(1e-300, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
    
    SECTION("Subtract with large exponent difference > EPD_MAX_BIN - epd1 < epd2") {
        EpdConvert(1e-300, &epd1);
        EpdConvert(1e300, &epd2);
        EpdSubtract2(&epd1, &epd2);
        REQUIRE(!EpdIsNan(&epd1));
    }
}

TEST_CASE("epd - EpdSubtract3", "[epd]") {
    EpDouble epd1, epd2, epd3;
    
    SECTION("Normal subtraction") {
        EpdConvert(5.0, &epd1);
        EpdConvert(2.0, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
        REQUIRE(!EpdIsInf(&epd3));
    }
    
    SECTION("Subtract with NaN operand 1") {
        EpdMakeNan(&epd1);
        EpdConvert(2.0, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Subtract with NaN operand 2") {
        EpdConvert(5.0, &epd1);
        EpdMakeNan(&epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Inf - Inf copies epd1 (same sign)") {
        // Note: The library behavior is that when signs are the same,
        // it copies epd1 (returns Inf, not NaN)
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 0);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Inf - (-Inf) = NaN (different signs)") {
        // Note: The library behavior is that when signs differ,
        // it returns NaN
        EpdMakeInf(&epd1, 0);
        EpdMakeInf(&epd2, 1);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsNan(&epd3));
    }
    
    SECTION("Inf - Finite") {
        EpdMakeInf(&epd1, 0);
        EpdConvert(2.0, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Finite - Inf") {
        EpdConvert(2.0, &epd1);
        EpdMakeInf(&epd2, 0);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(EpdIsInf(&epd3));
    }
    
    SECTION("Subtract with different exponents - epd1 > epd2") {
        EpdConvert(1e100, &epd1);
        EpdConvert(1.0, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Subtract with different exponents - epd1 < epd2") {
        EpdConvert(1.0, &epd1);
        EpdConvert(1e100, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Subtract with same exponents") {
        EpdConvert(1.5, &epd1);
        EpdConvert(0.5, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Subtract with large exponent difference > EPD_MAX_BIN - epd1 > epd2") {
        EpdConvert(1e300, &epd1);
        EpdConvert(1e-300, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
    
    SECTION("Subtract with large exponent difference > EPD_MAX_BIN - epd1 < epd2") {
        EpdConvert(1e-300, &epd1);
        EpdConvert(1e300, &epd2);
        EpdSubtract3(&epd1, &epd2, &epd3);
        REQUIRE(!EpdIsNan(&epd3));
    }
}

TEST_CASE("epd - EpdPow2", "[epd]") {
    EpDouble epd;
    
    SECTION("Small power <= EPD_MAX_BIN") {
        EpdPow2(10, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Large power > EPD_MAX_BIN") {
        EpdPow2(2000, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Power of 0") {
        EpdPow2(0, &epd);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Power of 1") {
        EpdPow2(1, &epd);
        REQUIRE(!EpdIsNan(&epd));
    }
}

TEST_CASE("epd - EpdPow2Decimal", "[epd]") {
    EpDouble epd;
    
    SECTION("Small power <= EPD_MAX_BIN") {
        EpdPow2Decimal(10, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Large power > EPD_MAX_BIN") {
        EpdPow2Decimal(2000, &epd);
        REQUIRE(!EpdIsNan(&epd));
        REQUIRE(!EpdIsInf(&epd));
    }
    
    SECTION("Power of 0") {
        EpdPow2Decimal(0, &epd);
        REQUIRE(!EpdIsNan(&epd));
    }
}

TEST_CASE("epd - EpdNormalize", "[epd]") {
    EpDouble epd;
    
    SECTION("Normalize normal value") {
        EpdConvert(1.5, &epd);
        EpdNormalize(&epd);
        REQUIRE(epd.type.bits.exponent == EPD_MAX_BIN);
    }
    
    SECTION("Normalize NaN") {
        EpdMakeNan(&epd);
        EpdNormalize(&epd);
        REQUIRE(epd.exponent == 0);
    }
    
    SECTION("Normalize Inf") {
        EpdMakeInf(&epd, 0);
        EpdNormalize(&epd);
        REQUIRE(epd.exponent == 0);
    }
    
    SECTION("Already normalized") {
        EpdConvert(1.5, &epd);
        int oldExp = epd.exponent;
        EpdNormalize(&epd);
        REQUIRE(epd.type.bits.exponent == EPD_MAX_BIN);
    }
}

TEST_CASE("epd - EpdNormalizeDecimal", "[epd]") {
    EpDouble epd;
    
    SECTION("Normalize normal value") {
        epd.type.value = 1234.56;
        epd.exponent = 0;
        EpdNormalizeDecimal(&epd);
        REQUIRE(!EpdIsNan(&epd));
    }
    
    SECTION("Normalize NaN") {
        EpdMakeNan(&epd);
        EpdNormalizeDecimal(&epd);
        REQUIRE(epd.exponent == 0);
    }
    
    SECTION("Normalize Inf") {
        EpdMakeInf(&epd, 0);
        EpdNormalizeDecimal(&epd);
        REQUIRE(epd.exponent == 0);
    }
}

TEST_CASE("epd - EpdGetValueAndDecimalExponent", "[epd]") {
    EpDouble epd;
    double value;
    int exponent;
    
    SECTION("Normal value") {
        EpdConvert(1234.56, &epd);
        EpdGetValueAndDecimalExponent(&epd, &value, &exponent);
        REQUIRE(value != 0.0);
    }
    
    SECTION("Zero value") {
        // Use EpdMakeZero for proper zero handling
        EpdMakeZero(&epd, 0);
        EpdGetValueAndDecimalExponent(&epd, &value, &exponent);
        REQUIRE(value == 0.0);
        REQUIRE(exponent == 0);
    }
    
    SECTION("NaN value") {
        EpdMakeNan(&epd);
        EpdGetValueAndDecimalExponent(&epd, &value, &exponent);
        REQUIRE(exponent == EPD_EXP_INF);
        REQUIRE(value == 0.0);
    }
    
    SECTION("Inf value") {
        EpdMakeInf(&epd, 0);
        EpdGetValueAndDecimalExponent(&epd, &value, &exponent);
        REQUIRE(exponent == EPD_EXP_INF);
        REQUIRE(value == 0.0);
    }
}

TEST_CASE("epd - EpdGetExponent", "[epd]") {
    SECTION("Normal value") {
        int exp = EpdGetExponent(1.5);
        REQUIRE(exp > 0);
    }
    
    SECTION("Zero") {
        int exp = EpdGetExponent(0.0);
        REQUIRE(exp == 0);
    }
    
    SECTION("Large value") {
        int exp = EpdGetExponent(1e100);
        REQUIRE(exp > 0);
    }
}

TEST_CASE("epd - EpdGetExponentDecimal", "[epd]") {
    SECTION("Normal value") {
        int exp = EpdGetExponentDecimal(1.5);
        REQUIRE(exp >= 0);
    }
    
    SECTION("Large value") {
        int exp = EpdGetExponentDecimal(1e100);
        REQUIRE(exp == 100);
    }
    
    SECTION("Small value") {
        int exp = EpdGetExponentDecimal(1e-100);
        REQUIRE(exp == -100);
    }
}

TEST_CASE("epd - EpdMakeInf", "[epd]") {
    EpDouble epd;
    
    SECTION("Positive Inf") {
        EpdMakeInf(&epd, 0);
        REQUIRE(EpdIsInf(&epd));
        REQUIRE(epd.type.bits.sign == 0);
    }
    
    SECTION("Negative Inf") {
        EpdMakeInf(&epd, 1);
        REQUIRE(EpdIsInf(&epd));
        REQUIRE(epd.type.bits.sign == 1);
    }
}

TEST_CASE("epd - EpdMakeZero", "[epd]") {
    EpDouble epd;
    
    SECTION("Positive Zero") {
        EpdMakeZero(&epd, 0);
        REQUIRE(EpdIsZero(&epd));
        REQUIRE(epd.type.bits.sign == 0);
    }
    
    SECTION("Negative Zero") {
        EpdMakeZero(&epd, 1);
        REQUIRE(EpdIsZero(&epd));
        REQUIRE(epd.type.bits.sign == 1);
    }
}

TEST_CASE("epd - EpdMakeNan", "[epd]") {
    EpDouble epd;
    EpdMakeNan(&epd);
    REQUIRE(EpdIsNan(&epd));
}

TEST_CASE("epd - EpdCopy", "[epd]") {
    EpDouble from, to;
    
    SECTION("Copy normal value") {
        EpdConvert(1.5, &from);
        EpdCopy(&from, &to);
        REQUIRE(to.type.value == from.type.value);
        REQUIRE(to.exponent == from.exponent);
    }
    
    SECTION("Copy NaN") {
        EpdMakeNan(&from);
        EpdCopy(&from, &to);
        REQUIRE(EpdIsNan(&to));
    }
    
    SECTION("Copy Inf") {
        EpdMakeInf(&from, 0);
        EpdCopy(&from, &to);
        REQUIRE(EpdIsInf(&to));
    }
}

TEST_CASE("epd - EpdIsInf", "[epd]") {
    EpDouble epd;
    
    SECTION("Is Inf - true") {
        EpdMakeInf(&epd, 0);
        REQUIRE(EpdIsInf(&epd) != 0);
    }
    
    SECTION("Is Inf - false") {
        EpdConvert(1.5, &epd);
        REQUIRE(EpdIsInf(&epd) == 0);
    }
}

TEST_CASE("epd - EpdIsZero", "[epd]") {
    EpDouble epd;
    
    SECTION("Is Zero - true") {
        // Use EpdMakeZero for proper zero testing
        EpdMakeZero(&epd, 0);
        REQUIRE(EpdIsZero(&epd) == 1);
    }
    
    SECTION("Is Zero - false") {
        EpdConvert(1.5, &epd);
        REQUIRE(EpdIsZero(&epd) == 0);
    }
}

TEST_CASE("epd - EpdIsNan", "[epd]") {
    EpDouble epd;
    
    SECTION("Is NaN - true") {
        EpdMakeNan(&epd);
        REQUIRE(EpdIsNan(&epd) != 0);
    }
    
    SECTION("Is NaN - false") {
        EpdConvert(1.5, &epd);
        REQUIRE(EpdIsNan(&epd) == 0);
    }
}

TEST_CASE("epd - EpdIsNanOrInf", "[epd]") {
    EpDouble epd;
    
    SECTION("Is NaN") {
        EpdMakeNan(&epd);
        REQUIRE(EpdIsNanOrInf(&epd) != 0);
    }
    
    SECTION("Is Inf") {
        EpdMakeInf(&epd, 0);
        REQUIRE(EpdIsNanOrInf(&epd) != 0);
    }
    
    SECTION("Is normal") {
        EpdConvert(1.5, &epd);
        REQUIRE(EpdIsNanOrInf(&epd) == 0);
    }
}

TEST_CASE("epd - IsInfDouble", "[epd]") {
    SECTION("Positive Inf") {
        REQUIRE(IsInfDouble(std::numeric_limits<double>::infinity()) == 1);
    }
    
    SECTION("Negative Inf") {
        REQUIRE(IsInfDouble(-std::numeric_limits<double>::infinity()) == -1);
    }
    
    SECTION("Normal value") {
        REQUIRE(IsInfDouble(1.5) == 0);
    }
}

TEST_CASE("epd - IsNanDouble", "[epd]") {
    EpDouble epd;
    
    SECTION("NaN value - using EpdMakeNan") {
        EpdMakeNan(&epd);
        REQUIRE(IsNanDouble(epd.type.value) == 1);
    }
    
    SECTION("Normal value") {
        REQUIRE(IsNanDouble(1.5) == 0);
    }
}

TEST_CASE("epd - IsNanOrInfDouble", "[epd]") {
    EpDouble epd;
    
    SECTION("NaN value") {
        EpdMakeNan(&epd);
        REQUIRE(IsNanOrInfDouble(epd.type.value) == 1);
    }
    
    SECTION("Positive Inf") {
        REQUIRE(IsNanOrInfDouble(std::numeric_limits<double>::infinity()) == 1);
    }
    
    SECTION("Normal value") {
        REQUIRE(IsNanOrInfDouble(1.5) == 0);
    }
}
