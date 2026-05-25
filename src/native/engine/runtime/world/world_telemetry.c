#include "world_telemetry.h"

int runtime_world_entity_count(const World *world)
{
    if (!world)
        return 0;

    return world->entity_count;
}

float runtime_world_entity_x(const World *world, int entity_index)
{
    if (!world || entity_index < 0 || entity_index >= world->entity_count)
        return 0.f;

    return world->entities[entity_index] ? world->entities[entity_index]->position[0] : 0.f;
}

float runtime_world_entity_z(const World *world, int entity_index)
{
    if (!world || entity_index < 0 || entity_index >= world->entity_count)
        return 0.f;

    return world->entities[entity_index] ? world->entities[entity_index]->position[2] : 0.f;
}

uint64_t runtime_world_telemetry_area_identity_hash(const char *world_id,
                                                    const char *lane_id,
                                                    int chunk_x,
                                                    int chunk_z,
                                                    uint32_t partition_epoch)
{
    return terrain_chunks_area_identity_hash(world_id,
                                             lane_id,
                                             chunk_x,
                                             chunk_z,
                                             partition_epoch);
}

uint64_t runtime_world_telemetry_chunk_generation_fingerprint(const TerrainChunk *chunk)
{
    return chunk ? chunk->generation_fingerprint : 0ULL;
}

uint64_t runtime_world_telemetry_chunk_boundary_hash(const TerrainChunk *chunk)
{
    return chunk ? chunk->boundary_hash : 0ULL;
}
