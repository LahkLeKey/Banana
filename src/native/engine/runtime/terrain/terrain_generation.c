#include "terrain_generation.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <math.h>
#include <string.h>

#include "../tools/domain/banana_asset_domain.h"

static unsigned int runtime_terrain_hash(unsigned int x, unsigned int z)
{
    unsigned int n = x * 374761393u + z * 668265263u + 20260509u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

static unsigned int runtime_terrain_hash_seeded(unsigned int x,
                                                unsigned int z,
                                                unsigned int seed)
{
    return runtime_terrain_hash(x ^ (seed * 2246822519u), z ^ (seed * 3266489917u));
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

static float runtime_terrain_noise2_seeded(unsigned int x,
                                           unsigned int z,
                                           unsigned int frequency,
                                           unsigned int salt,
                                           unsigned int seed)
{
    unsigned int cell_x = x / frequency;
    unsigned int cell_z = z / frequency;
    unsigned int frac_x = x % frequency;
    unsigned int frac_z = z % frequency;
    float tx = (float)frac_x / (float)frequency;
    float tz = (float)frac_z / (float)frequency;
    unsigned int seeded_salt = salt ^ (seed * 747796405u);

    unsigned int h00 = runtime_terrain_hash_seeded(cell_x + seeded_salt, cell_z + seeded_salt, seed);
    unsigned int h10 = runtime_terrain_hash_seeded(cell_x + 1u + seeded_salt, cell_z + seeded_salt, seed);
    unsigned int h01 = runtime_terrain_hash_seeded(cell_x + seeded_salt, cell_z + 1u + seeded_salt, seed);
    unsigned int h11 = runtime_terrain_hash_seeded(cell_x + 1u + seeded_salt, cell_z + 1u + seeded_salt, seed);

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

static int runtime_terrain_generate_island_seeded(unsigned char *height_grid,
                                                  int terrain_size,
                                                  int terrain_max_layers,
                                                  unsigned int generation_seed)
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
            uint32_t rng_state = runtime_terrain_hash_seeded((unsigned int)x,
                                                             (unsigned int)z,
                                                             generation_seed);
            float dx = ((float)x - center) / max_dist;
            float dz = ((float)z - center) / max_dist;
            float dist = sqrtf(dx * dx + dz * dz);
            float continental = runtime_terrain_noise2_seeded((unsigned int)x,
                                                              (unsigned int)z,
                                                              9u,
                                                              13u,
                                                              generation_seed);
            float detail = runtime_terrain_noise2_seeded((unsigned int)x,
                                                         (unsigned int)z,
                                                         3u,
                                                         97u,
                                                         generation_seed);
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

int runtime_terrain_generate_island(unsigned char *height_grid,
                                    int terrain_size,
                                    int terrain_max_layers)
{
    return runtime_terrain_generate_island_seeded(height_grid,
                                                  terrain_size,
                                                  terrain_max_layers,
                                                  1337u);
}

static unsigned int runtime_seed_hash_string(unsigned int seed, const char *text)
{
    const unsigned char *cursor = (const unsigned char *)(text ? text : "");
    unsigned int hash = seed;

    while (*cursor != '\0')
    {
        hash ^= (unsigned int)(*cursor);
        hash *= 16777619u;
        cursor++;
    }

    return hash;
}

unsigned int runtime_terrain_derive_world_seed(const char *world_id,
                                               const char *lane_id,
                                               unsigned int partition_epoch)
{
    const char *normalized_world_id = (world_id && world_id[0] != '\0') ? world_id : "default-world";
    const char *normalized_lane_id = (lane_id && lane_id[0] != '\0') ? lane_id : "default-lane";
    unsigned int seed = 2166136261u;

    seed = runtime_seed_hash_string(seed, normalized_world_id);
    seed ^= 0x9e3779b9u;
    seed *= 16777619u;

    seed = runtime_seed_hash_string(seed, normalized_lane_id);
    seed ^= partition_epoch;
    seed *= 16777619u;

    if (seed == 0u)
        seed = 1u;

    return seed;
}

unsigned int runtime_terrain_generation_input_fingerprint(
    const char *world_id,
    const char *lane_id,
    unsigned int partition_epoch,
    int chunk_x,
    int chunk_z,
    const char *generation_contract_version,
    unsigned int retry_attempt)
{
    unsigned int hash = 2166136261u;
    const char *contract_version = generation_contract_version ? generation_contract_version
                                                               : RUNTIME_TERRAIN_GENERATION_CONTRACT_V1;

    hash = runtime_seed_hash_string(hash, world_id ? world_id : "");
    hash ^= (unsigned int)chunk_x;
    hash *= 16777619u;
    hash = runtime_seed_hash_string(hash, lane_id ? lane_id : "");
    hash ^= (unsigned int)chunk_z;
    hash *= 16777619u;
    hash ^= partition_epoch;
    hash *= 16777619u;
    hash = runtime_seed_hash_string(hash, contract_version);
    hash ^= retry_attempt;
    hash *= 16777619u;

    if (hash == 0u)
        hash = 1u;
    return hash;
}

int runtime_terrain_generate_unexplored_contract(
    unsigned char *height_grid,
    int terrain_size,
    int terrain_max_layers,
    const char *world_id,
    const char *lane_id,
    unsigned int partition_epoch,
    int chunk_x,
    int chunk_z,
    const char *generation_contract_version,
    unsigned int retry_attempt,
    int inject_failure_code,
    unsigned int *out_generation_input_fingerprint)
{
    unsigned int world_seed = 0u;
    unsigned int generation_seed = 0u;
    const char *contract_version = generation_contract_version ? generation_contract_version
                                                               : RUNTIME_TERRAIN_GENERATION_CONTRACT_V1;

    if (!height_grid || !world_id || !lane_id || !out_generation_input_fingerprint ||
        world_id[0] == '\0' || lane_id[0] == '\0')
    {
        return RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY;
    }

    if (strcmp(contract_version, RUNTIME_TERRAIN_GENERATION_CONTRACT_V1) != 0)
        return RUNTIME_TERRAIN_GENERATION_STATUS_CONTRACT_VERSION_UNSUPPORTED;

    *out_generation_input_fingerprint = runtime_terrain_generation_input_fingerprint(
        world_id,
        lane_id,
        partition_epoch,
        chunk_x,
        chunk_z,
        contract_version,
        retry_attempt);

    if (inject_failure_code != 0)
        return inject_failure_code;

    world_seed = runtime_terrain_derive_world_seed(world_id, lane_id, partition_epoch);
    generation_seed = runtime_terrain_hash_seeded((unsigned int)(chunk_x + 4096),
                                                  (unsigned int)(chunk_z + 4096),
                                                  world_seed ^ *out_generation_input_fingerprint);
    generation_seed ^= (retry_attempt * 2246822519u);
    generation_seed ^= world_seed;

    if (!runtime_terrain_generate_island_seeded(height_grid,
                                                terrain_size,
                                                terrain_max_layers,
                                                generation_seed))
    {
        return RUNTIME_TERRAIN_GENERATION_STATUS_DEPENDENCY_UNAVAILABLE;
    }

    return RUNTIME_TERRAIN_GENERATION_STATUS_OK;
}