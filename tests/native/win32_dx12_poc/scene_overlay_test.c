#include "win32_dx12_poc/scene_overlay.h"
#include "win32_dx12_poc/scene/demo_scene_catalog.h"
#include "engine.h"

#include "../runtime/support/test_support.h"

#include <string.h>

static int g_begin_calls = 0;
static int g_end_calls = 0;
static int g_panel_calls = 0;
static int g_text_calls = 0;
static int g_inventory_calls = 0;
static int g_merchant_calls = 0;

void engine_ui_begin_frame(void) { g_begin_calls++; }
void engine_ui_end_frame(void) { g_end_calls++; }
void engine_ui_panel(float x, float y, float width, float height, unsigned int fill_rgba, float border_width) { (void)x; (void)y; (void)width; (void)height; (void)fill_rgba; (void)border_width; g_panel_calls++; }
void engine_ui_text(float x, float y, const char *text) { (void)x; (void)y; (void)text; g_text_calls++; }
int engine_ui_inventory_panel(float x, float y) { (void)x; (void)y; g_inventory_calls++; return 1; }
int engine_ui_merchant_dialog(float x, float y, int npc_id) { (void)x; (void)y; (void)npc_id; g_merchant_calls++; return 1; }

int engine_get_entity_count(void) { return 4; }
int engine_get_click_count(void) { return 5; }
int engine_get_target_reached_count(void) { return 6; }
float engine_get_terrain_half_span(void) { return 32.f; }
int engine_get_player_position(float *out_x, float *out_z) { if (out_x) *out_x = 4.f; if (out_z) *out_z = -5.f; return 1; }
int engine_get_pbj_pickup_position(float *out_x, float *out_z) { if (out_x) *out_x = -3.f; if (out_z) *out_z = 7.f; return 1; }
int engine_get_controller_war_intelligence_stage(void) { return 2; }
int engine_get_team_banana_count(void) { return 3; }
int engine_get_team_bean_count(void) { return 4; }
int engine_get_controller_war_escalation_tier(void) { return 1; }
int engine_get_controller_war_biome_unlock_count(void) { return 2; }
float engine_get_controller_war_radius(void) { return 8.f; }
int engine_get_controller_war_reinforcements_per_tick(void) { return 2; }
int engine_get_controller_war_effective_reinforcements_per_tick(void) { return 3; }
int engine_get_controller_war_population_cap(void) { return 100; }
int engine_get_controller_war_reinforcement_hits_total(void) { return 10; }
int engine_get_controller_war_reinforcement_hits_biome(int biome_index) { return biome_index + 1; }
int engine_get_controller_war_reinforcement_hits_family_banana_scout(void) { return 1; }
int engine_get_controller_war_reinforcement_hits_family_banana_siege(void) { return 2; }
int engine_get_controller_war_reinforcement_hits_family_bean_raider(void) { return 3; }
int engine_get_controller_war_reinforcement_hits_family_bean_warbrute(void) { return 4; }
int engine_get_controller_war_reinforcement_hits_family_banana_apex(void) { return 5; }
int engine_get_controller_war_reinforcement_hits_family_bean_apex(void) { return 6; }
int engine_get_controller_war_reinforcement_hits_family_banana_mythic(void) { return 7; }
int engine_get_controller_war_reinforcement_hits_family_bean_mythic(void) { return 8; }
int engine_get_controller_war_overcrowd_pct(void) { return 70; }
int engine_get_controller_war_overcrowd_expand_bonus_chunks(void) { return 2; }
int engine_get_controller_war_overcrowd_intelligence_bonus_per_stage(void) { return 1; }
int engine_get_controller_war_frontier_chunks(void) { return 9; }
int engine_get_controller_war_biome_stage_index(void) { return 2; }
int engine_get_controller_war_apex_feature_active(void) { return 1; }
int engine_get_controller_war_mythic_feature_active(void) { return 0; }
int engine_get_controller_war_reinforcement_hits_stage_banana_apex(int stage_index) { return stage_index + 1; }
int engine_get_controller_war_reinforcement_hits_stage_bean_apex(int stage_index) { return stage_index + 2; }
int engine_get_controller_war_reinforcement_hits_stage_banana_mythic(int stage_index) { return stage_index + 3; }
int engine_get_controller_war_reinforcement_hits_stage_bean_mythic(int stage_index) { return stage_index + 4; }
int engine_get_controller_war_life_generation(void) { return 11; }
int engine_get_controller_war_life_alive_cells(void) { return 12; }
int engine_get_controller_war_life_frontline_cells(void) { return 4; }
int engine_get_controller_war_procgen_biome_bias(void) { return 1; }
int engine_get_controller_war_procgen_biome_variance(void) { return 3; }
int engine_get_controller_war_sentience_humanoid_index(void) { return 22; }
int engine_get_controller_war_sentience_coordination_level(void) { return 5; }
int engine_get_controller_war_sentience_empathy_level(void) { return 6; }
int engine_get_controller_war_sentience_behavior_mode_banana(void) { return 1; }
int engine_get_controller_war_sentience_behavior_mode_bean(void) { return 2; }
int engine_get_controller_war_sentience_negotiate_streak_ticks(void) { return 7; }
int engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick(void) { return 1; }
int engine_get_controller_war_sentience_comeback_bonus_last_tick(void) { return 2; }
int engine_get_controller_war_sentience_spawn_mode_hits_banana(int mode) { return mode + 1; }
int engine_get_controller_war_sentience_spawn_mode_hits_bean(int mode) { return mode + 2; }
int engine_get_entity_team(int idx) { return (idx % 2) ? CONTROLLER_TEAM_BANANA : CONTROLLER_TEAM_BEAN; }
float engine_get_entity_x(int idx) { return (float)idx; }
float engine_get_entity_z(int idx) { return (float)(-idx); }

const char *banana_poc_demo_scene_catalog_display_name_for_variant(int browser_variant) { (void)browser_variant; return "Variant"; }
const char *banana_poc_demo_scene_catalog_gameplay_theme_for_variant(int browser_variant) { (void)browser_variant; return "Theme"; }
int banana_poc_demo_scene_catalog_build_coherent_profile(int browser_variant, BananaPocCoherentWorldProfile *out_profile) { if (!out_profile) return 0; memset(out_profile, 0, sizeof(*out_profile)); out_profile->bootstrap_signature = 123u + (unsigned int)browser_variant; return 1; }
int banana_poc_demo_scene_catalog_coherent_profile_consistent(const BananaPocCoherentWorldProfile *profile) { return profile != NULL; }
int banana_poc_demo_scene_catalog_count(void) { return 2; }
int banana_poc_demo_scene_catalog_clamp_index(int requested_index) { return requested_index < 0 ? 0 : (requested_index > 1 ? 1 : requested_index); }
const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_at_index(int index) { static BananaPocDemoSceneCatalogEntry entries[2] = {{0,1,BANANA_POC_DEMO_SCENE_KIND_CONTINENT,RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES,RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID,RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID,"port-koba","scene-0","Scene 0","pack-0","tag-0"},{1,1,BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION,RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA,RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID,RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE,"neo-musa","scene-1","Scene 1","pack-1","tag-1"}}; return (index < 0 || index > 1) ? NULL : &entries[index]; }

static void reset_ui(void)
{
    g_begin_calls = g_end_calls = g_panel_calls = g_text_calls = 0;
    g_inventory_calls = g_merchant_calls = 0;
}

static void test_menu_scene_overlay_paths(void **state)
{
    (void)state;
    reset_ui();
    BananaPocProtoConfig proto = {0};
    banana_poc_render_scene_overlay(BANANA_POC_SCENE_MAIN_MENU, 0, 0, 0, 0, 0, 0, 0, 0, 1, 30, 0, 0, &proto);
    BANANA_TEST_ASSERT_INT_EQ(g_begin_calls, 1, "main menu overlay must begin frame");
    BANANA_TEST_ASSERT_INT_EQ(g_end_calls, 1, "main menu overlay must end frame");
    BANANA_TEST_ASSERT_TRUE(g_panel_calls > 0, "main menu overlay must draw panels");
    BANANA_TEST_ASSERT_TRUE(g_text_calls > 0, "main menu overlay must draw text");

    reset_ui();
    banana_poc_render_scene_overlay(BANANA_POC_SCENE_SCENE_BROWSER, 0, 0, 0, 0, 0, 0, 0, 0, 1, 30, 0, 0, &proto);
    BANANA_TEST_ASSERT_TRUE(g_text_calls > 0, "scene browser overlay must render catalog text");

    reset_ui();
    banana_poc_render_scene_overlay(BANANA_POC_SCENE_CONFIG_LAB, 0, 0, 0, 0, 0, 0, 0, 0, 1, 30, 0, 0, &proto);
    BANANA_TEST_ASSERT_TRUE(g_text_calls > 0, "config lab overlay must render option text");
}

static void test_world_scene_overlay_paths(void **state)
{
    (void)state;
    reset_ui();
    BananaPocProtoConfig proto = {0};
    proto.active_world_variant = 1;
    banana_poc_render_scene_overlay(BANANA_POC_SCENE_WORLD, 0, 0, 0, 0, 1, 1, 1, 0, 10, 30, 0, 0, &proto);
    BANANA_TEST_ASSERT_INT_EQ(g_begin_calls, 1, "world overlay must begin frame");
    BANANA_TEST_ASSERT_INT_EQ(g_end_calls, 1, "world overlay must end frame");
    BANANA_TEST_ASSERT_TRUE(g_panel_calls > 0, "world overlay must draw panels and minimap markers");
    BANANA_TEST_ASSERT_TRUE(g_text_calls > 0, "world overlay must draw hud text");
    BANANA_TEST_ASSERT_INT_EQ(g_inventory_calls, 1, "world overlay must draw inventory panel");

    reset_ui();
    proto.option_compact_hud = 1;
    proto.render_debug_overlay = 1;
    banana_poc_render_scene_overlay(BANANA_POC_SCENE_LEVEL_EDITOR, 0, 0, 0, 0, 1, 1, 1, 2, 10, 30, 1, 1, &proto);
    BANANA_TEST_ASSERT_TRUE(g_text_calls > 0, "level editor overlay must draw editor/debug text");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_menu_scene_overlay_paths),
    BANANA_TEST_CASE(test_world_scene_overlay_paths)
)
