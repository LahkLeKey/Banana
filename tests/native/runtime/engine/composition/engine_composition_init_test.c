#include "runtime/engine/composition/engine_composition_init.h"
#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/state/engine_state.h"
#include "render/backend.h"
#include "player/build/player_builds.h"
#include "engine_serialize.h"
#include "ai/wildlife_controller.h"
#include "ai/combat_controller.h"
#include "render/window.h"
#include "render/renderer.h"
#include "render/mesh.h"
#include "physics/world.h"
#include "world/world.h"
#include "../../support/test_support.h"

#include <stdint.h>
#include <string.h>

static int g_service_ports_null = 0;
static int g_preflight_result = 0;
static int g_window_fail = 0;
static int g_renderer_fail = 0;
static int g_mesh_fail = 0;
static int g_player_builds_result = 0;
static int g_terrain_build_result = 1;
static int g_bootstrap_result = 1;
static int g_requested_backend = BANANA_RENDER_BACKEND_DX12;
static int g_active_backend = BANANA_RENDER_BACKEND_HEADLESS;
static int g_renderer_attach_calls = 0;
static int g_wildlife_register_calls = 0;
static int g_combat_register_calls = 0;
static int g_serialize_reset_calls = 0;
static int g_spawn_entity_calls = 0;
static int g_last_spawn_type = -1;
static EngineRuntimeState *g_last_preflight_state = NULL;
static EngineRuntimeState *g_last_bootstrap_state = NULL;
static World *g_last_spawn_default_state = NULL;

static const RuntimeApplicationServicePorts g_ports = {{0}};
static World g_world;
static Window *const g_window = (Window *)(uintptr_t)0x1001;
static Renderer *const g_renderer = (Renderer *)(uintptr_t)0x1002;
static PhysicsWorld *const g_physics_world = (PhysicsWorld *)(uintptr_t)0x1003;
static Mesh *const g_mesh = (Mesh *)(uintptr_t)0x1004;

int runtime_tick_budget_policy_controller_war_reinforcements_per_tick(void)
{
    return 4;
}

static float stub_sample_height(float x, float z)
{
    return x + z;
}

static uint32_t stub_attach_controller(uint32_t entity_id, const char *type_name)
{
    (void)entity_id;
    (void)type_name;
    return 321u;
}

static void reset_state(void)
{
    g_service_ports_null = 0;
    g_preflight_result = 0;
    g_window_fail = 0;
    g_renderer_fail = 0;
    g_mesh_fail = 0;
    g_player_builds_result = 0;
    g_terrain_build_result = 1;
    g_bootstrap_result = 1;
    g_requested_backend = BANANA_RENDER_BACKEND_DX12;
    g_active_backend = BANANA_RENDER_BACKEND_HEADLESS;
    g_renderer_attach_calls = 0;
    g_wildlife_register_calls = 0;
    g_combat_register_calls = 0;
    g_serialize_reset_calls = 0;
    g_spawn_entity_calls = 0;
    g_last_spawn_type = -1;
    g_last_preflight_state = NULL;
    g_last_bootstrap_state = NULL;
    g_last_spawn_default_state = NULL;
}

const RuntimeApplicationServicePorts *runtime_application_service_ports(void)
{
    return g_service_ports_null ? NULL : &g_ports;
}

int runtime_engine_lifecycle_preflight_launch_gate(EngineRuntimeState *state)
{
    g_last_preflight_state = state;
    return g_preflight_result;
}

void wildlife_controller_register(void)
{
    g_wildlife_register_calls += 1;
}

void combat_controller_register(void)
{
    g_combat_register_calls += 1;
}

BananaRenderBackend banana_render_backend_requested(void)
{
    return (BananaRenderBackend)g_requested_backend;
}

BananaRenderBackend banana_render_backend_active(void)
{
    return (BananaRenderBackend)g_active_backend;
}

const char *banana_render_backend_name(BananaRenderBackend backend)
{
    switch (backend)
    {
    case BANANA_RENDER_BACKEND_GLFW:
        return "glfw";
    case BANANA_RENDER_BACKEND_DX12:
        return "dx12";
    case BANANA_RENDER_BACKEND_HEADLESS:
        return "headless";
    default:
        return "unknown";
    }
}

const char *banana_render_backend_status(void)
{
    return "ok";
}

Window *window_create(int width, int height, const char *title)
{
    (void)width;
    (void)height;
    (void)title;
    return g_window_fail ? NULL : g_window;
}

Renderer *renderer_create(int width, int height)
{
    (void)width;
    (void)height;
    return g_renderer_fail ? NULL : g_renderer;
}

void *window_get_native_handle(Window *window)
{
    (void)window;
    return (void *)(uintptr_t)0x1111;
}

void renderer_attach_native_window(Renderer *renderer, void *native_window)
{
    (void)renderer;
    (void)native_window;
    g_renderer_attach_calls += 1;
}

PhysicsWorld *physics_world_create(void)
{
    return g_physics_world;
}

World *world_create(void)
{
    memset(&g_world, 0, sizeof(g_world));
    return &g_world;
}

EntityId world_spawn_entity(World *world, EntityType type, float x, float y, float z)
{
    (void)world;
    (void)x;
    (void)y;
    (void)z;
    g_spawn_entity_calls += 1;
    g_last_spawn_type = (int)type;
    return 99u;
}

Entity *world_get_entity(World *world, EntityId id)
{
    static Entity entity;

    (void)world;
    if (id == 0u)
        return NULL;

    memset(&entity, 0, sizeof(entity));
    entity.id = id;
    return &entity;
}

Mesh *mesh_create_banana_vector(float radius_scale, float length_scale, float curve_scale,
                                float tip_taper, int quality)
{
    (void)radius_scale;
    (void)length_scale;
    (void)curve_scale;
    (void)tip_taper;
    (void)quality;
    return g_mesh_fail ? NULL : g_mesh;
}

Mesh *mesh_create_peanut_butter_pickup_asset(int quality)
{
    (void)quality;
    return g_mesh_fail ? NULL : g_mesh;
}

Mesh *mesh_create_banana_block_like(int quality)
{
    (void)quality;
    return g_mesh_fail ? NULL : g_mesh;
}

Mesh *mesh_create_banana_orb_like(int quality)
{
    (void)quality;
    return g_mesh_fail ? NULL : g_mesh;
}

int player_builds_init(void)
{
    return g_player_builds_result;
}

int runtime_engine_lifecycle_build_terrain(unsigned char *height_grid,
                                          int terrain_size,
                                          int terrain_max_layers,
                                          int chunk_size,
                                          RuntimeTerrainChunk *chunks,
                                          int chunk_cols,
                                          int chunk_rows)
{
    (void)height_grid;
    (void)terrain_size;
    (void)terrain_max_layers;
    (void)chunk_size;
    (void)chunks;
    (void)chunk_cols;
    (void)chunk_rows;
    return g_terrain_build_result;
}

int runtime_engine_lifecycle_bootstrap_primary_player(World *world,
                                                      EntityId player_id,
                                                      RuntimeTerrainSampleFn terrain_sample_height)
{
    (void)world;
    (void)player_id;
    (void)terrain_sample_height;
    g_last_bootstrap_state = (EngineRuntimeState *)world;
    return g_bootstrap_result;
}

int runtime_engine_lifecycle_spawn_default_actors(World *world,
                                                  RuntimeTerrainSampleFn terrain_sample_height,
                                                  RuntimeEngineAttachControllerFn attach_controller)
{
    (void)world;
    (void)terrain_sample_height;
    (void)attach_controller;
    g_last_spawn_default_state = world;
    return 0;
}

void engine_serialize_reset(void)
{
    g_serialize_reset_calls += 1;
}

static void test_composition_init_rejects_null_inputs(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state = {0};

    reset_state();
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(NULL,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must reject null state");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    NULL,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must reject null terrain sampler");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    NULL),
                              -1,
                              "composition init must reject null controller attachment");
}

static void test_composition_init_rejects_ports_and_preflight_failures(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state = {0};

    reset_state();
    g_service_ports_null = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must reject missing service ports");

    reset_state();
    g_preflight_result = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must propagate preflight failures");
    BANANA_TEST_ASSERT_TRUE(g_last_preflight_state == &runtime_state,
                            "preflight must receive the runtime state pointer");
}

static void test_composition_init_short_circuits_when_already_initialized(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state = {0};

    reset_state();
    runtime_state.engine_initialized = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              0,
                              "composition init must short-circuit when already initialized");
}

static void test_composition_init_covers_backend_and_resource_failures(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state = {0};

    reset_state();
    memset(&runtime_state, 0, sizeof(runtime_state));
    g_requested_backend = BANANA_RENDER_BACKEND_DX12;
    g_active_backend = BANANA_RENDER_BACKEND_HEADLESS;
    g_window_fail = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must fail when window creation fails");
    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_register_calls, 1,
                              "controller registration must occur before window creation");
    BANANA_TEST_ASSERT_INT_EQ(g_combat_register_calls, 1,
                              "controller registration must occur before window creation");

    reset_state();
    memset(&runtime_state, 0, sizeof(runtime_state));
    g_renderer_fail = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must fail when renderer creation fails");

    reset_state();
    memset(&runtime_state, 0, sizeof(runtime_state));
    g_mesh_fail = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must fail when mesh creation fails");

    reset_state();
    memset(&runtime_state, 0, sizeof(runtime_state));
    g_player_builds_result = -1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must fail when player build initialization fails");

    reset_state();
    memset(&runtime_state, 0, sizeof(runtime_state));
    g_terrain_build_result = 0;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must fail when terrain build fails");

    reset_state();
    memset(&runtime_state, 0, sizeof(runtime_state));
    g_bootstrap_result = 0;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1280,
                                                                    720,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              -1,
                              "composition init must fail when the primary player bootstrap fails");
}

static void test_composition_init_success_path(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state = {0};

    reset_state();
    g_requested_backend = BANANA_RENDER_BACKEND_DX12;
    g_active_backend = BANANA_RENDER_BACKEND_HEADLESS;
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init_state(&runtime_state,
                                                                    1920,
                                                                    1080,
                                                                    stub_sample_height,
                                                                    stub_attach_controller),
                              0,
                              "composition init must succeed with healthy dependencies");
    BANANA_TEST_ASSERT_TRUE(runtime_state.engine_initialized,
                            "success path must mark the runtime as initialized");
    BANANA_TEST_ASSERT_TRUE(runtime_state.window == g_window,
                            "success path must store the created window");
    BANANA_TEST_ASSERT_TRUE(runtime_state.renderer == g_renderer,
                            "success path must store the created renderer");
    BANANA_TEST_ASSERT_TRUE(runtime_state.physics == g_physics_world,
                            "success path must store the created physics world");
    BANANA_TEST_ASSERT_TRUE(runtime_state.world == &g_world,
                            "success path must store the created world");
    BANANA_TEST_ASSERT_INT_EQ(g_renderer_attach_calls, 1,
                              "success path must attach the native window once");
    BANANA_TEST_ASSERT_INT_EQ(g_serialize_reset_calls, 1,
                              "success path must reset serialization state");
    BANANA_TEST_ASSERT_TRUE(g_last_spawn_default_state == &g_world,
                            "success path must invoke default actor spawning");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_composition_init_rejects_null_inputs),
    BANANA_TEST_CASE(test_composition_init_rejects_ports_and_preflight_failures),
    BANANA_TEST_CASE(test_composition_init_short_circuits_when_already_initialized),
    BANANA_TEST_CASE(test_composition_init_covers_backend_and_resource_failures),
    BANANA_TEST_CASE(test_composition_init_success_path)
)
