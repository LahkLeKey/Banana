#include "runtime/terrain/terrain_host.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 18
#define TERRAIN_LAYERS 4
#define CHUNK_SIZE 6
#define CHUNK_COLS (TERRAIN_SIZE / CHUNK_SIZE)
#define CHUNK_ROWS (TERRAIN_SIZE / CHUNK_SIZE)
#define TOTAL_CHUNKS (CHUNK_COLS * CHUNK_ROWS)

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    RuntimeTerrainChunk chunks[TOTAL_CHUNKS];
    unsigned char height_grid[TERRAIN_SIZE * TERRAIN_SIZE];

    memset(chunks, 0, sizeof(chunks));
    memset(height_grid, 0, sizeof(height_grid));

    runtime_terrain_host_mark_chunk_dirty(chunks, CHUNK_COLS, CHUNK_ROWS, 1, 1);
    if (!expect_int("mark dirty", chunks[(1 * CHUNK_COLS) + 1].dirty, 1))
        return 1;

    runtime_terrain_host_mark_chunk_dirty(chunks, CHUNK_COLS, CHUNK_ROWS, -1, 0);
    if (!expect_int("out of range untouched", chunks[0].dirty, 0))
        return 1;

    if (!expect_int("rebuild ok",
                    runtime_terrain_host_rebuild_dirty(height_grid,
                                                      TERRAIN_SIZE,
                                                      CHUNK_SIZE,
                                                      chunks,
                                                      CHUNK_COLS,
                                                      CHUNK_ROWS,
                                                      0),
                    1))
        return 1;

    return 0;
}
