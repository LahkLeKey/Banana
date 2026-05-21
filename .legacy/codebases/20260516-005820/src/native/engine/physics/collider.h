#ifndef BANANA_ENGINE_COLLIDER_H
#define BANANA_ENGINE_COLLIDER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "body.h"

    typedef struct
    {
        PhysicsBodyId body_a;
        PhysicsBodyId body_b;
        float normal[3]; /* collision normal pointing from b → a */
        float contact_point[3];
        float penetration;
    } Collision;

#define BANANA_MAX_COLLISIONS 512

    typedef struct
    {
        Collision items[BANANA_MAX_COLLISIONS];
        int count;
    } CollisionList;

    /* Broad-phase: generate candidate pairs from body list. */
    CollisionList collider_broad_phase(PhysicsBody **bodies, int body_count);

    /* Narrow-phase tests. */
    int collider_box_vs_box(const PhysicsBody *a, const PhysicsBody *b, Collision *out);
    int collider_sphere_vs_sphere(const PhysicsBody *a, const PhysicsBody *b, Collision *out);
    int collider_box_vs_sphere(const PhysicsBody *a, const PhysicsBody *b, Collision *out);

    /* Impulse-based resolution (modifies body velocities in place). */
    void collider_resolve(const Collision *c, PhysicsBody *a, PhysicsBody *b);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_COLLIDER_H */
