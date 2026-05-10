#include "engine.h"
#include "ai/wildlife_controller.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Global singletons ────────────────────────────────────────────────────── */
static Window *s_window = NULL;
static Renderer *s_renderer = NULL;
static PhysicsWorld *s_physics = NULL;
static World *s_world = NULL;
static Mesh *s_entity_mesh = NULL;  /* banana mesh for actors */
static EntityId s_player_id = 0;
/* Initialization guard */
static int s_engine_initialized = 0;
/* Terrain is chunked so WFC/CA updates can rebuild only dirty regions. */

#define BANANA_TERRAIN_SIZE 18
#define BANANA_TERRAIN_MAX_LAYERS 4
#define BANANA_TERRAIN_CHUNK_SIZE 6
#define BANANA_TERRAIN_CHUNK_STRIDE (BANANA_TERRAIN_CHUNK_SIZE + 1)
#define BANANA_TERRAIN_CHUNK_COLS (BANANA_TERRAIN_SIZE / BANANA_TERRAIN_CHUNK_SIZE)
#define BANANA_TERRAIN_CHUNK_ROWS (BANANA_TERRAIN_SIZE / BANANA_TERRAIN_CHUNK_SIZE)
#define BANANA_TERRAIN_TOTAL_CHUNKS (BANANA_TERRAIN_CHUNK_COLS * BANANA_TERRAIN_CHUNK_ROWS)

typedef struct TerrainChunk
{
    Mesh *mesh;
    unsigned char dirty;
} TerrainChunk;

static unsigned char s_terrain_height[BANANA_TERRAIN_SIZE][BANANA_TERRAIN_SIZE];
static TerrainChunk s_terrain_chunks[BANANA_TERRAIN_TOTAL_CHUNKS];
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

/* ── Simple 4×4 column-major mat multiply (for MVP construction) ────────── */
static void mat4_mul(const float *A, const float *B, float *C)
{
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
        {
            float sum = 0.f;
            for (int k = 0; k < 4; k++)
                sum += A[k * 4 + row] * B[col * 4 + k];
            C[col * 4 + row] = sum;
        }
}

/* ── Entity colors by type ───────────────────────────────────────────────── */
static Material mat_for_entity(EntityType t)
{
    switch (t)
    {
    case ENTITY_TYPE_NPC:
        return material_solid(0.20f, 0.80f, 0.30f, 1.f); /* green  */
    case ENTITY_TYPE_DYNAMIC:
        return material_solid(0.30f, 0.55f, 0.90f, 1.f); /* blue   */
    case ENTITY_TYPE_PLAYER:
        return material_solid(0.95f, 0.75f, 0.10f, 1.f); /* yellow */
    default:
        return material_solid(0.55f, 0.55f, 0.55f, 1.f); /* gray   */
    }
}

/* ── Deterministic terrain generation/rendering (C-owned) ───────────────── */

static unsigned int terrain_hash(unsigned int x, unsigned int z)
{
    unsigned int n = x * 374761393u + z * 668265263u + 20260509u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

static int terrain_chunk_index(int chunk_x, int chunk_z)
{
    return chunk_z * BANANA_TERRAIN_CHUNK_COLS + chunk_x;
}

static void terrain_mark_chunk_dirty(int chunk_x, int chunk_z)
{
    if (chunk_x < 0 || chunk_z < 0 || chunk_x >= BANANA_TERRAIN_CHUNK_COLS ||
        chunk_z >= BANANA_TERRAIN_CHUNK_ROWS)
        return;
    s_terrain_chunks[terrain_chunk_index(chunk_x, chunk_z)].dirty = 1u;
}

static void terrain_mark_all_chunks_dirty(void)
{
    for (int i = 0; i < BANANA_TERRAIN_TOTAL_CHUNKS; i++)
        s_terrain_chunks[i].dirty = 1u;
}

static int terrain_rebuild_chunk(int chunk_x, int chunk_z)
{
    unsigned char local_heights[BANANA_TERRAIN_CHUNK_STRIDE * BANANA_TERRAIN_CHUNK_STRIDE];
    const int stride = BANANA_TERRAIN_CHUNK_STRIDE;

    int base_x = chunk_x * BANANA_TERRAIN_CHUNK_SIZE;
    int base_z = chunk_z * BANANA_TERRAIN_CHUNK_SIZE;

    for (int z = 0; z <= BANANA_TERRAIN_CHUNK_SIZE; z++)
    {
        for (int x = 0; x <= BANANA_TERRAIN_CHUNK_SIZE; x++)
        {
            int sample_x = base_x + x;
            int sample_z = base_z + z;
            if (sample_x >= BANANA_TERRAIN_SIZE)
                sample_x = BANANA_TERRAIN_SIZE - 1;
            if (sample_z >= BANANA_TERRAIN_SIZE)
                sample_z = BANANA_TERRAIN_SIZE - 1;
            local_heights[z * stride + x] = s_terrain_height[sample_z][sample_x];
        }
    }

    int idx = terrain_chunk_index(chunk_x, chunk_z);
    mesh_destroy(s_terrain_chunks[idx].mesh);
    s_terrain_chunks[idx].mesh =
        mesh_create_terrain_heightfield(local_heights, stride, stride, 1.0f, 0.30f);
    s_terrain_chunks[idx].dirty = 0u;
    return s_terrain_chunks[idx].mesh ? 1 : 0;
}

static int terrain_rebuild_dirty_chunks(int max_chunks)
{
    int rebuilt = 0;
    for (int cz = 0; cz < BANANA_TERRAIN_CHUNK_ROWS; cz++)
    {
        for (int cx = 0; cx < BANANA_TERRAIN_CHUNK_COLS; cx++)
        {
            int idx = terrain_chunk_index(cx, cz);
            if (!s_terrain_chunks[idx].dirty)
                continue;
            if (!terrain_rebuild_chunk(cx, cz))
                return 0;
            rebuilt++;
            if (max_chunks > 0 && rebuilt >= max_chunks)
                return 1;
        }
    }
    return 1;
}

static int clampi(int v, int lo, int hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static float clampf_local(float v, float lo, float hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static void vec3_set(float *out, float x, float y, float z)
{
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

static float vec3_len(const float *v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static void vec3_normalize(float *v)
{
    float len = vec3_len(v);
    if (len <= 1e-6f)
        return;
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

static void vec3_cross(const float *a, const float *b, float *out)
{
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

static float terrain_sample_height(float x, float z);

static int compute_camera_ground_basis(float *forward, float *right)
{
    if (!s_camera_valid)
        return 0;

    vec3_set(forward, s_camera_target[0] - s_camera_eye[0], 0.f,
             s_camera_target[2] - s_camera_eye[2]);
    if (vec3_len(forward) <= 1e-6f)
        vec3_set(forward, 0.f, 0.f, -1.f);
    vec3_normalize(forward);

    {
        float world_up[3] = {0.f, 1.f, 0.f};
        vec3_cross(forward, world_up, right);
    }
    if (vec3_len(right) <= 1e-6f)
        vec3_set(right, 1.f, 0.f, 0.f);
    vec3_normalize(right);

    return 1;
}

static void terrain_build(void)
{
    const float center = (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f;
    const float max_dist = center * 0.92f;

    for (int z = 0; z < BANANA_TERRAIN_SIZE; z++)
    {
        for (int x = 0; x < BANANA_TERRAIN_SIZE; x++)
        {
            unsigned int detail = terrain_hash((unsigned int)x, (unsigned int)z);
            float dx = ((float)x - center) / max_dist;
            float dz = ((float)z - center) / max_dist;
            float dist = sqrtf(dx * dx + dz * dz);
            float island = 1.0f - clampf_local(dist, 0.0f, 1.0f);
            unsigned int blend = (unsigned int)(island * (float)(BANANA_TERRAIN_MAX_LAYERS - 1));
            if (((detail >> 8) & 0x03u) == 0u && blend < BANANA_TERRAIN_MAX_LAYERS - 1)
                blend += 1u;
            if (dist > 1.0f)
                blend = 0u;
            if (blend >= BANANA_TERRAIN_MAX_LAYERS)
                blend = BANANA_TERRAIN_MAX_LAYERS - 1;
            s_terrain_height[z][x] = (unsigned char)blend;
        }
    }

    terrain_mark_all_chunks_dirty();
    if (!terrain_rebuild_dirty_chunks(0))
        return;

    s_terrain_initialized = 1;
}

static void terrain_draw(void)
{
    if (!s_terrain_initialized)
        return;

    float identity[4] = {0.f, 0.f, 0.f, 1.f};
    float unit[3] = {1.f, 0.48f, 1.f};
    Material terrain_mat = material_solid(0.35f, 0.66f, 0.30f, 1.f);
    float world_origin = (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f;

    for (int cz = 0; cz < BANANA_TERRAIN_CHUNK_ROWS; cz++)
    {
        for (int cx = 0; cx < BANANA_TERRAIN_CHUNK_COLS; cx++)
        {
            int idx = terrain_chunk_index(cx, cz);
            Mesh *mesh = s_terrain_chunks[idx].mesh;
            if (!mesh)
                continue;

            float pos[3] = {
                (float)(cx * BANANA_TERRAIN_CHUNK_SIZE) - world_origin,
                -1.45f,
                (float)(cz * BANANA_TERRAIN_CHUNK_SIZE) - world_origin,
            };
            renderer_draw_mesh(s_renderer, mesh, pos, identity, unit, &terrain_mat);
        }
    }
}

static float terrain_sample_height(float x, float z)
{
    float world_origin = (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f;
    int grid_x = clampi((int)roundf(x + world_origin), 0, BANANA_TERRAIN_SIZE - 1);
    int grid_z = clampi((int)roundf(z + world_origin), 0, BANANA_TERRAIN_SIZE - 1);
    return -1.45f + ((float)s_terrain_height[grid_z][grid_x] * 0.48f);
}

static void update_player_motion(float dt)
{
    if (!s_world || !s_player_id)
        return;

    Entity *player = world_get_entity(s_world, s_player_id);
    if (!player || !player->active)
        return;

    {
        float island_span = (float)(BANANA_TERRAIN_SIZE - 1) * 0.5f;
        float speed = 5.5f;
        float move_x = s_move_input_x;
        float move_z = s_move_input_z;

        if (fabsf(move_x) <= 1e-4f && fabsf(move_z) <= 1e-4f)
            return;

        {
            float forward[3] = {0.f, 0.f, 0.f};
            float right[3] = {0.f, 0.f, 0.f};
            float move_dir[3] = {0.f, 0.f, 0.f};

            if (!compute_camera_ground_basis(forward, right))
            {
                vec3_set(forward, 0.f, 0.f, -1.f);
                vec3_set(right, 1.f, 0.f, 0.f);
            }

            move_dir[0] = right[0] * move_x + forward[0] * move_z;
            move_dir[2] = right[2] * move_x + forward[2] * move_z;
            if (vec3_len(move_dir) > 1e-5f)
                vec3_normalize(move_dir);

            player->position[0] += move_dir[0] * speed * dt;
            player->position[2] += move_dir[2] * speed * dt;
            player->position[0] = clampf_local(player->position[0], -island_span, island_span);
            player->position[2] = clampf_local(player->position[2], -island_span, island_span);
            player->position[1] = terrain_sample_height(player->position[0], player->position[2]) +
                                  0.55f;
        }
    }
}

static void follow_player_camera(void)
{
    if (!s_renderer || !s_world || !s_player_id || s_viewport_width <= 0 || s_viewport_height <= 0)
        return;

    Entity *player = world_get_entity(s_world, s_player_id);
    if (!player || !player->active)
        return;

    Camera camera = camera_create(55.f, (float)s_viewport_width / (float)s_viewport_height,
                                  0.1f, 1000.f);
    camera_look_at(&camera, player->position[0] + 10.f, player->position[1] + 15.f,
                   player->position[2] + 10.f, player->position[0], player->position[1] - 0.5f,
                   player->position[2]);
    s_camera_eye[0] = player->position[0] + 10.f;
    s_camera_eye[1] = player->position[1] + 15.f;
    s_camera_eye[2] = player->position[2] + 10.f;
    s_camera_target[0] = player->position[0];
    s_camera_target[1] = player->position[1] - 0.5f;
    s_camera_target[2] = player->position[2];
    s_camera_valid = 1;
    renderer_set_camera(s_renderer, &camera);
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

    terrain_build();
    if (!s_terrain_initialized)
        return -1;

    if (s_player_id)
    {
        Entity *player = world_get_entity(s_world, s_player_id);
        if (player)
        {
            player->position[1] = terrain_sample_height(0.f, 0.f) + 0.55f;
            player->scale[0] = 1.25f;
            player->scale[1] = 0.95f;
            player->scale[2] = 1.10f;
        }
    }

    fprintf(stdout, "[engine] initialized %dx%d\n", width, height);
    return 0;
}

/* ── Tick ────────────────────────────────────────────────────────────────── */
int engine_tick(float dt)
{
    if (!s_window)
        return -1;

    window_poll_events(s_window);
    physics_world_step(s_physics, dt);
    world_tick(s_world, dt);
    update_player_motion(dt);
    follow_player_camera();

    /* C-owned viewport sizing: detect canvas/framebuffer changes and resize renderer. */
    int width = 0;
    int height = 0;
    window_get_size(s_window, &width, &height);
    if (width > 0 && height > 0 && (width != s_viewport_width || height != s_viewport_height))
    {
        s_viewport_width = width;
        s_viewport_height = height;
        renderer_resize(s_renderer, width, height);
    }

    /* Keep chunk rebuilds within a per-frame budget for mobile smoothness. */
    if (!terrain_rebuild_dirty_chunks(2))
        return -1;

    /* Update all AI controllers; sync positions back to entities */
    for (int i = 0; i < s_controller_count; i++)
    {
        ControllerInstance *c = s_controllers[i];
        controller_update(c, dt);
        /* Find bound entity and mirror position */
        for (int j = 0; j < s_world->entity_count; j++)
        {
            Entity *e = s_world->entities[j];
            if (e && e->active && e->controller_id == c->id)
            {
                e->position[0] = c->position[0];
                e->position[1] = c->position[1];
                e->position[2] = c->position[2];
            }
        }
    }

    /* ── Render ─────────────────────────────────────────────────────────── */
    renderer_begin_frame(s_renderer);

    if (s_terrain_initialized)
        terrain_draw();

    if (s_entity_mesh && s_world)
    {
        float identity[4] = {0.f, 0.f, 0.f, 1.f}; /* quaternion identity */
        for (int j = 0; j < s_world->entity_count; j++)
        {
            Entity *e = s_world->entities[j];
            if (!e || !e->active)
                continue;
            Material mat = mat_for_entity(e->type);
            renderer_draw_mesh(s_renderer, s_entity_mesh, e->position, identity, e->scale, &mat);
        }
    }

    renderer_end_frame(s_renderer);
    return 0;
}

/* ── Shutdown ────────────────────────────────────────────────────────────── */
void engine_shutdown(void)
{
    for (int i = 0; i < s_controller_count; i++)
    {
        controller_destroy(s_controllers[i]);
        s_controllers[i] = NULL;
    }
    s_controller_count = 0;

    mesh_destroy(s_entity_mesh);
    for (int i = 0; i < BANANA_TERRAIN_TOTAL_CHUNKS; i++)
    {
        mesh_destroy(s_terrain_chunks[i].mesh);
        s_terrain_chunks[i].mesh = NULL;
        s_terrain_chunks[i].dirty = 0u;
    }
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
    s_engine_initialized = 0;
    s_camera_valid = 0;

    fprintf(stdout, "[engine] shutdown complete\n");
}

/* ── WASM ABI — backed by singletons above ──────────────────────────────── */

void engine_render_frame(void)
{
    if (!s_renderer)
        return;
    renderer_begin_frame(s_renderer);
    renderer_end_frame(s_renderer);
}

const unsigned char *engine_get_frame_buffer(void)
{
    if (!s_renderer)
        return NULL;
    return renderer_get_frame_buffer(s_renderer);
}

/* Physics ABI */
void physics_step(float dt)
{
    if (s_physics)
        physics_world_step(s_physics, dt);
}

void physics_add_body(uint32_t id, float mass, float x, float y, float z)
{
    if (!s_physics)
        return;
    PhysicsBody *b = physics_world_add_body(s_physics, mass, x, y, z, SHAPE_BOX);
    if (b)
        b->id = id;
}

void physics_update_body(uint32_t id, const float *pos, const float *vel)
{
    if (!s_physics)
        return;
    for (int i = 0; i < s_physics->body_count; i++)
    {
        PhysicsBody *b = s_physics->bodies[i];
        if (b->id == id)
        {
            if (pos)
            {
                b->position[0] = pos[0];
                b->position[1] = pos[1];
                b->position[2] = pos[2];
            }
            if (vel)
            {
                b->velocity[0] = vel[0];
                b->velocity[1] = vel[1];
                b->velocity[2] = vel[2];
            }
            return;
        }
    }
}

/* World ABI */
uint32_t engine_world_spawn(int type, float x, float y, float z)
{
    if (!s_world)
        return 0;
    return world_spawn_entity(s_world, (EntityType)type, x, y, z);
}

void engine_world_tick(float dt)
{
    if (s_world)
        world_tick(s_world, dt);
}

/* Terrain ABI */
int engine_terrain_set_height(int x, int z, int elevation)
{
    if (x < 0 || z < 0 || x >= BANANA_TERRAIN_SIZE || z >= BANANA_TERRAIN_SIZE)
        return 0;

    elevation = clampi(elevation, 0, BANANA_TERRAIN_MAX_LAYERS - 1);
    if ((int)s_terrain_height[z][x] == elevation)
        return 1;

    s_terrain_height[z][x] = (unsigned char)elevation;

    int cx = x / BANANA_TERRAIN_CHUNK_SIZE;
    int cz = z / BANANA_TERRAIN_CHUNK_SIZE;
    terrain_mark_chunk_dirty(cx, cz);

    /* Heightfield normals depend on neighboring samples, so update adjacent chunks too. */
    if (x % BANANA_TERRAIN_CHUNK_SIZE == 0)
        terrain_mark_chunk_dirty(cx - 1, cz);
    if (x % BANANA_TERRAIN_CHUNK_SIZE == BANANA_TERRAIN_CHUNK_SIZE - 1)
        terrain_mark_chunk_dirty(cx + 1, cz);
    if (z % BANANA_TERRAIN_CHUNK_SIZE == 0)
        terrain_mark_chunk_dirty(cx, cz - 1);
    if (z % BANANA_TERRAIN_CHUNK_SIZE == BANANA_TERRAIN_CHUNK_SIZE - 1)
        terrain_mark_chunk_dirty(cx, cz + 1);

    return 1;
}

void engine_terrain_mark_region_dirty(int min_x, int min_z, int max_x, int max_z)
{
    if (min_x > max_x)
    {
        int t = min_x;
        min_x = max_x;
        max_x = t;
    }
    if (min_z > max_z)
    {
        int t = min_z;
        min_z = max_z;
        max_z = t;
    }

    min_x = clampi(min_x, 0, BANANA_TERRAIN_SIZE - 1);
    max_x = clampi(max_x, 0, BANANA_TERRAIN_SIZE - 1);
    min_z = clampi(min_z, 0, BANANA_TERRAIN_SIZE - 1);
    max_z = clampi(max_z, 0, BANANA_TERRAIN_SIZE - 1);

    int min_cx = min_x / BANANA_TERRAIN_CHUNK_SIZE;
    int max_cx = max_x / BANANA_TERRAIN_CHUNK_SIZE;
    int min_cz = min_z / BANANA_TERRAIN_CHUNK_SIZE;
    int max_cz = max_z / BANANA_TERRAIN_CHUNK_SIZE;

    for (int cz = min_cz; cz <= max_cz; cz++)
        for (int cx = min_cx; cx <= max_cx; cx++)
            terrain_mark_chunk_dirty(cx, cz);
}

/* Controller ABI */
uint32_t engine_controller_create(const char *type, float x, float y, float z)
{
    if (s_controller_count >= BANANA_MAX_ACTIVE_CONTROLLERS)
        return 0;
    ControllerInstance *c = controller_create(type, x, y, z);
    if (!c)
        return 0;
    c->id = (uint32_t)(s_controller_count + 1);
    s_controllers[s_controller_count++] = c;
    return c->id;
}

uint32_t engine_controller_attach(uint32_t entity_id, const char *type)
{
    if (!s_world)
        return 0;
    Entity *e = world_get_entity(s_world, entity_id);
    if (!e)
        return 0;
    uint32_t cid = engine_controller_create(type, e->position[0], e->position[1], e->position[2]);
    e->controller_id = cid;
    return cid;
}

void engine_controller_update(uint32_t id, float dt)
{
    for (int i = 0; i < s_controller_count; i++)
        if (s_controllers[i]->id == id)
        {
            controller_update(s_controllers[i], dt);
            return;
        }
}

void engine_controller_signal(uint32_t id, const char *sig)
{
    for (int i = 0; i < s_controller_count; i++)
        if (s_controllers[i]->id == id)
        {
            controller_signal(s_controllers[i], sig, NULL);
            return;
        }
}

/* Entity query helpers for React telemetry overlay */
int engine_get_entity_count(void)
{
    return s_world ? s_world->entity_count : 0;
}

float engine_get_entity_x(int idx)
{
    if (!s_world || idx < 0 || idx >= s_world->entity_count)
        return 0.f;
    return s_world->entities[idx] ? s_world->entities[idx]->position[0] : 0.f;
}

float engine_get_entity_z(int idx)
{
    if (!s_world || idx < 0 || idx >= s_world->entity_count)
        return 0.f;
    return s_world->entities[idx] ? s_world->entities[idx]->position[2] : 0.f;
}

/* Returns current FSM state index via the bound controller (0 = no controller) */
int engine_get_entity_state(int idx)
{
    if (!s_world || idx < 0 || idx >= s_world->entity_count)
        return 0;
    Entity *e = s_world->entities[idx];
    if (!e || e->controller_id == 0)
        return 0;
    for (int i = 0; i < s_controller_count; i++)
    {
        ControllerInstance *c = s_controllers[i];
        if (c->id == e->controller_id)
        {
            /* WildlifeState embeds FSM at the start of its state struct */
            /* We can't safely cast here without the full type, so return 1 = active */
            return 1;
        }
    }
    return 0;
}

/* Keep the unused mat4_mul visible to avoid LTO drop */
static void (*_mat4_mul_ref)(const float *, const float *, float *) = mat4_mul;
static void _mat4_force_link(void)
{
    (void)_mat4_mul_ref;
}

/* ── Debug diagnostics (exported for JavaScript) ───────────────────────── */
int engine_get_click_count(void)
{
    return 0;
}

int engine_get_target_reached_count(void)
{
    return 0;
}

int engine_get_has_move_target(void)
{
    return 0;
}

int engine_handle_right_click(float canvas_x, float canvas_y)
{
    /* US3 arbitration invariant: right-click bridge is UI-only and never mutates movement state. */
    (void)canvas_x;
    (void)canvas_y;
    return 0;
}

int engine_handle_right_click_normalized(float screen_x, float screen_y)
{
    /* US3 arbitration invariant: normalized mouse bridge remains intentionally inert. */
    (void)screen_x;
    (void)screen_y;
    return 0;
}

void engine_set_move_input(float input_x, float input_z)
{
    if (!isfinite(input_x))
        input_x = 0.f;
    if (!isfinite(input_z))
        input_z = 0.f;

    s_move_input_x = clampf_local(input_x, -1.f, 1.f);
    s_move_input_z = clampf_local(input_z, -1.f, 1.f);
}
