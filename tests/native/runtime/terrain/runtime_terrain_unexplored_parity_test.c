#include "runtime/terrain/terrain_generation.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 32
#define TERRAIN_LAYERS 6

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    unsigned char baseline_a[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned char baseline_b[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned char different_area[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned int fingerprint_a = 0u;
    unsigned int fingerprint_b = 0u;
    unsigned int fingerprint_c = 0u;

    memset(baseline_a, 0, sizeof(baseline_a));
    memset(baseline_b, 0, sizeof(baseline_b));
    memset(different_area, 0, sizeof(different_area));

    if (!expect_true("unexplored contract generate a",
                     runtime_terrain_generate_unexplored_contract(
                         baseline_a,
                         TERRAIN_SIZE,
                         TERRAIN_LAYERS,
                         "world-a",
                         "lane-a",
                         1u,
                         10,
                         7,
                         RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
                         0u,
                         0,
                         &fingerprint_a) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
        return 1;

    if (!expect_true("unexplored contract generate b",
                     runtime_terrain_generate_unexplored_contract(
                         baseline_b,
                         TERRAIN_SIZE,
                         TERRAIN_LAYERS,
                         "world-a",
                         "lane-a",
                         1u,
                         10,
                         7,
                         RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
                         0u,
                         0,
                         &fingerprint_b) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
        return 1;

    if (!expect_true("equivalent inputs parity", memcmp(baseline_a, baseline_b, sizeof(baseline_a)) == 0))
        return 1;
    if (!expect_true("equivalent input fingerprint parity", fingerprint_a == fingerprint_b))
        return 1;

    if (!expect_true("different area generate",
                     runtime_terrain_generate_unexplored_contract(
                         different_area,
                         TERRAIN_SIZE,
                         TERRAIN_LAYERS,
                         "world-a",
                         "lane-a",
                         1u,
                         11,
                         7,
                         RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
                         0u,
                         0,
                         &fingerprint_c) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
        return 1;

    if (!expect_true("different-area fingerprint drift", fingerprint_c != fingerprint_a))
        return 1;

    return 0;
}
