#ifndef BANANA_ENGINE_RUNTIME_ENGINE_LIFECYCLE_H
#define BANANA_ENGINE_RUNTIME_ENGINE_LIFECYCLE_H

#include "../../ai/controller.h"
#include "engine_state.h"
#include "../../world/world.h"
#include "../player/player_registry.h"
#include "../terrain/terrain_runtime.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef uint32_t (*RuntimeAttachControllerFn)(uint32_t entity_id, const char *type_name);

    int runtime_engine_lifecycle_build_terrain(unsigned char *height_grid,
                                               int terrain_size,
                                               int terrain_max_layers,
                                               int chunk_size,
                                               RuntimeTerrainChunk *chunks,
                                               int chunk_cols,
                                               int chunk_rows);

    int runtime_engine_lifecycle_bootstrap_primary_player(World *world,
                                                          EntityId player_id,
                                                          RuntimeTerrainSampleFn terrain_sample_height);

    int runtime_engine_lifecycle_spawn_default_actors(World *world,
                                                      RuntimeTerrainSampleFn terrain_sample_height,
                                                      RuntimeAttachControllerFn attach_controller);

    int runtime_engine_lifecycle_preflight_launch_gate(EngineRuntimeState *state);

    void runtime_engine_lifecycle_destroy_controllers(ControllerInstance **controllers,
                                                      int max_controllers,
                                                      int *inout_controller_count);

    void runtime_engine_lifecycle_reset_terrain_chunks(RuntimeTerrainChunk *chunks,
                                                       int total_chunks);

#ifdef __cplusplus
}
#endif

#endif
