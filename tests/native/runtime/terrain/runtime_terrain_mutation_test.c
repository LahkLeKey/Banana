#include "runtime/terrain/terrain_mutation.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 18
#define TERRAIN_LAYERS 4
#define CHUNK_SIZE 6
#define CHUNK_COLS (TERRAIN_SIZE / CHUNK_SIZE)

static int s_dirty_hits[CHUNK_COLS][CHUNK_COLS];

static void mark_chunk_dirty(int cx, int cz)
{
    if (cx < 0 || cz < 0 || cx >= CHUNK_COLS || cz >= CHUNK_COLS)
        return;
    s_dirty_hits[cz][cx] += 1;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int count_dirty_hits(void)
{
    int total = 0;
    for (int z = 0; z < CHUNK_COLS; z++)
        for (int x = 0; x < CHUNK_COLS; x++)
            total += s_dirty_hits[z][x];
    return total;
}

int main(void)
{
    unsigned char heights[TERRAIN_SIZE * TERRAIN_SIZE];
    memset(heights, 0, sizeof(heights));
    memset(s_dirty_hits, 0, sizeof(s_dirty_hits));

    if (!expect_int("no-change write",
                    runtime_terrain_set_height(heights,
                                               TERRAIN_SIZE,
                                               TERRAIN_LAYERS,
                                               CHUNK_SIZE,
                                               7,
                                               7,
                                               0,
                                               mark_chunk_dirty),
                    1))
        return 1;
    if (!expect_int("no-change dirty count", count_dirty_hits(), 0))
        return 1;

    if (!expect_int("interior write",
                    runtime_terrain_set_height(heights,
                                               TERRAIN_SIZE,
                                               TERRAIN_LAYERS,
                                               CHUNK_SIZE,
                                               7,
                                               7,
                                               2,
                                               mark_chunk_dirty),
                    1))
        return 1;
    if (!expect_int("interior dirty hit", s_dirty_hits[1][1], 1))
        return 1;

    memset(s_dirty_hits, 0, sizeof(s_dirty_hits));
    if (!expect_int("boundary write",
                    runtime_terrain_set_height(heights,
                                               TERRAIN_SIZE,
                                               TERRAIN_LAYERS,
                                               CHUNK_SIZE,
                                               6,
                                               7,
                                               3,
                                               mark_chunk_dirty),
                    1))
        return 1;
    if (!expect_int("boundary dirty current chunk", s_dirty_hits[1][1], 1))
        return 1;
    if (!expect_int("boundary dirty neighbor chunk", s_dirty_hits[1][0], 1))
        return 1;

    memset(s_dirty_hits, 0, sizeof(s_dirty_hits));
    runtime_terrain_mark_region_dirty(TERRAIN_SIZE,
                                      CHUNK_SIZE,
                                      16,
                                      16,
                                      1,
                                      1,
                                      mark_chunk_dirty);
    if (!expect_int("region dirty total", count_dirty_hits(), 9))
        return 1;

    return 0;
}
