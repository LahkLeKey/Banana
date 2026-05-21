/**
 * @file interactive_collection_test.c
 * @brief Unit tests for interactive object collection system
 */

#include "interactive_collection.h"
#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT_EQ(actual, expected, msg)                                                       \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((actual) == (expected))                                                            \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected %d, got %d)\n", msg, expected, actual);                \
        }                                                                                      \
    } while (0)

void test_init(void)
{
    int ret = interactive_collection_init();
    ASSERT_EQ(ret, 0, "Init should succeed");
    interactive_collection_cleanup();
}

void test_register_collectible(void)
{
    interactive_collection_init();

    int ret = interactive_collection_register(1, RESOURCE_WOOD, 10, 5, 1000);
    ASSERT_EQ(ret, 0, "Register should succeed");

    const CollectibleObject *obj = interactive_collection_get(1);
    ASSERT_EQ((obj != NULL), 1, "Object should be registered");

    if (obj)
    {
        ASSERT_EQ(obj->object_id, 1, "Object ID should match");
        ASSERT_EQ(obj->resource_type, RESOURCE_WOOD, "Resource type should be WOOD");
        ASSERT_EQ(obj->resource_per_hit, 10, "Amount should be 10");
        ASSERT_EQ(obj->max_hits, 5, "Max hits should be 5");
        ASSERT_EQ(obj->remaining_hits, 5, "Remaining hits should be 5");
    }

    interactive_collection_cleanup();
}

void test_collect_resource(void)
{
    interactive_collection_init();

    interactive_collection_register(2, RESOURCE_ORE, 20, 3, 500);

    int ret = interactive_collection_collect(2, "player1");
    ASSERT_EQ(ret, COLLECTION_OK, "Collection should succeed");

    int balance = player_resources_get_balance("player1", RESOURCE_ORE);
    ASSERT_EQ(balance, 20, "Player should have 20 ore");

    const CollectibleObject *obj = interactive_collection_get(2);
    ASSERT_EQ(obj->remaining_hits, 2, "Remaining hits should be 2");

    interactive_collection_cleanup();
}

void test_collect_multiple(void)
{
    interactive_collection_init();

    interactive_collection_register(3, RESOURCE_STONE, 5, 3, 0);

    interactive_collection_collect(3, "player2");
    interactive_collection_collect(3, "player2");
    interactive_collection_collect(3, "player2");

    int balance = player_resources_get_balance("player2", RESOURCE_STONE);
    ASSERT_EQ(balance, 15, "Player should have 15 stone");

    const CollectibleObject *obj = interactive_collection_get(3);
    ASSERT_EQ(obj->remaining_hits, 0, "Remaining hits should be 0");

    interactive_collection_cleanup();
}

void test_depleted_object(void)
{
    interactive_collection_init();

    interactive_collection_register(4, RESOURCE_WOOD, 10, 1, 0);

    interactive_collection_collect(4, "player3");

    int ret = interactive_collection_collect(4, "player3");
    ASSERT_EQ(ret, COLLECTION_OBJECT_DEPLETED, "Collection should fail when depleted");

    int balance = player_resources_get_balance("player3", RESOURCE_WOOD);
    ASSERT_EQ(balance, 10, "Player should still have 10 wood");

    interactive_collection_cleanup();
}

void test_cooldown(void)
{
    interactive_collection_init();

    interactive_collection_register(5, RESOURCE_ORE, 10, 10, 1000);

    interactive_collection_collect(5, "player4");

    /* Try to collect again immediately */
    int ret = interactive_collection_collect(5, "player4");
    ASSERT_EQ(ret, COLLECTION_OBJECT_COOLDOWN, "Collection should fail with cooldown");

    int balance = player_resources_get_balance("player4", RESOURCE_ORE);
    ASSERT_EQ(balance, 10, "Player should still have 10 ore");

    /* Advance time */
    interactive_collection_tick(1000);

    /* Try again */
    ret = interactive_collection_collect(5, "player4");
    ASSERT_EQ(ret, COLLECTION_OK, "Collection should succeed after cooldown");

    balance = player_resources_get_balance("player4", RESOURCE_ORE);
    ASSERT_EQ(balance, 20, "Player should have 20 ore");

    interactive_collection_cleanup();
}

void test_respawn_object(void)
{
    interactive_collection_init();

    interactive_collection_register(6, RESOURCE_WOOD, 5, 2, 0);

    interactive_collection_collect(6, "player5");
    interactive_collection_collect(6, "player5");

    const CollectibleObject *obj = interactive_collection_get(6);
    ASSERT_EQ(obj->remaining_hits, 0, "Remaining hits should be 0");

    int ret = interactive_collection_respawn(6);
    ASSERT_EQ(ret, 0, "Respawn should succeed");

    obj = interactive_collection_get(6);
    ASSERT_EQ(obj->remaining_hits, 2, "Remaining hits should be reset to 2");

    interactive_collection_cleanup();
}

void test_invalid_object(void)
{
    interactive_collection_init();

    int ret = interactive_collection_collect(999, "player6");
    ASSERT_EQ(ret, COLLECTION_OBJECT_NOT_FOUND, "Collecting non-existent object should fail");

    interactive_collection_cleanup();
}

void test_multiple_objects(void)
{
    interactive_collection_init();

    interactive_collection_register(10, RESOURCE_WOOD, 5, 1, 0);
    interactive_collection_register(11, RESOURCE_ORE, 10, 1, 0);
    interactive_collection_register(12, RESOURCE_STONE, 15, 1, 0);

    interactive_collection_collect(10, "player7");
    interactive_collection_collect(11, "player7");
    interactive_collection_collect(12, "player7");

    int wood = player_resources_get_balance("player7", RESOURCE_WOOD);
    int ore = player_resources_get_balance("player7", RESOURCE_ORE);
    int stone = player_resources_get_balance("player7", RESOURCE_STONE);

    ASSERT_EQ(wood, 5, "Player should have 5 wood");
    ASSERT_EQ(ore, 10, "Player should have 10 ore");
    ASSERT_EQ(stone, 15, "Player should have 15 stone");

    interactive_collection_cleanup();
}

int main(void)
{
    printf("=== Interactive Collection Tests ===\n\n");

    test_init();
    test_register_collectible();
    test_collect_resource();
    test_collect_multiple();
    test_depleted_object();
    test_cooldown();
    test_respawn_object();
    test_invalid_object();
    test_multiple_objects();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
