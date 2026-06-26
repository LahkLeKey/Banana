#include "runtime/player/resource/player_resources.h"

#include "runtime/support/test_support.h"

static void test_init_cleanup(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(player_resources_init(),
                              0,
                              "init must succeed");
    player_resources_cleanup();
    BANANA_TEST_ASSERT_INT_EQ(player_resources_init(),
                              0,
                              "re-init after cleanup must succeed");
}

static void test_add_subtract_balance(void **state)
{
    (void)state;
    player_resources_init();

    /* null/invalid guards */
    BANANA_TEST_ASSERT_INT_EQ(player_resources_add(NULL, RESOURCE_WOOD, 10),
                              -1,
                              "null player guid must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_add("player-1", RESOURCE_COUNT, 10),
                              -1,
                              "invalid resource type must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_add("player-1", RESOURCE_WOOD, 0),
                              -1,
                              "zero amount must be rejected");

    /* add resources */
    BANANA_TEST_ASSERT_INT_EQ(player_resources_add("player-1", RESOURCE_WOOD, 15),
                              0,
                              "add must succeed");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("player-1", RESOURCE_WOOD),
                              15,
                              "balance must reflect added amount");

    BANANA_TEST_ASSERT_INT_EQ(player_resources_add("player-1", RESOURCE_ORE, 8),
                              0,
                              "add ore must succeed");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("player-1", RESOURCE_ORE),
                              8,
                              "ore balance must be correct");

    /* subtract */
    BANANA_TEST_ASSERT_INT_EQ(player_resources_subtract("player-1", RESOURCE_WOOD, 5),
                              0,
                              "subtract with sufficient balance must succeed");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("player-1", RESOURCE_WOOD),
                              10,
                              "balance must decrease after subtract");

    BANANA_TEST_ASSERT_INT_EQ(player_resources_subtract("player-1", RESOURCE_WOOD, 100),
                              -1,
                              "subtract exceeding balance must fail");

    /* unknown player */
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("nobody", RESOURCE_WOOD),
                              -1,
                              "get_balance for unknown player must return -1");

    player_resources_cleanup();
}

static void test_snapshot_and_reset(void **state)
{
    (void)state;
    int snapshot[RESOURCE_COUNT];
    player_resources_init();

    player_resources_add("alice", RESOURCE_WOOD, 7);
    player_resources_add("alice", RESOURCE_STONE, 3);

    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_snapshot("alice", snapshot),
                              0,
                              "get_snapshot must succeed for known player");
    BANANA_TEST_ASSERT_INT_EQ(snapshot[RESOURCE_WOOD], 7,
                              "snapshot must include wood amount");
    BANANA_TEST_ASSERT_INT_EQ(snapshot[RESOURCE_STONE], 3,
                              "snapshot must include stone amount");

    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_snapshot(NULL, snapshot),
                              -1,
                              "null guid must be rejected by snapshot");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_snapshot("alice", NULL),
                              -1,
                              "null output array must be rejected by snapshot");

    BANANA_TEST_ASSERT_INT_EQ(player_resources_reset("alice"),
                              0,
                              "reset must succeed for known player");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("alice", RESOURCE_WOOD),
                              0,
                              "balance must be zero after reset");

    player_resources_cleanup();
}

static void test_transfer_and_type_name(void **state)
{
    (void)state;
    player_resources_init();

    player_resources_add("bob", RESOURCE_ORE, 20);

    BANANA_TEST_ASSERT_INT_EQ(player_resources_transfer("bob", "carol", RESOURCE_ORE, 8),
                              0,
                              "valid transfer must succeed");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("bob", RESOURCE_ORE),
                              12,
                              "source must lose transferred amount");
    BANANA_TEST_ASSERT_INT_EQ(player_resources_get_balance("carol", RESOURCE_ORE),
                              8,
                              "destination must gain transferred amount");

    BANANA_TEST_ASSERT_INT_EQ(player_resources_transfer("bob", "carol", RESOURCE_ORE, 999),
                              -1,
                              "transfer exceeding balance must fail");

    BANANA_TEST_ASSERT_TRUE(player_resources_type_name(RESOURCE_WOOD) != NULL,
                            "type name for wood must not be null");
    BANANA_TEST_ASSERT_TRUE(player_resources_type_name(RESOURCE_ORE) != NULL,
                            "type name for ore must not be null");
    BANANA_TEST_ASSERT_TRUE(player_resources_type_name(RESOURCE_STONE) != NULL,
                            "type name for stone must not be null");

    player_resources_cleanup();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_init_cleanup),
    BANANA_TEST_CASE(test_add_subtract_balance),
    BANANA_TEST_CASE(test_snapshot_and_reset),
    BANANA_TEST_CASE(test_transfer_and_type_name)
)
