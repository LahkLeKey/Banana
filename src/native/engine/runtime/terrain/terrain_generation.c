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

static float lerpf_local(float a, float b, float t)
{
    return a + (b - a) * t;
}

static float smoothstep_local(float t)
{
    t = clampf_local(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static float runtime_terrain_noise2(unsigned int x,
                                    unsigned int z,
                                    unsigned int frequency,
                                    unsigned int salt)
{
    unsigned int cell_x = x / frequency;
    unsigned int cell_z = z / frequency;
    unsigned int frac_x = x % frequency;
    unsigned int frac_z = z % frequency;
    float tx = (float)frac_x / (float)frequency;
    float tz = (float)frac_z / (float)frequency;

    unsigned int h00 = runtime_terrain_hash(cell_x + salt, cell_z + salt);
    unsigned int h10 = runtime_terrain_hash(cell_x + 1u + salt, cell_z + salt);
    unsigned int h01 = runtime_terrain_hash(cell_x + salt, cell_z + 1u + salt);
    unsigned int h11 = runtime_terrain_hash(cell_x + 1u + salt, cell_z + 1u + salt);

    float v00 = (float)(h00 & 0xFFFFu) / 65535.0f;
    float v10 = (float)(h10 & 0xFFFFu) / 65535.0f;
    float v01 = (float)(h01 & 0xFFFFu) / 65535.0f;
    float v11 = (float)(h11 & 0xFFFFu) / 65535.0f;

    tx = smoothstep_local(tx);
    tz = smoothstep_local(tz);

    return lerpf_local(lerpf_local(v00, v10, tx),
                       lerpf_local(v01, v11, tx),
                       tz);
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
    const float max_dist = center * 1.02f;

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
            float continental = runtime_terrain_noise2((unsigned int)x, (unsigned int)z, 9u, 13u);
            float detail = runtime_terrain_noise2((unsigned int)x, (unsigned int)z, 3u, 97u);
            float ridged = 1.0f - fabsf((detail * 2.0f) - 1.0f);
            float macro_shape = 1.0f - smoothstep_local((dist - 0.78f) / 0.42f);
            float land = (continental * 0.62f) + (ridged * 0.28f) + (macro_shape * 0.34f);
            int biome_index = (int)roundf(clampf_local(land, 0.0f, 1.0f) *
                                          (float)(banana_tile_count() - 1));
            int tile = banana_pick_tile_for_biome(biome_index, &rng_state);
            int elevation_jitter = (int)(banana_next_random(&rng_state) % 7u) - 3;
            int elevation_bonus = (int)roundf((land - 0.50f) * (float)terrain_max_layers);
            int layer = runtime_layer_from_asset_elevation(defs[tile].elevation_base + elevation_jitter,
                                                           min_asset_elevation,
                                                           max_asset_elevation,
                                                           terrain_max_layers);

            layer += elevation_bonus;
            if (layer < 0)
                layer = 0;
            if (layer >= terrain_max_layers)
                layer = terrain_max_layers - 1;

            if (dist > 1.06f)
                layer = 0;

            height_grid[idx] = (unsigned char)layer;
        }
    }

    return 1;
}