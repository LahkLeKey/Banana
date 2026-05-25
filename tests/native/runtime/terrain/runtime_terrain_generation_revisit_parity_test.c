#include "runtime/terrain/terrain_generation.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 64
#define TERRAIN_LAYERS 8

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static unsigned int fingerprint_grid(const unsigned char *grid, int count)
{
    unsigned int hash = 2166136261u;
    int i = 0;

    for (i = 0; i < count; i++)
    {
        hash ^= (unsigned int)grid[i];
        hash *= 16777619u;
    }

    return hash;
}

int main(void)
{
    unsigned char baseline[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned char revisit[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned int baseline_hash = 0u;
    unsigned int revisit_hash = 0u;

    memset(baseline, 0, sizeof(baseline));
    memset(revisit, 0, sizeof(revisit));

    if (!expect_true("generate baseline", runtime_terrain_generate_island(baseline, TERRAIN_SIZE, TERRAIN_LAYERS)))
        return 1;

    if (!expect_true("generate revisit", runtime_terrain_generate_island(revisit, TERRAIN_SIZE, TERRAIN_LAYERS)))
        return 1;

    if (!expect_true("baseline/revisit parity", memcmp(baseline, revisit, sizeof(baseline)) == 0))
        return 1;

    baseline_hash = fingerprint_grid(baseline, (int)(sizeof(baseline)));
    revisit_hash = fingerprint_grid(revisit, (int)(sizeof(revisit)));
    if (!expect_true("fingerprint parity", baseline_hash == revisit_hash))
        return 1;

    printf("runtime_terrain_generation_revisit_parity_test: pass\n");
    return 0;
}
