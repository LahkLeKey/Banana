#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_ABI_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_ABI_H

#include "../application_service_ports.h"

#ifdef __cplusplus
extern "C"
{
#endif

    float runtime_terrain_abi_sample_height(const RuntimeApplicationServicePorts *ports,
                                            const EngineRuntimeState *state,
                                            float x,
                                            float z);

    int runtime_terrain_abi_set_height(const RuntimeApplicationServicePorts *ports,
                                       EngineRuntimeState *state,
                                       int x,
                                       int z,
                                       int elevation);

    void runtime_terrain_abi_mark_region_dirty(const RuntimeApplicationServicePorts *ports,
                                               EngineRuntimeState *state,
                                               int min_x,
                                               int min_z,
                                               int max_x,
                                               int max_z);

#ifdef __cplusplus
}
#endif

#endif