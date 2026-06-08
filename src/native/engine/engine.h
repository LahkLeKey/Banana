#ifndef BANANA_ENGINE_H
#define BANANA_ENGINE_H

/* ─────────────────────────────────────────────────────────────────────────────
 * banana_engine — public C API
 *
 * This header is the single include for client/runtime orchestration layers.
 * Native code can include individual module headers directly.
 * ─────────────────────────────────────────────────────────────────────────── */

#include "ai/controller.h"
#include "ai/navigation.h"
#include "ai/perception.h"
#include "ai/state_machine.h"
#include "physics/body.h"
#include "physics/collider.h"
#include "physics/dynamics.h"
#include "physics/world.h"
#include "render/camera.h"
#include "render/backend.h"
#include "render/material.h"
#include "render/mesh.h"
#include "render/renderer.h"
#include "render/shader.h"
#include "render/window.h"
#include "world/entity.h"
#include "world/signals.h"
#include "world/world.h"
#include "engine_serialize.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* ── Top-level lifecycle ────────────────────────────────────────────────────*/

    /* Initialize engine subsystems (render, physics, world).
     * Must be called once before any other engine function.
     * width/height: initial framebuffer dimensions. */
    int engine_init(int width, int height);

    /* Advance one game tick: physics step → controller updates → render frame.
     * dt: elapsed seconds since last tick.
     * Returns 0 on success. */
    int engine_tick(float dt);
    const char *engine_tick_serialize(float dt);

    /* Tear down all subsystems. */
    void engine_shutdown(void);

    /* ── Controller helpers ─────────────────────────────────────────────────────*/

    /* Create a controller and attach it to an existing world entity.
     * Returns the controller ID (0 on failure). */
    uint32_t engine_controller_attach(uint32_t entity_id, const char *type_name);

    /* ── Entity query helpers (for runtime telemetry overlays) ───────────────── */
    int engine_get_entity_count(void);
    float engine_get_entity_x(int idx);
    float engine_get_entity_z(int idx);
    int engine_get_entity_state(int idx); /* 0 = inactive, 1 = active */
    int engine_get_entity_team(int idx);  /* ControllerTeam enum; neutral when unavailable */
    int engine_get_active_player_count(void);
    int engine_get_parallel_sync_available(void);
    int engine_get_parallel_sync_enabled(void);

    /* Keyboard movement intent only. Inputs are expected in [-1, 1]. */
    void engine_set_move_input(float input_x, float input_z);

    /* Upsert a GUID-backed player entity with a controller strategy.
     * controller_kind: "human" or "ai".
     * active: 1 when connected/active, 0 when disconnected/inactive. */
    uint32_t engine_player_upsert(const char *player_guid, const char *player_name,
                                  const char *controller_kind, int active);

    /* Apply movement intent to one player GUID for the next tick. */
    void engine_player_apply_input(const char *player_guid, float input_x, float input_z);

    /* Set authoritative transform for a player GUID.
     * active: 1 when visible/connected, 0 to hide/deactivate. */
    void engine_player_set_transform(const char *player_guid, float x, float y, float z, int active);

    /* ── Multiplayer sync ownership (C-owned reconciliation) ─────────────── */

    /* Mark a player as seen/active at the given timestamp (milliseconds).
     * Called when a player sends input or a snapshot is received. */
    void engine_player_sync_mark_seen(const char *player_guid, int64_t current_time_ms);

    /* Update staleness flags for all players based on current time and threshold.
     * Call once per tick before deactivate_stale. */
    void engine_player_sync_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms);

    /* Deactivate all players marked as stale (call after update_staleness).
     * Next tick will reflect inactive state in entity list. */
    void engine_player_sync_deactivate_stale(void);

    /* Count active (non-stale, connected) players in session. */
    int engine_player_sync_count_active(void);

    /* Debug diagnostics for de-scoped mouse movement path. */
    int engine_get_click_count(void);
    int engine_get_target_reached_count(void);
    int engine_get_has_move_target(void);
    int engine_get_pbj_pickup_collected(void);
    int engine_handle_right_click(float canvas_x, float canvas_y);
    int engine_handle_right_click_normalized(float screen_x, float screen_y);

    /* ── Terrain update helpers (WFC/CA integration) ─────────────────────── */
    int engine_terrain_set_height(int x, int z, int elevation);
    void engine_terrain_mark_region_dirty(int min_x, int min_z, int max_x, int max_z);

    /* ── UI System (Immediate-Mode Native UI) ──────────────────────────── */

    /* Initialize UI system for given framebuffer dimensions. */
    int engine_ui_init(int width, int height);

    /* Begin a new UI frame (clear element queue). */
    void engine_ui_begin_frame(void);

    /* Draw inventory panel at screen coordinates. */
    int engine_ui_inventory_panel(float x, float y);

    /* Draw merchant trade dialog for NPC. */
    int engine_ui_merchant_dialog(float x, float y, int npc_id);

    /* Draw a UI panel rectangle in the native immediate-mode UI framebuffer. */
    void engine_ui_panel(float x,
                         float y,
                         float width,
                         float height,
                         unsigned int fill_rgba,
                         float border_width);

    /* Draw text in the native immediate-mode UI framebuffer. */
    void engine_ui_text(float x, float y, const char *text);

    /* End UI frame: render all queued elements to framebuffer. */
    void engine_ui_end_frame(void);

    /* Get rendered UI framebuffer (RGBA 32-bit pixels). */
    const unsigned char *engine_ui_get_framebuffer(void);

    /* Shutdown UI system. */
    void engine_ui_shutdown(void);

    /* ── Player Resource Management (UI Integration) ───────────────────── */

    /* Get current player resource count by type ("wood", "ore", "gold").
     * Returns -1 if resource type not found. */
    int engine_player_get_resource(const char *resource_type);

    /* Add resource to player inventory. Returns new count or -1 on error. */
    int engine_player_add_resource(const char *resource_type, int amount);

    /* Query current player world-space x/z position. Returns 1 on success. */
    int engine_get_player_position(float *out_x, float *out_z);

    /* Query active PBJ pickup world-space x/z position. Returns 1 on success. */
    int engine_get_pbj_pickup_position(float *out_x, float *out_z);

    /* Query world half-span used for terrain-space mapping (centered at origin). */
    float engine_get_terrain_half_span(void);

    /* Query live hostile-team populations tracked by active NPC controllers. */
    int engine_get_team_banana_count(void);
    int engine_get_team_bean_count(void);

    /* Query live war budget policy values used by runtime ticks. */
    float engine_get_controller_war_radius(void);
    int engine_get_controller_war_reinforcements_per_tick(void);
    int engine_get_controller_war_population_cap(void);
    int engine_get_controller_war_escalation_tier(void);
    int engine_get_controller_war_effective_reinforcements_per_tick(void);
    int engine_get_controller_war_intelligence_stage(void);
    int engine_get_controller_war_biome_unlock_count(void);
    int engine_get_controller_war_frontier_chunks(void);
    int engine_get_controller_war_biome_stage_index(void);
    int engine_get_controller_war_overcrowd_pct(void);
    int engine_get_controller_war_overcrowd_expand_bonus_chunks(void);
    int engine_get_controller_war_overcrowd_intelligence_bonus_per_stage(void);
    int engine_get_controller_war_life_tick_interval(void);
    int engine_get_controller_war_life_intelligence_bonus_max(void);
    int engine_get_controller_war_procgen_biome_variance(void);
    int engine_get_controller_war_sentience_gain_per_tick(void);
    int engine_get_controller_war_sentience_comeback_bonus_per_coordination(void);
    int engine_get_controller_war_apex_feature_active(void);
    int engine_get_controller_war_mythic_feature_active(void);
    int engine_get_controller_war_life_generation(void);
    int engine_get_controller_war_life_alive_cells(void);
    int engine_get_controller_war_life_frontline_cells(void);
    int engine_get_controller_war_procgen_biome_bias(void);
    int engine_get_controller_war_sentience_humanoid_index(void);
    int engine_get_controller_war_sentience_coordination_level(void);
    int engine_get_controller_war_sentience_empathy_level(void);
    int engine_get_controller_war_sentience_behavior_mode_banana(void);
    int engine_get_controller_war_sentience_behavior_mode_bean(void);
    int engine_get_controller_war_sentience_spawn_mode_hits_banana(int mode);
    int engine_get_controller_war_sentience_spawn_mode_hits_bean(int mode);
    int engine_get_controller_war_sentience_negotiate_streak_ticks(void);
    int engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick(void);
    int engine_get_controller_war_sentience_comeback_bonus_last_tick(void);
    int engine_get_controller_war_sentience_truce_variant_hits_total(void);
    int engine_get_controller_war_sentience_truce_variant_hits_banana(void);
    int engine_get_controller_war_sentience_truce_variant_hits_bean(void);
    int engine_get_controller_war_sentience_truce_variant_hits_base(void);
    int engine_get_controller_war_sentience_truce_variant_hits_apex(void);
    int engine_get_controller_war_sentience_truce_variant_hits_mythic(void);
    int engine_get_controller_war_sentience_truce_variant_hits_stage(int stage_index);
    int engine_get_controller_war_sentience_truce_gate_checks_total(void);
    int engine_get_controller_war_sentience_truce_gate_checks_granted(void);
    int engine_get_controller_war_sentience_truce_gate_checks_stage(int stage_index);
    int engine_get_controller_war_sentience_truce_gate_granted_stage(int stage_index);
    int engine_get_controller_war_sentience_truce_gate_block_behavior(void);
    int engine_get_controller_war_sentience_truce_gate_block_empathy(void);
    int engine_get_controller_war_sentience_truce_gate_block_coordination(void);
    int engine_get_controller_war_sentience_truce_gate_block_streak(void);
    int engine_get_controller_war_sentience_truce_gate_block_intelligence(void);
    int engine_get_controller_war_reinforcement_hits_total(void);
    int engine_get_controller_war_reinforcement_hits_biome(int biome_index);
    int engine_get_controller_war_reinforcement_hits_family_banana_scout(void);
    int engine_get_controller_war_reinforcement_hits_family_banana_siege(void);
    int engine_get_controller_war_reinforcement_hits_family_banana_apex(void);
    int engine_get_controller_war_reinforcement_hits_family_banana_mythic(void);
    int engine_get_controller_war_reinforcement_hits_family_bean_raider(void);
    int engine_get_controller_war_reinforcement_hits_family_bean_warbrute(void);
    int engine_get_controller_war_reinforcement_hits_family_bean_apex(void);
    int engine_get_controller_war_reinforcement_hits_family_bean_mythic(void);
    int engine_get_controller_war_reinforcement_hits_stage_banana_apex(int stage_index);
    int engine_get_controller_war_reinforcement_hits_stage_banana_mythic(int stage_index);
    int engine_get_controller_war_reinforcement_hits_stage_bean_apex(int stage_index);
    int engine_get_controller_war_reinforcement_hits_stage_bean_mythic(int stage_index);

    /* ── Player Build & Combo Systems ───────────────────────────────────── */

    /* Set player class (0=vanguard, 1=arcanist, 2=ranger). */
    int engine_player_build_set_class(const char *player_guid, int class_type);

    /* Allocate build points into offense/defense/utility. */
    int engine_player_build_set_allocations(const char *player_guid,
                                            int offense_points,
                                            int defense_points,
                                            int utility_points);

    /* Equip one gear slot (0=weapon, 1=armor, 2=trinket) with tier and modifiers. */
    int engine_player_build_equip(const char *player_guid,
                                  int slot,
                                  int tier,
                                  int attack_bonus,
                                  int defense_bonus,
                                  int utility_bonus);

    /* Query one build stat by name: "health", "attack", "defense", "utility". */
    int engine_player_build_get_stat(const char *player_guid, const char *stat_name);

    /* Evaluate a combo and return triggered flag plus bonus outputs. */
    int engine_player_combo_evaluate(const char *player_guid,
                                     const char *first_skill,
                                     const char *second_skill,
                                     int elapsed_ms,
                                     int party_size,
                                     int *out_damage_bonus_pct,
                                     int *out_mitigation_bonus_pct,
                                     int *out_party_synergy_bonus_pct);

    /* ── Merchant NPC Queries (UI Integration) ──────────────────────────── */

    /* Get price for item type at a merchant. */
    int engine_npc_merchant_get_price(int npc_id, const char *item_type);

    /* Execute buy trade (player buys from merchant).
     * Returns 0 on success, non-zero error code on failure. */
    int engine_npc_merchant_trade_buy(int npc_id, const char *item_type, int quantity);

    /* Execute sell trade (player sells to merchant).
     * Returns 0 on success, non-zero error code on failure. */
    int engine_npc_merchant_trade_sell(int npc_id, const char *item_type, int quantity);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_H */
