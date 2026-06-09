#ifndef BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_TERRAIN_PORT_H
#define BANANA_ENGINE_RUNTIME_ENGINE_APPLICATION_SERVICE_TERRAIN_PORT_H

#include "../../engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

float runtime_application_terrain_sample_height_port(const EngineRuntimeState *state,
                                                     float x,
                                                     float z);
int runtime_application_terrain_rebuild_dirty_chunks_port(EngineRuntimeState *state,
                                                          int max_chunks);
void runtime_application_terrain_draw_port(EngineRuntimeState *state);
int runtime_application_terrain_set_height_port(EngineRuntimeState *state,
                                                int x,
                                                int z,
                                                int elevation);
void runtime_application_terrain_mark_region_dirty_port(EngineRuntimeState *state,
                                                        int min_x,
                                                        int min_z,
                                                        int max_x,
                                                        int max_z);

#ifdef __cplusplus
}
#endif

#endif