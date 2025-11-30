#include <catch2/catch_test_macros.hpp>

#include <cstdio>

// Include CUDD headers
#include "cudd/cudd.h"
#include "mtr.h"
#include "mtrInt.h"

/**
 * @brief Test file for mtrBasic.c
 * 
 * This file contains comprehensive tests for the mtrBasic module
 * to achieve 90%+ code coverage.
 */

// =============================================================================
// Tests for Mtr_AllocNode
// =============================================================================

TEST_CASE("mtrBasic - Mtr_AllocNode", "[mtrBasic]") {
    SECTION("Allocate node initializes fields to zero") {
        MtrNode* node = Mtr_AllocNode();
        REQUIRE(node != nullptr);
        REQUIRE(node->flags == 0);
        REQUIRE(node->low == 0);
        REQUIRE(node->size == 0);
        REQUIRE(node->index == 0);
        Mtr_DeallocNode(node);
    }
}

// =============================================================================
// Tests for Mtr_DeallocNode
// =============================================================================

TEST_CASE("mtrBasic - Mtr_DeallocNode", "[mtrBasic]") {
    SECTION("Deallocate a valid node") {
        MtrNode* node = Mtr_AllocNode();
        REQUIRE(node != nullptr);
        Mtr_DeallocNode(node);
        // No crash means success - memory is freed
    }
}

// =============================================================================
// Tests for Mtr_InitTree
// =============================================================================

TEST_CASE("mtrBasic - Mtr_InitTree", "[mtrBasic]") {
    SECTION("Initialize tree creates root with NULL pointers") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        REQUIRE(root->parent == nullptr);
        REQUIRE(root->child == nullptr);
        REQUIRE(root->elder == nullptr);
        REQUIRE(root->younger == nullptr);
        REQUIRE(root->flags == 0);
        REQUIRE(root->low == 0);
        REQUIRE(root->size == 0);
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_FreeTree
// =============================================================================

TEST_CASE("mtrBasic - Mtr_FreeTree", "[mtrBasic]") {
    SECTION("Free NULL tree does nothing") {
        // Should not crash
        Mtr_FreeTree(nullptr);
    }

    SECTION("Free single node tree") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        Mtr_FreeTree(root);
        // No crash means success
    }

    SECTION("Free tree with child") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        
        MtrNode* child = Mtr_CreateFirstChild(root);
        REQUIRE(child != nullptr);
        
        Mtr_FreeTree(root);
        // No crash means success
    }

    SECTION("Free tree with multiple children") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        
        Mtr_FreeTree(root);
        // No crash means success
    }

    SECTION("Free tree with nested children") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        
        MtrNode* child = Mtr_CreateFirstChild(root);
        REQUIRE(child != nullptr);
        
        MtrNode* grandchild = Mtr_CreateFirstChild(child);
        REQUIRE(grandchild != nullptr);
        
        Mtr_FreeTree(root);
        // No crash means success
    }

    SECTION("Free tree with terminal node") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        
        MtrNode* terminal = Mtr_CreateFirstChild(root);
        REQUIRE(terminal != nullptr);
        MTR_SET(terminal, MTR_TERMINAL);
        
        Mtr_FreeTree(root);
        // No crash means success
    }

    SECTION("Free tree with younger sibling") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_AllocNode();
        REQUIRE(child2 != nullptr);
        child2->child = nullptr;
        Mtr_MakeNextSibling(child1, child2);
        
        Mtr_FreeTree(root);
        // No crash means success - younger siblings are freed
    }
}

// =============================================================================
// Tests for Mtr_CopyTree
// =============================================================================

TEST_CASE("mtrBasic - Mtr_CopyTree", "[mtrBasic]") {
    SECTION("Copy NULL tree returns NULL") {
        MtrNode* copy = Mtr_CopyTree(nullptr, 1);
        REQUIRE(copy == nullptr);
    }

    SECTION("Copy with expansion < 1 returns NULL") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        
        MtrNode* copy = Mtr_CopyTree(root, 0);
        REQUIRE(copy == nullptr);
        
        copy = Mtr_CopyTree(root, -1);
        REQUIRE(copy == nullptr);
        
        Mtr_FreeTree(root);
    }

    SECTION("Copy single node tree with expansion 1") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->flags = MTR_FIXED;
        root->low = 5;
        root->size = 10;
        root->index = 3;
        
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->flags == MTR_FIXED);
        REQUIRE(copy->low == 5);
        REQUIRE(copy->size == 10);
        REQUIRE(copy->index == 3);
        REQUIRE(copy->parent == nullptr);
        REQUIRE(copy->child == nullptr);
        REQUIRE(copy->elder == nullptr);
        REQUIRE(copy->younger == nullptr);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Copy single node tree with expansion > 1") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 5;
        root->size = 10;
        root->index = 3;
        
        MtrNode* copy = Mtr_CopyTree(root, 2);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->low == 10);   // 5 * 2
        REQUIRE(copy->size == 20);  // 10 * 2
        REQUIRE(copy->index == 6);  // 3 * 2
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Copy tree with child") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 10;
        
        MtrNode* child = Mtr_CreateFirstChild(root);
        REQUIRE(child != nullptr);
        child->low = 0;
        child->size = 5;
        
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->child != nullptr);
        REQUIRE(copy->child->parent == copy);
        REQUIRE(copy->child->low == 0);
        REQUIRE(copy->child->size == 5);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Copy tree with younger sibling") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 20;
        
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        child1->low = 0;
        child1->size = 5;
        
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        child2->low = 5;
        child2->size = 5;
        
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->child != nullptr);
        REQUIRE(copy->child->younger != nullptr);
        REQUIRE(copy->child->younger->elder == copy->child);
        REQUIRE(copy->child->younger->parent == copy);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Copy tree with nested children") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 20;
        
        MtrNode* child = Mtr_CreateFirstChild(root);
        REQUIRE(child != nullptr);
        child->low = 0;
        child->size = 10;
        
        MtrNode* grandchild = Mtr_CreateFirstChild(child);
        REQUIRE(grandchild != nullptr);
        grandchild->low = 0;
        grandchild->size = 5;
        
        MtrNode* copy = Mtr_CopyTree(root, 2);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->child != nullptr);
        REQUIRE(copy->child->child != nullptr);
        REQUIRE(copy->child->child->parent == copy->child);
        REQUIRE(copy->low == 0);
        REQUIRE(copy->size == 40);  // 20 * 2
        REQUIRE(copy->child->size == 20);  // 10 * 2
        REQUIRE(copy->child->child->size == 10);  // 5 * 2
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Copy tree with multiple siblings and nested children") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 30;
        
        // First child with grandchild
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        child1->low = 0;
        child1->size = 10;
        
        MtrNode* grandchild = Mtr_CreateFirstChild(child1);
        REQUIRE(grandchild != nullptr);
        grandchild->low = 0;
        grandchild->size = 5;
        
        // Second child (sibling of first)
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        child2->low = 10;
        child2->size = 10;
        
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->child != nullptr);
        REQUIRE(copy->child->child != nullptr);
        REQUIRE(copy->child->younger != nullptr);
        
        // Verify parent/child/sibling relationships
        REQUIRE(copy->child->parent == copy);
        REQUIRE(copy->child->child->parent == copy->child);
        REQUIRE(copy->child->younger->parent == copy);
        REQUIRE(copy->child->younger->elder == copy->child);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }
}

// =============================================================================
// Tests for Mtr_MakeFirstChild
// =============================================================================

TEST_CASE("mtrBasic - Mtr_MakeFirstChild", "[mtrBasic]") {
    SECTION("Make first child when parent has no children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* child = Mtr_AllocNode();
        REQUIRE(child != nullptr);
        child->child = nullptr;  // Must initialize before Mtr_FreeTree can be called
        
        Mtr_MakeFirstChild(parent, child);
        
        REQUIRE(parent->child == child);
        REQUIRE(child->parent == parent);
        REQUIRE(child->elder == nullptr);
        REQUIRE(child->younger == nullptr);
        
        Mtr_FreeTree(parent);
    }

    SECTION("Make first child when parent already has a child") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* existingChild = Mtr_AllocNode();
        REQUIRE(existingChild != nullptr);
        existingChild->child = nullptr;
        Mtr_MakeFirstChild(parent, existingChild);
        
        MtrNode* newChild = Mtr_AllocNode();
        REQUIRE(newChild != nullptr);
        newChild->child = nullptr;
        Mtr_MakeFirstChild(parent, newChild);
        
        REQUIRE(parent->child == newChild);
        REQUIRE(newChild->parent == parent);
        REQUIRE(newChild->elder == nullptr);
        REQUIRE(newChild->younger == existingChild);
        REQUIRE(existingChild->elder == newChild);
        
        Mtr_FreeTree(parent);
    }
}

// =============================================================================
// Tests for Mtr_MakeLastChild
// =============================================================================

TEST_CASE("mtrBasic - Mtr_MakeLastChild", "[mtrBasic]") {
    SECTION("Make last child when parent has no children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* child = Mtr_AllocNode();
        REQUIRE(child != nullptr);
        child->child = nullptr;  // Must initialize before Mtr_FreeTree can be called
        
        Mtr_MakeLastChild(parent, child);
        
        REQUIRE(parent->child == child);
        REQUIRE(child->parent == parent);
        REQUIRE(child->elder == nullptr);
        REQUIRE(child->younger == nullptr);
        
        Mtr_FreeTree(parent);
    }

    SECTION("Make last child when parent already has children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* firstChild = Mtr_AllocNode();
        REQUIRE(firstChild != nullptr);
        firstChild->child = nullptr;
        Mtr_MakeLastChild(parent, firstChild);
        
        MtrNode* lastChild = Mtr_AllocNode();
        REQUIRE(lastChild != nullptr);
        lastChild->child = nullptr;
        Mtr_MakeLastChild(parent, lastChild);
        
        REQUIRE(parent->child == firstChild);
        REQUIRE(firstChild->younger == lastChild);
        REQUIRE(lastChild->elder == firstChild);
        REQUIRE(lastChild->parent == parent);
        REQUIRE(lastChild->younger == nullptr);
        
        Mtr_FreeTree(parent);
    }

    SECTION("Make last child when parent has multiple children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* child1 = Mtr_AllocNode();
        REQUIRE(child1 != nullptr);
        child1->child = nullptr;
        Mtr_MakeLastChild(parent, child1);
        
        MtrNode* child2 = Mtr_AllocNode();
        REQUIRE(child2 != nullptr);
        child2->child = nullptr;
        Mtr_MakeLastChild(parent, child2);
        
        MtrNode* child3 = Mtr_AllocNode();
        REQUIRE(child3 != nullptr);
        child3->child = nullptr;
        Mtr_MakeLastChild(parent, child3);
        
        REQUIRE(parent->child == child1);
        REQUIRE(child1->younger == child2);
        REQUIRE(child2->younger == child3);
        REQUIRE(child3->elder == child2);
        REQUIRE(child2->elder == child1);
        
        Mtr_FreeTree(parent);
    }
}

// =============================================================================
// Tests for Mtr_CreateFirstChild
// =============================================================================

TEST_CASE("mtrBasic - Mtr_CreateFirstChild", "[mtrBasic]") {
    SECTION("Create first child when parent has no children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* child = Mtr_CreateFirstChild(parent);
        REQUIRE(child != nullptr);
        REQUIRE(parent->child == child);
        REQUIRE(child->parent == parent);
        REQUIRE(child->child == nullptr);
        
        Mtr_FreeTree(parent);
    }

    SECTION("Create first child when parent has existing children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* existingChild = Mtr_CreateLastChild(parent);
        REQUIRE(existingChild != nullptr);
        
        MtrNode* newChild = Mtr_CreateFirstChild(parent);
        REQUIRE(newChild != nullptr);
        REQUIRE(parent->child == newChild);
        REQUIRE(newChild->younger == existingChild);
        REQUIRE(existingChild->elder == newChild);
        
        Mtr_FreeTree(parent);
    }
}

// =============================================================================
// Tests for Mtr_CreateLastChild
// =============================================================================

TEST_CASE("mtrBasic - Mtr_CreateLastChild", "[mtrBasic]") {
    SECTION("Create last child when parent has no children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* child = Mtr_CreateLastChild(parent);
        REQUIRE(child != nullptr);
        REQUIRE(parent->child == child);
        REQUIRE(child->parent == parent);
        REQUIRE(child->child == nullptr);
        
        Mtr_FreeTree(parent);
    }

    SECTION("Create last child when parent has existing children") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* firstChild = Mtr_CreateFirstChild(parent);
        REQUIRE(firstChild != nullptr);
        
        MtrNode* lastChild = Mtr_CreateLastChild(parent);
        REQUIRE(lastChild != nullptr);
        REQUIRE(firstChild->younger == lastChild);
        REQUIRE(lastChild->elder == firstChild);
        
        Mtr_FreeTree(parent);
    }
}

// =============================================================================
// Tests for Mtr_MakeNextSibling
// =============================================================================

TEST_CASE("mtrBasic - Mtr_MakeNextSibling", "[mtrBasic]") {
    SECTION("Make next sibling when first has no younger sibling") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* first = Mtr_CreateFirstChild(parent);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_AllocNode();
        REQUIRE(second != nullptr);
        second->child = nullptr;
        
        Mtr_MakeNextSibling(first, second);
        
        REQUIRE(second->parent == parent);
        REQUIRE(second->elder == first);
        REQUIRE(second->younger == nullptr);
        REQUIRE(first->younger == second);
        
        Mtr_FreeTree(parent);
    }

    SECTION("Make next sibling when first has younger sibling") {
        MtrNode* parent = Mtr_InitTree();
        REQUIRE(parent != nullptr);
        
        MtrNode* first = Mtr_CreateFirstChild(parent);
        REQUIRE(first != nullptr);
        
        MtrNode* third = Mtr_CreateLastChild(parent);
        REQUIRE(third != nullptr);
        
        MtrNode* second = Mtr_AllocNode();
        REQUIRE(second != nullptr);
        second->child = nullptr;
        
        Mtr_MakeNextSibling(first, second);
        
        REQUIRE(second->parent == parent);
        REQUIRE(second->elder == first);
        REQUIRE(second->younger == third);
        REQUIRE(third->elder == second);
        REQUIRE(first->younger == second);
        
        Mtr_FreeTree(parent);
    }
}

// =============================================================================
// Tests for Mtr_PrintTree
// =============================================================================

TEST_CASE("mtrBasic - Mtr_PrintTree", "[mtrBasic]") {
    SECTION("Print NULL tree does nothing") {
        // Should not crash
        Mtr_PrintTree(nullptr);
    }

    SECTION("Print single node tree") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->flags = MTR_DEFAULT;
        root->low = 0;
        root->size = 10;
        
        Mtr_PrintTree(root);
        
        Mtr_FreeTree(root);
    }

    SECTION("Print tree with child") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 20;
        
        MtrNode* child = Mtr_CreateFirstChild(root);
        REQUIRE(child != nullptr);
        child->low = 0;
        child->size = 10;
        
        Mtr_PrintTree(root);
        
        Mtr_FreeTree(root);
    }

    SECTION("Print tree with terminal node") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 20;
        
        MtrNode* terminal = Mtr_CreateFirstChild(root);
        REQUIRE(terminal != nullptr);
        MTR_SET(terminal, MTR_TERMINAL);
        terminal->low = 0;
        terminal->size = 10;
        
        // Terminal nodes should not recurse into children
        Mtr_PrintTree(root);
        
        Mtr_FreeTree(root);
    }

    SECTION("Print tree with siblings") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 30;
        
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        child1->low = 0;
        child1->size = 10;
        
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        child2->low = 10;
        child2->size = 10;
        
        Mtr_PrintTree(root);
        
        Mtr_FreeTree(root);
    }

    SECTION("Print tree with nested children and siblings") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 30;
        
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        child1->low = 0;
        child1->size = 15;
        
        MtrNode* grandchild = Mtr_CreateFirstChild(child1);
        REQUIRE(grandchild != nullptr);
        grandchild->low = 0;
        grandchild->size = 5;
        
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        child2->low = 15;
        child2->size = 10;
        
        Mtr_PrintTree(root);
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Additional edge case tests for comprehensive coverage
// =============================================================================

TEST_CASE("mtrBasic - Edge cases", "[mtrBasic]") {
    SECTION("Complex tree structure with all operations") {
        // Create a complex tree structure
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 100;
        root->flags = MTR_DEFAULT;
        
        // Add multiple children
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        child1->low = 0;
        child1->size = 30;
        
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        child2->low = 30;
        child2->size = 30;
        
        // Insert between using MakeNextSibling
        MtrNode* child1_5 = Mtr_AllocNode();
        REQUIRE(child1_5 != nullptr);
        child1_5->child = nullptr;
        child1_5->low = 15;
        child1_5->size = 15;
        Mtr_MakeNextSibling(child1, child1_5);
        
        // Add grandchildren
        MtrNode* grandchild1 = Mtr_CreateFirstChild(child1);
        REQUIRE(grandchild1 != nullptr);
        grandchild1->low = 0;
        grandchild1->size = 10;
        
        MtrNode* grandchild2 = Mtr_CreateLastChild(child1);
        REQUIRE(grandchild2 != nullptr);
        grandchild2->low = 10;
        grandchild2->size = 10;
        
        // Copy the tree
        MtrNode* copy = Mtr_CopyTree(root, 2);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->size == 200);  // 100 * 2
        REQUIRE(copy->child != nullptr);
        REQUIRE(copy->child->size == 60);  // 30 * 2
        
        // Print both trees
        Mtr_PrintTree(root);
        Mtr_PrintTree(copy);
        
        // Free both trees
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Tree with terminal nodes at various levels") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 50;
        
        MtrNode* child1 = Mtr_CreateFirstChild(root);
        REQUIRE(child1 != nullptr);
        child1->low = 0;
        child1->size = 20;
        
        // Make child1 terminal
        MTR_SET(child1, MTR_TERMINAL);
        
        MtrNode* child2 = Mtr_CreateLastChild(root);
        REQUIRE(child2 != nullptr);
        child2->low = 20;
        child2->size = 20;
        
        MtrNode* grandchild = Mtr_CreateFirstChild(child2);
        REQUIRE(grandchild != nullptr);
        grandchild->low = 20;
        grandchild->size = 10;
        
        // Make grandchild terminal
        MTR_SET(grandchild, MTR_TERMINAL);
        
        // Copy should work with terminal nodes
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        REQUIRE(MTR_TEST(copy->child, MTR_TERMINAL));
        REQUIRE(copy->child->younger != nullptr);
        REQUIRE(copy->child->younger->child != nullptr);
        REQUIRE(MTR_TEST(copy->child->younger->child, MTR_TERMINAL));
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Various flag combinations") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 40;
        
        MtrNode* fixed = Mtr_CreateFirstChild(root);
        REQUIRE(fixed != nullptr);
        MTR_SET(fixed, MTR_FIXED);
        
        MtrNode* soft = Mtr_CreateLastChild(root);
        REQUIRE(soft != nullptr);
        MTR_SET(soft, MTR_SOFT);
        
        MtrNode* combined = Mtr_AllocNode();
        REQUIRE(combined != nullptr);
        combined->child = nullptr;
        MTR_SET(combined, MTR_FIXED | MTR_SOFT | MTR_NEWNODE);
        Mtr_MakeNextSibling(fixed, combined);
        
        // Copy preserves flags
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        REQUIRE(MTR_TEST(copy->child, MTR_FIXED));
        REQUIRE(copy->child->younger != nullptr);
        REQUIRE(MTR_TEST(copy->child->younger, MTR_FIXED | MTR_SOFT | MTR_NEWNODE));
        
        Mtr_PrintTree(root);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Deep nesting test") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 100;
        
        MtrNode* current = root;
        for (int i = 0; i < 5; i++) {
            MtrNode* child = Mtr_CreateFirstChild(current);
            REQUIRE(child != nullptr);
            child->low = 0;
            child->size = 100 - i * 10;
            current = child;
        }
        
        // Copy deeply nested tree
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        
        // Verify depth
        MtrNode* copyNode = copy;
        int depth = 0;
        while (copyNode->child != nullptr) {
            copyNode = copyNode->child;
            depth++;
        }
        REQUIRE(depth == 5);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Wide tree with many siblings") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 0;
        root->size = 100;
        
        // Create 10 sibling children
        for (int i = 0; i < 10; i++) {
            MtrNode* child = Mtr_CreateLastChild(root);
            REQUIRE(child != nullptr);
            child->low = static_cast<MtrHalfWord>(i * 10);
            child->size = 10;
        }
        
        // Copy wide tree
        MtrNode* copy = Mtr_CopyTree(root, 1);
        REQUIRE(copy != nullptr);
        
        // Verify sibling count
        int siblingCount = 0;
        MtrNode* sibling = copy->child;
        while (sibling != nullptr) {
            siblingCount++;
            REQUIRE(sibling->parent == copy);
            sibling = sibling->younger;
        }
        REQUIRE(siblingCount == 10);
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }

    SECTION("Copy tree expansion preserves structure") {
        MtrNode* root = Mtr_InitTree();
        REQUIRE(root != nullptr);
        root->low = 1;
        root->size = 10;
        root->index = 2;
        
        MtrNode* child = Mtr_CreateFirstChild(root);
        REQUIRE(child != nullptr);
        child->low = 3;
        child->size = 4;
        child->index = 5;
        
        MtrNode* sibling = Mtr_CreateLastChild(root);
        REQUIRE(sibling != nullptr);
        sibling->low = 7;
        sibling->size = 3;
        sibling->index = 8;
        
        MtrNode* copy = Mtr_CopyTree(root, 3);
        REQUIRE(copy != nullptr);
        REQUIRE(copy->low == 3);    // 1 * 3
        REQUIRE(copy->size == 30);  // 10 * 3
        REQUIRE(copy->index == 6);  // 2 * 3
        REQUIRE(copy->child->low == 9);    // 3 * 3
        REQUIRE(copy->child->size == 12);  // 4 * 3
        REQUIRE(copy->child->index == 15); // 5 * 3
        REQUIRE(copy->child->younger->low == 21);    // 7 * 3
        REQUIRE(copy->child->younger->size == 9);    // 3 * 3
        REQUIRE(copy->child->younger->index == 24);  // 8 * 3
        
        Mtr_FreeTree(root);
        Mtr_FreeTree(copy);
    }
}
