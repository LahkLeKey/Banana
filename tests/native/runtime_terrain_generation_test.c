#include "runtime/terrain_generation.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 18
#define TERRAIN_LAYERS 4

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    unsigned char grid_a[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned char grid_b[TERRAIN_SIZE * TERRAIN_SIZE];

    memset(grid_a, 0, sizeof(grid_a));
    memset(grid_b, 0, sizeof(grid_b));

    if (!expect_true("generate a", runtime_terrain_generate_island(grid_a, TERRAIN_SIZE, TERRAIN_LAYERS)))
        return 1;
    if (!expect_true("generate b", runtime_terrain_generate_island(grid_b, TERRAIN_SIZE, TERRAIN_LAYERS)))
        return 1;

    if (!expect_true("deterministic output", memcmp(grid_a, grid_b, sizeof(grid_a)) == 0))
        return 1;

    {
        int saw_nonzero = 0;
        int saw_not_max = 0;
        int center_sum = 0;
        int center_count = 0;
        int edge_sum = 0;
        int edge_count = 0;

        for (int i = 0; i < TERRAIN_SIZE * TERRAIN_SIZE; i++)
        {
            unsigned char v = grid_a[i];
            int x = i % TERRAIN_SIZE;
            int y = i / TERRAIN_SIZE;
            if (!expect_true("value in range", v < TERRAIN_LAYERS))
                return 1;
            if (v > 0)
                saw_nonzero = 1;
            if (v < (TERRAIN_LAYERS - 1))
                saw_not_max = 1;

            if (x >= 6 && x <= 11 && y >= 6 && y <= 11)
            {
                center_sum += (int)v;
                center_count++;
            }
            if (x <= 1 || x >= TERRAIN_SIZE - 2 || y <= 1 || y >= TERRAIN_SIZE - 2)
            {
                edge_sum += (int)v;
                edge_count++;
            }
        }

        if (!expect_true("contains non-zero", saw_nonzero))
            return 1;
        if (!expect_true("contains non-max", saw_not_max))
            return 1;
        if (!expect_true("center higher than edges",
                         center_count > 0 && edge_count > 0 &&
                             (center_sum / center_count) > (edge_sum / edge_count)))
            return 1;
    }

    return 0;
}
