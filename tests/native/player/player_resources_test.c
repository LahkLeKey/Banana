/**
 * @file player_resources_test.c
 * @brief Unit tests for player resources system
 */

#include "runtime/player/player_resources.h"
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
    int ret = player_resources_init();
    ASSERT_EQ(ret, 0, "Init should succeed");
}

void test_create_player(void)
{
    player_resources_init();

    PlayerResources *player = player_resources_get_or_create("player1");
    ASSERT_EQ((player != NULL), 1, "Player should be created");

    if (player)
    {
        ASSERT_EQ(strcmp(player->player_guid, "player1"), 0, "Player GUID should match");
        ASSERT_EQ(player->resources[RESOURCE_WOOD], 0, "Initial wood should be 0");
    }

    player_resources_cleanup();
}

void test_add_resources(void)
{
    player_resources_init();

    int ret = player_resources_add("player1", RESOURCE_WOOD, 10);
    ASSERT_EQ(ret, 0, "Add wood should succeed");

    int balance = player_resources_get_balance("player1", RESOURCE_WOOD);
    ASSERT_EQ(balance, 10, "Wood balance should be 10");

    ret = player_resources_add("player1", RESOURCE_ORE, 5);
    ASSERT_EQ(ret, 0, "Add ore should succeed");

    balance = player_resources_get_balance("player1", RESOURCE_ORE);
    ASSERT_EQ(balance, 5, "Ore balance should be 5");

    player_resources_cleanup();
}

void test_subtract_resources(void)
{
    player_resources_init();

    player_resources_add("player2", RESOURCE_WOOD, 20);

    int ret = player_resources_subtract("player2", RESOURCE_WOOD, 5);
    ASSERT_EQ(ret, 0, "Subtract should succeed");

    int balance = player_resources_get_balance("player2", RESOURCE_WOOD);
    ASSERT_EQ(balance, 15, "Wood balance should be 15");

    player_resources_cleanup();
}

void test_insufficient_resources(void)
{
    player_resources_init();

    player_resources_add("player3", RESOURCE_WOOD, 5);

    int ret = player_resources_subtract("player3", RESOURCE_WOOD, 10);
    ASSERT_EQ(ret, -1, "Subtract with insufficient resources should fail");

    int balance = player_resources_get_balance("player3", RESOURCE_WOOD);
    ASSERT_EQ(balance, 5, "Balance should be unchanged");

    player_resources_cleanup();
}

void test_transfer_resources(void)
{
    player_resources_init();

    player_resources_add("player4", RESOURCE_ORE, 30);

    int ret = player_resources_transfer("player4", "player5", RESOURCE_ORE, 20);
    ASSERT_EQ(ret, 0, "Transfer should succeed");

    int balance4 = player_resources_get_balance("player4", RESOURCE_ORE);
    int balance5 = player_resources_get_balance("player5", RESOURCE_ORE);

    ASSERT_EQ(balance4, 10, "Player4 ore should be 10");
    ASSERT_EQ(balance5, 20, "Player5 ore should be 20");

    player_resources_cleanup();
}

void test_transfer_insufficient(void)
{
    player_resources_init();

    player_resources_add("player6", RESOURCE_STONE, 5);

    int ret = player_resources_transfer("player6", "player7", RESOURCE_STONE, 10);
    ASSERT_EQ(ret, -1, "Transfer with insufficient resources should fail");

    int balance6 = player_resources_get_balance("player6", RESOURCE_STONE);

    ASSERT_EQ(balance6, 5, "Player6 stone should remain 5");
    /* Player7 should not have been created since transfer failed */

    player_resources_cleanup();
}

void test_snapshot(void)
{
    player_resources_init();

    player_resources_add("player8", RESOURCE_WOOD, 10);
    player_resources_add("player8", RESOURCE_ORE, 5);
    player_resources_add("player8", RESOURCE_STONE, 2);

    int resources[RESOURCE_COUNT] = {0};
    int ret = player_resources_get_snapshot("player8", resources);

    ASSERT_EQ(ret, 0, "Snapshot should succeed");
    ASSERT_EQ(resources[RESOURCE_WOOD], 10, "Snapshot wood should be 10");
    ASSERT_EQ(resources[RESOURCE_ORE], 5, "Snapshot ore should be 5");
    ASSERT_EQ(resources[RESOURCE_STONE], 2, "Snapshot stone should be 2");

    player_resources_cleanup();
}

void test_reset(void)
{
    player_resources_init();

    player_resources_add("player9", RESOURCE_WOOD, 100);

    int ret = player_resources_reset("player9");
    ASSERT_EQ(ret, 0, "Reset should succeed");

    int balance = player_resources_get_balance("player9", RESOURCE_WOOD);
    ASSERT_EQ(balance, 0, "Wood balance should be 0 after reset");

    player_resources_cleanup();
}

void test_resource_names(void)
{
    const char *name_wood = player_resources_type_name(RESOURCE_WOOD);
    const char *name_ore = player_resources_type_name(RESOURCE_ORE);

    ASSERT_EQ(strcmp(name_wood, "wood"), 0, "Wood name should be 'wood'");
    ASSERT_EQ(strcmp(name_ore, "ore"), 0, "Ore name should be 'ore'");
}

void test_multiple_resource_types(void)
{
    player_resources_init();

    player_resources_add("player10", RESOURCE_WOOD, 10);
    player_resources_add("player10", RESOURCE_ORE, 20);
    player_resources_add("player10", RESOURCE_STONE, 30);

    int resources[RESOURCE_COUNT] = {0};
    player_resources_get_snapshot("player10", resources);

    ASSERT_EQ(resources[RESOURCE_WOOD], 10, "Wood should be 10");
    ASSERT_EQ(resources[RESOURCE_ORE], 20, "Ore should be 20");
    ASSERT_EQ(resources[RESOURCE_STONE], 30, "Stone should be 30");

    player_resources_cleanup();
}

int main(void)
{
    printf("=== Player Resources Tests ===\n\n");

    test_init();
    test_create_player();
    test_add_resources();
    test_subtract_resources();
    test_insufficient_resources();
    test_transfer_resources();
    test_transfer_insufficient();
    test_snapshot();
    test_reset();
    test_resource_names();
    test_multiple_resource_types();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
