#include "terrain_abi.h"

float runtime_terrain_abi_sample_height(const RuntimeApplicationServicePorts *ports,
                                        const EngineRuntimeState *state,
                                        float x,
                                        float z)
{
    if (!ports || !ports->terrain.sample_height)
        return 0.f;

    return ports->terrain.sample_height(state, x, z);
}

int runtime_terrain_abi_set_height(const RuntimeApplicationServicePorts *ports,
                                   EngineRuntimeState *state,
                                   int x,
                                   int z,
                                   int elevation)
{
    if (!ports || !ports->terrain.set_height)
        return -1;

    return ports->terrain.set_height(state, x, z, elevation);
}

void runtime_terrain_abi_mark_region_dirty(const RuntimeApplicationServicePorts *ports,
                                           EngineRuntimeState *state,
                                           int min_x,
                                           int min_z,
                                           int max_x,
                                           int max_z)
{
    if (!ports || !ports->terrain.mark_region_dirty)
        return;

    ports->terrain.mark_region_dirty(state, min_x, min_z, max_x, max_z);
}