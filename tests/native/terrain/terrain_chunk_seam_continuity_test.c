#include "runtime/terrain/terrain_chunks.h"

#include <stdio.h>
#include <stdlib.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    const TerrainChunk *origin = NULL;
    const TerrainChunk *east = NULL;
    const TerrainChunk *south = NULL;
    int x = 0;
    int z = 0;

    if (!expect_true("terrain init", terrain_chunks_init(1337u, 128) == 0))
        return 1;

    origin = terrain_chunks_get(0, 0);
    east = terrain_chunks_get(1, 0);
    south = terrain_chunks_get(0, 1);

    if (!expect_true("origin chunk", origin != NULL))
    {
        terrain_chunks_cleanup();
        return 1;
    }
    if (!expect_true("east chunk", east != NULL))
    {
        terrain_chunks_cleanup();
        return 1;
    }
    if (!expect_true("south chunk", south != NULL))
    {
        terrain_chunks_cleanup();
        return 1;
    }

    for (z = 0; z < TERRAIN_CHUNK_SIZE; z++)
    {
        int delta = abs((int)origin->heights[z][TERRAIN_CHUNK_SIZE - 1] - (int)east->heights[z][0]);
        if (!expect_true("east-west seam continuity", delta <= 40))
        {
            terrain_chunks_cleanup();
            return 1;
        }
    }

    for (x = 0; x < TERRAIN_CHUNK_SIZE; x++)
    {
        int delta = abs((int)origin->heights[TERRAIN_CHUNK_SIZE - 1][x] - (int)south->heights[0][x]);
        if (!expect_true("north-south seam continuity", delta <= 40))
        {
            terrain_chunks_cleanup();
            return 1;
        }
    }

    {
        float world_x = 0.0f;
        float world_z = 0.0f;
        int chunk_x = 0;
        int chunk_z = 0;

        terrain_chunks_chunk_to_world(1, 0, &world_x, &world_z);
        terrain_chunks_world_to_chunk(world_x, world_z, &chunk_x, &chunk_z);

        if (!expect_true("chunk world-to-grid continuity", chunk_x == 1 && chunk_z == 0))
        {
            terrain_chunks_cleanup();
            return 1;
        }
    }

    terrain_chunks_cleanup();
    printf("terrain_chunk_seam_continuity_test: pass\n");
    return 0;
}
