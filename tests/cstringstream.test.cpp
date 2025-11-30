#include <catch2/catch_test_macros.hpp>

#include "cstringstream.h"
#include <cstring>
#include <climits>

/**
 * @brief Test file for cstringstream.c
 * 
 * This file contains comprehensive tests to ensure high coverage
 * of the cstringstream module.
 */

TEST_CASE("cstringstream - newStringStream and deleteStringStream", "[cstringstream]") {
    SECTION("Create and delete stream") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        deleteStringStream(ss);
    }
    
    SECTION("Delete NULL stream should not crash") {
        deleteStringStream(nullptr);
        REQUIRE(true);  // Should reach here without crashing
    }
}

TEST_CASE("cstringstream - clearStringStream", "[cstringstream]") {
    SECTION("Clear valid stream") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        // Add some data
        REQUIRE(appendCharStringStream(ss, 'a') == 0);
        REQUIRE(appendCharStringStream(ss, 'b') == 0);
        
        // Clear
        REQUIRE(clearStringStream(ss) == 0);
        
        // Check size is now 0
        size_t size = 999;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 0);
        
        deleteStringStream(ss);
    }
    
    SECTION("Clear NULL stream returns -1") {
        REQUIRE(clearStringStream(nullptr) == -1);
    }
}

TEST_CASE("cstringstream - copyStringStream", "[cstringstream]") {
    SECTION("Copy valid stream") {
        cstringstream src = newStringStream();
        REQUIRE(src != nullptr);
        
        // Add some data
        REQUIRE(appendStringStringStream(src, "hello") == 0);
        
        // Copy
        cstringstream dest = copyStringStream(src);
        REQUIRE(dest != nullptr);
        
        // Verify content
        char *srcStr = stringFromStringStream(src);
        char *destStr = stringFromStringStream(dest);
        REQUIRE(srcStr != nullptr);
        REQUIRE(destStr != nullptr);
        REQUIRE(strcmp(srcStr, destStr) == 0);
        
        free(srcStr);
        free(destStr);
        deleteStringStream(src);
        deleteStringStream(dest);
    }
    
    SECTION("Copy NULL stream returns 0") {
        cstringstream result = copyStringStream(nullptr);
        REQUIRE(result == nullptr);
    }
    
    SECTION("Copy empty stream") {
        cstringstream src = newStringStream();
        REQUIRE(src != nullptr);
        
        cstringstream dest = copyStringStream(src);
        REQUIRE(dest != nullptr);
        
        size_t size = 999;
        REQUIRE(sizeStringStream(dest, &size) == 0);
        REQUIRE(size == 0);
        
        deleteStringStream(src);
        deleteStringStream(dest);
    }
}

TEST_CASE("cstringstream - resizeStringStream", "[cstringstream]") {
    SECTION("Resize to smaller size") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        // Add data
        REQUIRE(appendStringStringStream(ss, "hello world") == 0);
        
        // Resize smaller (doesn't actually shrink capacity but changes inUse)
        REQUIRE(resizeStringStream(ss, 5) == 0);
        
        size_t size;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 5);
        
        deleteStringStream(ss);
    }
    
    SECTION("Resize triggers doubling") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        // Initial capacity is 1, resize to 2 should trigger doubling to 2
        REQUIRE(resizeStringStream(ss, 2) == 0);
        
        size_t size;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 2);
        
        deleteStringStream(ss);
    }
    
    SECTION("Resize triggers larger allocation when newSize > 2*capacity") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        // Initial capacity is 1, resize to 10 should allocate 10 (not 2)
        REQUIRE(resizeStringStream(ss, 10) == 0);
        
        size_t size;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 10);
        
        deleteStringStream(ss);
    }
}

TEST_CASE("cstringstream - sizeStringStream", "[cstringstream]") {
    SECTION("Get size of valid stream") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        size_t size = 999;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 0);
        
        REQUIRE(appendCharStringStream(ss, 'x') == 0);
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 1);
        
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        size_t size;
        REQUIRE(sizeStringStream(nullptr, &size) == -1);
    }
    
    SECTION("NULL num pointer returns -1") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        REQUIRE(sizeStringStream(ss, nullptr) == -1);
        deleteStringStream(ss);
    }
}

TEST_CASE("cstringstream - getStringStream", "[cstringstream]") {
    SECTION("Get character at valid index") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "abc") == 0);
        
        char c = '\0';
        REQUIRE(getStringStream(ss, 0, &c) == 0);
        REQUIRE(c == 'a');
        
        REQUIRE(getStringStream(ss, 1, &c) == 0);
        REQUIRE(c == 'b');
        
        REQUIRE(getStringStream(ss, 2, &c) == 0);
        REQUIRE(c == 'c');
        
        deleteStringStream(ss);
    }
    
    SECTION("Get character at invalid index returns -1") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "ab") == 0);
        
        char c = 'x';
        REQUIRE(getStringStream(ss, 2, &c) == -1);  // Index out of range
        REQUIRE(getStringStream(ss, 100, &c) == -1);
        
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        char c;
        REQUIRE(getStringStream(nullptr, 0, &c) == -1);
    }
    
    SECTION("NULL char pointer returns -1") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        REQUIRE(appendCharStringStream(ss, 'a') == 0);
        REQUIRE(getStringStream(ss, 0, nullptr) == -1);
        deleteStringStream(ss);
    }
}

TEST_CASE("cstringstream - appendCharStringStream", "[cstringstream]") {
    SECTION("Append single character") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendCharStringStream(ss, 'H') == 0);
        REQUIRE(appendCharStringStream(ss, 'i') == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "Hi") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendCharStringStream(nullptr, 'x') == -1);
    }
}

TEST_CASE("cstringstream - appendStringStringStream", "[cstringstream]") {
    SECTION("Append string") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "Hello") == 0);
        REQUIRE(appendStringStringStream(ss, " ") == 0);
        REQUIRE(appendStringStringStream(ss, "World") == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "Hello World") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append empty string") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "") == 0);
        
        size_t size;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 0);
        
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendStringStringStream(nullptr, "test") == -1);
    }
}

TEST_CASE("cstringstream - appendIntStringStream", "[cstringstream]") {
    SECTION("Append positive integer") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendIntStringStream(ss, 42) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "42") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append negative integer") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendIntStringStream(ss, -123) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "-123") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append zero") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendIntStringStream(ss, 0) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "0") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendIntStringStream(nullptr, 42) == -1);
    }
}

TEST_CASE("cstringstream - appendUnsignedStringStream", "[cstringstream]") {
    SECTION("Append unsigned integer") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendUnsignedStringStream(ss, 42u) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "42") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append large unsigned integer") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendUnsignedStringStream(ss, UINT_MAX) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strlen(str) > 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendUnsignedStringStream(nullptr, 42u) == -1);
    }
}

TEST_CASE("cstringstream - appendLongStringStream", "[cstringstream]") {
    SECTION("Append positive long") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendLongStringStream(ss, 123456789L) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "123456789") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append negative long") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendLongStringStream(ss, -987654321L) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "-987654321") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendLongStringStream(nullptr, 42L) == -1);
    }
}

TEST_CASE("cstringstream - appendUnsignedLongStringStream", "[cstringstream]") {
    SECTION("Append unsigned long") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendUnsignedLongStringStream(ss, 123456789UL) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "123456789") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append large unsigned long") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendUnsignedLongStringStream(ss, ULONG_MAX) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strlen(str) > 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendUnsignedLongStringStream(nullptr, 42UL) == -1);
    }
}

TEST_CASE("cstringstream - appendDoubleStringStream", "[cstringstream]") {
    SECTION("Append positive double") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendDoubleStringStream(ss, 3.14) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strlen(str) > 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append negative double") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendDoubleStringStream(ss, -2.5) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(str[0] == '-');
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Append zero double") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendDoubleStringStream(ss, 0.0) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "0") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(appendDoubleStringStream(nullptr, 3.14) == -1);
    }
}

TEST_CASE("cstringstream - putStringStream", "[cstringstream]") {
    SECTION("Put character at valid index") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "abc") == 0);
        REQUIRE(putStringStream(ss, 1, 'X') == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "aXc") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Put at first position") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "abc") == 0);
        REQUIRE(putStringStream(ss, 0, 'Z') == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "Zbc") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Put at last position") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "abc") == 0);
        REQUIRE(putStringStream(ss, 2, 'Y') == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "abY") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Put at invalid index returns -1") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "ab") == 0);
        REQUIRE(putStringStream(ss, 2, 'X') == -1);  // Index out of range
        REQUIRE(putStringStream(ss, 100, 'X') == -1);
        
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns -1") {
        REQUIRE(putStringStream(nullptr, 0, 'x') == -1);
    }
}

TEST_CASE("cstringstream - stringFromStringStream", "[cstringstream]") {
    SECTION("Get string from valid stream") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "test") == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "test") == 0);
        REQUIRE(strlen(str) == 4);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Get string from empty stream") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "") == 0);
        REQUIRE(strlen(str) == 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("NULL stream returns 0") {
        char *str = stringFromStringStream(nullptr);
        REQUIRE(str == nullptr);
    }
}

TEST_CASE("cstringstream - Combined operations", "[cstringstream]") {
    SECTION("Build complex string") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "Value: ") == 0);
        REQUIRE(appendIntStringStream(ss, 42) == 0);
        REQUIRE(appendStringStringStream(ss, ", ") == 0);
        REQUIRE(appendDoubleStringStream(ss, 3.14) == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strlen(str) > 0);
        
        free(str);
        deleteStringStream(ss);
    }
    
    SECTION("Multiple resizes") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        // Force multiple resizes by adding many characters
        for (int i = 0; i < 100; i++) {
            REQUIRE(appendCharStringStream(ss, 'x') == 0);
        }
        
        size_t size;
        REQUIRE(sizeStringStream(ss, &size) == 0);
        REQUIRE(size == 100);
        
        deleteStringStream(ss);
    }
    
    SECTION("Clear and reuse") {
        cstringstream ss = newStringStream();
        REQUIRE(ss != nullptr);
        
        REQUIRE(appendStringStringStream(ss, "first") == 0);
        REQUIRE(clearStringStream(ss) == 0);
        REQUIRE(appendStringStringStream(ss, "second") == 0);
        
        char *str = stringFromStringStream(ss);
        REQUIRE(str != nullptr);
        REQUIRE(strcmp(str, "second") == 0);
        
        free(str);
        deleteStringStream(ss);
    }
}
