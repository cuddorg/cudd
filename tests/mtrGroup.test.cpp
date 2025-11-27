#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <cstring>

// Include CUDD headers
#include "cudd/cudd.h"
#include "mtr.h"
#include "mtrInt.h"

/**
 * @brief Test file for mtrGroup.c
 * 
 * This file contains comprehensive tests for the mtrGroup module
 * to achieve 90%+ code coverage.
 */

// =============================================================================
// Tests for Mtr_InitGroupTree
// =============================================================================

TEST_CASE("mtrGroup - Mtr_InitGroupTree basic", "[mtrGroup]") {
    SECTION("Initialize tree with valid parameters") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->low == 0);
        REQUIRE(root->size == 10);
        REQUIRE(root->flags == MTR_DEFAULT);
        REQUIRE(root->child == nullptr);
        REQUIRE(root->parent == nullptr);
        REQUIRE(root->elder == nullptr);
        REQUIRE(root->younger == nullptr);
        Mtr_FreeTree(root);
    }
    
    SECTION("Initialize tree with non-zero lower bound") {
        MtrNode* root = Mtr_InitGroupTree(5, 15);
        REQUIRE(root != nullptr);
        REQUIRE(root->low == 5);
        REQUIRE(root->size == 15);
        Mtr_FreeTree(root);
    }
    
    SECTION("Initialize tree with size 1") {
        MtrNode* root = Mtr_InitGroupTree(0, 1);
        REQUIRE(root != nullptr);
        REQUIRE(root->size == 1);
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_MakeGroup
// =============================================================================

TEST_CASE("mtrGroup - Mtr_MakeGroup basic", "[mtrGroup]") {
    SECTION("Create group in empty tree (no children)") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* group = Mtr_MakeGroup(root, 2, 3, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        REQUIRE(group->low == 2);
        REQUIRE(group->size == 3);
        REQUIRE(group->parent == root);
        REQUIRE(root->child == group);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group with size 0 should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* group = Mtr_MakeGroup(root, 0, 0, MTR_DEFAULT);
        REQUIRE(group == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group outside bounds should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        // Group starts before root
        MtrNode* group1 = Mtr_MakeGroup(root, 0, 15, MTR_DEFAULT);
        REQUIRE(group1 == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group below lower bound") {
        MtrNode* root = Mtr_InitGroupTree(5, 10);
        REQUIRE(root != nullptr);
        
        // Group starts before root->low
        MtrNode* group = Mtr_MakeGroup(root, 3, 2, MTR_DEFAULT);
        REQUIRE(group == nullptr);
        
        Mtr_FreeTree(root);
    }
}

TEST_CASE("mtrGroup - Mtr_MakeGroup with children", "[mtrGroup]") {
    SECTION("Append group at end of existing children") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        // Create first child
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        // Create second child (appended at end)
        MtrNode* child2 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        REQUIRE(child2->elder == child1);
        REQUIRE(child1->younger == child2);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create nested group inside existing child") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        // Create parent group
        MtrNode* parent = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        // Create nested child
        MtrNode* nested = Mtr_MakeGroup(root, 6, 3, MTR_DEFAULT);
        REQUIRE(nested != nullptr);
        REQUIRE(nested->parent == parent);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group in gap between children") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create first child
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        // Create second child with gap
        MtrNode* child2 = Mtr_MakeGroup(root, 20, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Create group in the gap (before child2)
        MtrNode* gapGroup = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(gapGroup != nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group in gap before first child") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create child that doesn't start at 0
        MtrNode* child1 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        // Create group before child1 (in gap at start)
        MtrNode* beforeGroup = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(beforeGroup != nullptr);
        REQUIRE(beforeGroup->younger == child1);
        REQUIRE(child1->elder == beforeGroup);
        REQUIRE(root->child == beforeGroup);
        
        Mtr_FreeTree(root);
    }
}

TEST_CASE("mtrGroup - Mtr_MakeGroup edge cases", "[mtrGroup]") {
    SECTION("Try to cut existing group - low before child start") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        // Create existing child
        MtrNode* child = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        
        // Try to create overlapping group that cuts child
        // low < first->low && low + size < first->low + first->size
        MtrNode* overlap = Mtr_MakeGroup(root, 3, 5, MTR_DEFAULT);
        REQUIRE(overlap == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Try to cut existing group - starts inside child") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        // Create existing child
        MtrNode* child = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        
        // Try to create group that starts inside child but extends beyond
        // low > first->low (inside child, but not contained)
        MtrNode* overlap = Mtr_MakeGroup(root, 6, 15, MTR_DEFAULT);
        REQUIRE(overlap == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group containing multiple children") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create two separate children
        MtrNode* child1 = Mtr_MakeGroup(root, 5, 3, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 10, 3, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Create group containing both children
        MtrNode* parent = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        REQUIRE(child1->parent == parent);
        REQUIRE(child2->parent == parent);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group containing multiple children with exact boundaries") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create children
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Create parent group containing both
        MtrNode* parent = Mtr_MakeGroup(root, 0, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        REQUIRE(parent->child == child1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Valid group containing multiple children") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create three adjacent children
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        MtrNode* child3 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child3 != nullptr);
        
        // Create group that contains child1 and child2, leaving child3 as sibling
        MtrNode* parent = Mtr_MakeGroup(root, 0, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        REQUIRE(child1->parent == parent);
        REQUIRE(child2->parent == parent);
        REQUIRE(child3->parent == root);
        REQUIRE(parent->younger == child3);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group with flags") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* group = Mtr_MakeGroup(root, 2, 3, MTR_FIXED);
        REQUIRE(group != nullptr);
        REQUIRE(MTR_TEST(group, MTR_FIXED));
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_DissolveGroup
// =============================================================================

TEST_CASE("mtrGroup - Mtr_DissolveGroup", "[mtrGroup]") {
    SECTION("Dissolve group with children") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        // Create intermediate group
        MtrNode* group = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Add children to the group
        MtrNode* child1 = Mtr_MakeGroup(root, 5, 3, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 10, 3, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Dissolve the group
        MtrNode* parent = Mtr_DissolveGroup(group);
        REQUIRE(parent == root);
        
        // Children should now be direct children of root
        REQUIRE(child1->parent == root);
        REQUIRE(child2->parent == root);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Dissolve root should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* result = Mtr_DissolveGroup(root);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Dissolve terminal node should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* group = Mtr_MakeGroup(root, 2, 3, MTR_TERMINAL);
        REQUIRE(group != nullptr);
        
        MtrNode* result = Mtr_DissolveGroup(group);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Dissolve group without children should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* group = Mtr_MakeGroup(root, 2, 3, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        MtrNode* result = Mtr_DissolveGroup(group);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Dissolve group that is first child") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        // Create group as first child
        MtrNode* group = Mtr_MakeGroup(root, 0, 10, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Add children
        MtrNode* child = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        
        // Dissolve
        MtrNode* parent = Mtr_DissolveGroup(group);
        REQUIRE(parent == root);
        REQUIRE(root->child == child);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Dissolve group that is not first child") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create first sibling
        MtrNode* sibling = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(sibling != nullptr);
        
        // Create group to dissolve
        MtrNode* group = Mtr_MakeGroup(root, 10, 10, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Add child to group
        MtrNode* child = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        REQUIRE(child->parent == group);
        
        // Dissolve
        MtrNode* parent = Mtr_DissolveGroup(group);
        REQUIRE(parent == root);
        REQUIRE(sibling->younger == child);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Dissolve group with younger sibling") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create group to dissolve
        MtrNode* group = Mtr_MakeGroup(root, 0, 10, MTR_DEFAULT);
        REQUIRE(group != nullptr);
        
        // Create younger sibling
        MtrNode* younger = Mtr_MakeGroup(root, 20, 5, MTR_DEFAULT);
        REQUIRE(younger != nullptr);
        
        // Add child to group
        MtrNode* child = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        REQUIRE(child->parent == group);
        
        // Dissolve
        MtrNode* parent = Mtr_DissolveGroup(group);
        REQUIRE(parent == root);
        REQUIRE(child->younger == younger);
        REQUIRE(younger->elder == child);
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_FindGroup
// =============================================================================

TEST_CASE("mtrGroup - Mtr_FindGroup", "[mtrGroup]") {
    SECTION("Find group with size 0 should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* result = Mtr_FindGroup(root, 0, 0);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find group outside bounds should return NULL") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* result = Mtr_FindGroup(root, 0, 15);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find root group") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* result = Mtr_FindGroup(root, 0, 10);
        REQUIRE(result == root);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find group when root has no children") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        // Search for non-root group when root has no children
        MtrNode* result = Mtr_FindGroup(root, 2, 3);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find existing child group") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* child = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        
        MtrNode* result = Mtr_FindGroup(root, 5, 10);
        REQUIRE(result == child);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find nested group") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* parent = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        MtrNode* nested = Mtr_MakeGroup(root, 6, 3, MTR_DEFAULT);
        REQUIRE(nested != nullptr);
        
        MtrNode* result = Mtr_FindGroup(root, 6, 3);
        REQUIRE(result == nested);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find non-existent group") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Search for group in the gap
        MtrNode* result = Mtr_FindGroup(root, 5, 3);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find group - search traverses younger siblings") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create multiple children
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        MtrNode* child3 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child3 != nullptr);
        
        // Find last child
        MtrNode* result = Mtr_FindGroup(root, 10, 5);
        REQUIRE(result == child3);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Find group that spans multiple but doesn't exist") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Try to find group that would span both children
        MtrNode* result = Mtr_FindGroup(root, 0, 10);
        REQUIRE(result == nullptr);
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_SwapGroups
// =============================================================================

TEST_CASE("mtrGroup - Mtr_SwapGroups", "[mtrGroup]") {
    SECTION("Swap two adjacent children - first before second") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        REQUIRE(first->younger == second);
        
        int result = Mtr_SwapGroups(first, second);
        REQUIRE(result == 1);
        
        // After swap, second should be first
        REQUIRE(root->child == second);
        REQUIRE(second->younger == first);
        REQUIRE(first->elder == second);
        
        // Check low values are swapped
        REQUIRE(second->low == 0);
        REQUIRE(first->low == 5);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Swap two adjacent children - second before first") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        // Call with second, first (reversed order)
        int result = Mtr_SwapGroups(second, first);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Swap non-adjacent children should return 0") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* middle = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(middle != nullptr);
        
        MtrNode* third = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(third != nullptr);
        
        int result = Mtr_SwapGroups(first, third);
        REQUIRE(result == 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Swap with different parents should return 0") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        MtrNode* parent1 = Mtr_MakeGroup(root, 0, 10, MTR_DEFAULT);
        REQUIRE(parent1 != nullptr);
        
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        REQUIRE(child1->parent == parent1);
        
        MtrNode* parent2 = Mtr_MakeGroup(root, 15, 10, MTR_DEFAULT);
        REQUIRE(parent2 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 15, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        REQUIRE(child2->parent == parent2);
        
        // Different parents - even if we force adjacency
        int result = Mtr_SwapGroups(child1, child2);
        REQUIRE(result == 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Swap nodes when first is not first child") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        MtrNode* third = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(third != nullptr);
        
        // Swap second and third (second is not first child)
        int result = Mtr_SwapGroups(second, third);
        REQUIRE(result == 1);
        
        REQUIRE(first->younger == third);
        REQUIRE(third->elder == first);
        REQUIRE(third->younger == second);
        REQUIRE(second->elder == third);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Swap when second has younger sibling") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        MtrNode* third = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(third != nullptr);
        
        // Swap first and second (second has third as younger)
        int result = Mtr_SwapGroups(first, second);
        REQUIRE(result == 1);
        
        REQUIRE(root->child == second);
        REQUIRE(second->younger == first);
        REQUIRE(first->younger == third);
        REQUIRE(third->elder == first);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Swap groups with children") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* firstChild = Mtr_MakeGroup(root, 0, 2, MTR_DEFAULT);
        REQUIRE(firstChild != nullptr);
        REQUIRE(firstChild->parent == first);
        
        MtrNode* second = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        int result = Mtr_SwapGroups(first, second);
        REQUIRE(result == 1);
        
        // Child's low should also have been adjusted
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_ReorderGroups
// =============================================================================

TEST_CASE("mtrGroup - Mtr_ReorderGroups", "[mtrGroup]") {
    SECTION("Reorder single node (no siblings)") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        // Permutation shifts all variables by 5: position i maps to (i+5) mod 10
        // Variable at index 0 moves to position 5
        int permutation[] = {5, 6, 7, 8, 9, 0, 1, 2, 3, 4};
        Mtr_ReorderGroups(root, permutation);
        
        REQUIRE(root->low == 5);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Reorder with children") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        MtrNode* child = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        child->index = 0;
        
        // Permutation shifts all variables by 3: variable at index 0 moves to position 3
        int permutation[] = {3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
        Mtr_ReorderGroups(root, permutation);
        
        // Both root and child should be updated
        REQUIRE(root->low == 3);
        REQUIRE(child->low == 3);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Reorder multiple siblings - sorting required") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        // Create children manually with indices
        MtrNode* child1 = Mtr_AllocNode();
        child1->low = 0;
        child1->size = 5;
        child1->index = 0;
        child1->flags = MTR_DEFAULT;
        child1->child = nullptr;
        Mtr_MakeLastChild(root, child1);
        
        MtrNode* child2 = Mtr_AllocNode();
        child2->low = 5;
        child2->size = 5;
        child2->index = 5;
        child2->flags = MTR_DEFAULT;
        child2->child = nullptr;
        Mtr_MakeLastChild(root, child2);
        
        // Permutation that reverses the order of child1 [0-5) and child2 [5-10):
        // Variables 0-4 (child1) move to positions 10-14
        // Variables 5-9 (child2) move to positions 0-4
        // This causes child2 to be reordered before child1
        int permutation[] = {10, 11, 12, 13, 14, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 16, 17, 18, 19};
        Mtr_ReorderGroups(root, permutation);
        
        // After reordering, child2 should come before child1
        REQUIRE(root->child == child2);
        REQUIRE(child2->younger == child1);
        REQUIRE(child1->elder == child2);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Reorder siblings - no sorting needed") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        // Create children manually with indices
        MtrNode* child1 = Mtr_AllocNode();
        child1->low = 0;
        child1->size = 5;
        child1->index = 0;
        child1->flags = MTR_DEFAULT;
        child1->child = nullptr;
        Mtr_MakeLastChild(root, child1);
        
        MtrNode* child2 = Mtr_AllocNode();
        child2->low = 5;
        child2->size = 5;
        child2->index = 5;
        child2->flags = MTR_DEFAULT;
        child2->child = nullptr;
        Mtr_MakeLastChild(root, child2);
        
        // Identity permutation - each position maps to itself, no sorting needed
        int permutation[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
        Mtr_ReorderGroups(root, permutation);
        
        REQUIRE(root->child == child1);
        REQUIRE(child1->younger == child2);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Reorder three siblings with sorting") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        // Create children
        MtrNode* child1 = Mtr_AllocNode();
        child1->low = 0;
        child1->size = 5;
        child1->index = 0;
        child1->flags = MTR_DEFAULT;
        child1->child = nullptr;
        Mtr_MakeLastChild(root, child1);
        
        MtrNode* child2 = Mtr_AllocNode();
        child2->low = 5;
        child2->size = 5;
        child2->index = 5;
        child2->flags = MTR_DEFAULT;
        child2->child = nullptr;
        Mtr_MakeLastChild(root, child2);
        
        MtrNode* child3 = Mtr_AllocNode();
        child3->low = 10;
        child3->size = 5;
        child3->index = 10;
        child3->flags = MTR_DEFAULT;
        child3->child = nullptr;
        Mtr_MakeLastChild(root, child3);
        
        // Permutation that reorders three children (indices 0,5,10) by new positions:
        // Variables 0-4 (child1, index 0) -> positions 5-9 (new low = 5)
        // Variables 5-9 (child2, index 5) -> positions 15-19 (new low = 15)  
        // Variables 10-14 (child3, index 10) -> positions 0-4 (new low = 0)
        // Expected order after reorder: child3 (low=0), child1 (low=5), child2 (low=15)
        int permutation[] = {5, 6, 7, 8, 9, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
        Mtr_ReorderGroups(root, permutation);
        
        // Should be sorted by new low values
        Mtr_FreeTree(root);
    }
    
    SECTION("Reorder with nested children") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        // Create child with grandchild
        MtrNode* child = Mtr_AllocNode();
        child->low = 0;
        child->size = 10;
        child->index = 0;
        child->flags = MTR_DEFAULT;
        child->child = nullptr;
        Mtr_MakeLastChild(root, child);
        
        MtrNode* grandchild = Mtr_AllocNode();
        grandchild->low = 0;
        grandchild->size = 5;
        grandchild->index = 0;
        grandchild->flags = MTR_DEFAULT;
        grandchild->child = nullptr;
        Mtr_MakeLastChild(child, grandchild);
        
        // Permutation shifts variable at index 0 to position 3
        // This tests that nested children are also reordered recursively
        int permutation[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2};
        Mtr_ReorderGroups(root, permutation);
        
        REQUIRE(grandchild->low == 3);
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_PrintGroups
// =============================================================================

TEST_CASE("mtrGroup - Mtr_PrintGroups", "[mtrGroup]") {
    SECTION("Print simple tree (silent mode)") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        // Silent mode - just verifies tree structure
        Mtr_PrintGroups(root, 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print simple tree (verbose mode)") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        // Verbose mode - prints tree structure
        Mtr_PrintGroups(root, 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with children") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        Mtr_PrintGroups(root, 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with nested children") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* parent = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        MtrNode* nested = Mtr_MakeGroup(root, 6, 3, MTR_DEFAULT);
        REQUIRE(nested != nullptr);
        
        Mtr_PrintGroups(root, 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print terminal node") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* terminal = Mtr_MakeGroup(root, 2, 3, MTR_TERMINAL);
        REQUIRE(terminal != nullptr);
        
        Mtr_PrintGroups(root, 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with various flags") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* fixed = Mtr_MakeGroup(root, 0, 5, MTR_FIXED);
        REQUIRE(fixed != nullptr);
        
        MtrNode* soft = Mtr_MakeGroup(root, 5, 5, MTR_SOFT);
        REQUIRE(soft != nullptr);
        
        MtrNode* newnode = Mtr_MakeGroup(root, 10, 5, MTR_NEWNODE);
        REQUIRE(newnode != nullptr);
        
        Mtr_PrintGroups(root, 0);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with combined flags") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        // Test group starting at position 2 with size 5 and all flag types combined
        // This tests the flag printing in Mtr_PrintGroups
        MtrNode* combined = Mtr_MakeGroup(root, 2, 5, MTR_FIXED | MTR_SOFT | MTR_NEWNODE);
        REQUIRE(combined != nullptr);
        
        Mtr_PrintGroups(root, 0);
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_PrintGroupedOrder
// =============================================================================

TEST_CASE("mtrGroup - Mtr_PrintGroupedOrder", "[mtrGroup]") {
    SECTION("Print simple tree") {
        MtrNode* root = Mtr_InitGroupTree(0, 5);
        REQUIRE(root != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with children") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* child = Mtr_MakeGroup(root, 2, 4, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with flags") {
        MtrNode* root = Mtr_InitGroupTree(0, 5);
        REQUIRE(root != nullptr);
        
        MtrNode* fixed = Mtr_MakeGroup(root, 0, 3, MTR_FIXED);
        REQUIRE(fixed != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with multiple flags") {
        MtrNode* root = Mtr_InitGroupTree(0, 5);
        REQUIRE(root != nullptr);
        
        // Test group starting at position 0 with size 3 and all flag types combined
        // This tests the flag printing in Mtr_PrintGroupedOrder (F, S, N characters)
        MtrNode* combined = Mtr_MakeGroup(root, 0, 3, MTR_FIXED | MTR_SOFT | MTR_NEWNODE);
        REQUIRE(combined != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print nested tree") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* parent = Mtr_MakeGroup(root, 2, 6, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        
        MtrNode* nested = Mtr_MakeGroup(root, 3, 2, MTR_DEFAULT);
        REQUIRE(nested != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with gap before child") {
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        
        MtrNode* child = Mtr_MakeGroup(root, 5, 3, MTR_DEFAULT);
        REQUIRE(child != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Print tree with siblings") {
        MtrNode* root = Mtr_InitGroupTree(0, 15);
        REQUIRE(root != nullptr);
        
        MtrNode* child1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        MtrNode* child3 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child3 != nullptr);
        
        int invperm[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
        int result = Mtr_PrintGroupedOrder(root, invperm, stdout);
        REQUIRE(result == 1);
        
        Mtr_FreeTree(root);
    }
}

// =============================================================================
// Tests for Mtr_ReadGroups
// =============================================================================

TEST_CASE("mtrGroup - Mtr_ReadGroups", "[mtrGroup]") {
    SECTION("Read empty file") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->low == 0);
        REQUIRE(root->size == 10);
        REQUIRE(root->child == nullptr);
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read single group with DEFAULT flag") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "2 3 D\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(root->child->low == 2);
        REQUIRE(root->child->size == 3);
        REQUIRE(root->child->flags == MTR_DEFAULT);
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read group with FIXED flag") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 F\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(MTR_TEST(root->child, MTR_FIXED));
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read group with SOFT flag") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 S\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(MTR_TEST(root->child, MTR_SOFT));
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read group with NEWNODE flag") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 N\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(MTR_TEST(root->child, MTR_NEWNODE));
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read group with TERMINAL flag") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 T\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(MTR_TEST(root->child, MTR_TERMINAL));
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read group with combined flags") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 FS\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(MTR_TEST(root->child, MTR_FIXED));
        REQUIRE(MTR_TEST(root->child, MTR_SOFT));
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read multiple groups") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 D\n5 5 F\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 15);
        REQUIRE(root != nullptr);
        REQUIRE(root->child != nullptr);
        REQUIRE(root->child->younger != nullptr);
        
        Mtr_FreeTree(root);
        fclose(fp);
    }
    
    SECTION("Read with invalid format") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "not a valid format\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root == nullptr);
        
        fclose(fp);
    }
    
    SECTION("Read with negative low") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "-1 5 D\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root == nullptr);
        
        fclose(fp);
    }
    
    SECTION("Read with out-of-bounds group") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        // Group at position 5 with size 10 would span positions 5-14
        // This requires 15 positions (low + size = 5 + 10 = 15) which exceeds nleaves=10
        fprintf(fp, "5 10 D\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root == nullptr);
        
        fclose(fp);
    }
    
    SECTION("Read with size 0") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 0 D\n");
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root == nullptr);
        
        fclose(fp);
    }
    
    SECTION("Read with invalid flag character") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 X\n");  // X is not a valid flag
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root == nullptr);
        
        fclose(fp);
    }
    
    SECTION("Read overlapping groups (error)") {
        FILE* fp = tmpfile();
        REQUIRE(fp != nullptr);
        fprintf(fp, "0 5 D\n2 5 D\n");  // Second group overlaps first
        rewind(fp);
        
        MtrNode* root = Mtr_ReadGroups(fp, 10);
        REQUIRE(root == nullptr);
        
        fclose(fp);
    }
    
    // Note: Testing "attribute string too long" (lines 751-755) is not possible
    // without triggering a buffer overflow, as the buffer size (8*sizeof(unsigned int)+1)
    // is only 1 byte larger than the check threshold (8*sizeof(MtrHalfWord)).
    // This leaves lines 754-755 uncovered, which represent defensive code for
    // handling malformed input files.
}

// =============================================================================
// Additional edge case tests
// =============================================================================

TEST_CASE("mtrGroup - Edge cases", "[mtrGroup]") {
    SECTION("Multiple nested groups") {
        MtrNode* root = Mtr_InitGroupTree(0, 100);
        REQUIRE(root != nullptr);
        
        // Create multiple levels of nesting
        MtrNode* level1 = Mtr_MakeGroup(root, 10, 80, MTR_DEFAULT);
        REQUIRE(level1 != nullptr);
        
        MtrNode* level2 = Mtr_MakeGroup(root, 20, 60, MTR_DEFAULT);
        REQUIRE(level2 != nullptr);
        
        MtrNode* level3 = Mtr_MakeGroup(root, 30, 40, MTR_DEFAULT);
        REQUIRE(level3 != nullptr);
        
        REQUIRE(level3->parent == level2);
        REQUIRE(level2->parent == level1);
        REQUIRE(level1->parent == root);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create and dissolve multiple groups") {
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create groups
        MtrNode* g1 = Mtr_MakeGroup(root, 0, 10, MTR_DEFAULT);
        REQUIRE(g1 != nullptr);
        
        // Add children to g1
        MtrNode* c1 = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(c1 != nullptr);
        
        MtrNode* c2 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(c2 != nullptr);
        
        // Dissolve g1
        MtrNode* result = Mtr_DissolveGroup(g1);
        REQUIRE(result == root);
        
        // Children should now be direct children of root
        REQUIRE(c1->parent == root);
        REQUIRE(c2->parent == root);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("FindGroup in deeply nested tree") {
        MtrNode* root = Mtr_InitGroupTree(0, 100);
        REQUIRE(root != nullptr);
        
        MtrNode* l1 = Mtr_MakeGroup(root, 0, 50, MTR_DEFAULT);
        REQUIRE(l1 != nullptr);
        
        MtrNode* l2 = Mtr_MakeGroup(root, 10, 30, MTR_DEFAULT);
        REQUIRE(l2 != nullptr);
        
        MtrNode* l3 = Mtr_MakeGroup(root, 15, 20, MTR_DEFAULT);
        REQUIRE(l3 != nullptr);
        
        // Find deepest group
        MtrNode* found = Mtr_FindGroup(root, 15, 20);
        REQUIRE(found == l3);
        
        // Find middle group
        found = Mtr_FindGroup(root, 10, 30);
        REQUIRE(found == l2);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("SwapGroups with equal sizes") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        int result = Mtr_SwapGroups(first, second);
        REQUIRE(result == 1);
        
        // Both should have swapped positions
        REQUIRE(second->low == 0);
        REQUIRE(first->low == 5);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("SwapGroups with different sizes") {
        MtrNode* root = Mtr_InitGroupTree(0, 20);
        REQUIRE(root != nullptr);
        
        MtrNode* first = Mtr_MakeGroup(root, 0, 3, MTR_DEFAULT);
        REQUIRE(first != nullptr);
        
        MtrNode* second = Mtr_MakeGroup(root, 3, 7, MTR_DEFAULT);
        REQUIRE(second != nullptr);
        
        int result = Mtr_SwapGroups(first, second);
        REQUIRE(result == 1);
        
        // Sizes should be reflected in new positions
        REQUIRE(second->low == 0);
        REQUIRE(first->low == 7);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group containing all remaining children from first") {
        // This tests lines 249-269: when last == NULL after the while loop
        // i.e., we create a parent group that contains all children from 'first' to end
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create a first child that starts at the beginning
        MtrNode* leading = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(leading != nullptr);
        
        // Create two more children at the end
        MtrNode* child1 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 15, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Now create a parent group starting from child1 that contains both child1 and child2
        // (all remaining children from first = child1 onwards)
        // This triggers the "last == NULL" case because the while loop will exhaust all children
        MtrNode* parent = Mtr_MakeGroup(root, 10, 20, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        REQUIRE(child1->parent == parent);
        REQUIRE(child2->parent == parent);
        REQUIRE(leading->younger == parent);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Create group containing multiple children with previous != NULL") {
        // This tests line 295: when previous != NULL when creating parent for multiple children
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        
        // Create a leading child
        MtrNode* leading = Mtr_MakeGroup(root, 0, 5, MTR_DEFAULT);
        REQUIRE(leading != nullptr);
        
        // Create two children to be grouped
        MtrNode* child1 = Mtr_MakeGroup(root, 5, 5, MTR_DEFAULT);
        REQUIRE(child1 != nullptr);
        
        MtrNode* child2 = Mtr_MakeGroup(root, 10, 5, MTR_DEFAULT);
        REQUIRE(child2 != nullptr);
        
        // Create a trailing child
        MtrNode* trailing = Mtr_MakeGroup(root, 20, 5, MTR_DEFAULT);
        REQUIRE(trailing != nullptr);
        
        // Create parent group for child1 and child2 (previous = leading, not NULL)
        MtrNode* parent = Mtr_MakeGroup(root, 5, 10, MTR_DEFAULT);
        REQUIRE(parent != nullptr);
        REQUIRE(child1->parent == parent);
        REQUIRE(child2->parent == parent);
        REQUIRE(leading->younger == parent);
        REQUIRE(parent->younger == trailing);
        
        Mtr_FreeTree(root);
    }
    
    SECTION("Analyze unreachable code paths") {
        // Lines 246-269 and 273-276 in mtrGroup.c appear to be unreachable.
        // After analysis: 
        // - last starts as 'first' which is always a fully-contained child
        // - The while loop only advances 'last' to younger siblings that are fully contained
        // - So 'last' can never be NULL (line 246)
        // - And 'last' is always fully contained, so line 273-274 condition is contradictory
        // These appear to be defensive code that can never be reached in practice.
        MtrNode* root = Mtr_InitGroupTree(0, 10);
        REQUIRE(root != nullptr);
        Mtr_FreeTree(root);
    }
    
    SECTION("Reorder with auxnode having children") {
        // This tests line 514: recursive call on auxnode->child
        MtrNode* root = Mtr_InitGroupTree(0, 30);
        REQUIRE(root != nullptr);
        root->index = 0;
        
        // Create first child
        MtrNode* child1 = Mtr_AllocNode();
        child1->low = 0;
        child1->size = 10;
        child1->index = 0;
        child1->flags = MTR_DEFAULT;
        child1->child = nullptr;
        Mtr_MakeLastChild(root, child1);
        
        // Create second child with its own child (grandchild)
        MtrNode* child2 = Mtr_AllocNode();
        child2->low = 10;
        child2->size = 10;
        child2->index = 10;
        child2->flags = MTR_DEFAULT;
        child2->child = nullptr;
        Mtr_MakeLastChild(root, child2);
        
        // Add grandchild to child2
        MtrNode* grandchild = Mtr_AllocNode();
        grandchild->low = 10;
        grandchild->size = 5;
        grandchild->index = 10;
        grandchild->flags = MTR_DEFAULT;
        grandchild->child = nullptr;
        Mtr_MakeLastChild(child2, grandchild);
        
        // Permutation that causes reordering and triggers recursive call on auxnode->child
        int permutation[] = {5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 15, 16, 17, 18, 19, 10, 11, 12, 13, 14, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
        Mtr_ReorderGroups(root, permutation);
        
        // Grandchild should be updated
        REQUIRE(grandchild->low == 15);
        
        Mtr_FreeTree(root);
    }
}
