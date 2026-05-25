#ifndef BANANA_ENGINE_RUNTIME_WORLD_TELEMETRY_H
#define BANANA_ENGINE_RUNTIME_WORLD_TELEMETRY_H

#include "../../world/world.h"
#include "../terrain/terrain_chunks.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_world_entity_count(const World *world);
    float runtime_world_entity_x(const World *world, int entity_index);
    float runtime_world_entity_z(const World *world, int entity_index);
    uint64_t runtime_world_telemetry_area_identity_hash(const char *world_id,
                                                        const char *lane_id,
                                                        int chunk_x,
                                                        int chunk_z,
                                                        uint32_t partition_epoch);
    uint64_t runtime_world_telemetry_chunk_generation_fingerprint(const TerrainChunk *chunk);
    uint64_t runtime_world_telemetry_chunk_boundary_hash(const TerrainChunk *chunk);

#ifdef __cplusplus
}
#endif

#endif
