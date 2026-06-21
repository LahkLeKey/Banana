#include "runtime/player/runtime/player_motion_host.h"

#include "../../support/test_support.h"

#include <string.h>

static int g_basis_calls = 0;
static int g_registry_count = 0;
static NativePlayerBinding g_bindings[BANANA_MAX_NATIVE_PLAYERS + 2];
static int g_apply_calls = 0;
static int g_apply_binding_count = 0;
static EntityId g_apply_primary_player_id = 0;

int runtime_camera_compute_ground_basis(const float *camera_eye,
                                        const float *camera_target,
                                        int camera_valid,
                                        float *forward,
                                        float *right)
{
    (void)camera_eye;
    (void)camera_target;
    (void)camera_valid;

    g_basis_calls += 1;
    forward[0] = 0.0f;
    forward[1] = 0.0f;
    forward[2] = -1.0f;
    right[0] = 1.0f;
    right[1] = 0.0f;
    right[2] = 0.0f;
    return 1;
}

int runtime_player_registry_count(void)
{
    return g_registry_count;
}

NativePlayerBinding *runtime_player_registry_get(int index)
{
    if (index < 0 || index >= g_registry_count)
        return NULL;
    return &g_bindings[index];
}

void runtime_player_motion_apply(World *world,
                                 NativePlayerBinding **bindings,
                                 int binding_count,
                                 EntityId primary_player_id,
                                 float move_input_x,
                                 float move_input_z,
                                 const float *forward,
                                 const float *right,
                                 float speed,
                                 float dt,
                                 float island_span,
                                 RuntimeTerrainSampleHeightFn sample_height)
{
    (void)world;
    (void)bindings;
    (void)move_input_x;
    (void)move_input_z;
    (void)forward;
    (void)right;
    (void)speed;
    (void)dt;
    (void)island_span;
    (void)sample_height;

    g_apply_calls += 1;
    g_apply_binding_count = binding_count;
    g_apply_primary_player_id = primary_player_id;
}

static void reset_state(void)
{
    memset(g_bindings, 0, sizeof(g_bindings));
    g_basis_calls = 0;
    g_registry_count = 0;
    g_apply_calls = 0;
    g_apply_binding_count = -1;
    g_apply_primary_player_id = 0;
}

static void test_motion_host_returns_early_for_null_world(void **state)
{
    float camera_eye[3] = {0.0f, 0.0f, 0.0f};
    float camera_target[3] = {0.0f, 0.0f, -1.0f};

    (void)state;
    reset_state();

    runtime_player_motion_tick(NULL,
                               7,
                               1.0f,
                               0.0f,
                               camera_eye,
                               camera_target,
                               1,
                               10.0f,
                               0.016f,
                               100.0f,
                               NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_basis_calls, 0, "null world should skip basis compute");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_calls, 0, "null world should skip motion apply");
}

static void test_motion_host_clamps_binding_count_and_forwards_inputs(void **state)
{
    World fake_world;
    float camera_eye[3] = {0.0f, 12.0f, 8.0f};
    float camera_target[3] = {0.0f, 0.0f, 0.0f};

    (void)state;
    reset_state();

    memset(&fake_world, 0, sizeof(fake_world));
    g_registry_count = BANANA_MAX_NATIVE_PLAYERS + 2;

    runtime_player_motion_tick(&fake_world,
                               99,
                               -1.0f,
                               1.0f,
                               camera_eye,
                               camera_target,
                               1,
                               7.0f,
                               0.032f,
                               300.0f,
                               NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_basis_calls, 1, "motion host should compute camera basis once");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_calls, 1, "motion host should call motion apply once");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_binding_count,
                              BANANA_MAX_NATIVE_PLAYERS,
                              "motion host should clamp binding count to max native players");
    BANANA_TEST_ASSERT_INT_EQ((int)g_apply_primary_player_id,
                              99,
                              "motion host should forward primary player id");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_motion_host_returns_early_for_null_world),
    BANANA_TEST_CASE(test_motion_host_clamps_binding_count_and_forwards_inputs)
)
