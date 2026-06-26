#include "runtime/terrain/terrain_chunks.h"

#include "runtime/support/test_support.h"

static void test_terrain_chunks_identity_and_partition_helpers(void **state)
{
    (void)state;
    uint64_t hash_default = terrain_chunks_area_identity_hash(NULL, NULL, 2, -3, 7);
    uint64_t hash_named = terrain_chunks_area_identity_hash("overworld", "lane-a", 2, -3, 7);
    TerrainChunkPartitionTuple tuple = {0};
    int coords[9][2] = {{0}};

    BANANA_TEST_ASSERT_TRUE(hash_default != 0ULL,
                            "default identity hash must be non-zero");
    BANANA_TEST_ASSERT_TRUE(hash_named != 0ULL,
                            "named identity hash must be non-zero");
    BANANA_TEST_ASSERT_TRUE(hash_default != hash_named,
                            "different identity tuples should hash differently");

    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_partition_tuple_from_world(130.0f, -2.0f, 4, &tuple),
                              0,
                              "partition tuple conversion must succeed for valid inputs");
    BANANA_TEST_ASSERT_TRUE(tuple.partition_x >= 0,
                            "partition x must be computed from world-space chunk coordinates");

    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_partition_tuple_from_world(1.0f, 1.0f, 0, &tuple),
                              -1,
                              "partition conversion must reject invalid span sizes");
    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_collect_adjacency_order(3, 4, 1, coords, 9),
                              9,
                              "adjacency helper must emit expected coordinate count");
    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_collect_adjacency_order(0, 0, -1, coords, 9),
                              -1,
                              "adjacency helper must reject negative radius");
}

static void test_terrain_chunks_cache_generation_and_cleanup(void **state)
{
    (void)state;
    const TerrainChunk *chunk_a = NULL;
    const TerrainChunk *chunk_b = NULL;
    const TerrainChunk *chunk_c = NULL;
    float world_positions[1][2] = {{32.0f, 32.0f}};
    int cached = 0;
    int generated = 0;
    int cache_size = 0;
    float world_x = 0.0f;
    float world_z = 0.0f;
    int chunk_x = 0;
    int chunk_z = 0;

    terrain_chunks_cleanup();

    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_init(7u, 0),
                              -1,
                              "terrain chunk init must reject non-positive cache size");
    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_init(123u, 64),
                              0,
                              "terrain chunk init must succeed for valid cache size");

    chunk_a = terrain_chunks_get(0, 0);
    BANANA_TEST_ASSERT_TRUE(chunk_a != NULL,
                            "first chunk lookup must succeed");

    chunk_b = terrain_chunks_get(1, 0);
    chunk_c = terrain_chunks_get(2, 0);

    BANANA_TEST_ASSERT_TRUE(chunk_a != NULL && chunk_b != NULL && chunk_c != NULL,
                            "chunk lookups must generate cache entries on demand");

    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_update_for_players(world_positions, 1, 1),
                              9,
                              "update-for-players must materialize adjacency ring chunks");
    BANANA_TEST_ASSERT_INT_EQ(terrain_chunks_update_for_players(NULL, 1, 1),
                              -1,
                              "update-for-players must reject null position arrays");

    terrain_chunks_world_to_chunk(130.0f, -130.0f, &chunk_x, &chunk_z);
    terrain_chunks_chunk_to_world(chunk_x, chunk_z, &world_x, &world_z);
    BANANA_TEST_ASSERT_TRUE(world_x <= 130.0f,
                            "chunk-to-world conversion must map chunk origin to world space");

    terrain_chunks_get_stats(&cached, &generated, &cache_size);
    BANANA_TEST_ASSERT_TRUE(cached > 0,
                            "stats must report cached chunks after lookups");
    BANANA_TEST_ASSERT_TRUE(generated > 0,
                            "stats must report generated chunk count after lookups");
    BANANA_TEST_ASSERT_INT_EQ(cache_size,
                              64,
                              "stats must report configured cache size");

    terrain_chunks_cleanup();
    BANANA_TEST_ASSERT_TRUE(terrain_chunks_get(0, 0) == NULL,
                            "chunk get must fail once terrain chunk cache is cleaned up");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_terrain_chunks_identity_and_partition_helpers),
    BANANA_TEST_CASE(test_terrain_chunks_cache_generation_and_cleanup)
)
