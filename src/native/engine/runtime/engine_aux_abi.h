#ifndef BANANA_ENGINE_RUNTIME_ENGINE_AUX_ABI_H
#define BANANA_ENGINE_RUNTIME_ENGINE_AUX_ABI_H

#include "../ai/controller.h"
#include "../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeEngineAuxContext
    {
        World *world;
        ControllerInstance **controllers;
        int controller_count;
    } RuntimeEngineAuxContext;

    uint32_t runtime_engine_aux_controller_create(ControllerInstance **controllers,
                                                  int max_active_controllers,
                                                  int *inout_controller_count,
                                                  const char *type,
                                                  float x,
                                                  float y,
                                                  float z);

    uint32_t runtime_engine_aux_controller_attach(RuntimeEngineAuxContext context,
                                                  int max_active_controllers,
                                                  int *inout_controller_count,
                                                  uint32_t entity_id,
                                                  const char *type);

    int runtime_engine_aux_controller_update(RuntimeEngineAuxContext context,
                                             uint32_t controller_id,
                                             float dt);

    int runtime_engine_aux_controller_signal(RuntimeEngineAuxContext context,
                                             uint32_t controller_id,
                                             const char *signal_name);

    int runtime_engine_aux_entity_count(World *world);

    float runtime_engine_aux_entity_x(World *world, int index);

    float runtime_engine_aux_entity_z(World *world, int index);

    int runtime_engine_aux_entity_state(RuntimeEngineAuxContext context, int index);

    int runtime_engine_aux_active_player_count(World *world);

    int runtime_engine_aux_parallel_sync_available(void);

#ifdef __cplusplus
}
#endif

#endif