#include "runtime/interaction/spawner/interactive_object_spawner.h"
#include "runtime/terrain/terrain_chunks.h"

#include "runtime/support/test_support.h"

#include <string.h>

/* Stubs for terrain chunk dependencies */
void terrain_chunks_cleanup(void)
{
    /* no-op stub */
}

void terrain_chunks_world_to_chunk(float world_x, float world_z, int *out_cx, int *out_cz)
{
    if (out_cx) *out_cx = (int)(world_x / 64);
    if (out_cz) *out_cz = (int)(world_z / 64);
}


static void test_spawner_init_register_count(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(interactive_object_spawner_init(32),
                              0,
                              "spawner init must succeed");
    BANANA_TEST_ASSERT_INT_EQ(interactive_object_spawner_count(),
                              0,
                              "spawner count must be zero after init");

    /* validation guards */
    BANANA_TEST_ASSERT_INT_EQ(interactive_object_spawner_register_rule(NULL),
                              -1,
                              "null rule must be rejected");

    BiomeSpawnRule bad_density = {
        .biome = BIOME_FOREST,
        .object_type_id = 1,
        .resource_type = RESOURCE_WOOD,
        .reward_amount = 5,
        .max_collections = 3,
        .cooldown_ms = 1000,
        .spawn_density = 1.5f,   /* invalid > 1.0 */
    };
    BANANA_TEST_ASSERT_INT_EQ(interactive_object_spawner_register_rule(&bad_density),
                              -1,
                              "density > 1.0 must be rejected");

    BiomeSpawnRule valid = {
        .biome = BIOME_FOREST,
        .object_type_id = 1,
        .resource_type = RESOURCE_WOOD,
        .reward_amount = 5,
        .max_collections = 3,
        .cooldown_ms = 1000,
        .spawn_density = 0.3f,
    };
    BANANA_TEST_ASSERT_INT_EQ(interactive_object_spawner_register_rule(&valid),
                              0,
                              "valid rule registration must succeed");
}

static void test_spawner_spawn_chunk_paths(void **state)
{
    (void)state;
    interactive_object_spawner_init(32);

    BiomeSpawnRule rule = {
        .biome = BIOME_FOREST,
        .object_type_id = 2,
        .resource_type = RESOURCE_WOOD,
        .reward_amount = 3,
        .max_collections = 2,
        .cooldown_ms = 500,
        .spawn_density = 1.0f,   /* always spawn */
    };
    interactive_object_spawner_register_rule(&rule);

    /* spawn from null chunk must fail gracefully */
    int n = interactive_object_spawner_spawn_chunk(NULL, 0, 0);
    BANANA_TEST_ASSERT_INT_EQ(n, -1,
                              "null chunk must fail spawn_chunk");

    /* Despawn from unknown chunk must return 0 */
    int despawned = interactive_object_spawner_despawn_chunk(99, 99);
    BANANA_TEST_ASSERT_INT_EQ(despawned, 0,
                              "despawn from unknown chunk must return 0");

    interactive_object_spawner_cleanup();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_spawner_init_register_count),
    BANANA_TEST_CASE(test_spawner_spawn_chunk_paths)
)
