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
#include "runtime/player/player_motion.h"
#include "runtime/player/runtime/player_motion_host.h"
#include "runtime/player/api/player_api.h"
#include "runtime/player/build/abi/player_build_abi.h"
#include "runtime/merchant/query/merchant_query_abi.h"
#include "runtime/merchant/trade/merchant_trade_domain.h"
#include "runtime/player/resource/player_resource_abi.h"
#include "runtime/player/merchant/player_merchant_adapter.h"
#include "runtime/player/sync/player_sync_abi.h"
#include "runtime/player/player_registry.h"
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
static EngineRuntimeState s_engine_state;
static const RuntimeApplicationServicePorts *s_service_ports = NULL;

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

/* ── C ABI — backed by singletons above ─────────────────────────────────── */

void engine_render_frame(void)
{
    runtime_engine_host_render_frame(s_renderer);
}

/* Physics ABI */
void physics_step(float dt)
{
    runtime_physics_abi_step(s_physics, dt);
}

void physics_add_body(uint32_t id, float mass, float x, float y, float z)
{
    runtime_physics_abi_add_body(s_physics, id, mass, x, y, z);
}

void physics_update_body(uint32_t id, const float *pos, const float *vel)
{
    runtime_physics_abi_update_body(s_physics, id, pos, vel);
}

/* World ABI */
uint32_t engine_world_spawn(int type, float x, float y, float z)
{
    return runtime_world_abi_spawn(s_world, type, x, y, z);
}

void engine_world_tick(float dt)
{
    runtime_world_abi_tick(s_world, dt);
}

/* Terrain ABI */
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

/* Controller ABI */
uint32_t engine_controller_create(const char *type, float x, float y, float z)
{
    return runtime_engine_aux_controller_create(s_controllers,
                                                BANANA_MAX_ACTIVE_CONTROLLERS,
                                                &s_controller_count,
                                                type,
                                                x,
                                                y,
                                                z);
}

uint32_t engine_controller_attach(uint32_t entity_id, const char *type)
{
    RuntimeEngineAuxContext context = runtime_engine_aux_context(s_world,
                                                                 s_controllers,
                                                                 s_controller_count);

    return runtime_engine_aux_controller_attach(context,
                                                BANANA_MAX_ACTIVE_CONTROLLERS,
                                                &s_controller_count,
                                                entity_id,
                                                type);
}

void engine_controller_update(uint32_t id, float dt)
{
    RuntimeEngineAuxContext context = runtime_engine_aux_context(s_world,
                                                                 s_controllers,
                                                                 s_controller_count);
    runtime_engine_aux_controller_update(context, id, dt);
}

void engine_controller_signal(uint32_t id, const char *sig)
{
    RuntimeEngineAuxContext context = runtime_engine_aux_context(s_world,
                                                                 s_controllers,
                                                                 s_controller_count);
    runtime_engine_aux_controller_signal(context, id, sig);
}

/* Entity query helpers for React telemetry overlay */
int engine_get_entity_count(void)
{
    return runtime_engine_aux_entity_count(s_world);
}

float engine_get_entity_x(int idx)
{
    return runtime_engine_aux_entity_x(s_world, idx);
}

float engine_get_entity_z(int idx)
{
    return runtime_engine_aux_entity_z(s_world, idx);
}

/* Returns current FSM state index via the bound controller (0 = no controller) */
int engine_get_entity_state(int idx)
{
    RuntimeEngineAuxContext context = runtime_engine_aux_context(s_world,
                                                                 s_controllers,
                                                                 s_controller_count);
    return runtime_engine_aux_entity_state(context, idx);
}

int engine_get_entity_team(int idx)
{
    Entity *entity = NULL;
    ControllerInstance *controller = NULL;

    if (!s_world || idx < 0 || idx >= s_world->entity_count)
        return (int)CONTROLLER_TEAM_NEUTRAL;

    entity = s_world->entities[idx];
    if (!entity || !entity->active)
        return (int)CONTROLLER_TEAM_NEUTRAL;
    if (entity->type != ENTITY_TYPE_NPC || entity->controller_id == 0)
        return (int)CONTROLLER_TEAM_NEUTRAL;

    controller = runtime_controller_find_by_id(s_controllers,
                                               s_controller_count,
                                               entity->controller_id);
    if (!controller)
        return (int)CONTROLLER_TEAM_NEUTRAL;

    return (int)controller->team;
}

int engine_get_active_player_count(void)
{
    return runtime_engine_aux_active_player_count(s_world);
}

int engine_get_parallel_sync_available(void)
{
    return runtime_engine_aux_parallel_sync_available();
}

int engine_get_parallel_sync_enabled(void)
{
    return runtime_engine_aux_parallel_sync_available();
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

/* ─────────────────────────────────────────────────────────────────
   UI System (Immediate-Mode Native UI)
   ───────────────────────────────────────────────────────────────── */

int engine_ui_init(int width, int height)
{
    return runtime_ui_abi_init(width, height);
}

void engine_ui_begin_frame(void)
{
    runtime_ui_abi_begin_frame();
}

int engine_ui_inventory_panel(float x, float y)
{
    return runtime_ui_abi_inventory_panel(x, y);
}

int engine_ui_merchant_dialog(float x, float y, int npc_id)
{
    return runtime_ui_abi_merchant_dialog(x, y, npc_id);
}

void engine_ui_panel(float x,
                     float y,
                     float width,
                     float height,
                     unsigned int fill_rgba,
                     float border_width)
{
    runtime_ui_abi_panel(x, y, width, height, fill_rgba, border_width);
}

void engine_ui_text(float x, float y, const char *text)
{
    runtime_ui_abi_text(x, y, text);
}

void engine_ui_end_frame(void)
{
    runtime_ui_abi_end_frame();
}

const unsigned char *engine_ui_get_framebuffer(void)
{
    return runtime_ui_abi_get_framebuffer();
}

void engine_ui_shutdown(void)
{
    runtime_ui_abi_shutdown();
}

/* ─────────────────────────────────────────────────────────────────
   Player Resource Management (UI Integration)
   ───────────────────────────────────────────────────────────────── */

int engine_player_get_resource(const char *resource_type)
{
    return runtime_player_merchant_adapter_get_resource(resource_type, s_player_id);
}

int engine_player_add_resource(const char *resource_type, int amount)
{
    return runtime_player_merchant_adapter_add_resource(resource_type,
                                                        amount,
                                                        s_player_id);
}

int engine_get_player_position(float *out_x, float *out_z)
{
    Entity *player = NULL;

    if (!s_world || !s_player_id || !out_x || !out_z)
        return 0;

    player = world_get_entity(s_world, s_player_id);
    if (!player || !player->active)
        return 0;

    *out_x = player->position[0];
    *out_z = player->position[2];
    return 1;
}

int engine_get_pbj_pickup_position(float *out_x, float *out_z)
{
    int i = 0;

    if (!s_world || !out_x || !out_z)
        return 0;

    for (i = 0; i < s_world->entity_count; i++)
    {
        Entity *entity = s_world->entities[i];
        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_STATIC)
            continue;
        if (strcmp(entity->controller_kind, "pbj_pickup") != 0)
            continue;

        *out_x = entity->position[0];
        *out_z = entity->position[2];
        return 1;
    }

    return 0;
}

float engine_get_terrain_half_span(void)
{
    return ((float)BANANA_TERRAIN_SIZE - 1.0f) * 0.5f;
}

static int runtime_engine_count_active_team_members(ControllerTeam team)
{
    int count = 0;

    if (!s_world)
        return 0;

    for (int i = 0; i < s_world->entity_count; i++)
    {
        Entity *entity = s_world->entities[i];
        ControllerInstance *controller = NULL;

        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_NPC)
            continue;
        if (entity->controller_id == 0)
            continue;

        controller = runtime_controller_find_by_id(s_controllers,
                                                   s_controller_count,
                                                   entity->controller_id);
        if (!controller)
            continue;
        if (controller->team == team)
            count += 1;
    }

    return count;
}

int engine_get_team_banana_count(void)
{
    return runtime_engine_count_active_team_members(CONTROLLER_TEAM_BANANA);
}

int engine_get_team_bean_count(void)
{
    return runtime_engine_count_active_team_members(CONTROLLER_TEAM_BEAN);
}

float engine_get_controller_war_radius(void)
{
    return runtime_tick_budget_policy_controller_war_radius();
}

int engine_get_controller_war_reinforcements_per_tick(void)
{
    return runtime_tick_budget_policy_controller_war_reinforcements_per_tick();
}

int engine_get_controller_war_population_cap(void)
{
    return runtime_tick_budget_policy_controller_war_population_cap();
}

int engine_get_controller_war_escalation_tier(void)
{
    return (int)s_engine_state.war_escalation_tier;
}

int engine_get_controller_war_effective_reinforcements_per_tick(void)
{
    if (!s_engine_state.engine_initialized)
        return runtime_tick_budget_policy_controller_war_reinforcements_per_tick();

    return s_engine_state.war_effective_reinforcements_per_tick;
}

int engine_get_controller_war_intelligence_stage(void)
{
    return s_engine_state.war_intelligence_stage;
}

int engine_get_controller_war_biome_unlock_count(void)
{
    int count = 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
    {
        if ((s_engine_state.war_biome_unlock_mask & (1u << i)) != 0u)
            count += 1;
    }

    return count;
}

int engine_get_controller_war_frontier_chunks(void)
{
    return s_engine_state.war_frontier_chunks;
}

int engine_get_controller_war_biome_stage_index(void)
{
    return s_engine_state.war_biome_stage_index;
}

int engine_get_controller_war_overcrowd_pct(void)
{
    return runtime_tick_budget_policy_controller_war_overcrowd_pct();
}

int engine_get_controller_war_overcrowd_expand_bonus_chunks(void)
{
    return runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks();
}

int engine_get_controller_war_overcrowd_intelligence_bonus_per_stage(void)
{
    return runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage();
}

int engine_get_controller_war_life_tick_interval(void)
{
    return runtime_tick_budget_policy_controller_war_life_tick_interval();
}

int engine_get_controller_war_life_intelligence_bonus_max(void)
{
    return runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max();
}

int engine_get_controller_war_procgen_biome_variance(void)
{
    return runtime_tick_budget_policy_controller_war_procgen_biome_variance();
}

int engine_get_controller_war_sentience_gain_per_tick(void)
{
    return runtime_tick_budget_policy_controller_war_sentience_gain_per_tick();
}

int engine_get_controller_war_sentience_comeback_bonus_per_coordination(void)
{
    return runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination();
}

int engine_get_controller_war_apex_feature_active(void)
{
    return s_engine_state.war_intelligence_stage >= 3 ? 1 : 0;
}

int engine_get_controller_war_mythic_feature_active(void)
{
    return s_engine_state.war_intelligence_stage >= 5 ? 1 : 0;
}

int engine_get_controller_war_life_generation(void)
{
    return s_engine_state.war_life_generation;
}

int engine_get_controller_war_life_alive_cells(void)
{
    return s_engine_state.war_life_alive_cells;
}

int engine_get_controller_war_life_frontline_cells(void)
{
    return s_engine_state.war_life_frontline_cells;
}

int engine_get_controller_war_procgen_biome_bias(void)
{
    return s_engine_state.war_procgen_biome_bias;
}

int engine_get_controller_war_sentience_humanoid_index(void)
{
    return s_engine_state.war_sentience_humanoid_index;
}

int engine_get_controller_war_sentience_coordination_level(void)
{
    return s_engine_state.war_sentience_coordination_level;
}

int engine_get_controller_war_sentience_empathy_level(void)
{
    return s_engine_state.war_sentience_empathy_level;
}

int engine_get_controller_war_sentience_behavior_mode_banana(void)
{
    return (int)s_engine_state.war_sentience_behavior_banana;
}

int engine_get_controller_war_sentience_behavior_mode_bean(void)
{
    return (int)s_engine_state.war_sentience_behavior_bean;
}

static int runtime_engine_war_sentience_mode_hits_at(const int *hits, int mode)
{
    if (!hits)
        return 0;
    if (mode < 0 || mode >= BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT)
        return 0;

    return hits[mode];
}

int engine_get_controller_war_sentience_spawn_mode_hits_banana(int mode)
{
    return runtime_engine_war_sentience_mode_hits_at(s_engine_state.war_sentience_spawn_mode_hits_banana,
                                                     mode);
}

int engine_get_controller_war_sentience_spawn_mode_hits_bean(int mode)
{
    return runtime_engine_war_sentience_mode_hits_at(s_engine_state.war_sentience_spawn_mode_hits_bean,
                                                     mode);
}

int engine_get_controller_war_sentience_negotiate_streak_ticks(void)
{
    return s_engine_state.war_sentience_negotiate_streak_ticks;
}

int engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick(void)
{
    return s_engine_state.war_sentience_negotiate_deescalation_trim_last_tick;
}

int engine_get_controller_war_sentience_comeback_bonus_last_tick(void)
{
    return s_engine_state.war_sentience_comeback_bonus_last_tick;
}

static int runtime_engine_sum_war_reinforcement_hits(const int *hits)
{
    int total = 0;

    if (!hits)
        return 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
        total += hits[i];

    return total;
}

static int runtime_engine_war_reinforcement_stage_hits_at(const int *hits, int stage_index)
{
    if (!hits)
        return 0;
    if (stage_index < 0 || stage_index >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
        return 0;

    return hits[stage_index];
}

int engine_get_controller_war_reinforcement_hits_total(void)
{
    return s_engine_state.war_reinforcement_spawns_total;
}

int engine_get_controller_war_reinforcement_hits_biome(int biome_index)
{
    if (biome_index < 0 || biome_index >= BANANA_ENGINE_TERRAIN_MAX_LAYERS)
        return 0;

    return s_engine_state.war_reinforcement_biome_hits[biome_index];
}

int engine_get_controller_war_reinforcement_hits_family_banana_scout(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_scout_hits);
}

int engine_get_controller_war_reinforcement_hits_family_banana_siege(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_siege_hits);
}

int engine_get_controller_war_reinforcement_hits_family_banana_apex(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_apex_hits);
}

int engine_get_controller_war_reinforcement_hits_family_banana_mythic(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_mythic_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_raider(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_raider_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_warbrute(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_warbrute_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_apex(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_apex_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_mythic(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_mythic_hits);
}

int engine_get_controller_war_reinforcement_hits_stage_banana_apex(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_banana_apex_stage_hits,
                                                          stage_index);
}

int engine_get_controller_war_reinforcement_hits_stage_banana_mythic(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_banana_mythic_stage_hits,
                                                          stage_index);
}

int engine_get_controller_war_reinforcement_hits_stage_bean_apex(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_bean_apex_stage_hits,
                                                          stage_index);
}

int engine_get_controller_war_reinforcement_hits_stage_bean_mythic(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_bean_mythic_stage_hits,
                                                          stage_index);
}

int engine_player_build_set_class(const char *player_guid, int class_type)
{
    return runtime_player_build_abi_set_class(player_guid, class_type);
}

int engine_player_build_set_allocations(const char *player_guid,
                                        int offense_points,
                                        int defense_points,
                                        int utility_points)
{
    return runtime_player_build_abi_set_allocations(player_guid,
                                                    offense_points,
                                                    defense_points,
                                                    utility_points);
}

int engine_player_build_equip(const char *player_guid,
                              int slot,
                              int tier,
                              int attack_bonus,
                              int defense_bonus,
                              int utility_bonus)
{
    return runtime_player_build_abi_equip(player_guid,
                                          slot,
                                          tier,
                                          attack_bonus,
                                          defense_bonus,
                                          utility_bonus);
}

int engine_player_build_get_stat(const char *player_guid, const char *stat_name)
{
    return runtime_player_build_abi_get_stat(player_guid, stat_name);
}

int engine_player_combo_evaluate(const char *player_guid,
                                 const char *first_skill,
                                 const char *second_skill,
                                 int elapsed_ms,
                                 int party_size,
                                 int *out_damage_bonus_pct,
                                 int *out_mitigation_bonus_pct,
                                 int *out_party_synergy_bonus_pct)
{
    return runtime_player_build_abi_evaluate_combo(player_guid,
                                                   first_skill,
                                                   second_skill,
                                                   elapsed_ms,
                                                   party_size,
                                                   out_damage_bonus_pct,
                                                   out_mitigation_bonus_pct,
                                                   out_party_synergy_bonus_pct);
}

/* ─────────────────────────────────────────────────────────────────
   Merchant NPC Queries (UI Integration)
   ───────────────────────────────────────────────────────────────── */

int engine_npc_merchant_get_price(int npc_id, const char *item_type)
{
    return runtime_merchant_query_abi_get_price(s_service_ports,
                                                npc_id,
                                                item_type,
                                                &s_merchants_seeded);
}

int engine_npc_merchant_trade_buy(int npc_id, const char *item_type, int quantity)
{
    return runtime_player_merchant_adapter_trade_buy(s_service_ports,
                                                     s_world != NULL,
                                                     npc_id,
                                                     item_type,
                                                     quantity,
                                                     &s_merchants_seeded,
                                                     s_player_id);
}

int engine_npc_merchant_trade_sell(int npc_id, const char *item_type, int quantity)
{
    return runtime_player_merchant_adapter_trade_sell(s_service_ports,
                                                      s_world != NULL,
                                                      npc_id,
                                                      item_type,
                                                      quantity,
                                                      &s_merchants_seeded,
                                                      s_player_id);
}
