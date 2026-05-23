#include "engine.h"
#include "engine_serialize.h"
#include "render/backend.h"
#include "ai/npc_merchant.h"
#include "ai/wildlife_controller.h"
#include "ui/ui.h"
#include "runtime/controller_sync.h"
#include "runtime/application_service_ports.h"
#include "runtime/controller_runtime.h"
#include "runtime/controller_attach.h"
#include "runtime/engine_composition.h"
#include "runtime/engine_host.h"
#include "runtime/engine_state.h"
#include "runtime/engine_lifecycle.h"
#include "runtime/engine_tick.h"
#include "runtime/gameplay_service.h"
#include "runtime/camera_follow.h"
#include "runtime/camera_basis.h"
#include "runtime/move_target_service.h"
#include "runtime/player_runtime_service.h"
#include "runtime/render_material.h"
#include "runtime/move_target_domain.h"
#include "runtime/player_motion.h"
#include "runtime/player_motion_host.h"
#include "runtime/player_api.h"
#include "runtime/player_builds.h"
#include "runtime/merchant_abi.h"
#include "runtime/merchant_trade_domain.h"
#include "runtime/player_resource_abi.h"
#include "runtime/player_registry.h"
#include "runtime/physics_abi.h"
#include "runtime/input_contract.h"
#include "runtime/resource_domain.h"
#include "runtime/render_submit.h"
#include "runtime/terrain_generation.h"
#include "runtime/terrain_runtime.h"
#include "runtime/tick_phases.h"
#include "runtime/world_metrics.h"
#include "runtime/world_abi.h"
#include "runtime/world_telemetry.h"
#include "runtime/wildlife_gameplay.h"
#include <math.h>
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
#define s_viewport_width (s_engine_state.viewport_width)
#define s_viewport_height (s_engine_state.viewport_height)
#define s_camera_eye (s_engine_state.camera_eye)
#define s_camera_target (s_engine_state.camera_target)
#define s_camera_valid (s_engine_state.camera_valid)
#define s_move_input_x (s_engine_state.move_input_x)
#define s_move_input_z (s_engine_state.move_input_z)
#define s_move_target_state (s_engine_state.move_target_state)
#define s_merchants_seeded (s_engine_state.merchants_seeded)
#define s_pbj_pickup_collected (s_engine_state.pbj_pickup_collected)
#define s_controllers (s_engine_state.controllers)
#define s_controller_count (s_engine_state.controller_count)

/* ── Deterministic terrain generation/rendering (C-owned) ───────────────── */

static float terrain_sample_height(float x, float z);

static float terrain_sample_height(float x, float z)
{
    if (!s_service_ports || !s_service_ports->terrain.sample_height)
        return 0.f;

    return s_service_ports->terrain.sample_height(&s_engine_state, x, z);
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

static const char *engine_player_active_guid(void)
{
    const char *player_guid = NULL;
    int binding_count = runtime_player_registry_count();
    for (int i = 0; i < binding_count; i++) {
        NativePlayerBinding *binding = runtime_player_registry_get(i);
        if (!binding || !binding->guid[0]) {
            continue;
        }
        if (binding->entity_id == s_player_id) {
            return binding->guid;
        }
        if (!player_guid) {
            player_guid = binding->guid;
        }
    }
    return player_guid;
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

/* ── WASM ABI — backed by singletons above ──────────────────────────────── */

void engine_render_frame(void)
{
    runtime_engine_host_render_frame(s_renderer);
}

const unsigned char *engine_get_frame_buffer(void)
{
    return runtime_engine_host_get_frame_buffer(s_renderer);
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
    if (!s_service_ports || !s_service_ports->terrain.set_height)
        return -1;

    return s_service_ports->terrain.set_height(&s_engine_state, x, z, elevation);
}

void engine_terrain_mark_region_dirty(int min_x, int min_z, int max_x, int max_z)
{
    if (!s_service_ports || !s_service_ports->terrain.mark_region_dirty)
        return;

    s_service_ports->terrain.mark_region_dirty(&s_engine_state,
                                               min_x,
                                               min_z,
                                               max_x,
                                               max_z);
}

/* Controller ABI */
uint32_t engine_controller_create(const char *type, float x, float y, float z)
{
    return runtime_controller_create_and_register(s_controllers,
                                                  BANANA_MAX_ACTIVE_CONTROLLERS,
                                                  &s_controller_count,
                                                  type,
                                                  x,
                                                  y,
                                                  z);
}

uint32_t engine_controller_attach(uint32_t entity_id, const char *type)
{
    return runtime_controller_attach_to_entity(s_world,
                                               s_controllers,
                                               BANANA_MAX_ACTIVE_CONTROLLERS,
                                               &s_controller_count,
                                               entity_id,
                                               type);
}

void engine_controller_update(uint32_t id, float dt)
{
    runtime_controller_update_by_id(s_controllers, s_controller_count, id, dt);
}

void engine_controller_signal(uint32_t id, const char *sig)
{
    runtime_controller_signal_by_id(s_controllers, s_controller_count, id, sig, NULL);
}

/* Entity query helpers for React telemetry overlay */
int engine_get_entity_count(void)
{
    return runtime_world_entity_count(s_world);
}

float engine_get_entity_x(int idx)
{
    return runtime_world_entity_x(s_world, idx);
}

float engine_get_entity_z(int idx)
{
    return runtime_world_entity_z(s_world, idx);
}

/* Returns current FSM state index via the bound controller (0 = no controller) */
int engine_get_entity_state(int idx)
{
    return runtime_world_entity_state(s_world, idx, s_controllers, s_controller_count);
}

int engine_get_active_player_count(void)
{
    return runtime_world_active_player_count(s_world);
}

int engine_get_parallel_sync_available(void)
{
    return runtime_controller_sync_parallel_available();
}

int engine_get_parallel_sync_enabled(void)
{
    return runtime_controller_sync_parallel_available();
}

/* ── Debug diagnostics (exported for JavaScript) ───────────────────────── */
int engine_get_click_count(void)
{
    return runtime_input_contract_get_click_count();
}

int engine_get_target_reached_count(void)
{
    return runtime_input_contract_get_target_reached_count();
}

int engine_get_has_move_target(void)
{
    return runtime_input_contract_get_has_move_target();
}

int engine_handle_right_click(float canvas_x, float canvas_y)
{
    int accepted = runtime_input_contract_handle_right_click(canvas_x, canvas_y);
    if (accepted && s_viewport_width > 0 && s_viewport_height > 0)
    {
        float normalized_x = (canvas_x / (float)s_viewport_width) * 2.f - 1.f;
        float normalized_y = 1.f - (canvas_y / (float)s_viewport_height) * 2.f;
        if (runtime_input_contract_handle_right_click_normalized(normalized_x, normalized_y))
            runtime_engine_composition_apply_click_input(&s_engine_state,
                                                         normalized_x,
                                                         normalized_y,
                                                         terrain_sample_height);
    }
    return accepted;
}

int engine_handle_right_click_normalized(float screen_x, float screen_y)
{
    int accepted = runtime_input_contract_handle_right_click_normalized(screen_x, screen_y);
    if (accepted)
        runtime_engine_composition_apply_click_input(&s_engine_state,
                                                     screen_x,
                                                     screen_y,
                                                     terrain_sample_height);
    return accepted;
}

void engine_set_move_input(float input_x, float input_z)
{
    runtime_input_contract_sanitize_move_input(input_x,
                                               input_z,
                                               &s_move_input_x,
                                               &s_move_input_z);

    if (fabsf(s_move_input_x) > 1e-4f || fabsf(s_move_input_z) > 1e-4f)
    {
        runtime_move_target_clear(&s_move_target_state);
        runtime_input_contract_set_has_move_target(0);
    }
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
    runtime_player_registry_mark_seen(player_guid, current_time_ms);
}

void engine_player_sync_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms)
{
    runtime_player_registry_update_staleness(current_time_ms, stale_threshold_ms);
}

void engine_player_sync_deactivate_stale(void)
{
    runtime_player_registry_deactivate_stale();
}

int engine_player_sync_count_active(void)
{
    return runtime_player_registry_count_active();
}

/* ─────────────────────────────────────────────────────────────────
   UI System (Immediate-Mode Native UI)
   ───────────────────────────────────────────────────────────────── */

/* Global UI context (managed by engine) */
UIContext *g_ui_context = NULL;

int engine_ui_init(int width, int height)
{
    if (g_ui_context) {
        return 0; /* Already initialized */
    }
    
    g_ui_context = ui_context_create(width, height);
    return g_ui_context ? 0 : -1;
}

void engine_ui_begin_frame(void)
{
    if (g_ui_context) {
        ui_begin_frame(g_ui_context);
    }
}

int engine_ui_inventory_panel(float x, float y)
{
    if (!g_ui_context) return 0;
    return ui_inventory_panel(g_ui_context, x, y);
}

int engine_ui_merchant_dialog(float x, float y, int npc_id)
{
    if (!g_ui_context) return 0;
    return ui_merchant_dialog(g_ui_context, x, y, npc_id);
}

void engine_ui_end_frame(void)
{
    if (g_ui_context) {
        ui_end_frame(g_ui_context);
    }
}

const unsigned char *engine_ui_get_framebuffer(void)
{
    return g_ui_context ? ui_get_framebuffer(g_ui_context) : NULL;
}

void engine_ui_shutdown(void)
{
    if (g_ui_context) {
        ui_context_destroy(g_ui_context);
        g_ui_context = NULL;
    }
}

/* ─────────────────────────────────────────────────────────────────
   Player Resource Management (UI Integration)
   ───────────────────────────────────────────────────────────────── */

int engine_player_get_resource(const char *resource_type)
{
    return runtime_player_resource_abi_get(engine_player_active_guid(), resource_type);
}

int engine_player_add_resource(const char *resource_type, int amount)
{
    return runtime_player_resource_abi_add(engine_player_active_guid(), resource_type, amount);
}

int engine_player_build_set_class(const char *player_guid, int class_type)
{
    BuildClass parsed_class;
    if (player_builds_parse_class(class_type, &parsed_class) != 0)
    {
        return -1;
    }

    return player_builds_upsert(player_guid, parsed_class);
}

int engine_player_build_set_allocations(const char *player_guid,
                                        int offense_points,
                                        int defense_points,
                                        int utility_points)
{
    return player_builds_set_allocations(player_guid,
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
    GearSlot parsed_slot;
    GearModifier gear;

    if (player_builds_parse_gear_slot(slot, &parsed_slot) != 0)
    {
        return -1;
    }

    gear.tier = tier;
    gear.attack_bonus = attack_bonus;
    gear.defense_bonus = defense_bonus;
    gear.utility_bonus = utility_bonus;
    return player_builds_equip(player_guid, parsed_slot, &gear);
}

int engine_player_build_get_stat(const char *player_guid, const char *stat_name)
{
    BuildStat stat;
    BuildStats stats;
    int stat_value = -1;

    if (player_builds_parse_stat_name(stat_name, &stat) != 0 ||
        player_builds_get_stats(player_guid, &stats) != 0)
    {
        return -1;
    }

    if (player_builds_stat_value(&stats, stat, &stat_value) != 0)
    {
        return -1;
    }

    return stat_value;
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
    ComboOutcome outcome;
    int result = player_builds_evaluate_combo(player_guid,
                                              first_skill,
                                              second_skill,
                                              elapsed_ms,
                                              party_size,
                                              &outcome);
    if (result < 0) {
        return -1;
    }

    if (out_damage_bonus_pct) *out_damage_bonus_pct = outcome.damage_bonus_pct;
    if (out_mitigation_bonus_pct) *out_mitigation_bonus_pct = outcome.mitigation_bonus_pct;
    if (out_party_synergy_bonus_pct) *out_party_synergy_bonus_pct = outcome.party_synergy_bonus_pct;
    return outcome.triggered;
}

static int engine_player_get_resource_by_key(RuntimeResourceKey resource_key)
{
    return runtime_player_resource_abi_get_by_key(engine_player_active_guid(),
                                                  resource_key);
}

static int engine_player_set_resource_total_by_key(RuntimeResourceKey resource_key, int target_amount)
{
    return runtime_player_resource_abi_set_total_by_key(engine_player_active_guid(),
                                                        resource_key,
                                                        target_amount);
}

static int engine_player_add_resource_by_key(RuntimeResourceKey resource_key, int amount)
{
    return runtime_player_resource_abi_add_by_key(engine_player_active_guid(),
                                                  resource_key,
                                                  amount);
}

/* ─────────────────────────────────────────────────────────────────
   Merchant NPC Queries (UI Integration)
   ───────────────────────────────────────────────────────────────── */

int engine_npc_merchant_get_price(int npc_id, const char *item_type)
{
    return runtime_merchant_abi_get_price(s_service_ports,
                                          npc_id,
                                          item_type,
                                          &s_merchants_seeded);
}

int engine_npc_merchant_trade_buy(int npc_id, const char *item_type, int quantity)
{
    RuntimeMerchantResourceGateway resource_gateway;

    resource_gateway.get = engine_player_get_resource_by_key;
    resource_gateway.set_total = engine_player_set_resource_total_by_key;
    resource_gateway.add = engine_player_add_resource_by_key;

    return runtime_merchant_abi_trade_buy(s_service_ports,
                                          s_world != NULL,
                                          npc_id,
                                          item_type,
                                          quantity,
                                          &s_merchants_seeded,
                                          resource_gateway);
}

int engine_npc_merchant_trade_sell(int npc_id, const char *item_type, int quantity)
{
    RuntimeMerchantResourceGateway resource_gateway;

    resource_gateway.get = engine_player_get_resource_by_key;
    resource_gateway.set_total = engine_player_set_resource_total_by_key;
    resource_gateway.add = engine_player_add_resource_by_key;

    return runtime_merchant_abi_trade_sell(s_service_ports,
                                           s_world != NULL,
                                           npc_id,
                                           item_type,
                                           quantity,
                                           &s_merchants_seeded,
                                           resource_gateway);
}
