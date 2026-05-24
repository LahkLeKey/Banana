#include "world.h"
#include "collider.h"
#include "dynamics.h"
#include "raycast.h"
#include "world_collision.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

PhysicsWorld *physics_world_create(void)
{
    PhysicsWorld *w = calloc(1, sizeof(PhysicsWorld));
    w->gravity[0] = 0.f;
    w->gravity[1] = -9.81f;
    w->gravity[2] = 0.f;
    w->fixed_dt = 1.f / 60.f;
    w->body_count = 0;
    return w;
}

PhysicsBody *physics_world_add_body(PhysicsWorld *w, float mass, float x, float y, float z,
                                    ShapeType shape)
{
    if (w->body_count >= BANANA_MAX_PHYSICS_BODIES)
        return NULL;
    PhysicsBodyId id = (PhysicsBodyId)(w->body_count + 1);
    PhysicsBody *b = physics_body_create(id, mass, x, y, z, shape);
    w->bodies[w->body_count++] = b;
    return b;
}

void physics_world_step_fixed(PhysicsWorld *w)
{
    float dt = w->fixed_dt;
    /* 1. Integrate forces → velocity → position */
    dynamics_integrate_all(w->bodies, w->body_count, dt);
    /* 2. Collision detection + resolution */
    CollisionList cols = collider_broad_phase(w->bodies, w->body_count);
    physics_world_resolve_collision_pairs(w, &cols);
}

void physics_world_step(PhysicsWorld *w, float dt)
{
    w->accumulator += dt;
    while (w->accumulator >= w->fixed_dt)
    {
        physics_world_step_fixed(w);
        w->accumulator -= w->fixed_dt;
    }
}

int physics_world_raycast(const PhysicsWorld *w, const float *origin, const float *direction,
                          PhysicsBodyId *hit_body_out, float *t_out)
{
    if (!w)
        return 0;

    return physics_raycast_bodies((PhysicsBody **)w->bodies,
                                  w->body_count,
                                  origin,
                                  direction,
                                  hit_body_out,
                                  t_out);
}

void physics_world_destroy(PhysicsWorld *w)
{
    if (!w)
        return;
    for (int i = 0; i < w->body_count; i++)
        physics_body_destroy(w->bodies[i]);
    free(w);
}

/* physics_add_body / physics_update_body / physics_step are WASM ABI entry points
 * that live in engine.c (they need the global singleton s_physics). */
