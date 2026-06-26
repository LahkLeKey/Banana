#include "runtime/orchestration/tick/loop/runtime_tick_orchestration.h"
#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/gameplay/service/gameplay_service.h"

#include "runtime/support/test_support.h"

#include <string.h>

static int g_gameplay_tick_calls = 0;
static int g_last_reinforcements = 0;
static int g_last_population_cap = 0;
static int g_last_war_tier = -1;
static int g_set_height_calls = 0;
static int g_mark_dirty_calls = 0;
static int g_submit_scene_calls = 0;

static void reset_render_tick_state(void)
{
    g_gameplay_tick_calls = 0;
    g_last_reinforcements = 0;
    g_last_population_cap = 0;
    g_last_war_tier = -1;
    g_set_height_calls = 0;
    g_mark_dirty_calls = 0;
    g_submit_scene_calls = 0;
}

void runtime_gameplay_service_tick(World *world,
                                   EngineRuntimeState *runtime_state,
                                   ControllerInstance **controllers,
                                   int max_controllers,
                                   int *inout_controller_count,
                                   EntityId player_id,
                                   int *inout_pbj_pickup_collected,
                                   float wildlife_signal_radius,
                                   float collect_radius,
                                   float controller_war_radius,
                                   int controller_war_reinforcements_per_tick,
                                   int controller_war_population_cap,
                                   int war_escalation_tier,
                                   int war_intelligence_stage,
                                   int war_biome_stage_index)
{
    (void)world;
    (void)runtime_state;
    (void)controllers;
    (void)max_controllers;
    (void)inout_controller_count;
    (void)player_id;
    (void)inout_pbj_pickup_collected;
    (void)wildlife_signal_radius;
    (void)collect_radius;
    (void)controller_war_radius;
    (void)war_intelligence_stage;
    (void)war_biome_stage_index;

    g_gameplay_tick_calls += 1;
    g_last_reinforcements = controller_war_reinforcements_per_tick;
    g_last_population_cap = controller_war_population_cap;
    g_last_war_tier = war_escalation_tier;
}

int runtime_tick_budget_policy_controller_war_intelligence_frontier_bonus_per_stage(void) { return 1; }
int runtime_tick_budget_policy_controller_war_intelligence_level_threshold_ticks(void) { return 3; }
int runtime_tick_budget_policy_controller_war_intelligence_max_stage(void) { return 5; }
int runtime_tick_budget_policy_controller_war_intelligence_progress_siege(void) { return 2; }
int runtime_tick_budget_policy_controller_war_intelligence_progress_skirmish(void) { return 1; }
int runtime_tick_budget_policy_controller_war_intelligence_reinforcement_bonus_per_stage(void) { return 1; }
int runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(void) { return 4; }
int runtime_tick_budget_policy_controller_war_life_tick_interval(void) { return 1; }
int runtime_tick_budget_policy_controller_war_max_frontier_chunks(void) { return 3; }
int runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(void) { return 2; }
int runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(void) { return 1; }
int runtime_tick_budget_policy_controller_war_overcrowd_pct(void) { return 70; }
int runtime_tick_budget_policy_controller_war_population_cap(void) { return 10; }
int runtime_tick_budget_policy_controller_war_procgen_biome_variance(void) { return 2; }
float runtime_tick_budget_policy_controller_war_radius(void) { return 6.0f; }
int runtime_tick_budget_policy_controller_war_reinforcements_per_tick(void) { return 2; }
int runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(void) { return 1; }
int runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(void) { return 1; }
int runtime_tick_budget_policy_controller_war_siege_pct(void) { return 70; }
int runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(void) { return 2; }
int runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(void) { return 1; }
int runtime_tick_budget_policy_controller_war_skirmish_pct(void) { return 40; }
int runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(void) { return 1; }
int runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(void) { return 1; }
int runtime_tick_budget_policy_controller_war_terrain_pressure_pct(void) { return 10; }

static int stub_set_height(EngineRuntimeState *state, int x, int z, int elevation)
{
    (void)state;
    (void)x;
    (void)z;
    (void)elevation;
    g_set_height_calls += 1;
    return 1;
}

static void stub_mark_region_dirty(EngineRuntimeState *state,
                                   int min_x,
                                   int min_z,
                                   int max_x,
                                   int max_z)
{
    (void)state;
    (void)min_x;
    (void)min_z;
    (void)max_x;
    (void)max_z;
    g_mark_dirty_calls += 1;
}

static void stub_submit_scene(EngineRuntimeState *state)
{
    (void)state;
    g_submit_scene_calls += 1;
}

static void test_gameplay_rejects_invalid_contexts(void **state)
{
    (void)state;
    RuntimeTickOrchestrationContext context = {0};

    reset_render_tick_state();

    runtime_tick_orchestration_gameplay(NULL);
    BANANA_TEST_ASSERT_INT_EQ(g_gameplay_tick_calls,
                              0,
                              "null context must skip gameplay orchestration");

    runtime_tick_orchestration_gameplay(&context);
    BANANA_TEST_ASSERT_INT_EQ(g_gameplay_tick_calls,
                              0,
                              "missing runtime state must skip gameplay orchestration");
}

static void test_gameplay_handles_peaceful_lane(void **state)
{
    (void)state;
    RuntimeApplicationServicePorts ports = {0};
    RuntimeTickOrchestrationContext context = {0};
    EngineRuntimeState runtime_state = {0};
    World world = {0};

    reset_render_tick_state();

    runtime_state.world = &world;
    runtime_state.war_sentience_humanoid_index = 5;
    context.state = &runtime_state;
    context.service_ports = &ports;

    runtime_tick_orchestration_gameplay(&context);

    BANANA_TEST_ASSERT_INT_EQ(g_gameplay_tick_calls,
                              1,
                              "peaceful gameplay tick must still call gameplay service");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.war_escalation_tier,
                              (int)RUNTIME_WAR_ESCALATION_PEACEFUL,
                              "zero active war NPCs must remain in peaceful tier");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.war_sentience_humanoid_index,
                              4,
                              "peaceful lane should cool down sentience index by one");
}

static void test_gameplay_updates_war_metrics_and_expands_frontier(void **state)
{
    (void)state;
    RuntimeApplicationServicePorts ports = {0};
    RuntimeTickOrchestrationContext context = {0};
    EngineRuntimeState runtime_state = {0};
    World world = {0};
    Entity npcs[8] = {0};
    int i = 0;

    reset_render_tick_state();

    for (i = 0; i < 8; i++)
    {
        npcs[i].id = (EntityId)(i + 1);
        npcs[i].type = ENTITY_TYPE_NPC;
        npcs[i].active = 1;
        strcpy(npcs[i].controller_kind, (i % 2 == 0) ? "combat" : "wildlife");
        world.entities[i] = &npcs[i];
    }
    world.entity_count = 8;

    ports.terrain.set_height = stub_set_height;
    ports.terrain.mark_region_dirty = stub_mark_region_dirty;

    runtime_state.world = &world;
    runtime_state.war_frontier_chunks = 0;
    runtime_state.war_biome_stage_index = 1;
    runtime_state.war_biome_unlock_mask = 0u;

    context.state = &runtime_state;
    context.service_ports = &ports;

    runtime_tick_orchestration_gameplay(&context);
    runtime_tick_orchestration_gameplay(&context);

    BANANA_TEST_ASSERT_INT_EQ(g_gameplay_tick_calls,
                              2,
                              "siege lane should invoke gameplay service on each tick");
    BANANA_TEST_ASSERT_INT_EQ(g_last_population_cap,
                              10,
                              "gameplay call must receive policy-defined population cap");
    BANANA_TEST_ASSERT_TRUE(g_last_reinforcements >= 2,
                            "effective reinforcements must be at least base policy value");
    BANANA_TEST_ASSERT_INT_EQ(g_last_war_tier,
                              (int)RUNTIME_WAR_ESCALATION_SIEGE,
                              "high occupancy must escalate lane to siege tier");
    BANANA_TEST_ASSERT_TRUE(runtime_state.war_frontier_chunks > 0,
                            "war terrain expansion must advance frontier chunks");
    BANANA_TEST_ASSERT_TRUE(g_set_height_calls > 0,
                            "terrain expansion must write tile elevations");
    BANANA_TEST_ASSERT_TRUE(g_mark_dirty_calls > 0,
                            "terrain expansion must mark expanded regions dirty");
    BANANA_TEST_ASSERT_TRUE(runtime_state.war_life_cells != 0u,
                            "war life seed/step must maintain non-empty cellular state");
    BANANA_TEST_ASSERT_TRUE(runtime_state.war_life_generation >= 1,
                            "life simulation should advance generation counter when interval elapses");
}

static void test_render_scene_guards_and_dispatch(void **state)
{
    (void)state;
    RuntimeApplicationServicePorts ports = {0};
    RuntimeTickOrchestrationContext context = {0};
    EngineRuntimeState runtime_state = {0};

    reset_render_tick_state();

    runtime_tick_orchestration_render_scene(NULL);
    BANANA_TEST_ASSERT_INT_EQ(g_submit_scene_calls,
                              0,
                              "null context must not submit a render scene");

    context.state = &runtime_state;
    context.service_ports = &ports;
    runtime_tick_orchestration_render_scene(&context);
    BANANA_TEST_ASSERT_INT_EQ(g_submit_scene_calls,
                              0,
                              "missing render callback must not submit a scene");

    ports.render.submit_scene = stub_submit_scene;
    runtime_tick_orchestration_render_scene(&context);
    BANANA_TEST_ASSERT_INT_EQ(g_submit_scene_calls,
                              1,
                              "valid render callback must be invoked once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_gameplay_rejects_invalid_contexts),
    BANANA_TEST_CASE(test_gameplay_handles_peaceful_lane),
    BANANA_TEST_CASE(test_gameplay_updates_war_metrics_and_expands_frontier),
    BANANA_TEST_CASE(test_render_scene_guards_and_dispatch)
)
