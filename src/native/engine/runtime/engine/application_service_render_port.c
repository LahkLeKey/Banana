#include "application_service_render_port.h"
#include "application_service_render_model_cache.h"

#include "../controller/kind/controller_kind_domain.h"
#include "../controller/runtime/controller_runtime.h"
#include "../render/material/render_material.h"
#include "../render/submit/render_submit.h"
#include "../terrain/terrain_height.h"

#include <string.h>
static EngineRuntimeState *s_render_state = NULL;

static float terrain_sample_height_for_render(const EngineRuntimeState *state,
                                              float x,
                                              float z)
{
    if (!state)
        return 0.f;

    return runtime_terrain_sample_height(&state->terrain_height[0][0],
                                         BANANA_ENGINE_TERRAIN_SIZE,
                                         x,
                                         z,
                                         0.48f,
                                         -1.45f);
}

static Mesh *runtime_render_mesh_for_actor(const Entity *entity, Mesh *default_mesh)
{
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;
    Mesh *model_mesh = NULL;

    if (!entity || !s_render_state)
        return default_mesh;

    kind = runtime_controller_kind_parse_or_unknown(entity->controller_kind);

    model_mesh = runtime_application_render_model_mesh_for_gameplay_model_id(entity->gameplay_model_id);
    if (model_mesh)
        return model_mesh;

    if (entity->type == ENTITY_TYPE_STATIC && strcmp(entity->controller_kind, "pbj_pickup") == 0)
    {
        if (s_render_state->pbj_pickup_mesh)
            return s_render_state->pbj_pickup_mesh;
    }

    if (entity->type == ENTITY_TYPE_NPC && entity->controller_id != 0)
    {
        ControllerInstance *controller = runtime_controller_find_by_id(s_render_state->controllers,
                                                                        s_render_state->controller_count,
                                                                        entity->controller_id);
        if (controller)
        {
            if (controller->team == CONTROLLER_TEAM_BANANA && s_render_state->banana_team_mesh)
                return s_render_state->banana_team_mesh;
            if (controller->team == CONTROLLER_TEAM_BEAN && s_render_state->bean_team_mesh)
                return s_render_state->bean_team_mesh;
        }
    }

    if (entity->type == ENTITY_TYPE_STATIC && kind == RUNTIME_CONTROLLER_KIND_CAMP)
    {
        if (s_render_state->landmark_mesh)
            return s_render_state->landmark_mesh;
    }

    if ((entity->type == ENTITY_TYPE_DYNAMIC && kind == RUNTIME_CONTROLLER_KIND_RESOURCE) ||
        (entity->type == ENTITY_TYPE_TRIGGER && kind == RUNTIME_CONTROLLER_KIND_QUEST))
    {
        if (s_render_state->traversal_mesh)
            return s_render_state->traversal_mesh;
    }

    return default_mesh;
}

static void runtime_render_command_for_actor(const Entity *entity,
                                             Mesh *resolved_mesh,
                                             Material resolved_material,
                                             RendererDrawCommand *out_command)
{
    float render_y = 0.0f;

    if (!entity || !resolved_mesh || !out_command)
        return;

    out_command->mesh = resolved_mesh;
    out_command->position[0] = entity->position[0];
    render_y = entity->position[1];

    if (s_render_state && entity->type != ENTITY_TYPE_PLAYER)
    {
        float ground_y = terrain_sample_height_for_render(s_render_state,
                                                          entity->position[0],
                                                          entity->position[2]);
        float min_visual_y = ground_y + (entity->scale[1] * 0.45f) + 0.12f;
        if (render_y < min_visual_y)
            render_y = min_visual_y;
    }

    out_command->position[1] = render_y;
    out_command->position[2] = entity->position[2];
    out_command->rotation[0] = 0.f;
    out_command->rotation[1] = 0.f;
    out_command->rotation[2] = 0.f;
    out_command->rotation[3] = 1.f;
    out_command->scale[0] = entity->scale[0];
    out_command->scale[1] = entity->scale[1];
    out_command->scale[2] = entity->scale[2];
    out_command->material = resolved_material;
}

static void terrain_draw_indirect(void)
{
    runtime_terrain_draw(s_render_state->renderer,
                         s_render_state->terrain_chunks,
                         s_render_state->terrain_initialized,
                         BANANA_ENGINE_TERRAIN_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                         BANANA_ENGINE_TERRAIN_CHUNK_ROWS);
}

void runtime_application_render_submit_scene_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    s_render_state = state;
    runtime_render_submit_frame(state->renderer,
                                state->world,
                                state->entity_mesh,
                                state->terrain_initialized,
                                terrain_draw_indirect,
                                runtime_render_material_for_actor,
                                runtime_render_mesh_for_actor,
                                runtime_render_command_for_actor);
    s_render_state = NULL;
}

void runtime_application_render_ports_reset(void)
{
    runtime_application_render_model_cache_reset();
}