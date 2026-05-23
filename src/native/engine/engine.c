#include "engine.h"
#include "engine_serialize.h"
#include "render/backend.h"
#include "ai/npc_merchant.h"
#include "ai/wildlife_controller.h"
#include "ui/ui.h"
#include "runtime/controller_sync.h"
#include "runtime/controller_runtime.h"
#include "runtime/controller_attach.h"
#include "runtime/engine_host.h"
#include "runtime/engine_lifecycle.h"
#include "runtime/engine_tick.h"
#include "runtime/camera_follow.h"
#include "runtime/camera_basis.h"
#include "runtime/render_material.h"
#include "runtime/move_target_domain.h"
#include "runtime/player_motion.h"
#include "runtime/player_motion_host.h"
#include "runtime/player_api.h"
#include "runtime/player_builds.h"
#include "runtime/player_registry.h"
#include "runtime/physics_abi.h"
#include "runtime/input_contract.h"
#include "runtime/render_submit.h"
#include "runtime/terrain_generation.h"
#include "runtime/terrain_height.h"
#include "runtime/terrain_host.h"
#include "runtime/terrain_mutation.h"
#include "runtime/terrain_runtime.h"
#include "runtime/tick_phases.h"
#include "runtime/world_metrics.h"
#include "runtime/world_abi.h"
#include "runtime/world_telemetry.h"
#include "runtime/wildlife_gameplay.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ── Global singletons ────────────────────────────────────────────────────── */
static Window *s_window = NULL;
static Renderer *s_renderer = NULL;
static PhysicsWorld *s_physics = NULL;
static World *s_world = NULL;
static Mesh *s_entity_mesh = NULL; /* banana mesh for actors */
static Mesh *s_pbj_pickup_mesh = NULL;
static EntityId s_player_id = 0;
/* Initialization guard */
static int s_engine_initialized = 0;
/* Terrain is chunked so WFC/CA updates can rebuild only dirty regions. */

#define BANANA_TERRAIN_SIZE 18
#define BANANA_TERRAIN_MAX_LAYERS 4
#define BANANA_TERRAIN_CHUNK_SIZE 6
#define BANANA_TERRAIN_CHUNK_COLS (BANANA_TERRAIN_SIZE / BANANA_TERRAIN_CHUNK_SIZE)
#define BANANA_TERRAIN_CHUNK_ROWS (BANANA_TERRAIN_SIZE / BANANA_TERRAIN_CHUNK_SIZE)
#define BANANA_TERRAIN_TOTAL_CHUNKS (BANANA_TERRAIN_CHUNK_COLS * BANANA_TERRAIN_CHUNK_ROWS)

static unsigned char s_terrain_height[BANANA_TERRAIN_SIZE][BANANA_TERRAIN_SIZE];
static RuntimeTerrainChunk s_terrain_chunks[BANANA_TERRAIN_TOTAL_CHUNKS];
static int s_terrain_initialized = 0;
static int s_viewport_width = 0;
static int s_viewport_height = 0;
static float s_camera_eye[3] = {0.f, 0.f, 0.f};
static float s_camera_target[3] = {0.f, 0.f, 0.f};
static int s_camera_valid = 0;
static float s_move_input_x = 0.f;
static float s_move_input_z = 0.f;
static RuntimeMoveTargetState s_move_target_state = {0};
static int s_merchants_seeded = 0;
static int s_pbj_pickup_collected = 0;

/* ── Active controller table ─────────────────────────────────────────────── */
#define BANANA_MAX_ACTIVE_CONTROLLERS 256
static ControllerInstance *s_controllers[BANANA_MAX_ACTIVE_CONTROLLERS];
static int s_controller_count = 0;

/* ── Deterministic terrain generation/rendering (C-owned) ───────────────── */

static void terrain_mark_chunk_dirty(int chunk_x, int chunk_z)
{
    runtime_terrain_host_mark_chunk_dirty(s_terrain_chunks,
                                          BANANA_TERRAIN_CHUNK_COLS,
                                          BANANA_TERRAIN_CHUNK_ROWS,
                                          chunk_x,
                                          chunk_z);
}

static int terrain_rebuild_dirty_chunks_runtime(int max_chunks)
{
    return runtime_terrain_host_rebuild_dirty(&s_terrain_height[0][0],
                                              BANANA_TERRAIN_SIZE,
                                              BANANA_TERRAIN_CHUNK_SIZE,
                                              s_terrain_chunks,
                                              BANANA_TERRAIN_CHUNK_COLS,
                                              BANANA_TERRAIN_CHUNK_ROWS,
                                              max_chunks);
}

static float terrain_sample_height(float x, float z);

static void terrain_draw_runtime(void)
{
    runtime_terrain_draw(s_renderer,
                         s_terrain_chunks,
                         s_terrain_initialized,
                         BANANA_TERRAIN_SIZE,
                         BANANA_TERRAIN_CHUNK_SIZE,
                         BANANA_TERRAIN_CHUNK_COLS,
                         BANANA_TERRAIN_CHUNK_ROWS);
}

static float terrain_sample_height(float x, float z)
{
    return runtime_terrain_sample_height(&s_terrain_height[0][0],
                                         BANANA_TERRAIN_SIZE,
                                         x,
                                         z,
                                         0.48f,
                                         -1.45f);
}

/* Banana raycast contract: map normalized screen input into a terrain-ground
 * world-space target on the island for click-to-move behavior. */
static int banana_raycast_click_to_world_target(float normalized_x,
                                                float normalized_y,
                                                float *out_target_x,
                                                float *out_target_z)
{
    float forward[3] = {0.f, 0.f, 0.f};
    float right[3] = {0.f, 0.f, 0.f};
    float up[3] = {0.f, 1.f, 0.f};
    float world_up[3] = {0.f, 1.f, 0.f};
    float dir[3] = {0.f, 0.f, 0.f};
    const float fov_radians = 44.f * (3.14159265358979323846f / 180.f);
    const float tan_half_fov = tanf(fov_radians * 0.5f);
    const float island_span = (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f;
    float aspect = 1.f;
    float ground_y = 0.f;
    float t = 0.f;
    float target_x = 0.f;
    float target_z = 0.f;

    if (!out_target_x || !out_target_z || !s_camera_valid || s_viewport_height <= 0)
        return 0;

    forward[0] = s_camera_target[0] - s_camera_eye[0];
    forward[1] = s_camera_target[1] - s_camera_eye[1];
    forward[2] = s_camera_target[2] - s_camera_eye[2];
    {
        float forward_len = sqrtf(forward[0] * forward[0] +
                                  forward[1] * forward[1] +
                                  forward[2] * forward[2]);
        if (forward_len <= 1e-6f)
            return 0;
        forward[0] /= forward_len;
        forward[1] /= forward_len;
        forward[2] /= forward_len;
    }

    right[0] = forward[1] * world_up[2] - forward[2] * world_up[1];
    right[1] = forward[2] * world_up[0] - forward[0] * world_up[2];
    right[2] = forward[0] * world_up[1] - forward[1] * world_up[0];
    {
        float right_len = sqrtf(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
        if (right_len <= 1e-6f)
            return 0;
        right[0] /= right_len;
        right[1] /= right_len;
        right[2] /= right_len;
    }

    up[0] = right[1] * forward[2] - right[2] * forward[1];
    up[1] = right[2] * forward[0] - right[0] * forward[2];
    up[2] = right[0] * forward[1] - right[1] * forward[0];
    {
        float up_len = sqrtf(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);
        if (up_len <= 1e-6f)
            return 0;
        up[0] /= up_len;
        up[1] /= up_len;
        up[2] /= up_len;
    }

    if (up[1] <= 1e-4f)
        return 0;

    aspect = (float)s_viewport_width / (float)s_viewport_height;
    dir[0] = forward[0] + right[0] * normalized_x * aspect * tan_half_fov + up[0] * normalized_y * tan_half_fov;
    dir[1] = forward[1] + right[1] * normalized_x * aspect * tan_half_fov + up[1] * normalized_y * tan_half_fov;
    dir[2] = forward[2] + right[2] * normalized_x * aspect * tan_half_fov + up[2] * normalized_y * tan_half_fov;

    {
        float dir_len = sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
        if (dir_len <= 1e-6f)
            return 0;
        dir[0] /= dir_len;
        dir[1] /= dir_len;
        dir[2] /= dir_len;
    }

    if (fabsf(dir[1]) <= 1e-6f)
        return 0;

    ground_y = terrain_sample_height(s_camera_target[0], s_camera_target[2]);
    t = (ground_y - s_camera_eye[1]) / dir[1];
    if (t <= 0.f)
        return 0;

    target_x = s_camera_eye[0] + dir[0] * t;
    target_z = s_camera_eye[2] + dir[2] * t;

    {
        int i = 0;
        for (i = 0; i < 2; i++)
        {
            ground_y = terrain_sample_height(target_x, target_z);
            t = (ground_y - s_camera_eye[1]) / dir[1];
            if (t <= 0.f)
                break;
            target_x = s_camera_eye[0] + dir[0] * t;
            target_z = s_camera_eye[2] + dir[2] * t;
        }
    }

    if (target_x < -island_span)
        target_x = -island_span;
    if (target_x > island_span)
        target_x = island_span;
    if (target_z < -island_span)
        target_z = -island_span;
    if (target_z > island_span)
        target_z = island_span;

    *out_target_x = target_x;
    *out_target_z = target_z;
    return 1;
}

static void update_player_motion(float dt)
{
    float move_x = s_move_input_x;
    float move_z = s_move_input_z;

    if (s_move_target_state.has_target && s_world && s_player_id)
    {
        Entity *player = world_get_entity(s_world, s_player_id);
        if (player && player->active)
        {
            RuntimeMoveTargetSteering steering = {0};
            if (runtime_move_target_compute_steering(&s_move_target_state,
                                                     player->position,
                                                     s_camera_eye,
                                                     s_camera_target,
                                                     s_camera_valid,
                                                     0.35f,
                                                     &steering) &&
                steering.reached_target)
            {
                runtime_move_target_clear(&s_move_target_state);
                runtime_input_contract_mark_target_reached();
                move_x = 0.f;
                move_z = 0.f;
            }
            else
            {
                move_x = steering.move_x;
                move_z = steering.move_z;
                runtime_input_contract_set_has_move_target(steering.has_target);
            }
        }
        else
        {
            runtime_move_target_clear(&s_move_target_state);
            runtime_input_contract_set_has_move_target(0);
        }
    }

    runtime_player_motion_tick(s_world,
                               s_player_id,
                               move_x,
                               move_z,
                               s_camera_eye,
                               s_camera_target,
                               s_camera_valid,
                               5.5f,
                               dt,
                               (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f,
                               terrain_sample_height);

    s_move_input_x = move_x;
    s_move_input_z = move_z;
}

static void follow_player_camera(void)
{
    runtime_camera_follow_player(s_renderer,
                                 s_world,
                                 s_player_id,
                                 s_viewport_width,
                                 s_viewport_height,
                                 s_camera_eye,
                                 s_camera_target,
                                 &s_camera_valid);
}

static void apply_click_move_input(float normalized_x, float normalized_y)
{
    float target_x = 0.f;
    float target_z = 0.f;
    if (banana_raycast_click_to_world_target(normalized_x, normalized_y, &target_x, &target_z))
    {
    RuntimeMoveTargetPoint target = {target_x, target_z};
    if (runtime_move_target_set(&s_move_target_state, target))
    {
        runtime_input_contract_set_has_move_target(1);
        fprintf(stdout,
            "[engine] banana-raycast target set x=%.2f z=%.2f from normalized=(%.2f, %.2f)\n",
            target_x,
            target_z,
            normalized_x,
            normalized_y);
    }
    else
    {
        runtime_move_target_clear(&s_move_target_state);
        runtime_input_contract_set_has_move_target(0);
        fprintf(stderr,
            "[engine] banana-raycast rejected invalid target from normalized=(%.2f, %.2f)\n",
            normalized_x,
            normalized_y);
    }
    }
    else
    {
    runtime_move_target_clear(&s_move_target_state);
        runtime_input_contract_set_has_move_target(0);
        fprintf(stderr,
                "[engine] banana-raycast failed for normalized=(%.2f, %.2f)\n",
                normalized_x,
                normalized_y);
    }
}

/* ── Init ────────────────────────────────────────────────────────────────── */
int engine_init(int width, int height)
{
    BananaRenderBackend requested_backend = BANANA_RENDER_BACKEND_UNKNOWN;
    BananaRenderBackend active_backend = BANANA_RENDER_BACKEND_UNKNOWN;

    /* Idempotent: only initialize once */
    if (s_engine_initialized)
        return 0;
    s_engine_initialized = 1;

    requested_backend = banana_render_backend_requested();
    active_backend = banana_render_backend_active();

    fprintf(stdout,
            "[engine] render backend requested=%s active=%s status=%s\n",
            banana_render_backend_name(requested_backend),
            banana_render_backend_name(active_backend),
            banana_render_backend_status());
    if (requested_backend != active_backend)
    {
        fprintf(stderr,
                "[engine] render backend fallback engaged for '%s'\n",
                banana_render_backend_name(requested_backend));
    }

    /* Register built-in controller types */
    wildlife_controller_register();

    s_window = window_create(width, height, "Banana Engine");
    if (!s_window)
    {
        fprintf(stderr, "[engine] window_create failed\n");
        return -1;
    }
    s_renderer = renderer_create(width, height);
    if (!s_renderer)
    {
        fprintf(stderr, "[engine] renderer_create failed\n");
        return -1;
    }
    renderer_attach_native_window(s_renderer, window_get_native_handle(s_window));
    s_physics = physics_world_create();
    s_world = world_create();
    {
        s_player_id = world_spawn_entity(s_world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
        s_entity_mesh = mesh_create_banana_vector(1.05f, 1.55f, 1.65f, 0.15f, 3);
        s_pbj_pickup_mesh = mesh_create_peanut_butter_pickup_asset(2);
    }
    if (!s_entity_mesh || !s_pbj_pickup_mesh)
    {
        fprintf(stderr, "[engine] actor mesh creation failed\n");
        return -1;
    }
    s_viewport_width = width;
    s_viewport_height = height;

    if (player_builds_init() != 0)
        return -1;

    s_terrain_initialized = runtime_engine_lifecycle_build_terrain(&s_terrain_height[0][0],
                                                                   BANANA_TERRAIN_SIZE,
                                                                   BANANA_TERRAIN_MAX_LAYERS,
                                                                   BANANA_TERRAIN_CHUNK_SIZE,
                                                                   s_terrain_chunks,
                                                                   BANANA_TERRAIN_CHUNK_COLS,
                                                                   BANANA_TERRAIN_CHUNK_ROWS);
    if (!s_terrain_initialized)
        return -1;

    if (!runtime_engine_lifecycle_bootstrap_primary_player(s_world,
                                                            s_player_id,
                                                            terrain_sample_height))
        return -1;

    runtime_engine_lifecycle_spawn_default_actors(s_world,
                                                  terrain_sample_height,
                                                  engine_controller_attach);
    s_pbj_pickup_collected = 0;

    fprintf(stdout, "[engine] initialized %dx%d\n", width, height);
    engine_serialize_reset();
    return 0;
}

static void tick_phase_gameplay_signals(void)
{
    runtime_wildlife_signal_player_nearby(s_world,
                                          s_controllers,
                                          s_controller_count,
                                          s_player_id,
                                          6.0f);
}

static void tick_phase_pbj_pickup_collection(void)
{
    const float collect_radius = 1.55f;
    Entity *player = NULL;

    if (s_pbj_pickup_collected || !s_world || !s_player_id)
        return;

    player = world_get_entity(s_world, s_player_id);
    if (!player || !player->active)
        return;

    for (int i = 0; i < s_world->entity_count; i++)
    {
        Entity *entity = s_world->entities[i];
        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_STATIC)
            continue;
        if (strcmp(entity->controller_kind, "pbj_pickup") != 0)
            continue;

        {
            float dx = player->position[0] - entity->position[0];
            float dz = player->position[2] - entity->position[2];
            float distance = sqrtf(dx * dx + dz * dz);
            if (distance <= collect_radius)
            {
                entity->active = 0;
                s_pbj_pickup_collected = 1;
                break;
            }
        }
    }
}

static void tick_phase_gameplay(void)
{
    tick_phase_gameplay_signals();
    tick_phase_pbj_pickup_collection();
}

static Mesh *runtime_render_mesh_for_actor(const Entity *entity, Mesh *default_mesh);

static void tick_phase_render_scene(void)
{
    runtime_render_submit_frame(s_renderer,
                                s_world,
                                s_entity_mesh,
                                s_terrain_initialized,
                                terrain_draw_runtime,
                                runtime_render_material_for_actor,
                                runtime_render_mesh_for_actor);
}

static Mesh *runtime_render_mesh_for_actor(const Entity *entity, Mesh *default_mesh)
{
    if (!entity)
        return default_mesh;

    if (entity->type == ENTITY_TYPE_STATIC && strcmp(entity->controller_kind, "pbj_pickup") == 0)
    {
        if (s_pbj_pickup_mesh)
            return s_pbj_pickup_mesh;
    }

    return default_mesh;
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

static int engine_seed_default_merchant_if_needed(int npc_id)
{
    if (npc_id < 0) {
        return -1;
    }

    if (!s_merchants_seeded) {
        npc_merchant_init();
        s_merchants_seeded = 1;
    }

    while (npc_merchant_count() <= npc_id) {
        int merchant_id = npc_merchant_register(npc_merchant_count(), 0.0f, 0.0f, 500);
        if (merchant_id < 0) {
            return -1;
        }
        npc_merchant_add_inventory_item(merchant_id, "wood", 100, 200, 5);
        npc_merchant_add_inventory_item(merchant_id, "ore", 50, 100, 15);
    }

    return 0;
}

/* ── Tick ────────────────────────────────────────────────────────────────── */
int engine_tick(float dt)
{
    return runtime_engine_tick_execute(s_window,
                                       s_renderer,
                                       s_physics,
                                       s_world,
                                       &s_viewport_width,
                                       &s_viewport_height,
                                       terrain_rebuild_dirty_chunks_runtime,
                                       s_controllers,
                                       s_controller_count,
                                       dt,
                                       update_player_motion,
                                       follow_player_camera,
                                       apply_click_move_input,
                                       tick_phase_gameplay,
                                       tick_phase_render_scene);
}
/* ── Shutdown ────────────────────────────────────────────────────────────── */
void engine_shutdown(void)
{
    runtime_engine_lifecycle_destroy_controllers(s_controllers,
                                                 BANANA_MAX_ACTIVE_CONTROLLERS,
                                                 &s_controller_count);

    mesh_destroy(s_entity_mesh);
    mesh_destroy(s_pbj_pickup_mesh);
    runtime_engine_lifecycle_reset_terrain_chunks(s_terrain_chunks, BANANA_TERRAIN_TOTAL_CHUNKS);
    world_destroy(s_world);
    physics_world_destroy(s_physics);
    renderer_destroy(s_renderer);
    window_destroy(s_window);

    s_entity_mesh = NULL;
    s_pbj_pickup_mesh = NULL;
    s_terrain_initialized = 0;
    s_viewport_width = 0;
    s_viewport_height = 0;
    s_window = NULL;
    s_renderer = NULL;
    s_physics = NULL;
    s_world = NULL;
    s_player_id = 0;
    if (s_merchants_seeded) {
        npc_merchant_shutdown();
        s_merchants_seeded = 0;
    }
    player_builds_cleanup();
    runtime_player_registry_reset();
    s_engine_initialized = 0;
    s_camera_valid = 0;
    s_pbj_pickup_collected = 0;
    runtime_move_target_reset(&s_move_target_state);
    runtime_input_contract_reset();
    engine_serialize_reset();

    runtime_engine_host_reset_state(&s_window,
                                    &s_renderer,
                                    &s_physics,
                                    &s_world,
                                    &s_entity_mesh,
                                    &s_player_id,
                                    &s_terrain_initialized,
                                    &s_viewport_width,
                                    &s_viewport_height,
                                    &s_engine_initialized,
                                    &s_camera_valid,
                                    &s_move_input_x,
                                    &s_move_input_z);

    fprintf(stdout, "[engine] shutdown complete\n");
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
    return runtime_terrain_set_height(&s_terrain_height[0][0],
                                      BANANA_TERRAIN_SIZE,
                                      BANANA_TERRAIN_MAX_LAYERS,
                                      BANANA_TERRAIN_CHUNK_SIZE,
                                      x,
                                      z,
                                      elevation,
                                      terrain_mark_chunk_dirty);
}

void engine_terrain_mark_region_dirty(int min_x, int min_z, int max_x, int max_z)
{
    runtime_terrain_mark_region_dirty(BANANA_TERRAIN_SIZE,
                                      BANANA_TERRAIN_CHUNK_SIZE,
                                      min_x,
                                      min_z,
                                      max_x,
                                      max_z,
                                      terrain_mark_chunk_dirty);
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
            apply_click_move_input(normalized_x, normalized_y);
    }
    return accepted;
}

int engine_handle_right_click_normalized(float screen_x, float screen_y)
{
    int accepted = runtime_input_contract_handle_right_click_normalized(screen_x, screen_y);
    if (accepted)
        apply_click_move_input(screen_x, screen_y);
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
    if (!resource_type) return 0;

    const char *player_guid = engine_player_active_guid();

    if (player_guid) {
        return runtime_player_registry_get_resource(player_guid, resource_type);
    }

    return 0;
}

int engine_player_add_resource(const char *resource_type, int amount)
{
    if (!resource_type) return 0;

    const char *player_guid = engine_player_active_guid();

    if (player_guid) {
        runtime_player_registry_add_resource(player_guid, resource_type, amount);
        return runtime_player_registry_get_resource(player_guid, resource_type);
    }

    return 0;
}

int engine_player_build_set_class(const char *player_guid, int class_type)
{
    return player_builds_upsert(player_guid, (BuildClass)class_type);
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
    GearModifier gear;
    gear.tier = tier;
    gear.attack_bonus = attack_bonus;
    gear.defense_bonus = defense_bonus;
    gear.utility_bonus = utility_bonus;
    return player_builds_equip(player_guid, (GearSlot)slot, &gear);
}

int engine_player_build_get_stat(const char *player_guid, const char *stat_name)
{
    BuildStats stats;
    if (!stat_name || player_builds_get_stats(player_guid, &stats) != 0) {
        return -1;
    }

    if (strcmp(stat_name, "health") == 0) return stats.health;
    if (strcmp(stat_name, "attack") == 0) return stats.attack;
    if (strcmp(stat_name, "defense") == 0) return stats.defense;
    if (strcmp(stat_name, "utility") == 0) return stats.utility;
    return -1;
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

static int engine_player_set_resource_total(const char *resource_type, int target_amount)
{
    int current = engine_player_get_resource(resource_type);
    return engine_player_add_resource(resource_type, target_amount - current);
}

/* ─────────────────────────────────────────────────────────────────
   Merchant NPC Queries (UI Integration)
   ───────────────────────────────────────────────────────────────── */

int engine_npc_merchant_get_price(int npc_id, const char *item_type)
{
    /* Query merchant pricing via npc_merchant module */
    if (!item_type || engine_seed_default_merchant_if_needed(npc_id) != 0) return 0;

    int stock = 0;
    int price = 0;
    if (npc_merchant_get_item_price(npc_id, item_type, &stock, &price) == 0) {
        return price;
    }

    return 0;
}

int engine_npc_merchant_trade_buy(int npc_id, const char *item_type, int quantity)
{
    if (!s_world || !item_type || quantity <= 0) return -1;
    if (engine_seed_default_merchant_if_needed(npc_id) != 0) return -1;

    int player_gold = engine_player_get_resource("gold");
    MerchantTradeResult result = npc_merchant_trade_buy(npc_id, item_type, quantity, &player_gold);
    if (result == MERCHANT_TRADE_INSUFFICIENT_GOLD) {
        return -2;
    }
    if (result == MERCHANT_TRADE_INSUFFICIENT_STOCK) {
        return -3;
    }
    if (result != MERCHANT_TRADE_OK) {
        return -1;
    }

    engine_player_set_resource_total("gold", player_gold);
    engine_player_add_resource(item_type, quantity);

    return 0; /* Success */
}

int engine_npc_merchant_trade_sell(int npc_id, const char *item_type, int quantity)
{
    if (!s_world || !item_type || quantity <= 0) return -1;
    if (engine_seed_default_merchant_if_needed(npc_id) != 0) return -1;

    int player_amount = engine_player_get_resource(item_type);
    if (player_amount < quantity) {
        return -2;
    }

    int player_gold = engine_player_get_resource("gold");
    MerchantTradeResult result = npc_merchant_trade_sell(npc_id, item_type, quantity, &player_gold);
    if (result == MERCHANT_TRADE_INSUFFICIENT_STOCK) {
        return -3;
    }
    if (result == MERCHANT_TRADE_INSUFFICIENT_GOLD) {
        return -4;
    }
    if (result != MERCHANT_TRADE_OK) {
        return -1;
    }

    engine_player_add_resource(item_type, -quantity);
    engine_player_set_resource_total("gold", player_gold);

    return 0; /* Success */
}
