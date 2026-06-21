#include "runtime/engine/support/engine_aux_abi.h"

#include "../../support/test_support.h"

static int g_create_calls = 0;
static int g_attach_calls = 0;
static int g_update_calls = 0;
static int g_signal_calls = 0;

uint32_t runtime_controller_create_and_register(ControllerInstance **controllers,
                                                int max_controllers,
                                                int *inout_controller_count,
                                                const char *type,
                                                float x,
                                                float y,
                                                float z)
{
    (void)controllers; (void)max_controllers; (void)inout_controller_count; (void)type; (void)x; (void)y; (void)z;
    g_create_calls++;
    return 41u;
}

uint32_t runtime_controller_attach_to_entity(World *world,
                                             ControllerInstance **controllers,
                                             int max_controllers,
                                             int *inout_controller_count,
                                             uint32_t entity_id,
                                             const char *type)
{
    (void)world; (void)controllers; (void)max_controllers; (void)inout_controller_count; (void)entity_id; (void)type;
    g_attach_calls++;
    return 42u;
}

int runtime_controller_update_by_id(ControllerInstance **controllers, int controller_count, uint32_t controller_id, float dt)
{
    (void)controllers; (void)controller_count; (void)controller_id; (void)dt;
    g_update_calls++;
    return 1;
}

int runtime_controller_signal_by_id(ControllerInstance **controllers, int controller_count, uint32_t controller_id, const char *signal_name, const void *data)
{
    (void)controllers; (void)controller_count; (void)controller_id; (void)signal_name; (void)data;
    g_signal_calls++;
    return 2;
}

int runtime_world_entity_count(World *world) { return world ? world->entity_count : 0; }
float runtime_world_entity_x(World *world, int index) { return (!world || index < 0 || index >= world->entity_count || !world->entities[index]) ? 0.f : world->entities[index]->position[0]; }
float runtime_world_entity_z(World *world, int index) { return (!world || index < 0 || index >= world->entity_count || !world->entities[index]) ? 0.f : world->entities[index]->position[2]; }
int runtime_world_entity_state(const World *world, int entity_index, ControllerInstance **controllers, int controller_count) { (void)controllers; (void)controller_count; return (world && entity_index >= 0 && entity_index < world->entity_count && world->entities[entity_index] && world->entities[entity_index]->active) ? 1 : 0; }
int runtime_world_active_player_count(const World *world) { int n = 0; if (!world) return 0; for (int i=0;i<world->entity_count;i++) if (world->entities[i] && world->entities[i]->type == ENTITY_TYPE_PLAYER && world->entities[i]->active) n++; return n; }
int runtime_controller_sync_parallel_available(void) { return 1; }

static void test_aux_context_and_wrappers(void **state)
{
    (void)state;
    World world = {0};
    Entity a = { .id = 1, .type = ENTITY_TYPE_PLAYER, .active = 1, .position = {3.f, 0.f, 4.f} };
    world.entities[0] = &a;
    world.entity_count = 1;
    ControllerInstance *controllers[1] = { NULL };
    int controller_count = 0;
    RuntimeEngineAuxContext context = runtime_engine_aux_context(&world, controllers, 1);

    BANANA_TEST_ASSERT_TRUE(context.world == &world, "context must retain world pointer");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_controller_create(controllers, 8, &controller_count, "combat", 0.f, 0.f, 0.f), 41,
                              "controller create must forward to runtime controller helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_controller_attach(context, 8, &controller_count, 1, "combat"), 42,
                              "controller attach must forward to runtime controller helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_controller_update(context, 99, 0.1f), 1,
                              "controller update must forward to runtime helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_controller_signal(context, 99, "sig"), 2,
                              "controller signal must forward to runtime helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_entity_count(&world), 1,
                              "entity count must forward to world helper");
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_engine_aux_entity_x(&world, 0), 3.f, 0.001f,
                                "entity x must forward to world helper");
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_engine_aux_entity_z(&world, 0), 4.f, 0.001f,
                                "entity z must forward to world helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_entity_state(context, 0), 1,
                              "entity state must forward to world helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_active_player_count(&world), 1,
                              "active player count must forward to world helper");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_parallel_sync_available(), 1,
                              "parallel sync availability must forward to controller sync helper");
}

static void test_remediable_reason_codes(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_launch_gate_reason_is_remediable(BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT), 1,
                              "unlinked account must be remediable");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_launch_gate_reason_is_remediable(BANANA_LAUNCH_GATE_REASON_STALE_SESSION), 1,
                              "stale session must be remediable");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_launch_gate_reason_is_remediable(BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE), 1,
                              "steam unavailable must be remediable");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_launch_gate_reason_is_remediable(BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE), 1,
                              "api unavailable must be remediable");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_launch_gate_reason_is_remediable(BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE), 1,
                              "offline unverifiable must be remediable");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_aux_launch_gate_reason_is_remediable(BANANA_LAUNCH_GATE_REASON_SUSPENDED_OR_RESTRICTED), 0,
                              "restricted account must not be remediable");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_aux_context_and_wrappers),
    BANANA_TEST_CASE(test_remediable_reason_codes)
)
