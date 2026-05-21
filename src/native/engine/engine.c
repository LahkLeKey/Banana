#include "engine.h"
#include "engine_serialize.h"
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
#include "runtime/player_motion.h"
#include "runtime/player_motion_host.h"
#include "runtime/player_api.h"
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
#include <stdio.h>
#include <string.h>

/* ── Global singletons ────────────────────────────────────────────────────── */
static Window *s_window = NULL;
static Renderer *s_renderer = NULL;
static PhysicsWorld *s_physics = NULL;
static World *s_world = NULL;
static Mesh *s_entity_mesh = NULL; /* banana mesh for actors */
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

static void update_player_motion(float dt)
{
    runtime_player_motion_tick(s_world,
                               s_player_id,
                               s_move_input_x,
                               s_move_input_z,
                               s_camera_eye,
                               s_camera_target,
                               s_camera_valid,
                               5.5f,
                               dt,
                               (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f,
                               terrain_sample_height);
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

/* ── Init ────────────────────────────────────────────────────────────────── */
int engine_init(int width, int height)
{
    /* Idempotent: only initialize once */
    if (s_engine_initialized)
        return 0;
    s_engine_initialized = 1;

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
    s_physics = physics_world_create();
    s_world = world_create();
    {
        s_player_id = world_spawn_entity(s_world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
        s_entity_mesh = mesh_create_banana_vector(1.05f, 1.55f, 1.65f, 0.15f, 3);
    }
    if (!s_entity_mesh)
    {
        fprintf(stderr, "[engine] entity mesh creation failed\n");
        return -1;
    }
    s_viewport_width = width;
    s_viewport_height = height;

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

static void tick_phase_render_scene(void)
{
    runtime_render_submit_frame(s_renderer,
                                s_world,
                                s_entity_mesh,
                                s_terrain_initialized,
                                terrain_draw_runtime,
                                runtime_render_material_for_entity);
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
                                       NULL,  /* run_gameplay */
                                       tick_phase_render_scene);
}

/* ── Shutdown ────────────────────────────────────────────────────────────── */
void engine_shutdown(void)
{
    runtime_engine_lifecycle_destroy_controllers(s_controllers,
                                                 BANANA_MAX_ACTIVE_CONTROLLERS,
                                                 &s_controller_count);

    mesh_destroy(s_entity_mesh);
    runtime_engine_lifecycle_reset_terrain_chunks(s_terrain_chunks, BANANA_TERRAIN_TOTAL_CHUNKS);
    world_destroy(s_world);
    physics_world_destroy(s_physics);
    renderer_destroy(s_renderer);
    window_destroy(s_window);

    s_entity_mesh = NULL;
    s_terrain_initialized = 0;
    s_viewport_width = 0;
    s_viewport_height = 0;
    s_window = NULL;
    s_renderer = NULL;
    s_physics = NULL;
    s_world = NULL;
    s_player_id = 0;
    runtime_player_registry_reset();
    s_engine_initialized = 0;
    s_camera_valid = 0;
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
    return runtime_input_contract_handle_right_click(canvas_x, canvas_y);
}

int engine_handle_right_click_normalized(float screen_x, float screen_y)
{
    return runtime_input_contract_handle_right_click_normalized(screen_x, screen_y);
}

void engine_set_move_input(float input_x, float input_z)
{
    runtime_input_contract_sanitize_move_input(input_x,
                                               input_z,
                                               &s_move_input_x,
                                               &s_move_input_z);
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
    
    /* TODO: Query player inventory via player_registry
       For now: return stub values */
    if (strcmp(resource_type, "wood") == 0) return 0;
    if (strcmp(resource_type, "ore") == 0) return 0;
    if (strcmp(resource_type, "gold") == 0) return 0;
    return 0;
}

int engine_player_add_resource(const char *resource_type, int amount)
{
    if (!resource_type) return 0;
    
    /* TODO: Add resource to player inventory via player_registry
       For now: return stub values */
    if (strcmp(resource_type, "wood") == 0) return amount;
    if (strcmp(resource_type, "ore") == 0) return amount;
    if (strcmp(resource_type, "gold") == 0) return amount;
    return 0;
}

/* ─────────────────────────────────────────────────────────────────
   Merchant NPC Queries (UI Integration)
   ───────────────────────────────────────────────────────────────── */

int engine_npc_merchant_get_price(int npc_id, const char *item_type)
{
    /* Query merchant pricing via npc_merchant module */
    if (!item_type) return 0;
    
    /* TODO: Implement price lookup from merchant registry
       For now: Return hardcoded prices
       wood: 5 gold, ore: 15 gold */
    
    if (strcmp(item_type, "wood") == 0) return 5;
    if (strcmp(item_type, "ore") == 0) return 15;
    return 0;
}

int engine_npc_merchant_trade_buy(int npc_id, const char *item_type, int quantity)
{
    if (!s_world || !item_type) return -1;
    
    /* Execute trade: player buys from merchant */
    int price = engine_npc_merchant_get_price(npc_id, item_type);
    int total_cost = price * quantity;
    
    int player_gold = engine_player_get_resource("gold");
    if (player_gold < total_cost) {
        return -2; /* Insufficient gold */
    }
    
    /* Deduct gold, add item */
    engine_player_add_resource("gold", -total_cost);
    engine_player_add_resource(item_type, quantity);
    
    return 0; /* Success */
}

int engine_npc_merchant_trade_sell(int npc_id, const char *item_type, int quantity)
{
    if (!s_world || !item_type) return -1;
    
    /* Execute trade: player sells to merchant */
    int price = engine_npc_merchant_get_price(npc_id, item_type) / 2; /* Half price for selling */
    int total_revenue = price * quantity;
    
    int player_amount = engine_player_get_resource(item_type);
    if (player_amount < quantity) {
        return -2; /* Insufficient items */
    }
    
    /* Deduct item, add gold */
    engine_player_add_resource(item_type, -quantity);
    engine_player_add_resource("gold", total_revenue);
    
    return 0; /* Success */
}
