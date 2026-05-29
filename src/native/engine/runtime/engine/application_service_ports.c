#include "application_service_ports.h"
#include "gameplay_model_profile.h"

#include "../input/move_target/move_target_service.h"
#include "../player/player_runtime_service.h"
#include "../controller/kind/controller_kind_domain.h"
#include "../controller/runtime/controller_runtime.h"
#include "../render/material/render_material.h"
#include "../render/submit/render_submit.h"
#include "../terrain/terrain_height.h"
#include "../terrain/terrain_host.h"
#include "../terrain/terrain_mutation.h"

#include <string.h>

typedef struct RuntimeGameplayModelMeshCacheEntry
{
    char model_id[96];
    Mesh *mesh;
} RuntimeGameplayModelMeshCacheEntry;

#define RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY 32

static RuntimeGameplayModelMeshCacheEntry s_model_mesh_cache[RUNTIME_GAMEPLAY_MODEL_MESH_CACHE_CAPACITY];
static int s_model_mesh_cache_count = 0;

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

static float terrain_sample_height_port(const EngineRuntimeState *state, float x, float z)
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

static int terrain_rebuild_dirty_chunks_port(EngineRuntimeState *state, int max_chunks)
{
    if (!state)
        return 0;

    return runtime_terrain_host_rebuild_dirty(&state->terrain_height[0][0],
                                              BANANA_ENGINE_TERRAIN_SIZE,
                                              BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                              state->terrain_chunks,
                                              BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                              BANANA_ENGINE_TERRAIN_CHUNK_ROWS,
                                              max_chunks);
}

static void terrain_draw_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_terrain_draw(state->renderer,
                         state->terrain_chunks,
                         state->terrain_initialized,
                         BANANA_ENGINE_TERRAIN_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                         BANANA_ENGINE_TERRAIN_CHUNK_ROWS);
}

static EngineRuntimeState *s_mutation_state = NULL;

static void mark_chunk_dirty_port(int chunk_x, int chunk_z)
{
    if (!s_mutation_state)
        return;

    runtime_terrain_host_mark_chunk_dirty(s_mutation_state->terrain_chunks,
                                          BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                          BANANA_ENGINE_TERRAIN_CHUNK_ROWS,
                                          chunk_x,
                                          chunk_z);
}

static int terrain_set_height_port(EngineRuntimeState *state, int x, int z, int elevation)
{
    int result = 0;

    if (!state)
        return -1;

    s_mutation_state = state;
    result = runtime_terrain_set_height(&state->terrain_height[0][0],
                                        BANANA_ENGINE_TERRAIN_SIZE,
                                        BANANA_ENGINE_TERRAIN_MAX_LAYERS,
                                        BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                        x,
                                        z,
                                        elevation,
                                        mark_chunk_dirty_port);
    s_mutation_state = NULL;
    return result;
}

static void terrain_mark_region_dirty_port(EngineRuntimeState *state,
                                           int min_x,
                                           int min_z,
                                           int max_x,
                                           int max_z)
{
    if (!state)
        return;

    s_mutation_state = state;
    runtime_terrain_mark_region_dirty(BANANA_ENGINE_TERRAIN_SIZE,
                                      BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                      min_x,
                                      min_z,
                                      max_x,
                                      max_z,
                                      mark_chunk_dirty_port);
    s_mutation_state = NULL;
}

static void player_update_motion_port(EngineRuntimeState *state,
                                      float dt,
                                      RuntimeTerrainSampleHeightFn sample_height)
{
    if (!state || !sample_height)
        return;

    runtime_player_runtime_update_motion(state->world,
                                         state->player_id,
                                         &state->move_target_state,
                                         &state->move_input_x,
                                         &state->move_input_z,
                                         state->camera_eye,
                                         state->camera_target,
                                         state->camera_valid,
                                         5.5f,
                                         dt,
                                         (float)(BANANA_ENGINE_TERRAIN_SIZE - 1) * 0.5f,
                                         sample_height);
}

static void player_follow_camera_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_player_runtime_follow_camera(state->renderer,
                                         state->world,
                                         state->player_id,
                                         state->viewport_width,
                                         state->viewport_height,
                                         state->camera_eye,
                                         state->camera_target,
                                         &state->camera_valid);
}

static void player_apply_click_input_port(EngineRuntimeState *state,
                                          float normalized_x,
                                          float normalized_y,
                                          RuntimeTerrainSampleHeightFn sample_height)
{
    RuntimeScreenNormalizedPoint normalized = {0};
    RuntimeViewportSize viewport = {0};

    if (!state || !sample_height)
        return;

    normalized.x = normalized_x;
    normalized.y = normalized_y;
    viewport.width = state->viewport_width;
    viewport.height = state->viewport_height;

    (void)runtime_move_target_apply_click(&state->move_target_state,
                                          normalized,
                                          state->camera_eye,
                                          state->camera_target,
                                          state->camera_valid,
                                          viewport,
                                          (float)(BANANA_ENGINE_TERRAIN_SIZE - 1) * 0.5f,
                                          sample_height);
}

static Mesh *runtime_render_mesh_for_actor(const Entity *entity, Mesh *default_mesh)
{
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;
    Mesh *model_mesh = NULL;

    if (!entity || !s_mutation_state)
        return default_mesh;

    kind = runtime_controller_kind_parse_or_unknown(entity->controller_kind);

    model_mesh = runtime_model_mesh_for_gameplay_model_id(entity->gameplay_model_id);
    if (model_mesh)
        return model_mesh;

    if (entity->type == ENTITY_TYPE_STATIC && strcmp(entity->controller_kind, "pbj_pickup") == 0)
    {
        if (s_mutation_state->pbj_pickup_mesh)
            return s_mutation_state->pbj_pickup_mesh;
    }

    if (entity->type == ENTITY_TYPE_NPC && entity->controller_id != 0)
    {
        ControllerInstance *controller = runtime_controller_find_by_id(s_mutation_state->controllers,
                                                                        s_mutation_state->controller_count,
                                                                        entity->controller_id);
        if (controller)
        {
            if (controller->team == CONTROLLER_TEAM_BANANA && s_mutation_state->banana_team_mesh)
                return s_mutation_state->banana_team_mesh;
            if (controller->team == CONTROLLER_TEAM_BEAN && s_mutation_state->bean_team_mesh)
                return s_mutation_state->bean_team_mesh;
        }
    }

    if (entity->type == ENTITY_TYPE_STATIC && kind == RUNTIME_CONTROLLER_KIND_CAMP)
    {
        if (s_mutation_state->landmark_mesh)
            return s_mutation_state->landmark_mesh;
    }

    if ((entity->type == ENTITY_TYPE_DYNAMIC && kind == RUNTIME_CONTROLLER_KIND_RESOURCE) ||
        (entity->type == ENTITY_TYPE_TRIGGER && kind == RUNTIME_CONTROLLER_KIND_QUEST))
    {
        if (s_mutation_state->traversal_mesh)
            return s_mutation_state->traversal_mesh;
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

    /* Keep non-player actors visibly above terrain even if runtime state drifts below slopes.
       This prevents "green box corners" from poking through terrain edges. */
    if (s_mutation_state && entity->type != ENTITY_TYPE_PLAYER)
    {
        float ground_y = terrain_sample_height_port(s_mutation_state,
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
    terrain_draw_port(s_mutation_state);
}

static void render_submit_scene_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    s_mutation_state = state;
    runtime_render_submit_frame(state->renderer,
                                state->world,
                                state->entity_mesh,
                                state->terrain_initialized,
                                terrain_draw_indirect,
                                runtime_render_material_for_actor,
                                runtime_render_mesh_for_actor,
                                runtime_render_command_for_actor);
    s_mutation_state = NULL;
}

static const RuntimeApplicationServicePorts k_ports = {
    .terrain = {
        .sample_height = terrain_sample_height_port,
        .rebuild_dirty_chunks = terrain_rebuild_dirty_chunks_port,
        .draw = terrain_draw_port,
        .set_height = terrain_set_height_port,
        .mark_region_dirty = terrain_mark_region_dirty_port,
    },
    .player = {
        .update_motion = player_update_motion_port,
        .follow_camera = player_follow_camera_port,
        .apply_click_input = player_apply_click_input_port,
    },
    .merchant = {
        .get_price = runtime_merchant_service_get_price,
        .trade_buy = runtime_merchant_service_trade_buy,
        .trade_sell = runtime_merchant_service_trade_sell,
    },
    .render = {
        .submit_scene = render_submit_scene_port,
    },
};

const RuntimeApplicationServicePorts *runtime_application_service_ports(void)
{
    return &k_ports;
}

void runtime_application_service_ports_reset(void)
{
    runtime_model_mesh_cache_reset();
}
