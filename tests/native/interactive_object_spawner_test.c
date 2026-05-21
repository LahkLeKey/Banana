/**
 * @file interactive_object_spawner_test.c
 * @brief Unit tests for interactive object spawner system
 */

#include "interactive_object_spawner.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
    int ret = interactive_object_spawner_init(10);
    ASSERT_EQ(ret, 0, "Init should succeed");
    interactive_object_spawner_cleanup();
}

void test_register_rule(void)
{
    interactive_object_spawner_init(10);

    BiomeSpawnRule rule = {.biome = BIOME_FOREST,
                           .object_type_id = 1,
                           .resource_type = RESOURCE_WOOD,
                           .reward_amount = 10,
                           .max_collections = 3,
                           .cooldown_ms = 500,
                           .spawn_density = 0.5f};

    int ret = interactive_object_spawner_register_rule(&rule);
    ASSERT_EQ(ret, 0, "Register rule should succeed");

    interactive_object_spawner_cleanup();
}

void test_spawn_empty_chunk(void)
{
    interactive_object_spawner_init(10);

    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));

    /* All biomes set to water (no rule) */
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_WATER;
        }
    }

    int spawned = interactive_object_spawner_spawn_chunk(&chunk, 0, 0);
    ASSERT_EQ(spawned, 0, "No objects should spawn in water-only chunk");

    int count = interactive_object_spawner_count();
    ASSERT_EQ(count, 0, "Object count should be 0");

    interactive_object_spawner_cleanup();
}

void test_spawn_with_rule(void)
{
    interactive_object_spawner_init(10);

    /* Register a rule for forests */
    BiomeSpawnRule rule = {.biome = BIOME_FOREST,
                           .object_type_id = 1,
                           .resource_type = RESOURCE_WOOD,
                           .reward_amount = 10,
                           .max_collections = 3,
                           .cooldown_ms = 0,
                           .spawn_density = 1.0f}; /* 100% spawn rate */

    interactive_object_spawner_register_rule(&rule);

    /* Create chunk with all forest tiles */
    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_FOREST;
        }
    }

    int spawned = interactive_object_spawner_spawn_chunk(&chunk, 0, 0);
    ASSERT_EQ(spawned > 0, 1, "Should spawn some objects");

    int count = interactive_object_spawner_count();
    ASSERT_EQ((count > 0), 1, "Object count should be > 0");

    interactive_object_spawner_cleanup();
}

void test_spawn_multiple_rules(void)
{
    interactive_object_spawner_init(10);

    BiomeSpawnRule rule1 = {.biome = BIOME_FOREST,
                            .object_type_id = 1,
                            .resource_type = RESOURCE_WOOD,
                            .reward_amount = 10,
                            .max_collections = 3,
                            .cooldown_ms = 0,
                            .spawn_density = 0.5f};

    BiomeSpawnRule rule2 = {.biome = BIOME_HILL,
                            .object_type_id = 2,
                            .resource_type = RESOURCE_ORE,
                            .reward_amount = 20,
                            .max_collections = 2,
                            .cooldown_ms = 0,
                            .spawn_density = 0.5f};

    interactive_object_spawner_register_rule(&rule1);
    interactive_object_spawner_register_rule(&rule2);

    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));

    /* Half forest, half hill */
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_FOREST;
        }
    }
    for (int y = 32; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_HILL;
        }
    }

    int spawned = interactive_object_spawner_spawn_chunk(&chunk, 0, 0);
    ASSERT_EQ(spawned > 0, 1, "Should spawn objects from multiple biomes");

    interactive_object_spawner_cleanup();
}

void test_collect_spawned_object(void)
{
    interactive_object_spawner_init(10);

    BiomeSpawnRule rule = {.biome = BIOME_FOREST,
                           .object_type_id = 1,
                           .resource_type = RESOURCE_WOOD,
                           .reward_amount = 15,
                           .max_collections = 1,
                           .cooldown_ms = 0,
                           .spawn_density = 1.0f};

    interactive_object_spawner_register_rule(&rule);

    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_FOREST;
        }
    }

    interactive_object_spawner_spawn_chunk(&chunk, 0, 0);
    int count = interactive_object_spawner_count();
    ASSERT_EQ((count > 0), 1, "Objects should be spawned");

    /* Try to collect from first object if it exists */
    if (count > 0)
    {
        const CollectibleObject *obj = interactive_collection_get(1000);
        if (obj && obj->object_id == 1000)
        {
            int ret = interactive_collection_collect(1000, "player1");
            ASSERT_EQ(ret, 0, "Collection should succeed");

            int balance = player_resources_get_balance("player1", RESOURCE_WOOD);
            ASSERT_EQ(balance, 15, "Player should have 15 wood");
        }
    }

    interactive_object_spawner_cleanup();
}

void test_despawn_chunk(void)
{
    interactive_object_spawner_init(10);

    BiomeSpawnRule rule = {.biome = BIOME_FOREST,
                           .object_type_id = 1,
                           .resource_type = RESOURCE_WOOD,
                           .reward_amount = 10,
                           .max_collections = 1,
                           .cooldown_ms = 0,
                           .spawn_density = 1.0f};

    interactive_object_spawner_register_rule(&rule);

    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_FOREST;
        }
    }

    interactive_object_spawner_spawn_chunk(&chunk, 0, 0);
    int count_before = interactive_object_spawner_count();
    ASSERT_EQ((count_before > 0), 1, "Objects should be spawned");

    int removed = interactive_object_spawner_despawn_chunk(0, 0);
    ASSERT_EQ((removed > 0), 1, "Some objects should be removed");

    int count_after = interactive_object_spawner_count();
    ASSERT_EQ(count_after, 0, "Object count should be 0");

    interactive_object_spawner_cleanup();
}

void test_spawn_density(void)
{
    interactive_object_spawner_init(10);

    BiomeSpawnRule rule = {.biome = BIOME_FOREST,
                           .object_type_id = 1,
                           .resource_type = RESOURCE_WOOD,
                           .reward_amount = 10,
                           .max_collections = 1,
                           .cooldown_ms = 0,
                           .spawn_density = 0.1f}; /* 10% spawn rate */

    interactive_object_spawner_register_rule(&rule);

    TerrainChunk chunk;
    memset(&chunk, 0, sizeof(chunk));
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            chunk.biomes[y][x] = (uint8_t)BIOME_FOREST;
        }
    }

    int spawned = interactive_object_spawner_spawn_chunk(&chunk, 0, 0);
    int count = interactive_object_spawner_count();

    /* With 10% density on 4096 tiles, expect roughly 400 objects */
    ASSERT_EQ((count > 0 && count < 4096), 1, "Object count should be between 0 and 4096");

    interactive_object_spawner_cleanup();
}

int main(void)
{
    printf("=== Interactive Object Spawner Tests ===\n\n");

    test_init();
    test_register_rule();
    test_spawn_empty_chunk();
    test_spawn_with_rule();
    test_spawn_multiple_rules();
    test_collect_spawned_object();
    test_despawn_chunk();
    test_spawn_density();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
