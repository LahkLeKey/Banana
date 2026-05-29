#include "engine_composition.h"

#include "application_service_ports.h"

#include "../orchestration/tick/runtime_tick_orchestration.h"

#include "../../ai/npc_merchant.h"
#include "../../ai/combat_controller.h"
#include "../../ai/wildlife_controller.h"
#include "../../render/backend.h"
#include "../../ui/ui.h"
#include "../../engine_serialize.h"
#include "engine_lifecycle.h"
#include "engine_tick.h"
#include "../input/contract/input_contract.h"
#include "../player/player_builds.h"
#include "../player/player_registry.h"
#include "../tick/tick_budget_policy.h"

#include <stddef.h>
#include <stdio.h>

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

    state->service_ports = runtime_application_service_ports();
    if (!state->service_ports)
        return -1;

    if (runtime_engine_lifecycle_preflight_launch_gate(state) != 0)
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
    combat_controller_register();

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
    state->banana_team_mesh = mesh_create_banana_block_like(3);
    state->bean_team_mesh = mesh_create_banana_orb_like(3);
    state->landmark_mesh = mesh_create_banana_block_like(2);
    state->traversal_mesh = mesh_create_banana_orb_like(2);
    if (!state->entity_mesh || !state->pbj_pickup_mesh ||
        !state->banana_team_mesh || !state->bean_team_mesh ||
        !state->landmark_mesh || !state->traversal_mesh)
    {
        fprintf(stderr, "[engine] actor mesh creation failed\n");
        return -1;
    }

    fprintf(stdout,
            "[engine] generated-vector assets scaffolded (banana-team, bean-team, landmark, traversal)\n");

    state->viewport_width = width;
    state->viewport_height = height;
    state->war_frontier_chunks = 1;
    state->war_biome_stage_index = 1;
    state->war_terrain_expand_tick_counter = 0;
    state->war_escalation_tier = RUNTIME_WAR_ESCALATION_PEACEFUL;
    state->war_effective_reinforcements_per_tick = runtime_tick_budget_policy_controller_war_reinforcements_per_tick();
    state->war_intelligence_stage = 0;
    state->war_intelligence_progress_ticks = 0;
    state->war_biome_unlock_mask = 1u;

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
    RuntimeTickOrchestrationContext context = {0};

    if (!state || !sample_height)
        return -1;

    context.state = state;
    context.sample_height = sample_height;

    context.service_ports = state->service_ports;

    result = runtime_engine_tick_execute(state->window,
                                         state->renderer,
                                         state->physics,
                                         state->world,
                                         &state->viewport_width,
                                         &state->viewport_height,
                                         runtime_tick_orchestration_rebuild_dirty_chunks,
                                         state->controllers,
                                         state->controller_count,
                                         dt,
                                         &context,
                                         runtime_tick_orchestration_update_player_motion,
                                         runtime_tick_orchestration_follow_player_camera,
                                         runtime_tick_orchestration_apply_click_move_input,
                                         runtime_tick_orchestration_gameplay,
                                         runtime_tick_orchestration_render_scene);
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
    mesh_destroy(state->banana_team_mesh);
    mesh_destroy(state->bean_team_mesh);
    mesh_destroy(state->landmark_mesh);
    mesh_destroy(state->traversal_mesh);
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
    runtime_application_service_ports_reset();
    engine_serialize_reset();
    runtime_engine_state_reset(state);

    fprintf(stdout, "[engine] shutdown complete\n");
}

void runtime_engine_composition_apply_click_input(EngineRuntimeState *state,
                                                  float normalized_x,
                                                  float normalized_y,
                                                  RuntimeTerrainSampleHeightFn sample_height)
{
    if (!state || !sample_height || !state->service_ports || !state->service_ports->player.apply_click_input)
        return;

    state->service_ports->player.apply_click_input(state,
                                                   normalized_x,
                                                   normalized_y,
                                                   sample_height);
}
