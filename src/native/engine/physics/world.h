#ifndef BANANA_ENGINE_PHYSICS_WORLD_H
#define BANANA_ENGINE_PHYSICS_WORLD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "body.h"
#include "collider.h"

#define BANANA_MAX_PHYSICS_BODIES 1024

    typedef struct PhysicsWorld
    {
        PhysicsBody *bodies[BANANA_MAX_PHYSICS_BODIES];
        int body_count;
        float gravity[3];
        float fixed_dt;    /* fixed timestep (default 1/60 s) */
        float accumulator; /* leftover time from last step call */
    } PhysicsWorld;

    PhysicsWorld *physics_world_create(void);

    PhysicsBody *physics_world_add_body(PhysicsWorld *w, float mass, float x, float y, float z,
                                        ShapeType shape);

    /* Advance simulation.  May run 0 or more fixed sub-steps internally. */
    void physics_world_step(PhysicsWorld *w, float dt);

    /* Synchronous fixed-step advance (skips accumulation — useful for tests). */
    void physics_world_step_fixed(PhysicsWorld *w);

    /* Raycast against all bodies.  Returns 1 on hit; fills hit_body and t. */
    int physics_world_raycast(const PhysicsWorld *w, const float *origin, const float *direction,
                              PhysicsBodyId *hit_body_out, float *t_out);

    void physics_world_destroy(PhysicsWorld *w);

    /* ── WASM ABI exports ─────────────────────────────────────────────────────── */
    void physics_add_body(uint32_t id, float mass, float x, float y, float z);
    void physics_update_body(uint32_t id, const float *position, const float *velocity);
    void physics_step(float dt);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_PHYSICS_WORLD_H */
