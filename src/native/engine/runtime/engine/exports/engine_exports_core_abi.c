#include "../../../engine.h"

#include "engine_runtime_context.h"

#include "../engine_host.h"
#include "../support/engine_aux_abi.h"
#include "../../controller/runtime/controller_runtime.h"
#include "../../abi/physics/physics_abi.h"
#include "../../terrain/terrain_abi.h"
#include "../../world/world_abi.h"

void engine_render_frame(void)
{
    runtime_engine_host_render_frame(s_engine_state.renderer);
}

void physics_step(float dt)
{
    runtime_physics_abi_step(s_engine_state.physics, dt);
}

void physics_add_body(uint32_t id, float mass, float x, float y, float z)
{
    runtime_physics_abi_add_body(s_engine_state.physics, id, mass, x, y, z);
}

void physics_update_body(uint32_t id, const float *pos, const float *vel)
{
    runtime_physics_abi_update_body(s_engine_state.physics, id, pos, vel);
}

uint32_t engine_world_spawn(int type, float x, float y, float z)
{
    return runtime_world_abi_spawn(s_engine_state.world, type, x, y, z);
}

void engine_world_tick(float dt)
{
    runtime_world_abi_tick(s_engine_state.world, dt);
}

int engine_terrain_set_height(int x, int z, int elevation)
{
    return runtime_terrain_abi_set_height(s_service_ports,
                                          &s_engine_state,
                                          x,
                                          z,
                                          elevation);
}

void engine_terrain_mark_region_dirty(int min_x, int min_z, int max_x, int max_z)
{
    runtime_terrain_abi_mark_region_dirty(s_service_ports,
                                          &s_engine_state,
                                          min_x,
                                          min_z,
                                          max_x,
                                          max_z);
}

uint32_t engine_controller_create(const char *type, float x, float y, float z)
{
    return runtime_engine_aux_controller_create(s_engine_state.controllers,
                                                BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS,
                                                &s_engine_state.controller_count,
                                                type,
                                                x,
                                                y,
                                                z);
}

uint32_t engine_controller_attach(uint32_t entity_id, const char *type)
{
    RuntimeEngineAuxContext context = runtime_engine_exports_aux_context();

    return runtime_engine_aux_controller_attach(context,
                                                BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS,
                                                &s_engine_state.controller_count,
                                                entity_id,
                                                type);
}

void engine_controller_update(uint32_t id, float dt)
{
    RuntimeEngineAuxContext context = runtime_engine_exports_aux_context();
    runtime_engine_aux_controller_update(context, id, dt);
}

void engine_controller_signal(uint32_t id, const char *sig)
{
    RuntimeEngineAuxContext context = runtime_engine_exports_aux_context();
    runtime_engine_aux_controller_signal(context, id, sig);
}

int engine_get_entity_count(void)
{
    return runtime_engine_aux_entity_count(s_engine_state.world);
}

float engine_get_entity_x(int idx)
{
    return runtime_engine_aux_entity_x(s_engine_state.world, idx);
}

float engine_get_entity_z(int idx)
{
    return runtime_engine_aux_entity_z(s_engine_state.world, idx);
}

int engine_get_entity_state(int idx)
{
    RuntimeEngineAuxContext context = runtime_engine_exports_aux_context();
    return runtime_engine_aux_entity_state(context, idx);
}

int engine_get_entity_team(int idx)
{
    Entity *entity = NULL;
    ControllerInstance *controller = NULL;

    if (!s_engine_state.world || idx < 0 || idx >= s_engine_state.world->entity_count)
        return (int)CONTROLLER_TEAM_NEUTRAL;

    entity = s_engine_state.world->entities[idx];
    if (!entity || !entity->active)
        return (int)CONTROLLER_TEAM_NEUTRAL;
    if (entity->type != ENTITY_TYPE_NPC || entity->controller_id == 0)
        return (int)CONTROLLER_TEAM_NEUTRAL;

    controller = runtime_controller_find_by_id(s_engine_state.controllers,
                                               s_engine_state.controller_count,
                                               entity->controller_id);
    if (!controller)
        return (int)CONTROLLER_TEAM_NEUTRAL;

    return (int)controller->team;
}

int engine_get_active_player_count(void)
{
    return runtime_engine_aux_active_player_count(s_engine_state.world);
}

int engine_get_parallel_sync_available(void)
{
    return runtime_engine_aux_parallel_sync_available();
}

int engine_get_parallel_sync_enabled(void)
{
    return runtime_engine_aux_parallel_sync_available();
}
