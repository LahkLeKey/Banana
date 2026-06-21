#include "runtime/engine/exports/engine_runtime_context.h"
#include "runtime/input/abi/input_abi.h"
#include "runtime/camera/basis/camera_basis.h"
#include "runtime/player/api/player_api.h"
#include "runtime/player/sync/player_sync_abi.h"
#include "runtime/terrain/terrain_abi.h"
#include "engine.h"

#include "../../support/test_support.h"

#include <string.h>

EngineRuntimeState s_engine_state = {0};
const RuntimeApplicationServicePorts *s_service_ports = NULL;

static int g_click_calls = 0;
static int g_click_norm_calls = 0;
static int g_set_move_calls = 0;
static int g_upsert_calls = 0;
static int g_apply_input_calls = 0;
static int g_set_transform_calls = 0;
static int g_mark_seen_calls = 0;
static int g_update_stale_calls = 0;
static int g_deactivate_calls = 0;
static int g_count_active_calls = 0;

int runtime_input_abi_get_click_count(void) { return 7; }
int runtime_input_abi_get_target_reached_count(void) { return 8; }
int runtime_input_abi_get_has_move_target(void) { return 1; }
int runtime_input_abi_handle_right_click(EngineRuntimeState *state, RuntimeInputCanvasPoint canvas, RuntimeTerrainSampleHeightFn sample_height) { (void)state; (void)canvas; (void)sample_height; g_click_calls++; return 1; }
int runtime_input_abi_handle_right_click_normalized(EngineRuntimeState *state, RuntimeScreenNormalizedPoint normalized, RuntimeTerrainSampleHeightFn sample_height) { (void)state; (void)normalized; (void)sample_height; g_click_norm_calls++; return 2; }
void runtime_input_abi_set_move_input(EngineRuntimeState *state, float input_x, float input_z) { (void)state; (void)input_x; (void)input_z; g_set_move_calls++; }
float runtime_terrain_abi_sample_height(const RuntimeApplicationServicePorts *ports, const EngineRuntimeState *state, float x, float z) { (void)ports; (void)state; return x + z + 1.f; }
int runtime_camera_compute_ground_basis(const float *camera_eye, const float *camera_target, int camera_valid, float *out_forward, float *out_right) { (void)camera_eye; (void)camera_target; (void)camera_valid; out_forward[0]=0.f; out_forward[1]=0.f; out_forward[2]=-1.f; out_right[0]=1.f; out_right[1]=0.f; out_right[2]=0.f; return 1; }
uint32_t runtime_player_api_upsert(World *world, const char *player_guid, const char *player_name, const char *controller_kind, int active, RuntimeTerrainSampleHeightFn terrain_sample_height, RuntimeControllerAttachFn attach_controller, EntityId *inout_primary_player_id) { (void)world; (void)player_guid; (void)player_name; (void)controller_kind; (void)active; (void)terrain_sample_height; (void)attach_controller; g_upsert_calls++; if (inout_primary_player_id) *inout_primary_player_id = 55; return 55; }
void runtime_player_api_apply_input(const char *player_guid, float input_x, float input_z) { (void)player_guid; (void)input_x; (void)input_z; g_apply_input_calls++; }
void runtime_player_api_set_transform(World *world, const char *player_guid, float x, float y, float z, int active, float island_span, RuntimeTerrainSampleHeightFn terrain_sample_height) { (void)world; (void)player_guid; (void)x; (void)y; (void)z; (void)active; (void)island_span; (void)terrain_sample_height; g_set_transform_calls++; }
void runtime_player_sync_abi_mark_seen(const char *player_guid, int64_t current_time_ms) { (void)player_guid; (void)current_time_ms; g_mark_seen_calls++; }
void runtime_player_sync_abi_update_staleness(RuntimePlayerSyncWindow window) { (void)window; g_update_stale_calls++; }
void runtime_player_sync_abi_deactivate_stale(void) { g_deactivate_calls++; }
int runtime_player_sync_abi_count_active(void) { g_count_active_calls++; return 9; }
uint32_t engine_controller_attach(uint32_t entity_id, const char *type_name) { (void)type_name; return entity_id + 1; }

static void reset_state(void)
{
    memset(&s_engine_state, 0, sizeof(s_engine_state));
    g_click_calls = g_click_norm_calls = g_set_move_calls = 0;
    g_upsert_calls = g_apply_input_calls = g_set_transform_calls = 0;
    g_mark_seen_calls = g_update_stale_calls = g_deactivate_calls = g_count_active_calls = 0;
}

static void test_input_export_wrappers(void **state)
{
    (void)state;
    reset_state();
    s_service_ports = (const RuntimeApplicationServicePorts *)0x1;
    BANANA_TEST_ASSERT_INT_EQ(engine_get_click_count(), 7, "click count getter must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_target_reached_count(), 8, "target reached getter must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_has_move_target(), 1, "move target getter must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_pbj_pickup_collected(), 0, "pickup collected getter must reflect state flag");
    s_engine_state.pbj_pickup_collected = 1;
    BANANA_TEST_ASSERT_INT_EQ(engine_get_pbj_pickup_collected(), 1, "pickup collected getter must return 1 when state flag set");
    BANANA_TEST_ASSERT_INT_EQ(engine_handle_right_click(10.f, 20.f), 1, "right click handler must forward to ABI");
    BANANA_TEST_ASSERT_INT_EQ(engine_handle_right_click_normalized(0.2f, -0.5f), 2, "normalized right click handler must forward to ABI");
    engine_set_move_input(1.f, 0.f);
    BANANA_TEST_ASSERT_INT_EQ(g_set_move_calls, 1, "set_move_input must forward to ABI");
}

static void test_player_export_wrappers(void **state)
{
    (void)state;
    reset_state();
    World world = {0};
    s_engine_state.world = &world;
    BANANA_TEST_ASSERT_INT_EQ((int)engine_player_upsert("guid", "name", "human", 1), 55, "player upsert must forward to API");
    BANANA_TEST_ASSERT_INT_EQ((int)s_engine_state.player_id, 55, "player upsert must update runtime player id");
    engine_player_apply_input("guid", 1.f, -1.f);
    engine_player_set_transform("guid", 1.f, 2.f, 3.f, 1);
    engine_player_sync_mark_seen("guid", 100);
    engine_player_sync_update_staleness(200, 50);
    engine_player_sync_deactivate_stale();
    BANANA_TEST_ASSERT_INT_EQ(engine_player_sync_count_active(), 9, "active count getter must forward to sync ABI");
    BANANA_TEST_ASSERT_INT_EQ(g_upsert_calls, 1, "player upsert API must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_input_calls, 1, "player apply input API must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_set_transform_calls, 1, "player set transform API must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_mark_seen_calls, 1, "mark seen sync ABI must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_update_stale_calls, 1, "update staleness sync ABI must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_deactivate_calls, 1, "deactivate stale sync ABI must be called once");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_input_export_wrappers),
    BANANA_TEST_CASE(test_player_export_wrappers)
)
