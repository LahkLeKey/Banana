/**
 * @file terrain_chunks_test.c
 * @brief Unit tests for terrain chunk system
 */

#include "terrain/terrain_chunks.h"
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

#define ASSERT_NOT_NULL(ptr, msg)                                                              \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((ptr) != NULL)                                                                     \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected non-NULL)\n", msg);                                    \
        }                                                                                      \
    } while (0)

void test_init(void)
{
    int ret = terrain_chunks_init(12345, 64);
    ASSERT_EQ(ret, 0, "Init should succeed");
    terrain_chunks_cleanup();
}

void test_chunk_generation(void)
{
    terrain_chunks_init(12345, 64);

    const TerrainChunk *chunk = terrain_chunks_get(0, 0);
    ASSERT_NOT_NULL(chunk, "Chunk (0,0) should be generated");

    if (chunk)
    {
        ASSERT_EQ(chunk->chunk_x, 0, "Chunk X should be 0");
        ASSERT_EQ(chunk->chunk_z, 0, "Chunk Z should be 0");
    }

    terrain_chunks_cleanup();
}

void test_chunk_caching(void)
{
    terrain_chunks_init(12345, 64);

    const TerrainChunk *chunk1 = terrain_chunks_get(0, 0);
    const TerrainChunk *chunk2 = terrain_chunks_get(0, 0);

    /* Same chunk should return same data */
    ASSERT_EQ(chunk1 == chunk2, 1, "Cached chunks should be identical");

    terrain_chunks_cleanup();
}

void test_multiple_chunks(void)
{
    terrain_chunks_init(12345, 64);

    const TerrainChunk *chunk1 = terrain_chunks_get(0, 0);
    const TerrainChunk *chunk2 = terrain_chunks_get(1, 0);
    const TerrainChunk *chunk3 = terrain_chunks_get(0, 1);

    ASSERT_NOT_NULL(chunk1, "Chunk (0,0) should exist");
    ASSERT_NOT_NULL(chunk2, "Chunk (1,0) should exist");
    ASSERT_NOT_NULL(chunk3, "Chunk (0,1) should exist");

    /* Chunks should be different */
    ASSERT_EQ((chunk1 != chunk2), 1, "Different coords should have different chunks");

    terrain_chunks_cleanup();
}

void test_world_to_chunk_conversion(void)
{
    terrain_chunks_init(12345, 64);

    int cx, cz;
    terrain_chunks_world_to_chunk(0.0f, 0.0f, &cx, &cz);
    ASSERT_EQ(cx, 0, "World (0,0) should map to chunk (0,0)");
    ASSERT_EQ(cz, 0, "World (0,0) should map to chunk (0,0)");

    terrain_chunks_world_to_chunk(64.0f, 64.0f, &cx, &cz);
    ASSERT_EQ(cx, 1, "World (64,64) should map to chunk (1,1)");
    ASSERT_EQ(cz, 1, "World (64,64) should map to chunk (1,1)");

    terrain_chunks_cleanup();
}

void test_chunk_to_world_conversion(void)
{
    terrain_chunks_init(12345, 64);

    float wx, wz;
    terrain_chunks_chunk_to_world(0, 0, &wx, &wz);
    ASSERT_EQ((int)wx, 0, "Chunk (0,0) should map to world ~(0,0)");
    ASSERT_EQ((int)wz, 0, "Chunk (0,0) should map to world ~(0,0)");

    terrain_chunks_chunk_to_world(1, 1, &wx, &wz);
    ASSERT_EQ((int)wx, 64, "Chunk (1,1) should map to world ~(64,64)");
    ASSERT_EQ((int)wz, 64, "Chunk (1,1) should map to world ~(64,64)");

    terrain_chunks_cleanup();
}

void test_player_update(void)
{
    terrain_chunks_init(12345, 64);

    float player_pos[1][2] = {{32.0f, 32.0f}};
    int ret = terrain_chunks_update_for_players(player_pos, 1, 1);

    /* Should generate chunks around player position */
    ASSERT_EQ(ret > 0, 1, "Player update should generate chunks");

    terrain_chunks_cleanup();
}

void test_origin_chunk_has_playable_land(void)
{
    terrain_chunks_init(12345, 64);

    const TerrainChunk *chunk = terrain_chunks_get(0, 0);
    ASSERT_NOT_NULL(chunk, "Origin chunk should exist");

    if (chunk)
    {
        int land_tiles = 0;
        int total_tiles = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE;
        for (int z = 0; z < TERRAIN_CHUNK_SIZE; z++)
        {
            for (int x = 0; x < TERRAIN_CHUNK_SIZE; x++)
            {
                if (chunk->biomes[z][x] != BIOME_WATER)
                    land_tiles++;
            }
        }

        ASSERT_EQ(land_tiles > (total_tiles / 2), 1,
                  "Origin chunk should have majority non-water tiles");
    }

    terrain_chunks_cleanup();
}

void test_biome_variety_in_origin_neighborhood(void)
{
    terrain_chunks_init(12345, 64);

    const TerrainChunk *chunk = terrain_chunks_get(0, 0);
    ASSERT_NOT_NULL(chunk, "Origin chunk should exist for biome variety");

    if (chunk)
    {
        int seen[BIOME_COUNT] = {0};
        int unique = 0;

        for (int z = 0; z < TERRAIN_CHUNK_SIZE; z++)
        {
            for (int x = 0; x < TERRAIN_CHUNK_SIZE; x++)
            {
                int biome = (int)chunk->biomes[z][x];
                if (biome >= 0 && biome < BIOME_COUNT && !seen[biome])
                {
                    seen[biome] = 1;
                    unique++;
                }
            }
        }

        ASSERT_EQ(unique >= 2, 1, "Origin chunk should contain multiple biome types");
    }

    terrain_chunks_cleanup();
}

int main(void)
{
    printf("=== Terrain Chunks Tests ===\n\n");

    test_init();
    test_chunk_generation();
    test_chunk_caching();
    test_multiple_chunks();
    test_world_to_chunk_conversion();
    test_chunk_to_world_conversion();
    test_player_update();
    test_origin_chunk_has_playable_land();
    test_biome_variety_in_origin_neighborhood();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
