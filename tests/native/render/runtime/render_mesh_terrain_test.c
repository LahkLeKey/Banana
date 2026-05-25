#include "render/mesh.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    unsigned char heights[4] = {0, 1, 2, 3};
    unsigned char heights_copy[4] = {0, 1, 2, 3};
    unsigned char heights_changed[4] = {0, 1, 2, 4};
    uint64_t signature = mesh_terrain_heightfield_signature(heights, 2, 2, 1.0f, 0.5f);

    Mesh *valid = mesh_create_terrain_heightfield(heights, 2, 2, 1.0f, 0.5f);
    if (!expect_int("valid terrain mesh", valid != NULL ? 1 : 0, 1))
        return 1;

    if (!expect_int("signature generated", signature != 0ULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("parity match same data",
                    mesh_terrain_heightfield_parity_match(heights,
                                                          heights_copy,
                                                          2,
                                                          2,
                                                          1.0f,
                                                          0.5f),
                    1))
        return 1;
    if (!expect_int("parity mismatch changed data",
                    mesh_terrain_heightfield_parity_match(heights,
                                                          heights_changed,
                                                          2,
                                                          2,
                                                          1.0f,
                                                          0.5f),
                    0))
        return 1;

    Mesh *invalid = mesh_create_terrain_heightfield(NULL, 2, 2, 1.0f, 0.5f);
    if (!expect_int("null heights invalid", invalid == NULL ? 1 : 0, 1))
        return 1;

    mesh_destroy(valid);
    return 0;
}
