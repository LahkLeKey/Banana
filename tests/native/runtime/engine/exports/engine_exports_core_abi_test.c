#include "runtime/engine/exports/engine_runtime_context.h"
#include "runtime/engine/support/engine_aux_abi.h"
#include "engine.h"

#include "runtime/support/test_support.h"

#include <string.h>

EngineRuntimeState s_engine_state = {0};
const RuntimeApplicationServicePorts *s_service_ports = (const RuntimeApplicationServicePorts *)0x1;

static int g_render_calls = 0;
static int g_physics_step_calls = 0;
static int g_add_body_calls = 0;
static int g_update_body_calls = 0;
static int g_world_spawn_calls = 0;
static int g_world_tick_calls = 0;
static int g_set_height_calls = 0;
static int g_mark_region_calls = 0;

void runtime_engine_host_render_frame(Renderer *renderer) { (void)renderer; g_render_calls++; }
void runtime_physics_abi_step(PhysicsWorld *physics, float dt) { (void)physics; (void)dt; g_physics_step_calls++; }
void runtime_physics_abi_add_body(PhysicsWorld *physics, uint32_t id, float mass, float x, float y, float z) { (void)physics; (void)id; (void)mass; (void)x; (void)y; (void)z; g_add_body_calls++; }
void runtime_physics_abi_update_body(PhysicsWorld *physics, uint32_t id, const float *pos, const float *vel) { (void)physics; (void)id; (void)pos; (void)vel; g_update_body_calls++; }
uint32_t runtime_world_abi_spawn(World *world, int type, float x, float y, float z) { (void)world; (void)type; (void)x; (void)y; (void)z; g_world_spawn_calls++; return 77u; }
void runtime_world_abi_tick(World *world, float dt) { (void)world; (void)dt; g_world_tick_calls++; }
int runtime_terrain_abi_set_height(const RuntimeApplicationServicePorts *ports, EngineRuntimeState *state, int x, int z, int elevation) { (void)ports; (void)state; (void)x; (void)z; (void)elevation; g_set_height_calls++; return 1; }
void runtime_terrain_abi_mark_region_dirty(const RuntimeApplicationServicePorts *ports, EngineRuntimeState *state, int min_x, int min_z, int max_x, int max_z) { (void)ports; (void)state; (void)min_x; (void)min_z; (void)max_x; (void)max_z; g_mark_region_calls++; }
RuntimeEngineAuxContext runtime_engine_aux_context(World *world, ControllerInstance **controllers, int controller_count) { RuntimeEngineAuxContext context; context.world = world; context.controllers = controllers; context.controller_count = controller_count; return context; }
RuntimeEngineAuxContext runtime_engine_exports_aux_context(void) { return runtime_engine_aux_context(s_engine_state.world, s_engine_state.controllers, s_engine_state.controller_count); }
uint32_t runtime_engine_aux_controller_create(ControllerInstance **controllers, int max_active_controllers, int *inout_controller_count, const char *type, float x, float y, float z) { (void)controllers; (void)max_active_controllers; (void)inout_controller_count; (void)type; (void)x; (void)y; (void)z; return 91u; }
uint32_t runtime_engine_aux_controller_attach(RuntimeEngineAuxContext context, int max_active_controllers, int *inout_controller_count, uint32_t entity_id, const char *type) { (void)context; (void)max_active_controllers; (void)inout_controller_count; (void)entity_id; (void)type; return 92u; }
int runtime_engine_aux_controller_update(RuntimeEngineAuxContext context, uint32_t controller_id, float dt) { (void)context; (void)controller_id; (void)dt; return 93; }
int runtime_engine_aux_controller_signal(RuntimeEngineAuxContext context, uint32_t controller_id, const char *signal_name) { (void)context; (void)controller_id; (void)signal_name; return 94; }
int runtime_engine_aux_entity_count(World *world) { return world ? world->entity_count : 0; }
float runtime_engine_aux_entity_x(World *world, int idx) { return (!world || idx < 0 || idx >= world->entity_count || !world->entities[idx]) ? 0.f : world->entities[idx]->position[0]; }
float runtime_engine_aux_entity_z(World *world, int idx) { return (!world || idx < 0 || idx >= world->entity_count || !world->entities[idx]) ? 0.f : world->entities[idx]->position[2]; }
int runtime_engine_aux_entity_state(RuntimeEngineAuxContext context, int idx) { return (context.world && idx >= 0 && idx < context.world->entity_count && context.world->entities[idx] && context.world->entities[idx]->active) ? 1 : 0; }
int runtime_engine_aux_active_player_count(World *world) { int n=0; if (!world) return 0; for (int i=0;i<world->entity_count;i++) if (world->entities[i] && world->entities[i]->type==ENTITY_TYPE_PLAYER && world->entities[i]->active) n++; return n; }
int runtime_engine_aux_parallel_sync_available(void) { return 1; }
ControllerInstance *runtime_controller_find_by_id(ControllerInstance **controllers, int controller_count, uint32_t controller_id) { for (int i=0;i<controller_count;i++) if (controllers[i] && controllers[i]->id==controller_id) return controllers[i]; return NULL; }

static void reset_calls(void)
{
    memset(&s_engine_state, 0, sizeof(s_engine_state));
    g_render_calls = g_physics_step_calls = g_add_body_calls = g_update_body_calls = 0;
    g_world_spawn_calls = g_world_tick_calls = g_set_height_calls = g_mark_region_calls = 0;
}

static void test_core_abi_wrappers(void **state)
{
    (void)state;
    reset_calls();
    World world = {0};
    Entity npc = { .id = 1, .type = ENTITY_TYPE_NPC, .active = 1, .controller_id = 11, .position = {2.f, 0.f, 3.f} };
    Entity player = { .id = 2, .type = ENTITY_TYPE_PLAYER, .active = 1, .position = {5.f, 0.f, 6.f} };
    ControllerInstance ctrl = { .id = 11, .team = CONTROLLER_TEAM_BEAN };
    world.entities[0] = &npc;
    world.entities[1] = &player;
    world.entity_count = 2;
    s_engine_state.world = &world;
    s_engine_state.controllers[0] = &ctrl;
    s_engine_state.controller_count = 1;

    engine_render_frame();
    physics_step(0.1f);
    physics_add_body(1, 1.f, 0.f, 0.f, 0.f);
    physics_update_body(1, NULL, NULL);
    BANANA_TEST_ASSERT_INT_EQ((int)engine_world_spawn(1, 0.f, 0.f, 0.f), 77, "world spawn must forward to abi");
    engine_world_tick(0.2f);
    BANANA_TEST_ASSERT_INT_EQ(engine_terrain_set_height(1, 2, 3), 1, "terrain set height must forward to abi");
    engine_terrain_mark_region_dirty(0, 0, 2, 2);
    BANANA_TEST_ASSERT_INT_EQ((int)engine_controller_create("combat", 0.f, 0.f, 0.f), 91, "controller create must forward to aux abi");
    BANANA_TEST_ASSERT_INT_EQ((int)engine_controller_attach(1, "combat"), 92, "controller attach must forward to aux abi");
    engine_controller_update(1, 0.1f);
    engine_controller_signal(1, "sig");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_entity_count(), 2, "entity count getter must forward to aux abi");
    BANANA_TEST_ASSERT_FLOAT_EQ(engine_get_entity_x(0), 2.f, 0.001f, "entity x getter must expose aux value");
    BANANA_TEST_ASSERT_FLOAT_EQ(engine_get_entity_z(0), 3.f, 0.001f, "entity z getter must expose aux value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_entity_state(0), 1, "entity state getter must expose aux value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_entity_team(0), (int)CONTROLLER_TEAM_BEAN, "entity team getter must resolve team from controller");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_entity_team(1), (int)CONTROLLER_TEAM_NEUTRAL, "non-NPC entity team must default neutral");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_active_player_count(), 1, "active player count getter must forward to aux abi");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_parallel_sync_available(), 1, "parallel sync available getter must forward to aux abi");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_parallel_sync_enabled(), 1, "parallel sync enabled getter currently mirrors availability");
    BANANA_TEST_ASSERT_INT_EQ(g_render_calls, 1, "render wrapper must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_mark_region_calls, 1, "mark region wrapper must be called once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_core_abi_wrappers)
)
