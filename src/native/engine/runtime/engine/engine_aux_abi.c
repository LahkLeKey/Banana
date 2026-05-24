#include "engine_aux_abi.h"

#include "../controller/controller_attach.h"
#include "../controller/controller_runtime.h"
#include "../controller/controller_sync.h"
#include "../world/world_metrics.h"
#include "../world/world_telemetry.h"

RuntimeEngineAuxContext runtime_engine_aux_context(World *world,
                                                   ControllerInstance **controllers,
                                                   int controller_count)
{
    RuntimeEngineAuxContext context;
    context.world = world;
    context.controllers = controllers;
    context.controller_count = controller_count;
    return context;
}

uint32_t runtime_engine_aux_controller_create(ControllerInstance **controllers,
                                              int max_active_controllers,
                                              int *inout_controller_count,
                                              const char *type,
                                              float x,
                                              float y,
                                              float z)
{
    return runtime_controller_create_and_register(controllers,
                                                  max_active_controllers,
                                                  inout_controller_count,
                                                  type,
                                                  x,
                                                  y,
                                                  z);
}

uint32_t runtime_engine_aux_controller_attach(RuntimeEngineAuxContext context,
                                              int max_active_controllers,
                                              int *inout_controller_count,
                                              uint32_t entity_id,
                                              const char *type)
{
    return runtime_controller_attach_to_entity(context.world,
                                               context.controllers,
                                               max_active_controllers,
                                               inout_controller_count,
                                               entity_id,
                                               type);
}

int runtime_engine_aux_controller_update(RuntimeEngineAuxContext context,
                                         uint32_t controller_id,
                                         float dt)
{
    return runtime_controller_update_by_id(context.controllers,
                                           context.controller_count,
                                           controller_id,
                                           dt);
}

int runtime_engine_aux_controller_signal(RuntimeEngineAuxContext context,
                                         uint32_t controller_id,
                                         const char *signal_name)
{
    return runtime_controller_signal_by_id(context.controllers,
                                           context.controller_count,
                                           controller_id,
                                           signal_name,
                                           NULL);
}

int runtime_engine_aux_entity_count(World *world)
{
    return runtime_world_entity_count(world);
}

float runtime_engine_aux_entity_x(World *world, int index)
{
    return runtime_world_entity_x(world, index);
}

float runtime_engine_aux_entity_z(World *world, int index)
{
    return runtime_world_entity_z(world, index);
}

int runtime_engine_aux_entity_state(RuntimeEngineAuxContext context, int index)
{
    return runtime_world_entity_state(context.world,
                                      index,
                                      context.controllers,
                                      context.controller_count);
}

int runtime_engine_aux_active_player_count(World *world)
{
    return runtime_world_active_player_count(world);
}

int runtime_engine_aux_parallel_sync_available(void)
{
    return runtime_controller_sync_parallel_available();
}