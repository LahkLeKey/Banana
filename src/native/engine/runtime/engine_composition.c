#include "engine_composition.h"

#include "application_service_ports.h"

#include "../ai/npc_merchant.h"
#include "../ai/wildlife_controller.h"
#include "../render/backend.h"
#include "../ui/ui.h"
#include "engine_serialize.h"
#include "engine_lifecycle.h"
#include "engine_tick.h"
#include "gameplay_service.h"
#include "input_contract.h"
#include "player_builds.h"
#include "player_registry.h"

#include <stddef.h>
#include <stdio.h>

static EngineRuntimeState *s_active_state = NULL;
static RuntimeTerrainSampleHeightFn s_active_sample_height = NULL;
static const RuntimeApplicationServicePorts *s_service_ports = NULL;

static int terrain_rebuild_dirty_chunks_runtime(int max_chunks)
{
    if (!s_active_state || !s_service_ports || !s_service_ports->terrain.rebuild_dirty_chunks)
        return 0;

    return s_service_ports->terrain.rebuild_dirty_chunks(s_active_state, max_chunks);
}

static void update_player_motion(float dt)
{
    if (!s_active_state || !s_active_sample_height || !s_service_ports || !s_service_ports->player.update_motion)
        return;

    s_service_ports->player.update_motion(s_active_state, dt, s_active_sample_height);
}

static void follow_player_camera(void)
{
    if (!s_active_state || !s_service_ports || !s_service_ports->player.follow_camera)
        return;

    s_service_ports->player.follow_camera(s_active_state);
}

static void apply_click_move_input(float normalized_x, float normalized_y)
{
    if (!s_active_state || !s_active_sample_height || !s_service_ports || !s_service_ports->player.apply_click_input)
        return;

    s_service_ports->player.apply_click_input(s_active_state,
                                              normalized_x,
                                              normalized_y,
                                              s_active_sample_height);
}

static void tick_phase_gameplay(void)
{
    if (!s_active_state)
        return;

    runtime_gameplay_service_tick(s_active_state->world,
                                  s_active_state->controllers,
                                  s_active_state->controller_count,
                                  s_active_state->player_id,
                                  &s_active_state->pbj_pickup_collected,
                                  6.0f,
                                  1.55f);
}

static void tick_phase_render_scene(void)
{
    if (!s_active_state || !s_service_ports || !s_service_ports->render.submit_scene)
        return;

    s_service_ports->render.submit_scene(s_active_state);
}

int runtime_engine_composition_init(EngineRuntimeState *state,
                                    int width,
                                    int height,
                                    RuntimeTerrainSampleHeightFn sample_height,
                                    RuntimeEngineAttachControllerFn attach_controller)
{
    BananaRenderBackend requested_backend = BANANA_RENDER_BACKEND_UNKNOWN;
    BananaRenderBackend active_backend = BANANA_RENDER_BACKEND_UNKNOWN;

    if (!state || !sample_height || !attach_controller)
        return -1;

    s_service_ports = runtime_application_service_ports();
    if (!s_service_ports)
        return -1;

    if (state->engine_initialized)
        return 0;
    state->engine_initialized = 1;

    requested_backend = banana_render_backend_requested();
    active_backend = banana_render_backend_active();

    fprintf(stdout,
            "[engine] render backend requested=%s active=%s status=%s\n",
            banana_render_backend_name(requested_backend),
            banana_render_backend_name(active_backend),
            banana_render_backend_status());
    if (requested_backend != active_backend)
    {
        fprintf(stderr,
                "[engine] render backend fallback engaged for '%s'\n",
                banana_render_backend_name(requested_backend));
    }

    wildlife_controller_register();

    state->window = window_create(width, height, "Banana Engine");
    if (!state->window)
    {
        fprintf(stderr, "[engine] window_create failed\n");
        return -1;
    }

    state->renderer = renderer_create(width, height);
    if (!state->renderer)
    {
        fprintf(stderr, "[engine] renderer_create failed\n");
        return -1;
    }

    renderer_attach_native_window(state->renderer, window_get_native_handle(state->window));
    state->physics = physics_world_create();
    state->world = world_create();

    state->player_id = world_spawn_entity(state->world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
    state->entity_mesh = mesh_create_banana_vector(1.05f, 1.55f, 1.65f, 0.15f, 3);
    state->pbj_pickup_mesh = mesh_create_peanut_butter_pickup_asset(2);
    if (!state->entity_mesh || !state->pbj_pickup_mesh)
    {
        fprintf(stderr, "[engine] actor mesh creation failed\n");
        return -1;
    }

    state->viewport_width = width;
    state->viewport_height = height;

    if (player_builds_init() != 0)
        return -1;

    state->terrain_initialized = runtime_engine_lifecycle_build_terrain(&state->terrain_height[0][0],
                                                                        BANANA_ENGINE_TERRAIN_SIZE,
                                                                        BANANA_ENGINE_TERRAIN_MAX_LAYERS,
                                                                        BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                                                        state->terrain_chunks,
                                                                        BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                                                        BANANA_ENGINE_TERRAIN_CHUNK_ROWS);
    if (!state->terrain_initialized)
        return -1;

    if (!runtime_engine_lifecycle_bootstrap_primary_player(state->world,
                                                           state->player_id,
                                                           sample_height))
        return -1;

    runtime_engine_lifecycle_spawn_default_actors(state->world,
                                                  sample_height,
                                                  attach_controller);
    state->pbj_pickup_collected = 0;

    fprintf(stdout, "[engine] initialized %dx%d\n", width, height);
    engine_serialize_reset();
    return 0;
}

int runtime_engine_composition_tick(EngineRuntimeState *state,
                                    float dt,
                                    RuntimeTerrainSampleHeightFn sample_height)
{
    int result = 0;

    if (!state || !sample_height)
        return -1;

    s_active_state = state;
    s_active_sample_height = sample_height;

    result = runtime_engine_tick_execute(state->window,
                                         state->renderer,
                                         state->physics,
                                         state->world,
                                         &state->viewport_width,
                                         &state->viewport_height,
                                         terrain_rebuild_dirty_chunks_runtime,
                                         state->controllers,
                                         state->controller_count,
                                         dt,
                                         update_player_motion,
                                         follow_player_camera,
                                         apply_click_move_input,
                                         tick_phase_gameplay,
                                         tick_phase_render_scene);

    s_active_sample_height = NULL;
    s_active_state = NULL;
    return result;
}

void runtime_engine_composition_shutdown(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_engine_lifecycle_destroy_controllers(state->controllers,
                                                 BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS,
                                                 &state->controller_count);

    mesh_destroy(state->entity_mesh);
    mesh_destroy(state->pbj_pickup_mesh);
    runtime_engine_lifecycle_reset_terrain_chunks(state->terrain_chunks,
                                                  BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS);
    world_destroy(state->world);
    physics_world_destroy(state->physics);
    renderer_destroy(state->renderer);
    window_destroy(state->window);

    if (state->merchants_seeded)
        npc_merchant_shutdown();

    player_builds_cleanup();
    runtime_player_registry_reset();
    runtime_input_contract_reset();
    engine_serialize_reset();
    runtime_engine_state_reset(state);

    fprintf(stdout, "[engine] shutdown complete\n");
}

void runtime_engine_composition_apply_click_input(EngineRuntimeState *state,
                                                  float normalized_x,
                                                  float normalized_y,
                                                  RuntimeTerrainSampleHeightFn sample_height)
{
    if (!state || !sample_height || !s_service_ports || !s_service_ports->player.apply_click_input)
        return;

    s_service_ports->player.apply_click_input(state,
                                              normalized_x,
                                              normalized_y,
                                              sample_height);
}
