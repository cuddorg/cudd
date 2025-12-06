#include <catch2/catch_test_macros.hpp>

// Include CUDD headers
#include "cudd/cudd.h"
#include "cuddInt.h"
#include "util.h"

/**
 * @brief Comprehensive test file for cuddLevelQ.c
 * 
 * This file contains tests to achieve >90% code coverage for
 * the cuddLevelQ module including:
 * - cuddLevelQueueInit
 * - cuddLevelQueueQuit
 * - cuddLevelQueueEnqueue
 * - cuddLevelQueueFirst
 * - cuddLevelQueueDequeue
 * - Static helper functions (indirectly): hashLookup, hashInsert, hashDelete, hashResize
 */

// ============== cuddLevelQueueInit Tests ==============

TEST_CASE("cuddLevelQueueInit - Basic initialization", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Initialize with minimal parameters") {
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->levels == 5);
        REQUIRE(queue->itemsize == sizeof(DdQueueItem));
        REQUIRE(queue->size == 0);
        REQUIRE(queue->first == nullptr);
        REQUIRE(queue->freelist == nullptr);
        REQUIRE(queue->manager == manager);
        cuddLevelQueueQuit(queue);
    }

    SECTION("Initialize with numBuckets < 2") {
        // numBuckets < 2 should be adjusted to 2
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(DdQueueItem), 1, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->numBuckets >= 2);
        cuddLevelQueueQuit(queue);
    }

    SECTION("Initialize with exact power of 2 buckets") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 16, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->numBuckets == 16);
        cuddLevelQueueQuit(queue);
    }

    SECTION("Initialize with non-power of 2 buckets") {
        // Should be rounded down to nearest power of 2
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 15, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->numBuckets == 8);  // 15 rounds down to 8
        cuddLevelQueueQuit(queue);
    }

    SECTION("Initialize with many levels") {
        DdLevelQueue *queue = cuddLevelQueueInit(100, sizeof(DdQueueItem), 32, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->levels == 100);
        cuddLevelQueueQuit(queue);
    }

    SECTION("Initialize with larger item size") {
        // Custom queue item with extra data
        struct CustomItem {
            DdQueueItem base;
            int extraData1;
            double extraData2;
        };
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(CustomItem), 8, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->itemsize == sizeof(CustomItem));
        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== cuddLevelQueueFirst Tests ==============

TEST_CASE("cuddLevelQueueFirst - Insert first key", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(10, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Insert first key at level 0") {
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key = Cudd_bddIthVar(manager, 0);
        void *item = cuddLevelQueueFirst(queue, key, 0);
        REQUIRE(item != nullptr);
        REQUIRE(queue->size == 1);
        REQUIRE(queue->first == item);
        REQUIRE(queue->last[0] == item);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Insert first key at higher level") {
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key = Cudd_bddIthVar(manager, 3);
        void *item = cuddLevelQueueFirst(queue, key, 3);
        REQUIRE(item != nullptr);
        REQUIRE(queue->size == 1);
        REQUIRE(queue->first == item);
        REQUIRE(queue->last[3] == item);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Insert first key with freelist available") {
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // First insert and dequeue to create freelist
        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        void *item1 = cuddLevelQueueFirst(queue, key1, 0);
        REQUIRE(item1 != nullptr);
        cuddLevelQueueDequeue(queue, 0);

        // Now freelist should have one item
        REQUIRE(queue->freelist != nullptr);

        // Insert again using freelist
        DdNode *key2 = Cudd_bddIthVar(manager, 1);
        void *item2 = cuddLevelQueueFirst(queue, key2, 1);
        REQUIRE(item2 != nullptr);
        REQUIRE(queue->size == 1);

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== cuddLevelQueueEnqueue Tests ==============

TEST_CASE("cuddLevelQueueEnqueue - Basic enqueue operations", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Enqueue to empty queue") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key = Cudd_bddIthVar(manager, 0);
        void *item = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item != nullptr);
        REQUIRE(queue->size == 1);
        REQUIRE(queue->first == item);
        REQUIRE(queue->last[0] == item);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue duplicate key returns existing item") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key = Cudd_bddIthVar(manager, 0);
        void *item1 = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item1 != nullptr);

        // Try to enqueue same key again
        void *item2 = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item2 == item1);  // Should return the same item
        REQUIRE(queue->size == 1);  // Size shouldn't change

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue multiple items at same level") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        DdNode *key2 = Cudd_bddIthVar(manager, 1);

        void *item1 = cuddLevelQueueEnqueue(queue, key1, 2);
        REQUIRE(item1 != nullptr);
        REQUIRE(queue->size == 1);
        REQUIRE(queue->last[2] == item1);

        void *item2 = cuddLevelQueueEnqueue(queue, key2, 2);
        REQUIRE(item2 != nullptr);
        REQUIRE(queue->size == 2);
        REQUIRE(queue->last[2] == item2);  // last[2] should be updated

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue items at different levels") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        DdNode *key1 = Cudd_bddIthVar(manager, 1);
        DdNode *key2 = Cudd_bddIthVar(manager, 2);

        void *item0 = cuddLevelQueueEnqueue(queue, key0, 0);
        void *item2 = cuddLevelQueueEnqueue(queue, key2, 2);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 1);  // Insert in middle

        REQUIRE(item0 != nullptr);
        REQUIRE(item1 != nullptr);
        REQUIRE(item2 != nullptr);
        REQUIRE(queue->size == 3);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue with preceding level having items") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Add item at level 0 first
        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        void *item0 = cuddLevelQueueEnqueue(queue, key0, 0);
        REQUIRE(item0 != nullptr);

        // Add item at level 5, no items at levels 1-4
        DdNode *key5 = Cudd_bddIthVar(manager, 5);
        void *item5 = cuddLevelQueueEnqueue(queue, key5, 5);
        REQUIRE(item5 != nullptr);
        REQUIRE(queue->size == 2);

        // Verify queue structure
        REQUIRE(queue->last[0] == item0);
        REQUIRE(queue->last[5] == item5);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue when queue is first empty, then not - complex scenario") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Insert at a middle level when queue is empty
        DdNode *key3 = Cudd_bddIthVar(manager, 3);
        void *item3 = cuddLevelQueueEnqueue(queue, key3, 3);
        REQUIRE(item3 != nullptr);
        REQUIRE(queue->first == item3);

        // Insert at lower level (should become first)
        DdNode *key1 = Cudd_bddIthVar(manager, 1);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 1);
        REQUIRE(item1 != nullptr);

        // Insert at higher level
        DdNode *key7 = Cudd_bddIthVar(manager, 7);
        void *item7 = cuddLevelQueueEnqueue(queue, key7, 7);
        REQUIRE(item7 != nullptr);
        REQUIRE(queue->size == 3);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue using freelist") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Create and then free an item
        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 0);
        REQUIRE(item1 != nullptr);
        cuddLevelQueueDequeue(queue, 0);

        // Now freelist has one item
        REQUIRE(queue->freelist != nullptr);

        // Enqueue should use freelist
        DdNode *key2 = Cudd_bddIthVar(manager, 1);
        void *item2 = cuddLevelQueueEnqueue(queue, key2, 1);
        REQUIRE(item2 != nullptr);

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== cuddLevelQueueDequeue Tests ==============

TEST_CASE("cuddLevelQueueDequeue - Basic dequeue operations", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Dequeue single item") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key = Cudd_bddIthVar(manager, 0);
        void *item = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item != nullptr);
        REQUIRE(queue->size == 1);

        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->size == 0);
        REQUIRE(queue->first == nullptr);
        REQUIRE(queue->last[0] == nullptr);
        REQUIRE(queue->freelist != nullptr);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Dequeue from queue with multiple items") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        DdNode *key1 = Cudd_bddIthVar(manager, 1);
        void *item0 = cuddLevelQueueEnqueue(queue, key0, 0);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 0);
        REQUIRE(queue->size == 2);
        REQUIRE(queue->first == item0);

        // Dequeue first item
        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->size == 1);
        REQUIRE(queue->first == item1);
        REQUIRE(queue->last[0] == item1);  // last[0] should still point to item1

        // Dequeue second item
        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->size == 0);
        REQUIRE(queue->first == nullptr);
        REQUIRE(queue->last[0] == nullptr);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Dequeue items at different levels") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        DdNode *key1 = Cudd_bddIthVar(manager, 1);
        DdNode *key2 = Cudd_bddIthVar(manager, 2);

        void *item0 = cuddLevelQueueEnqueue(queue, key0, 0);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 1);
        void *item2 = cuddLevelQueueEnqueue(queue, key2, 2);
        REQUIRE(queue->size == 3);
        REQUIRE(queue->first == item0);

        // Dequeue in order
        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->size == 2);
        REQUIRE(queue->first == item1);
        REQUIRE(queue->last[0] == nullptr);

        cuddLevelQueueDequeue(queue, 1);
        REQUIRE(queue->size == 1);
        REQUIRE(queue->first == item2);

        cuddLevelQueueDequeue(queue, 2);
        REQUIRE(queue->size == 0);
        REQUIRE(queue->first == nullptr);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Dequeue when not last item at level") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Add two items at same level
        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        DdNode *key1 = Cudd_bddIthVar(manager, 1);
        void *item0 = cuddLevelQueueEnqueue(queue, key0, 0);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 0);
        REQUIRE(queue->last[0] == item1);

        // Dequeue first - not the last at level 0
        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->last[0] == item1);  // Should remain pointing to item1

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== cuddLevelQueueQuit Tests ==============

TEST_CASE("cuddLevelQueueQuit - Clean up operations", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Quit empty queue") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);
        // Just clean up - no items added
        cuddLevelQueueQuit(queue);
        // If we get here without crash, it worked
    }

    SECTION("Quit queue with items in freelist") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Add and remove items to populate freelist
        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        DdNode *key2 = Cudd_bddIthVar(manager, 1);
        cuddLevelQueueEnqueue(queue, key1, 0);
        cuddLevelQueueEnqueue(queue, key2, 1);
        cuddLevelQueueDequeue(queue, 0);
        cuddLevelQueueDequeue(queue, 1);

        REQUIRE(queue->freelist != nullptr);
        cuddLevelQueueQuit(queue);
    }

    SECTION("Quit queue with items in first (active queue)") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Add items without removing them
        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        DdNode *key2 = Cudd_bddIthVar(manager, 1);
        DdNode *key3 = Cudd_bddIthVar(manager, 2);
        cuddLevelQueueEnqueue(queue, key1, 0);
        cuddLevelQueueEnqueue(queue, key2, 1);
        cuddLevelQueueEnqueue(queue, key3, 2);

        REQUIRE(queue->first != nullptr);
        cuddLevelQueueQuit(queue);
    }

    SECTION("Quit queue with both freelist and active items") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Add items
        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        DdNode *key2 = Cudd_bddIthVar(manager, 1);
        DdNode *key3 = Cudd_bddIthVar(manager, 2);
        DdNode *key4 = Cudd_bddIthVar(manager, 3);
        cuddLevelQueueEnqueue(queue, key1, 0);
        cuddLevelQueueEnqueue(queue, key2, 1);
        cuddLevelQueueEnqueue(queue, key3, 2);
        cuddLevelQueueEnqueue(queue, key4, 3);

        // Dequeue some to populate freelist
        cuddLevelQueueDequeue(queue, 0);
        cuddLevelQueueDequeue(queue, 1);

        REQUIRE(queue->first != nullptr);
        REQUIRE(queue->freelist != nullptr);
        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== Hash table related tests (indirectly testing static functions) ==============

TEST_CASE("Hash table operations - collision and resize", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(50, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Multiple inserts triggering potential hash collisions") {
        // Small number of buckets to increase collision probability
        DdLevelQueue *queue = cuddLevelQueueInit(20, sizeof(DdQueueItem), 2, manager);
        REQUIRE(queue != nullptr);

        // Insert many items - should cause hash collisions
        for (int i = 0; i < 15; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            void *item = cuddLevelQueueEnqueue(queue, key, i % 10);
            REQUIRE(item != nullptr);
        }
        REQUIRE(queue->size == 15);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Force hash table resize") {
        // Start with small bucket count
        DdLevelQueue *queue = cuddLevelQueueInit(30, sizeof(DdQueueItem), 2, manager);
        REQUIRE(queue != nullptr);
        int initialMaxSize = queue->maxsize;

        // Insert enough items to trigger resize
        // maxsize = numBuckets * DD_MAX_SUBTABLE_DENSITY (4)
        // With 2 buckets, maxsize = 8
        for (int i = 0; i < 20; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            void *item = cuddLevelQueueEnqueue(queue, key, i % 20);
            REQUIRE(item != nullptr);
        }

        // After resize, maxsize should have increased
        REQUIRE(queue->maxsize > initialMaxSize);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Hash lookup for existing and non-existing keys") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        DdNode *key2 = Cudd_bddIthVar(manager, 1);
        DdNode *key3 = Cudd_bddIthVar(manager, 2);

        // Insert two keys
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 0);
        void *item2 = cuddLevelQueueEnqueue(queue, key2, 1);

        // Re-enqueue same keys should return existing items
        void *lookup1 = cuddLevelQueueEnqueue(queue, key1, 0);
        void *lookup2 = cuddLevelQueueEnqueue(queue, key2, 1);
        REQUIRE(lookup1 == item1);
        REQUIRE(lookup2 == item2);

        // Enqueue new key should create new item
        void *item3 = cuddLevelQueueEnqueue(queue, key3, 2);
        REQUIRE(item3 != nullptr);
        REQUIRE(item3 != item1);
        REQUIRE(item3 != item2);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Hash delete - item in chain") {
        // Use small bucket count to force chains
        DdLevelQueue *queue = cuddLevelQueueInit(15, sizeof(DdQueueItem), 2, manager);
        REQUIRE(queue != nullptr);

        // Insert several items
        for (int i = 0; i < 10; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            void *item = cuddLevelQueueEnqueue(queue, key, i);
            REQUIRE(item != nullptr);
        }
        REQUIRE(queue->size == 10);

        // Dequeue all items one by one
        for (int i = 0; i < 10; i++) {
            cuddLevelQueueDequeue(queue, i);
        }
        REQUIRE(queue->size == 0);

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== Complex scenarios ==============

TEST_CASE("Complex level queue scenarios", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(50, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Enqueue and dequeue cycle") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 8, manager);
        REQUIRE(queue != nullptr);

        // Multiple cycles of enqueue/dequeue
        for (int cycle = 0; cycle < 3; cycle++) {
            // Enqueue items
            for (int i = 0; i < 5; i++) {
                DdNode *key = Cudd_bddIthVar(manager, cycle * 10 + i);
                void *item = cuddLevelQueueEnqueue(queue, key, i);
                REQUIRE(item != nullptr);
            }
            REQUIRE(queue->size == 5);

            // Dequeue all
            for (int i = 0; i < 5; i++) {
                cuddLevelQueueDequeue(queue, i);
            }
            REQUIRE(queue->size == 0);
        }

        cuddLevelQueueQuit(queue);
    }

    SECTION("Mixed level operations") {
        DdLevelQueue *queue = cuddLevelQueueInit(20, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Insert at various levels in non-sequential order
        DdNode *key5 = Cudd_bddIthVar(manager, 5);
        DdNode *key10 = Cudd_bddIthVar(manager, 10);
        DdNode *key2 = Cudd_bddIthVar(manager, 2);
        DdNode *key15 = Cudd_bddIthVar(manager, 15);
        DdNode *key7 = Cudd_bddIthVar(manager, 7);

        cuddLevelQueueEnqueue(queue, key5, 5);
        cuddLevelQueueEnqueue(queue, key10, 10);
        cuddLevelQueueEnqueue(queue, key2, 2);
        cuddLevelQueueEnqueue(queue, key15, 15);
        cuddLevelQueueEnqueue(queue, key7, 7);

        REQUIRE(queue->size == 5);

        // Process queue in order (by level)
        cuddLevelQueueDequeue(queue, 2);
        cuddLevelQueueDequeue(queue, 5);
        cuddLevelQueueDequeue(queue, 7);
        cuddLevelQueueDequeue(queue, 10);
        cuddLevelQueueDequeue(queue, 15);

        REQUIRE(queue->size == 0);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Stress test with many items") {
        DdLevelQueue *queue = cuddLevelQueueInit(50, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Insert many items
        for (int i = 0; i < 40; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            void *item = cuddLevelQueueEnqueue(queue, key, i % 50);
            REQUIRE(item != nullptr);
        }
        REQUIRE(queue->size == 40);

        // Dequeue half
        for (int i = 0; i < 20; i++) {
            cuddLevelQueueDequeue(queue, i % 50);
        }
        REQUIRE(queue->size == 20);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Re-enqueue after dequeue") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        DdNode *key = Cudd_bddIthVar(manager, 0);

        // Enqueue, dequeue, then re-enqueue same key
        void *item1 = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item1 != nullptr);

        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->size == 0);

        // Re-enqueue same key - should get a new/recycled item
        void *item2 = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item2 != nullptr);
        REQUIRE(queue->size == 1);

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== Edge cases ==============

TEST_CASE("Edge cases for level queue", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(50, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Single level queue") {
        DdLevelQueue *queue = cuddLevelQueueInit(1, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->levels == 1);

        DdNode *key = Cudd_bddIthVar(manager, 0);
        void *item = cuddLevelQueueEnqueue(queue, key, 0);
        REQUIRE(item != nullptr);

        cuddLevelQueueDequeue(queue, 0);
        REQUIRE(queue->size == 0);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Large number of levels") {
        DdLevelQueue *queue = cuddLevelQueueInit(1000, sizeof(DdQueueItem), 16, manager);
        REQUIRE(queue != nullptr);
        REQUIRE(queue->levels == 1000);

        // Insert at various levels including high ones
        DdNode *key1 = Cudd_bddIthVar(manager, 0);
        DdNode *key2 = Cudd_bddIthVar(manager, 1);

        void *item1 = cuddLevelQueueEnqueue(queue, key1, 999);
        void *item2 = cuddLevelQueueEnqueue(queue, key2, 0);
        REQUIRE(item1 != nullptr);
        REQUIRE(item2 != nullptr);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Consecutive levels with interleaved operations") {
        DdLevelQueue *queue = cuddLevelQueueInit(5, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Insert at consecutive levels
        for (int i = 0; i < 5; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            cuddLevelQueueEnqueue(queue, key, i);
        }
        REQUIRE(queue->size == 5);

        // Dequeue every other one
        cuddLevelQueueDequeue(queue, 0);
        cuddLevelQueueDequeue(queue, 1);
        REQUIRE(queue->size == 3);

        // Add more at levels that now have gaps
        DdNode *key5 = Cudd_bddIthVar(manager, 5);
        DdNode *key6 = Cudd_bddIthVar(manager, 6);
        cuddLevelQueueEnqueue(queue, key5, 0);
        cuddLevelQueueEnqueue(queue, key6, 1);
        REQUIRE(queue->size == 5);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Using cuddLevelQueueFirst then Enqueue") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Use First to insert the first item
        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        void *item0 = cuddLevelQueueFirst(queue, key0, 3);
        REQUIRE(item0 != nullptr);
        REQUIRE(queue->size == 1);

        // Then use Enqueue for subsequent items
        DdNode *key1 = Cudd_bddIthVar(manager, 1);
        DdNode *key2 = Cudd_bddIthVar(manager, 2);
        void *item1 = cuddLevelQueueEnqueue(queue, key1, 1);  // Lower level
        void *item2 = cuddLevelQueueEnqueue(queue, key2, 5);  // Higher level
        REQUIRE(item1 != nullptr);
        REQUIRE(item2 != nullptr);
        REQUIRE(queue->size == 3);

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}

// ============== Test with BDD nodes (simulating real usage) ==============

TEST_CASE("Level queue with BDD traversal simulation", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(20, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Simulate BDD level-order traversal") {
        // Create a simple BDD
        DdNode *x0 = Cudd_bddIthVar(manager, 0);
        DdNode *x1 = Cudd_bddIthVar(manager, 1);
        DdNode *x2 = Cudd_bddIthVar(manager, 2);

        DdNode *f = Cudd_bddAnd(manager, x0, x1);
        Cudd_Ref(f);
        DdNode *g = Cudd_bddOr(manager, f, x2);
        Cudd_Ref(g);

        // Use level queue to traverse
        DdLevelQueue *queue = cuddLevelQueueInit(
            Cudd_ReadSize(manager), sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Start with root
        DdNode *root = Cudd_Regular(g);
        void *item = cuddLevelQueueFirst(queue, root, Cudd_NodeReadIndex(root));
        REQUIRE(item != nullptr);

        // Simulate processing nodes
        int count = 0;
        while (queue->first != nullptr && count < 100) {  // Safety limit
            DdQueueItem *current = (DdQueueItem *)queue->first;
            DdNode *node = (DdNode *)current->key;

            if (!Cudd_IsConstant(node)) {
                // Enqueue children
                DdNode *T = Cudd_T(node);
                DdNode *E = Cudd_Regular(Cudd_E(node));

                if (!Cudd_IsConstant(T)) {
                    cuddLevelQueueEnqueue(queue, T, Cudd_NodeReadIndex(T));
                }
                if (!Cudd_IsConstant(E)) {
                    cuddLevelQueueEnqueue(queue, E, Cudd_NodeReadIndex(E));
                }
            }

            // Get level for dequeue
            int level = Cudd_IsConstant(node) ? 0 : Cudd_NodeReadIndex(node);
            cuddLevelQueueDequeue(queue, level);
            count++;
        }

        cuddLevelQueueQuit(queue);
        Cudd_RecursiveDeref(manager, g);
        Cudd_RecursiveDeref(manager, f);
    }

    Cudd_Quit(manager);
}

// ============== Additional tests for full coverage ==============

TEST_CASE("Additional coverage tests", "[cuddLevelQ]") {
    DdManager *manager = Cudd_Init(50, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    REQUIRE(manager != nullptr);

    SECTION("Enqueue at level 0 when queue has items at higher levels only") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // First add at higher level
        DdNode *key5 = Cudd_bddIthVar(manager, 5);
        void *item5 = cuddLevelQueueEnqueue(queue, key5, 5);
        REQUIRE(item5 != nullptr);
        REQUIRE(queue->first == item5);

        // Then add at level 0
        DdNode *key0 = Cudd_bddIthVar(manager, 0);
        void *item0 = cuddLevelQueueEnqueue(queue, key0, 0);
        REQUIRE(item0 != nullptr);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Multiple dequeues with hash collision handling") {
        // Very small bucket to maximize collisions
        DdLevelQueue *queue = cuddLevelQueueInit(20, sizeof(DdQueueItem), 2, manager);
        REQUIRE(queue != nullptr);

        // Insert many items at the same level
        for (int i = 0; i < 10; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            cuddLevelQueueEnqueue(queue, key, 0);
        }

        // Dequeue them all - tests hashDelete with chains
        for (int i = 0; i < 10; i++) {
            cuddLevelQueueDequeue(queue, 0);
        }
        REQUIRE(queue->size == 0);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Insert triggering while loop in enqueue - searching for preceding level") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Insert at level 5 first - becomes first
        DdNode *key5 = Cudd_bddIthVar(manager, 5);
        void *item5 = cuddLevelQueueEnqueue(queue, key5, 5);
        REQUIRE(item5 != nullptr);

        // Now insert at level 3 - no preceding level has items, so becomes first
        DdNode *key3 = Cudd_bddIthVar(manager, 3);
        void *item3 = cuddLevelQueueEnqueue(queue, key3, 3);
        REQUIRE(item3 != nullptr);

        // Insert at level 4 - level 3 precedes it
        DdNode *key4 = Cudd_bddIthVar(manager, 4);
        void *item4 = cuddLevelQueueEnqueue(queue, key4, 4);
        REQUIRE(item4 != nullptr);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Large resize operation") {
        DdLevelQueue *queue = cuddLevelQueueInit(50, sizeof(DdQueueItem), 2, manager);
        REQUIRE(queue != nullptr);

        // Track initial state
        unsigned int initialBuckets = queue->numBuckets;

        // Force multiple resizes by inserting many items
        for (int i = 0; i < 45; i++) {
            DdNode *key = Cudd_bddIthVar(manager, i);
            void *item = cuddLevelQueueEnqueue(queue, key, i % 50);
            REQUIRE(item != nullptr);
        }

        // Should have resized multiple times
        REQUIRE(queue->numBuckets > initialBuckets);

        cuddLevelQueueQuit(queue);
    }

    SECTION("Enqueue with existing items only at level 0") {
        DdLevelQueue *queue = cuddLevelQueueInit(10, sizeof(DdQueueItem), 4, manager);
        REQUIRE(queue != nullptr);

        // Insert at level 0
        DdNode *key0a = Cudd_bddIthVar(manager, 0);
        DdNode *key0b = Cudd_bddIthVar(manager, 1);
        cuddLevelQueueEnqueue(queue, key0a, 0);
        cuddLevelQueueEnqueue(queue, key0b, 0);

        // Insert at level 5 - while loop starts at plevel=5 and should find level 0
        DdNode *key5 = Cudd_bddIthVar(manager, 5);
        void *item5 = cuddLevelQueueEnqueue(queue, key5, 5);
        REQUIRE(item5 != nullptr);

        cuddLevelQueueQuit(queue);
    }

    Cudd_Quit(manager);
}
