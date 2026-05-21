/**
 * @file terrain_chunks.c
 * @brief Terrain chunk system implementation
 *
 * Server-side infinite terrain management with player-location-driven generation.
 */

#include "terrain_chunks.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Simple hash for chunk coordinates: hash(cx, cz) = cx + (cz * large_prime) */
#define CHUNK_HASH_MULTIPLIER 73856093

typedef struct
{
    int chunk_x, chunk_z;
    ChunkCacheEntry *entry;
} ChunkHashEntry;

static struct
{
    uint32_t seed;
    int cache_size;
    int cached_count;
    int total_generated;
    ChunkCacheEntry *cache;
    ChunkHashEntry *hash_table;
    int hash_table_size;
} g_terrain_chunks = {0};

/**
 * Simple deterministic hash function for chunk coordinates
 */
static uint32_t chunk_hash(int cx, int cz)
{
    uint32_t h = g_terrain_chunks.seed;
    h ^= (uint32_t)cx * 2654435761U;
    h ^= (uint32_t)cz * CHUNK_HASH_MULTIPLIER;
    return h;
}

/**
 * Procedural heightfield generation for a chunk.
 * Uses simple pseudo-random noise based on chunk hash.
 */
static void generate_heightfield(int chunk_x, int chunk_z, uint8_t heights[TERRAIN_CHUNK_SIZE][TERRAIN_CHUNK_SIZE])
{
    uint32_t base_hash = chunk_hash(chunk_x, chunk_z);

    for (int z = 0; z < TERRAIN_CHUNK_SIZE; z++)
    {
        for (int x = 0; x < TERRAIN_CHUNK_SIZE; x++)
        {
            /* Mix hash with local position for variation within chunk */
            uint32_t h = base_hash;
            h ^= (uint32_t)(x + z * TERRAIN_CHUNK_SIZE) * 2654435769U;

            /* Simple height gradient: higher in center, lower at edges */
            int dx = x - TERRAIN_CHUNK_SIZE / 2;
            int dz = z - TERRAIN_CHUNK_SIZE / 2;
            int dist_sq = dx * dx + dz * dz;

            /* Base elevation from hash */
            int base_elev = (int)((h >> 8) & 0xFF);

            /* Blend with distance-based gradient */
            int max_dist_sq = (TERRAIN_CHUNK_SIZE / 2) * (TERRAIN_CHUNK_SIZE / 2);
            int gradient = 100 - (dist_sq * 100) / max_dist_sq;
            if (gradient < 0)
                gradient = 0;

            int final_height = (base_elev * 60 + gradient * 60) / 120;
            if (final_height > 255)
                final_height = 255;

            heights[z][x] = (uint8_t)final_height;
        }
    }
}

/**
 * Procedural biome generation for a chunk.
 */
static void generate_biomes(int chunk_x, int chunk_z, const uint8_t heights[TERRAIN_CHUNK_SIZE][TERRAIN_CHUNK_SIZE],
                             uint8_t biomes[TERRAIN_CHUNK_SIZE][TERRAIN_CHUNK_SIZE])
{
    uint32_t base_hash = chunk_hash(chunk_x, chunk_z);

    for (int z = 0; z < TERRAIN_CHUNK_SIZE; z++)
    {
        for (int x = 0; x < TERRAIN_CHUNK_SIZE; x++)
        {
            uint8_t height = heights[z][x];

            /* Biome based on height and position hash */
            uint32_t h = base_hash;
            h ^= (uint32_t)(x + z * TERRAIN_CHUNK_SIZE) * 2654435769U;

            if (height < 80)
            {
                /* Low areas: plains or water */
                biomes[z][x] = (h & 1) ? BIOME_PLAINS : BIOME_WATER;
            }
            else if (height < 150)
            {
                /* Mid areas: forest or plains */
                biomes[z][x] = (h & 1) ? BIOME_FOREST : BIOME_PLAINS;
            }
            else
            {
                /* High areas: hills or ridges */
                biomes[z][x] = (h & 1) ? BIOME_RIDGE : BIOME_HILL;
            }
        }
    }
}

int terrain_chunks_init(uint32_t seed, int cache_size)
{
    if (cache_size <= 0)
    {
        return -1;
    }

    g_terrain_chunks.seed = seed;
    g_terrain_chunks.cache_size = cache_size;
    g_terrain_chunks.cached_count = 0;
    g_terrain_chunks.total_generated = 0;

    g_terrain_chunks.cache = (ChunkCacheEntry *)malloc(cache_size * sizeof(ChunkCacheEntry));
    if (!g_terrain_chunks.cache)
    {
        return -1;
    }

    memset(g_terrain_chunks.cache, 0, cache_size * sizeof(ChunkCacheEntry));

    /* Hash table is 2x cache size for reasonable load factor */
    g_terrain_chunks.hash_table_size = cache_size * 2;
    g_terrain_chunks.hash_table =
        (ChunkHashEntry *)malloc(g_terrain_chunks.hash_table_size * sizeof(ChunkHashEntry));
    if (!g_terrain_chunks.hash_table)
    {
        free(g_terrain_chunks.cache);
        return -1;
    }

    memset(g_terrain_chunks.hash_table, 0, g_terrain_chunks.hash_table_size * sizeof(ChunkHashEntry));

    return 0;
}

const TerrainChunk *terrain_chunks_get(int chunk_x, int chunk_z)
{
    if (!g_terrain_chunks.cache)
    {
        return NULL;
    }

    /* Check hash table for existing chunk */
    uint32_t h = chunk_hash(chunk_x, chunk_z) % g_terrain_chunks.hash_table_size;
    int probe = 0;
    while (probe < g_terrain_chunks.hash_table_size)
    {
        ChunkHashEntry *entry = &g_terrain_chunks.hash_table[h];
        if (entry->entry == NULL)
        {
            break; /* Not found, need to generate */
        }

        if (entry->chunk_x == chunk_x && entry->chunk_z == chunk_z)
        {
            /* Found in cache */
            entry->entry->last_accessed_tick = 0; /* Update access time */
            return &entry->entry->chunk;
        }

        h = (h + 1) % g_terrain_chunks.hash_table_size;
        probe++;
    }

    /* Not in cache, need to generate */
    if (g_terrain_chunks.cached_count >= g_terrain_chunks.cache_size)
    {
        /* Cache full, evict oldest (simplest LRU: just clear first entry for now) */
        /* In production, implement proper LRU with timestamps */
        memset(&g_terrain_chunks.cache[0], 0, sizeof(ChunkCacheEntry));
        g_terrain_chunks.cached_count--;
    }

    /* Generate new chunk */
    int idx = g_terrain_chunks.cached_count++;
    ChunkCacheEntry *cache_entry = &g_terrain_chunks.cache[idx];

    cache_entry->chunk.chunk_x = chunk_x;
    cache_entry->chunk.chunk_z = chunk_z;
    cache_entry->chunk.generation_tick = 0;
    cache_entry->chunk.version = 1;
    cache_entry->chunk.is_dirty = 0;
    cache_entry->ref_count = 0;
    cache_entry->last_accessed_tick = 0;

    generate_heightfield(chunk_x, chunk_z, cache_entry->chunk.heights);
    generate_biomes(chunk_x, chunk_z, cache_entry->chunk.heights, cache_entry->chunk.biomes);

    g_terrain_chunks.total_generated++;

    /* Add to hash table */
    h = chunk_hash(chunk_x, chunk_z) % g_terrain_chunks.hash_table_size;
    while (g_terrain_chunks.hash_table[h].entry != NULL)
    {
        h = (h + 1) % g_terrain_chunks.hash_table_size;
    }
    g_terrain_chunks.hash_table[h].chunk_x = chunk_x;
    g_terrain_chunks.hash_table[h].chunk_z = chunk_z;
    g_terrain_chunks.hash_table[h].entry = cache_entry;

    return &cache_entry->chunk;
}

int terrain_chunks_update_for_players(const float (*player_positions)[2], int player_count,
                                      int chunk_radius)
{
    if (!player_positions || player_count <= 0 || chunk_radius <= 0)
    {
        return -1;
    }

    /* For each player, ensure chunks in radius are cached */
    int chunks_updated = 0;
    for (int p = 0; p < player_count; p++)
    {
        int player_chunk_x, player_chunk_z;
        terrain_chunks_world_to_chunk(player_positions[p][0], player_positions[p][1],
                                      &player_chunk_x, &player_chunk_z);

        /* Load chunks in radius */
        for (int cx = player_chunk_x - chunk_radius; cx <= player_chunk_x + chunk_radius; cx++)
        {
            for (int cz = player_chunk_z - chunk_radius; cz <= player_chunk_z + chunk_radius; cz++)
            {
                const TerrainChunk *chunk = terrain_chunks_get(cx, cz);
                if (chunk)
                {
                    chunks_updated++;
                }
            }
        }
    }

    return chunks_updated;
}

void terrain_chunks_world_to_chunk(float world_x, float world_z, int *out_chunk_x, int *out_chunk_z)
{
    if (!out_chunk_x || !out_chunk_z)
    {
        return;
    }

    /* Each chunk is TERRAIN_CHUNK_SIZE world units */
    *out_chunk_x = (int)floorf(world_x / (float)TERRAIN_CHUNK_SIZE);
    *out_chunk_z = (int)floorf(world_z / (float)TERRAIN_CHUNK_SIZE);
}

void terrain_chunks_chunk_to_world(int chunk_x, int chunk_z, float *out_world_x,
                                   float *out_world_z)
{
    if (!out_world_x || !out_world_z)
    {
        return;
    }

    *out_world_x = (float)chunk_x * TERRAIN_CHUNK_SIZE;
    *out_world_z = (float)chunk_z * TERRAIN_CHUNK_SIZE;
}

void terrain_chunks_get_stats(int *out_cached_chunks, int *out_total_generated, int *out_cache_size)
{
    if (out_cached_chunks)
        *out_cached_chunks = g_terrain_chunks.cached_count;
    if (out_total_generated)
        *out_total_generated = g_terrain_chunks.total_generated;
    if (out_cache_size)
        *out_cache_size = g_terrain_chunks.cache_size;
}

void terrain_chunks_cleanup(void)
{
    if (g_terrain_chunks.cache)
    {
        free(g_terrain_chunks.cache);
        g_terrain_chunks.cache = NULL;
    }

    if (g_terrain_chunks.hash_table)
    {
        free(g_terrain_chunks.hash_table);
        g_terrain_chunks.hash_table = NULL;
    }

    memset(&g_terrain_chunks, 0, sizeof(g_terrain_chunks));
}
