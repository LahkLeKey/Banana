/**
 * @file terrain_chunks.h
 * @brief Server-side terrain chunk management for infinite procedural generation
 *
 * Manages chunked terrain generation, caching, and player-location-driven generation.
 * Each chunk is a fixed-size grid of heightfield and biome data, identified by
 * chunk coordinates (cx, cz). Chunks are generated deterministically from a seed.
 *
 * Design: Server maintains a cache of recently-generated chunks keyed by (cx, cz).
 * When a player moves, new chunks around the player position are generated on-demand.
 * Chunks can be evicted from cache when players move far away.
 */

#ifndef BANANA_ENGINE_TERRAIN_CHUNKS_H
#define BANANA_ENGINE_TERRAIN_CHUNKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /**
     * Chunk size in tiles (each tile is world_tile_size units)
     * A 64x64 chunk with 1.0 tile size = 64x64 world units
     */
#define TERRAIN_CHUNK_SIZE 64

    /**
     * Biome types for procedural generation
     */
    typedef enum
    {
        BIOME_PLAINS = 0,
        BIOME_FOREST = 1,
        BIOME_HILL = 2,
        BIOME_RIDGE = 3,
        BIOME_WATER = 4,
        BIOME_COUNT = 5
    } BiomeType;

    /**
     * Single terrain chunk: heightfield + biome map
     */
    typedef struct
    {
        int chunk_x, chunk_z;            /* Chunk coordinates */
        uint8_t heights[TERRAIN_CHUNK_SIZE][TERRAIN_CHUNK_SIZE];  /* Elevation 0-255 */
        uint8_t biomes[TERRAIN_CHUNK_SIZE][TERRAIN_CHUNK_SIZE];   /* BiomeType */
        int64_t generation_tick;         /* Server tick when generated */
        uint64_t generation_fingerprint; /* Deterministic terrain+biome fingerprint */
        uint64_t boundary_hash;          /* Deterministic seam/boundary hash */
        int version;                     /* For cache invalidation */
        int is_dirty;                    /* Needs rebuild */
    } TerrainChunk;

    typedef struct
    {
        int chunk_x;
        int chunk_z;
        int partition_x;
        int partition_z;
    } TerrainChunkPartitionTuple;

    /**
     * Chunk cache entry
     */
    typedef struct
    {
        TerrainChunk chunk;
        int64_t last_accessed_tick;  /* For LRU eviction */
        int ref_count;               /* Reference count for active players */
    } ChunkCacheEntry;

    /**
     * Initialize the terrain chunk system.
     * seed: deterministic seed for reproducible generation
     * cache_size: max number of chunks to keep in memory
     */
    int terrain_chunks_init(uint32_t seed, int cache_size);

    /**
     * Get or generate a chunk at (cx, cz).
     * If chunk is in cache and valid, returns cached version.
     * Otherwise generates new chunk deterministically from seed.
     * Returns pointer to chunk data (valid until next terrain_chunks_tick call).
     */
    const TerrainChunk *terrain_chunks_get(int chunk_x, int chunk_z);

    /**
     * Update terrain generation based on player locations.
     * Player locations drive which chunks should be generated/cached.
     * Generates new chunks in radius around each player.
     * Evicts distant chunks if cache is full.
     *
     * player_positions: array of (x, z) positions
     * player_count: number of players
     * chunk_radius: generate chunks within this radius of players (in chunk units)
     */
    int terrain_chunks_update_for_players(const float (*player_positions)[2], int player_count,
                                          int chunk_radius);

    uint64_t terrain_chunks_area_identity_hash(const char *world_id,
                                               const char *lane_id,
                                               int chunk_x,
                                               int chunk_z,
                                               uint32_t partition_epoch);

    int terrain_chunks_partition_tuple_from_world(float world_x,
                                                  float world_z,
                                                  int partition_span_chunks,
                                                  TerrainChunkPartitionTuple *out_tuple);

    int terrain_chunks_collect_adjacency_order(int chunk_x,
                                               int chunk_z,
                                               int radius,
                                               int (*out_coords)[2],
                                               int max_coords);

    uint64_t terrain_chunks_compute_boundary_hash(const TerrainChunk *chunk);

    /**
     * Get the chunk coordinates for a world position.
     */
    void terrain_chunks_world_to_chunk(float world_x, float world_z, int *out_chunk_x,
                                       int *out_chunk_z);

    /**
     * Get the world position of a chunk's origin.
     */
    void terrain_chunks_chunk_to_world(int chunk_x, int chunk_z, float *out_world_x,
                                       float *out_world_z);

    /**
     * Get statistics about the chunk cache.
     */
    void terrain_chunks_get_stats(int *out_cached_chunks, int *out_total_generated,
                                  int *out_cache_size);

    /**
     * Cleanup the terrain chunk system.
     */
    void terrain_chunks_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_TERRAIN_CHUNKS_H */
