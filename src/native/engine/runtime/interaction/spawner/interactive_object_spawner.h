/**
 * @file interactive_object_spawner.h
 * @brief Spawn interactive objects (trees, ore deposits) based on terrain biome
 *
 * When a terrain chunk is generated, this system scans biome tiles and spawns
 * appropriate interactive objects at world positions. Objects are registered
 * with the interactive_collection system so they can be collected.
 */

#ifndef BANANA_ENGINE_INTERACTIVE_OBJECT_SPAWNER_H
#define BANANA_ENGINE_INTERACTIVE_OBJECT_SPAWNER_H

#include "../../terrain/terrain_chunks.h"
#include "../collection/interactive_collection.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Biome spawn rules (object type + resource reward)
     */
    typedef struct
    {
        BiomeType biome;
        int object_type_id; /* From actor_mesh_registry */
        ResourceType resource_type;
        int reward_amount;
        int max_collections;
        int cooldown_ms;
        float spawn_density; /* 0.0 - 1.0, probability per tile */
    } BiomeSpawnRule;

    /**
     * Initialize spawner system
     * @param max_rules Maximum biome spawn rules to register
     * @return 0 on success
     */
    int interactive_object_spawner_init(int max_rules);

    /**
     * Register biome spawn rule
     * @param rule Spawn rule configuration
     * @return 0 on success
     */
    int interactive_object_spawner_register_rule(const BiomeSpawnRule *rule);

    /**
     * Spawn interactive objects in a chunk based on its biome tiles
     * @param chunk Terrain chunk with biome data
     * @param chunk_x Chunk coordinate X
     * @param chunk_z Chunk coordinate Z
     * @return Number of objects spawned, or -1 on error
     */
    int interactive_object_spawner_spawn_chunk(const TerrainChunk *chunk, int chunk_x,
                                               int chunk_z);

    /**
     * Get current spawned object count
     */
    int interactive_object_spawner_count(void);

    /**
     * Despawn all objects in a chunk
     * @return Number of objects despawned
     */
    int interactive_object_spawner_despawn_chunk(int chunk_x, int chunk_z);

    /**
     * Cleanup system
     */
    void interactive_object_spawner_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_INTERACTIVE_OBJECT_SPAWNER_H */
