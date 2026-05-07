#include "world.h"
#include "collider.h"
#include "dynamics.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

PhysicsWorld *physics_world_create(void) {
    PhysicsWorld *w = calloc(1, sizeof(PhysicsWorld));
    w->gravity[0] = 0.f;
    w->gravity[1] = -9.81f;
    w->gravity[2] = 0.f;
    w->fixed_dt   = 1.f / 60.f;
    w->body_count = 0;
    return w;
}

PhysicsBody *physics_world_add_body(PhysicsWorld *w, float mass,
                                     float x, float y, float z,
                                     ShapeType shape) {
    if (w->body_count >= BANANA_MAX_PHYSICS_BODIES) return NULL;
    PhysicsBodyId id = (PhysicsBodyId)(w->body_count + 1);
    PhysicsBody *b = physics_body_create(id, mass, x, y, z, shape);
    w->bodies[w->body_count++] = b;
    return b;
}

void physics_world_step_fixed(PhysicsWorld *w) {
    float dt = w->fixed_dt;
    /* 1. Integrate forces → velocity → position */
    dynamics_integrate_all(w->bodies, w->body_count, dt);
    /* 2. Collision detection + resolution */
    CollisionList cols = collider_broad_phase(w->bodies, w->body_count);
    for (int i = 0; i < cols.count; i++) {
        PhysicsBody *a = NULL, *b = NULL;
        for (int k = 0; k < w->body_count; k++) {
            if (w->bodies[k]->id == cols.items[i].body_a) a = w->bodies[k];
            if (w->bodies[k]->id == cols.items[i].body_b) b = w->bodies[k];
        }
        if (a && b) collider_resolve(&cols.items[i], a, b);
    }
}

void physics_world_step(PhysicsWorld *w, float dt) {
    w->accumulator += dt;
    while (w->accumulator >= w->fixed_dt) {
        physics_world_step_fixed(w);
        w->accumulator -= w->fixed_dt;
    }
}

int physics_world_raycast(const PhysicsWorld *w,
                           const float *origin, const float *direction,
                           PhysicsBodyId *hit_body_out, float *t_out) {
    /* Simple AABB slab method for box bodies only (T1b.3 will expand this) */
    float best_t = 1e9f;
    int   hit    = 0;
    for (int i = 0; i < w->body_count; i++) {
        PhysicsBody *b = w->bodies[i];
        if (b->shape_type != SHAPE_BOX) continue;
        float *p = b->position;
        float *he = b->shape.box.half_extents;
        float tmin = -1e9f, tmax = 1e9f;
        for (int axis = 0; axis < 3; axis++) {
            float d = direction[axis];
            float mn = p[axis] - he[axis];
            float mx = p[axis] + he[axis];
            if (d != 0.f) {
                float t1 = (mn - origin[axis]) / d;
                float t2 = (mx - origin[axis]) / d;
                if (t1 > t2) { float tmp=t1; t1=t2; t2=tmp; }
                if (t1 > tmin) tmin = t1;
                if (t2 < tmax) tmax = t2;
            } else if (origin[axis] < mn || origin[axis] > mx) {
                tmin = 1e9f; break;
            }
        }
        if (tmin < tmax && tmin > 0.f && tmin < best_t) {
            best_t = tmin;
            if (hit_body_out) *hit_body_out = b->id;
            if (t_out) *t_out = tmin;
            hit = 1;
        }
    }
    return hit;
}

void physics_world_destroy(PhysicsWorld *w) {
    if (!w) return;
    for (int i = 0; i < w->body_count; i++)
        physics_body_destroy(w->bodies[i]);
    free(w);
}

/* physics_add_body / physics_update_body / physics_step are WASM ABI entry points
 * that live in engine.c (they need the global singleton s_physics). */

