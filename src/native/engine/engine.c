#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

/* ── Global singletons ────────────────────────────────────────────────────── */
static Window        *s_window   = NULL;
static Renderer      *s_renderer = NULL;
static PhysicsWorld  *s_physics  = NULL;
static World         *s_world    = NULL;

int engine_init(int width, int height) {
    s_window = window_create(width, height, "Banana Engine");
    if (!s_window) {
        fprintf(stderr, "[engine] window_create failed\n");
        return -1;
    }
    s_renderer = renderer_create(width, height);
    if (!s_renderer) {
        fprintf(stderr, "[engine] renderer_create failed\n");
        return -1;
    }
    s_physics = physics_world_create();
    s_world   = world_create();

    fprintf(stdout, "[engine] initialized %dx%d\n", width, height);
    return 0;
}

int engine_tick(float dt) {
    window_poll_events(s_window);
    physics_world_step(s_physics, dt);
    world_tick(s_world, dt);
    renderer_begin_frame(s_renderer);
    renderer_end_frame(s_renderer);
    return 0;
}

void engine_shutdown(void) {
    world_destroy(s_world);
    physics_world_destroy(s_physics);
    renderer_destroy(s_renderer);
    window_destroy(s_window);
    s_window   = NULL;
    s_renderer = NULL;
    s_physics  = NULL;
    s_world    = NULL;
    fprintf(stdout, "[engine] shutdown complete\n");
}

/* ── WASM ABI — backed by singletons above ────────────────────────────────── */

void engine_render_frame(void) {
    if (!s_renderer) return;
    renderer_begin_frame(s_renderer);
    renderer_end_frame(s_renderer);
}

const unsigned char *engine_get_frame_buffer(void) {
    if (!s_renderer) return NULL;
    return renderer_get_frame_buffer(s_renderer);
}

void physics_step(float dt) {
    if (s_physics) physics_world_step(s_physics, dt);
}

void physics_add_body(uint32_t id, float mass, float x, float y, float z) {
    if (!s_physics) return;
    PhysicsBody *b = physics_world_add_body(s_physics, mass, x, y, z, SHAPE_BOX);
    if (b) b->id = id;
}

void physics_update_body(uint32_t id, const float *pos, const float *vel) {
    if (!s_physics) return;
    for (int i = 0; i < s_physics->body_count; i++) {
        PhysicsBody *b = s_physics->bodies[i];
        if (b->id == id) {
            if (pos) { b->position[0]=pos[0]; b->position[1]=pos[1]; b->position[2]=pos[2]; }
            if (vel) { b->velocity[0]=vel[0]; b->velocity[1]=vel[1]; b->velocity[2]=vel[2]; }
            return;
        }
    }
}

uint32_t engine_world_spawn(int type, float x, float y, float z) {
    if (!s_world) return 0;
    return world_spawn_entity(s_world, (EntityType)type, x, y, z);
}

void engine_world_tick(float dt) {
    if (s_world) world_tick(s_world, dt);
}
