#include "runtime/terrain/terrain_runtime.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 18
#define CHUNK_SIZE 6
#define CHUNK_COLS (TERRAIN_SIZE / CHUNK_SIZE)
#define CHUNK_ROWS (TERRAIN_SIZE / CHUNK_SIZE)
#define TOTAL_CHUNKS (CHUNK_COLS * CHUNK_ROWS)

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    RuntimeTerrainChunk chunks[TOTAL_CHUNKS];
    unsigned char height_grid[TERRAIN_SIZE * TERRAIN_SIZE];
    uint64_t initial_signature = 0ULL;

    memset(chunks, 0, sizeof(chunks));
    memset(height_grid, 0, sizeof(height_grid));

    runtime_terrain_mark_all_chunks_dirty(chunks, TOTAL_CHUNKS);
    if (!expect_true("initial rebuild",
                     runtime_terrain_rebuild_dirty_chunks(height_grid,
                                                         TERRAIN_SIZE,
                                                         CHUNK_SIZE,
                                                         chunks,
                                                         CHUNK_COLS,
                                                         CHUNK_ROWS,
                                                         0) == 1))
        return 1;

    initial_signature = chunks[0].baseline_signature;
    if (!expect_true("initial signature present", initial_signature != 0ULL))
        return 1;

    chunks[0].dirty = 1u;
    if (!expect_true("revisit fetch uses signature parity",
                     runtime_terrain_rebuild_dirty_chunks(height_grid,
                                                         TERRAIN_SIZE,
                                                         CHUNK_SIZE,
                                                         chunks,
                                                         CHUNK_COLS,
                                                         CHUNK_ROWS,
                                                         1) == 1))
        return 1;
    if (!expect_true("signature stable on revisit", chunks[0].baseline_signature == initial_signature))
        return 1;

    height_grid[0] = (unsigned char)(height_grid[0] + 1u);
    chunks[0].dirty = 1u;
    if (!expect_true("rebuild after baseline drift",
                     runtime_terrain_rebuild_dirty_chunks(height_grid,
                                                         TERRAIN_SIZE,
                                                         CHUNK_SIZE,
                                                         chunks,
                                                         CHUNK_COLS,
                                                         CHUNK_ROWS,
                                                         1) == 1))
        return 1;
    if (!expect_true("signature changes after drift", chunks[0].baseline_signature != initial_signature))
        return 1;

    return 0;
}
