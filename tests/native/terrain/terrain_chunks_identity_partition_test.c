#include "runtime/terrain/terrain_chunks.h"

#include <stdio.h>
#include <string.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_u64_diff(const char *label, uint64_t left, uint64_t right)
{
    if (left != right)
        return 1;

    fprintf(stderr, "%s expected different values but both were %llu\n", label, (unsigned long long)left);
    return 0;
}

int main(void)
{
    uint64_t hash_a = terrain_chunks_area_identity_hash("world-a", "lane-main", 10, 20, 5u);
    uint64_t hash_b = terrain_chunks_area_identity_hash("world-a", "lane-main", 10, 20, 5u);
    uint64_t hash_c = terrain_chunks_area_identity_hash("world-a", "lane-main", 11, 20, 5u);
    TerrainChunkPartitionTuple tuple;
    int coords[9][2];
    TerrainChunk chunk;
    TerrainChunk same_chunk;
    int count = 0;

    if (!expect_int("same area hash deterministic", hash_a == hash_b ? 1 : 0, 1))
        return 1;
    if (!expect_u64_diff("hash changes with chunk", hash_a, hash_c))
        return 1;

    if (!expect_int("partition tuple success",
                    terrain_chunks_partition_tuple_from_world(130.0f, -130.0f, 2, &tuple),
                    0))
        return 1;
    if (!expect_int("tuple chunk x", tuple.chunk_x, 2))
        return 1;
    if (!expect_int("tuple chunk z", tuple.chunk_z, -3))
        return 1;
    if (!expect_int("tuple partition x", tuple.partition_x, 1))
        return 1;
    if (!expect_int("tuple partition z", tuple.partition_z, -2))
        return 1;

    count = terrain_chunks_collect_adjacency_order(10, 20, 1, coords, 9);
    if (!expect_int("adjacency count", count, 9))
        return 1;
    if (!expect_int("adjacency first x", coords[0][0], 9))
        return 1;
    if (!expect_int("adjacency first z", coords[0][1], 19))
        return 1;
    if (!expect_int("adjacency last x", coords[8][0], 11))
        return 1;
    if (!expect_int("adjacency last z", coords[8][1], 21))
        return 1;

    memset(&chunk, 0, sizeof(chunk));
    memset(&same_chunk, 0, sizeof(same_chunk));
    chunk.chunk_x = 1;
    chunk.chunk_z = 2;
    same_chunk.chunk_x = 1;
    same_chunk.chunk_z = 2;

    for (int i = 0; i < TERRAIN_CHUNK_SIZE; i++)
    {
        chunk.heights[0][i] = (uint8_t)i;
        chunk.biomes[0][i] = (uint8_t)(i % BIOME_COUNT);
        same_chunk.heights[0][i] = (uint8_t)i;
        same_chunk.biomes[0][i] = (uint8_t)(i % BIOME_COUNT);
    }

    chunk.boundary_hash = terrain_chunks_compute_boundary_hash(&chunk);
    same_chunk.boundary_hash = terrain_chunks_compute_boundary_hash(&same_chunk);
    if (!expect_int("boundary hash deterministic",
                    chunk.boundary_hash == same_chunk.boundary_hash ? 1 : 0,
                    1))
        return 1;

    same_chunk.heights[0][0] ^= 1u;
    if (!expect_u64_diff("boundary hash changes on seam",
                         chunk.boundary_hash,
                         terrain_chunks_compute_boundary_hash(&same_chunk)))
        return 1;

    printf("terrain_chunks_identity_partition_test: pass\n");
    return 0;
}
