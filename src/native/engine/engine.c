#include "engine.h"
#include "ai/wildlife_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Global singletons ────────────────────────────────────────────────────── */
static Window *s_window = NULL;
static Renderer *s_renderer = NULL;
static PhysicsWorld *s_physics = NULL;
static World *s_world = NULL;
static Mesh *s_cube = NULL; /* shared default mesh for entities */

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

/* ── Init ────────────────────────────────────────────────────────────────── */
int engine_init(int width, int height)
{
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
    s_cube = mesh_create_cube();

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

    if (s_cube && s_world)
    {
        float scale[3] = {1.f, 1.f, 1.f};
        float identity[4] = {0.f, 0.f, 0.f, 1.f}; /* quaternion identity */
        for (int j = 0; j < s_world->entity_count; j++)
        {
            Entity *e = s_world->entities[j];
            if (!e || !e->active)
                continue;
            Material mat = mat_for_entity(e->type);
            renderer_draw_mesh(s_renderer, s_cube, e->position, identity, scale, &mat);
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

    mesh_destroy(s_cube);
    world_destroy(s_world);
    physics_world_destroy(s_physics);
    renderer_destroy(s_renderer);
    window_destroy(s_window);

    s_cube = NULL;
    s_window = NULL;
    s_renderer = NULL;
    s_physics = NULL;
    s_world = NULL;

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
