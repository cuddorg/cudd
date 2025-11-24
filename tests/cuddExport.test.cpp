#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

/**
 * @brief Test file for cuddExport.c
 * 
 * This file contains comprehensive tests to ensure 100% code coverage
 * for the cuddExport module, including all export functions:
 * - Cudd_DumpBlif
 * - Cudd_DumpBlifBody
 * - Cudd_DumpDot
 * - Cudd_DumpDaVinci
 * - Cudd_DumpDDcal
 * - Cudd_DumpFactoredForm
 * - Cudd_FactoredFormString
 */

// Helper function to create a temporary file
static FILE* create_temp_file() {
    char temp_name[] = "/tmp/cudd_export_test_XXXXXX";
    int fd = mkstemp(temp_name);
    if (fd == -1) return nullptr;
    FILE* fp = fdopen(fd, "w+");
    if (fp) {
        unlink(temp_name); // Delete on close
    }
    return fp;
}

// Helper to read file contents
static std::string read_file(FILE* fp) {
    rewind(fp);
    std::string content;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        content += buffer;
    }
    return content;
}

TEST_CASE("Cudd_DumpBlif - Basic BLIF export", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    FILE *fp = create_temp_file();
    REQUIRE(fp != nullptr);
    
    SECTION("Export simple BDD") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpBlif(manager, 1, outputs, nullptr, nullptr, nullptr, fp, 0);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find(".model DD") != std::string::npos);
        REQUIRE(content.find(".inputs") != std::string::npos);
        REQUIRE(content.find(".outputs") != std::string::npos);
        REQUIRE(content.find(".end") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with custom names") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        const char *inames[] = {"input_x", "input_y"};
        const char *onames[] = {"output_f"};
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpBlif(manager, 1, outputs, inames, onames, "TestModel", fp, 0);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find(".model TestModel") != std::string::npos);
        REQUIRE(content.find("input_x") != std::string::npos);
        REQUIRE(content.find("input_y") != std::string::npos);
        REQUIRE(content.find("output_f") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export multiple outputs") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        DdNode *f2 = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f1);
        Cudd_Ref(f2);
        
        DdNode *outputs[] = {f1, f2};
        int result = Cudd_DumpBlif(manager, 2, outputs, nullptr, nullptr, nullptr, fp, 0);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("f0") != std::string::npos);
        REQUIRE(content.find("f1") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with complemented output") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *f = Cudd_Not(x);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpBlif(manager, 1, outputs, nullptr, nullptr, nullptr, fp, 0);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("0 1") != std::string::npos); // Inverter pattern
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export constant functions") {
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        
        DdNode *outputs[] = {one};
        int result = Cudd_DumpBlif(manager, 1, outputs, nullptr, nullptr, nullptr, fp, 0);
        REQUIRE(result == 1);
        
        rewind(fp);
        outputs[0] = zero;
        result = Cudd_DumpBlif(manager, 1, outputs, nullptr, nullptr, nullptr, fp, 0);
        REQUIRE(result == 1);
    }
    
    SECTION("Export BLIF-MV format") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpBlif(manager, 1, outputs, nullptr, nullptr, nullptr, fp, 1);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find(".def") != std::string::npos); // MV specific
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    fclose(fp);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_DumpBlifBody - BLIF body export", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    FILE *fp = create_temp_file();
    REQUIRE(fp != nullptr);
    
    SECTION("Export body only") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpBlifBody(manager, 1, outputs, nullptr, nullptr, fp, 0);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find(".names") != std::string::npos);
        REQUIRE(content.find(".model") == std::string::npos); // No header
        REQUIRE(content.find(".end") == std::string::npos); // No footer
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with custom output names") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        const char *onames[] = {"my_output"};
        DdNode *outputs[] = {x};
        
        int result = Cudd_DumpBlifBody(manager, 1, outputs, nullptr, onames, fp, 0);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("my_output") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export with MV format") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpBlifBody(manager, 1, outputs, nullptr, nullptr, fp, 1);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find(".def") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    fclose(fp);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_DumpDot - DOT format export", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    FILE *fp = create_temp_file();
    REQUIRE(fp != nullptr);
    
    SECTION("Export simple BDD to DOT") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDot(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("digraph") != std::string::npos);
        REQUIRE(content.find("->") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with variable names") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        const char *inames[] = {"var_x", "var_y"};
        const char *onames[] = {"out_f"};
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDot(manager, 1, outputs, inames, onames, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("var_x") != std::string::npos);
        REQUIRE(content.find("var_y") != std::string::npos);
        REQUIRE(content.find("out_f") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export multiple outputs") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        DdNode *f2 = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f1);
        Cudd_Ref(f2);
        
        DdNode *outputs[] = {f1, f2};
        int result = Cudd_DumpDot(manager, 2, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("F0") != std::string::npos);
        REQUIRE(content.find("F1") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with complemented edges") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *f = Cudd_Not(x);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDot(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("dotted") != std::string::npos); // Complement edge style
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export constant nodes") {
        DdNode *one = Cudd_ReadOne(manager);
        
        DdNode *outputs[] = {one};
        int result = Cudd_DumpDot(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("CONST NODES") != std::string::npos);
        
    }
    
    SECTION("Export ADD with constants") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        // Create ADD with different constant values
        DdNode *add = Cudd_BddToAdd(manager, x);
        Cudd_Ref(add);
        
        DdNode *outputs[] = {add};
        int result = Cudd_DumpDot(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("label") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, add);
        Cudd_RecursiveDeref(manager, x);
    }
    
    fclose(fp);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_DumpDaVinci - daVinci format export", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    FILE *fp = create_temp_file();
    REQUIRE(fp != nullptr);
    
    SECTION("Export simple BDD to daVinci") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDaVinci(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("l(") != std::string::npos);
        REQUIRE(content.find("n(") != std::string::npos);
        REQUIRE(content.find("OBJECT") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with output names") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        const char *onames[] = {"output_name"};
        DdNode *outputs[] = {x};
        
        int result = Cudd_DumpDaVinci(manager, 1, outputs, nullptr, onames, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("output_name") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export with complemented output") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *f = Cudd_Not(x);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDaVinci(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("red") != std::string::npos); // Complement edge color
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export constant nodes") {
        DdNode *one = Cudd_ReadOne(manager);
        
        DdNode *outputs[] = {one};
        int result = Cudd_DumpDaVinci(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("constant") != std::string::npos);
    }
    
    SECTION("Export with input names") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        const char *inames[] = {"in_x", "in_y"};
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDaVinci(manager, 1, outputs, inames, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("in_x") != std::string::npos || content.find("in_y") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export multiple outputs") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        DdNode *f2 = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f1);
        Cudd_Ref(f2);
        
        DdNode *outputs[] = {f1, f2};
        int result = Cudd_DumpDaVinci(manager, 2, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("f0") != std::string::npos);
        REQUIRE(content.find("f1") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    fclose(fp);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_DumpDDcal - DDcal format export", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    FILE *fp = create_temp_file();
    REQUIRE(fp != nullptr);
    
    SECTION("Export simple BDD to DDcal") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDDcal(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find(" * ") != std::string::npos); // Variable separator
        REQUIRE(content.find("[") != std::string::npos);
        REQUIRE(content.find("]") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with variable names") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        const char *inames[] = {"input_a", "input_b"};
        const char *onames[] = {"output_result"};
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDDcal(manager, 1, outputs, inames, onames, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("input_a") != std::string::npos || content.find("input_b") != std::string::npos);
        REQUIRE(content.find("output_result") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export multiple outputs") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f1 = Cudd_bddAnd(manager, x, y);
        DdNode *f2 = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f1);
        Cudd_Ref(f2);
        
        DdNode *outputs[] = {f1, f2};
        int result = Cudd_DumpDDcal(manager, 2, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("f0") != std::string::npos);
        REQUIRE(content.find("f1") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f1);
        Cudd_RecursiveDeref(manager, f2);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with complemented output") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *f = Cudd_Not(x);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpDDcal(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("'") != std::string::npos); // Complement operator
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export constant nodes") {
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        
        DdNode *outputs[] = {one};
        int result = Cudd_DumpDDcal(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("1") != std::string::npos);
        
        rewind(fp);
        outputs[0] = zero;
        result = Cudd_DumpDDcal(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
    }
    
    fclose(fp);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_DumpFactoredForm - Factored form export", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    FILE *fp = create_temp_file();
    REQUIRE(fp != nullptr);
    
    SECTION("Export simple AND") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpFactoredForm(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("*") != std::string::npos || content.find("x") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export with variable names") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        const char *inames[] = {"A", "B"};
        const char *onames[] = {"OUT"};
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpFactoredForm(manager, 1, outputs, inames, onames, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("OUT") != std::string::npos);
        REQUIRE(content.find("A") != std::string::npos || content.find("B") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Export constant functions") {
        DdNode *one = Cudd_ReadOne(manager);
        DdNode *zero = Cudd_Not(one);
        
        DdNode *outputs[] = {one};
        int result = Cudd_DumpFactoredForm(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("CONST1") != std::string::npos);
        
        rewind(fp);
        outputs[0] = zero;
        result = Cudd_DumpFactoredForm(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        content = read_file(fp);
        REQUIRE(content.find("CONST0") != std::string::npos);
    }
    
    SECTION("Export complemented function") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *f = Cudd_Not(x);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpFactoredForm(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("!") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *outputs[] = {x};
        int result = Cudd_DumpFactoredForm(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(!content.empty());
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export without output name") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *outputs[] = {x};
        // n=0 means no output name printed
        int result = Cudd_DumpFactoredForm(manager, 0, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("=") == std::string::npos); // No equals sign when n=0
        
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Export complex expression") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        // (x & y) | (!x & z)
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *nxz = Cudd_bddAnd(manager, Cudd_Not(x), z);
        Cudd_Ref(nxz);
        DdNode *f = Cudd_bddOr(manager, xy, nxz);
        Cudd_Ref(f);
        
        DdNode *outputs[] = {f};
        int result = Cudd_DumpFactoredForm(manager, 1, outputs, nullptr, nullptr, fp);
        REQUIRE(result == 1);
        
        std::string content = read_file(fp);
        REQUIRE(content.find("+") != std::string::npos || content.find("*") != std::string::npos);
        
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, nxz);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    fclose(fp);
    Cudd_Quit(manager);
}

TEST_CASE("Cudd_FactoredFormString - String factored form", "[cuddExport]") {
    DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);
    
    SECTION("Get string for simple AND") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(f);
        
        char *str = Cudd_FactoredFormString(manager, f, nullptr);
        REQUIRE(str != nullptr);
        
        std::string result(str);
        REQUIRE(result.find("&") != std::string::npos || result.find("x") != std::string::npos);
        
        FREE(str);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Get string with variable names") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddOr(manager, x, y);
        Cudd_Ref(f);
        
        const char *inames[] = {"alpha", "beta"};
        
        char *str = Cudd_FactoredFormString(manager, f, inames);
        REQUIRE(str != nullptr);
        
        std::string result(str);
        REQUIRE(result.find("alpha") != std::string::npos || result.find("beta") != std::string::npos);
        
        FREE(str);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    SECTION("Get string for constant true") {
        DdNode *one = Cudd_ReadOne(manager);
        
        char *str = Cudd_FactoredFormString(manager, one, nullptr);
        REQUIRE(str != nullptr);
        
        std::string result(str);
        REQUIRE(result.find("true") != std::string::npos);
        
        FREE(str);
    }
    
    SECTION("Get string for constant false") {
        DdNode *zero = Cudd_Not(Cudd_ReadOne(manager));
        
        char *str = Cudd_FactoredFormString(manager, zero, nullptr);
        REQUIRE(str != nullptr);
        
        std::string result(str);
        REQUIRE(result.find("false") != std::string::npos);
        
        FREE(str);
    }
    
    SECTION("Get string for complemented variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        DdNode *f = Cudd_Not(x);
        
        char *str = Cudd_FactoredFormString(manager, f, nullptr);
        REQUIRE(str != nullptr);
        
        std::string result(str);
        REQUIRE(result.find("!") != std::string::npos);
        
        FREE(str);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Get string for single variable") {
        DdNode *x = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        
        char *str = Cudd_FactoredFormString(manager, x, nullptr);
        REQUIRE(str != nullptr);
        REQUIRE(strlen(str) > 0);
        
        FREE(str);
        Cudd_RecursiveDeref(manager, x);
    }
    
    SECTION("Get string for complex expression") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        DdNode *z = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        Cudd_Ref(z);
        
        DdNode *xy = Cudd_bddAnd(manager, x, y);
        Cudd_Ref(xy);
        DdNode *f = Cudd_bddOr(manager, xy, z);
        Cudd_Ref(f);
        
        char *str = Cudd_FactoredFormString(manager, f, nullptr);
        REQUIRE(str != nullptr);
        
        std::string result(str);
        REQUIRE((result.find("&") != std::string::npos || result.find("|") != std::string::npos));
        
        FREE(str);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, xy);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
        Cudd_RecursiveDeref(manager, z);
    }
    
    SECTION("Get string for XOR") {
        DdNode *x = Cudd_bddNewVar(manager);
        DdNode *y = Cudd_bddNewVar(manager);
        Cudd_Ref(x);
        Cudd_Ref(y);
        
        DdNode *f = Cudd_bddXor(manager, x, y);
        Cudd_Ref(f);
        
        char *str = Cudd_FactoredFormString(manager, f, nullptr);
        REQUIRE(str != nullptr);
        REQUIRE(strlen(str) > 0);
        
        FREE(str);
        Cudd_RecursiveDeref(manager, f);
        Cudd_RecursiveDeref(manager, x);
        Cudd_RecursiveDeref(manager, y);
    }
    
    Cudd_Quit(manager);
}
