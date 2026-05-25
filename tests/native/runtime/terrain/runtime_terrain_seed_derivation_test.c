#include "runtime/terrain/terrain_generation.h"

#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    unsigned int seed_a = runtime_terrain_derive_world_seed("world-main", "lane-alpha", 7u);
    unsigned int seed_b = runtime_terrain_derive_world_seed("world-main", "lane-alpha", 7u);
    unsigned int seed_c = runtime_terrain_derive_world_seed("world-main", "lane-alpha", 8u);
    unsigned int seed_d = runtime_terrain_derive_world_seed("world-alt", "lane-alpha", 7u);
    unsigned int seed_e = runtime_terrain_derive_world_seed("", "", 0u);

    if (!expect_true("same input deterministic", seed_a == seed_b))
        return 1;
    if (!expect_true("partition epoch changes seed", seed_a != seed_c))
        return 1;
    if (!expect_true("world id changes seed", seed_a != seed_d))
        return 1;
    if (!expect_true("default seed non-zero", seed_e != 0u))
        return 1;

    printf("runtime_terrain_seed_derivation_test: pass\n");
    return 0;
}
