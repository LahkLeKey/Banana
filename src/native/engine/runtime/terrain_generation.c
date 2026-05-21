#include "terrain_generation.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <math.h>

#include "../tools/domain/banana_asset_domain.h"

static unsigned int runtime_terrain_hash(unsigned int x, unsigned int z)
{
    unsigned int n = x * 374761393u + z * 668265263u + 20260509u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

static float clampf_local(float v, float lo, float hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static int runtime_layer_from_asset_elevation(int asset_elevation,
                                              int min_asset_elevation,
                                              int max_asset_elevation,
                                              int terrain_max_layers)
{
    int span = max_asset_elevation - min_asset_elevation;
    int normalized = 0;

    if (span <= 0 || terrain_max_layers <= 1)
        return 0;

    normalized = (asset_elevation - min_asset_elevation) * (terrain_max_layers - 1);
    normalized /= span;

    if (normalized < 0)
        return 0;
    if (normalized >= terrain_max_layers)
        return terrain_max_layers - 1;
    return normalized;
}

int runtime_terrain_generate_island(unsigned char *height_grid,
                                    int terrain_size,
                                    int terrain_max_layers)
{
    const banana_tile_def_t *defs = banana_tile_defs();
    int min_asset_elevation = defs[0].elevation_base;
    int max_asset_elevation = defs[0].elevation_base;
    const float center = (float)(terrain_size - 1) * 0.5f;
    const float max_dist = center * 0.92f;

    if (!height_grid || terrain_size <= 0 || terrain_max_layers <= 0 || max_dist <= 0.f)
        return 0;

    for (int i = 1; i < banana_tile_count(); i++)
    {
        int elevation = defs[i].elevation_base;
        if (elevation < min_asset_elevation)
            min_asset_elevation = elevation;
        if (elevation > max_asset_elevation)
            max_asset_elevation = elevation;
    }

    {
        int idx = 0;
        int cell_count = terrain_size * terrain_size;
#pragma omp parallel for schedule(static)
        for (idx = 0; idx < cell_count; idx++)
        {
            int z = idx / terrain_size;
            int x = idx % terrain_size;
            uint32_t rng_state = runtime_terrain_hash((unsigned int)x, (unsigned int)z);
            float dx = ((float)x - center) / max_dist;
            float dz = ((float)z - center) / max_dist;
            float dist = sqrtf(dx * dx + dz * dz);
            float island = 1.0f - clampf_local(dist, 0.0f, 1.0f);
            int biome_index = (int)roundf(island * (float)(banana_tile_count() - 1));
            int tile = banana_pick_tile_for_biome(biome_index, &rng_state);
            int elevation_jitter = (int)(banana_next_random(&rng_state) % 9u) - 4;
            int layer = runtime_layer_from_asset_elevation(defs[tile].elevation_base + elevation_jitter,
                                                           min_asset_elevation,
                                                           max_asset_elevation,
                                                           terrain_max_layers);

            if (dist > 1.0f)
                layer = 0;

            height_grid[idx] = (unsigned char)layer;
        }
    }

    return 1;
}