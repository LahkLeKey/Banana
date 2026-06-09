#include "engine.h"
#include "engine_serialize.h"
#include "render/backend.h"
#include "ai/npc_merchant.h"
#include "ai/wildlife_controller.h"
#include "runtime/controller/sync/controller_sync.h"
#include "runtime/controller/runtime/controller_runtime.h"
#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/engine_aux_abi.h"
#include "runtime/engine/composition/engine_composition.h"
#include "runtime/engine/engine_host.h"
#include "runtime/engine/state/engine_state.h"
#include "runtime/engine/lifecycle/engine_lifecycle.h"
#include "runtime/engine/tick/engine_tick.h"
#include "runtime/engine/gameplay/service/gameplay_service.h"
#include "runtime/camera/follow/camera_follow.h"
#include "runtime/camera/basis/camera_basis.h"
#include "runtime/input/move_target/move_target_service.h"
#include "runtime/player/runtime/player_runtime_service.h"
#include "runtime/player/gateway/player_gateway_abi.h"
#include "runtime/render/material/render_material.h"
#include "runtime/player/motion/player_motion.h"
#include "runtime/player/runtime/player_motion_host.h"
#include "runtime/player/api/player_api.h"
#include "runtime/player/build/abi/player_build_abi.h"
#include "runtime/merchant/query/merchant_query_abi.h"
#include "runtime/merchant/trade/merchant_trade_domain.h"
#include "runtime/player/resource/player_resource_abi.h"
#include "runtime/player/merchant/player_merchant_adapter.h"
#include "runtime/player/sync/player_sync_abi.h"
#include "runtime/player/registry/player_registry.h"
#include "runtime/abi/physics/physics_abi.h"
#include "runtime/input/abi/input_abi.h"
#include "runtime/resource/resource_domain.h"
#include "runtime/render/submit/render_submit.h"
#include "runtime/terrain/terrain_generation.h"
#include "runtime/terrain/terrain_abi.h"
#include "runtime/terrain/terrain_runtime.h"
#include "runtime/tick/tick_phases.h"
#include "runtime/tick/tick_budget_policy.h"
#include "runtime/world/world_metrics.h"
#include "runtime/world/world_abi.h"
#include "runtime/world/world_telemetry.h"
#include "runtime/wildlife/wildlife_gameplay.h"
#include "runtime/abi/ui/ui_abi.h"
#include <stdio.h>
#include <string.h>

/* ── Engine runtime aggregate ─────────────────────────────────────────────── */
EngineRuntimeState s_engine_state;
const RuntimeApplicationServicePorts *s_service_ports = NULL;

#define BANANA_TERRAIN_SIZE BANANA_ENGINE_TERRAIN_SIZE
#define BANANA_TERRAIN_MAX_LAYERS BANANA_ENGINE_TERRAIN_MAX_LAYERS
#define BANANA_TERRAIN_CHUNK_SIZE BANANA_ENGINE_TERRAIN_CHUNK_SIZE
#define BANANA_TERRAIN_CHUNK_COLS BANANA_ENGINE_TERRAIN_CHUNK_COLS
#define BANANA_TERRAIN_CHUNK_ROWS BANANA_ENGINE_TERRAIN_CHUNK_ROWS
#define BANANA_TERRAIN_TOTAL_CHUNKS BANANA_ENGINE_TERRAIN_TOTAL_CHUNKS
#define BANANA_MAX_ACTIVE_CONTROLLERS BANANA_ENGINE_MAX_ACTIVE_CONTROLLERS

#define s_window (s_engine_state.window)
#define s_renderer (s_engine_state.renderer)
#define s_physics (s_engine_state.physics)
#define s_world (s_engine_state.world)
#define s_entity_mesh (s_engine_state.entity_mesh)
#define s_pbj_pickup_mesh (s_engine_state.pbj_pickup_mesh)
#define s_player_id (s_engine_state.player_id)
#define s_engine_initialized (s_engine_state.engine_initialized)
#define s_terrain_height (s_engine_state.terrain_height)
#define s_terrain_chunks (s_engine_state.terrain_chunks)
#define s_terrain_initialized (s_engine_state.terrain_initialized)
#define s_camera_eye (s_engine_state.camera_eye)
#define s_camera_target (s_engine_state.camera_target)
#define s_camera_valid (s_engine_state.camera_valid)
#define s_merchants_seeded (s_engine_state.merchants_seeded)
#define s_pbj_pickup_collected (s_engine_state.pbj_pickup_collected)
#define s_controllers (s_engine_state.controllers)
#define s_controller_count (s_engine_state.controller_count)

/* ── Deterministic terrain generation/rendering (C-owned) ───────────────── */

static float terrain_sample_height(float x, float z);

static float terrain_sample_height(float x, float z)
{
    return runtime_terrain_abi_sample_height(s_service_ports,
                                             &s_engine_state,
                                             x,
                                             z);
}

/* ── Init ────────────────────────────────────────────────────────────────── */
int engine_init(int width, int height)
{
    s_service_ports = runtime_application_service_ports();
    if (!s_service_ports)
        return -1;

    return runtime_engine_composition_init(&s_engine_state,
                                           width,
                                           height,
                                           terrain_sample_height,
                                           engine_controller_attach);
}

/* ── Tick ────────────────────────────────────────────────────────────────── */
int engine_tick(float dt)
{
    return runtime_engine_composition_tick(&s_engine_state,
                                           dt,
                                           terrain_sample_height);
}
/* ── Shutdown ────────────────────────────────────────────────────────────── */
void engine_shutdown(void)
{
    runtime_engine_composition_shutdown(&s_engine_state);
}

/* ── Debug diagnostics (exported for JavaScript) ───────────────────────── */
int engine_get_click_count(void)
{
    return runtime_input_abi_get_click_count();
}

int engine_get_target_reached_count(void)
{
    return runtime_input_abi_get_target_reached_count();
}

int engine_get_has_move_target(void)
{
    return runtime_input_abi_get_has_move_target();
}

int engine_get_pbj_pickup_collected(void)
{
    return s_pbj_pickup_collected ? 1 : 0;
}

int engine_handle_right_click(float canvas_x, float canvas_y)
{
    RuntimeInputCanvasPoint canvas;
    canvas.x = canvas_x;
    canvas.y = canvas_y;

    return runtime_input_abi_handle_right_click(&s_engine_state,
                                                canvas,
                                                terrain_sample_height);
}

int engine_handle_right_click_normalized(float screen_x, float screen_y)
{
    RuntimeScreenNormalizedPoint normalized;
    normalized.x = screen_x;
    normalized.y = screen_y;

    return runtime_input_abi_handle_right_click_normalized(&s_engine_state,
                                                           normalized,
                                                           terrain_sample_height);
}

void engine_set_move_input(float input_x, float input_z)
{
    float basis_forward[3] = {0.f, 0.f, -1.f};
    float basis_right[3] = {1.f, 0.f, 0.f};
    float local_x = input_x;
    float local_z = input_z;

    /* Public API uses world-space intent; runtime consumes local move input. */
    (void)runtime_camera_compute_ground_basis(s_camera_eye,
                                              s_camera_target,
                                              s_camera_valid,
                                              basis_forward,
                                              basis_right);

    local_x = (input_x * basis_right[0]) + (input_z * basis_right[2]);
    local_z = (input_x * basis_forward[0]) + (input_z * basis_forward[2]);

    runtime_input_abi_set_move_input(&s_engine_state, local_x, local_z);
}

uint32_t engine_player_upsert(const char *player_guid, const char *player_name,
                              const char *controller_kind, int active)
{
    return runtime_player_api_upsert(s_world,
                                     player_guid,
                                     player_name,
                                     controller_kind,
                                     active,
                                     terrain_sample_height,
                                     engine_controller_attach,
                                     &s_player_id);
}

void engine_player_apply_input(const char *player_guid, float input_x, float input_z)
{
    runtime_player_api_apply_input(player_guid, input_x, input_z);
}

void engine_player_set_transform(const char *player_guid, float x, float y, float z, int active)
{
    float island_span = (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f;
    runtime_player_api_set_transform(s_world,
                                     player_guid,
                                     x,
                                     y,
                                     z,
                                     active,
                                     island_span,
                                     terrain_sample_height);
}

/* ─────────────────────────────────────────────────────────────────
   Multiplayer sync ownership (C-owned reconciliation)
   ───────────────────────────────────────────────────────────────── */

void engine_player_sync_mark_seen(const char *player_guid, int64_t current_time_ms)
{
    runtime_player_sync_abi_mark_seen(player_guid, current_time_ms);
}

void engine_player_sync_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms)
{
    RuntimePlayerSyncWindow window;
    window.current_time_ms = current_time_ms;
    window.stale_threshold_ms = stale_threshold_ms;
    runtime_player_sync_abi_update_staleness(window);
}

void engine_player_sync_deactivate_stale(void)
{
    runtime_player_sync_abi_deactivate_stale();
}

int engine_player_sync_count_active(void)
{
    return runtime_player_sync_abi_count_active();
}


