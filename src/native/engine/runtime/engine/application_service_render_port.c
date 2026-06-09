#include "application_service_render_port.h"

#include "gameplay_model_profile.h"

#include "../controller/kind/controller_kind_domain.h"
#include "../controller/runtime/controller_runtime.h"
#include "../render/material/render_material.h"
#include "../render/submit/render_submit.h"
#include "../terrain/terrain_height.h"

#include <string.h>

typedef struct RuntimeGameplayModelMeshCacheEntry
{
    char model_id[96];
    Mesh *mesh;
} RuntimeGameplayModelMeshCacheEntry;

#define RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY 32

static RuntimeGameplayModelMeshCacheEntry s_model_mesh_cache[RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY];
static int s_model_mesh_cache_count = 0;
static EngineRuntimeState *s_render_state = NULL;

static void runtime_model_mesh_cache_reset(void)
{
    int index = 0;

    for (index = 0; index < s_model_mesh_cache_count; index++)
    {
        if (s_model_mesh_cache[index].mesh)
        {
            mesh_destroy(s_model_mesh_cache[index].mesh);
            s_model_mesh_cache[index].mesh = NULL;
        }
        s_model_mesh_cache[index].model_id[0] = '\0';
    }

    s_model_mesh_cache_count = 0;
}

static Mesh *runtime_model_mesh_for_gameplay_model_id(const char *model_id)
{
    int index = 0;
    float radius_scale = 0.0f;
    float length_scale = 0.0f;
    float curve_scale = 0.0f;
    float tip_taper = 0.0f;
    int quality = 0;

    if (!model_id || model_id[0] == '\0')
        return NULL;

    for (index = 0; index < s_model_mesh_cache_count; index++)
    {
        if (strcmp(s_model_mesh_cache[index].model_id, model_id) == 0)
            return s_model_mesh_cache[index].mesh;
    }

    if (s_model_mesh_cache_count >= RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY)
        return NULL;

    if (!runtime_gameplay_model_vector_profile_for_model_id(model_id,
                                                            &radius_scale,
                                                            &length_scale,
                                                            &curve_scale,
                                                            &tip_taper,
                                                            &quality))
    {
        return NULL;
    }

    s_model_mesh_cache[s_model_mesh_cache_count].mesh =
        mesh_create_banana_vector(radius_scale,
                                  length_scale,
                                  curve_scale,
                                  tip_taper,
                                  quality);
    if (!s_model_mesh_cache[s_model_mesh_cache_count].mesh)
        return NULL;

    strncpy(s_model_mesh_cache[s_model_mesh_cache_count].model_id,
            model_id,
            sizeof(s_model_mesh_cache[s_model_mesh_cache_count].model_id) - 1);
    s_model_mesh_cache[s_model_mesh_cache_count]
        .model_id[sizeof(s_model_mesh_cache[s_model_mesh_cache_count].model_id) - 1] = '\0';

    s_model_mesh_cache_count += 1;
    return s_model_mesh_cache[s_model_mesh_cache_count - 1].mesh;
}

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

    model_mesh = runtime_model_mesh_for_gameplay_model_id(entity->gameplay_model_id);
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
    runtime_model_mesh_cache_reset();
}