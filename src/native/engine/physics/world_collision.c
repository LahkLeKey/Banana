#include "world_collision.h"

#include <string.h>

void physics_world_resolve_collision_pairs(PhysicsWorld *w, const CollisionList *cols)
{
    PhysicsBody *body_by_id[BANANA_MAX_PHYSICS_BODIES + 1];
    int i = 0;

    if (!w || !cols)
        return;

    memset(body_by_id, 0, sizeof(body_by_id));

#pragma omp parallel for schedule(static)
    for (i = 0; i < w->body_count; i++)
    {
        PhysicsBody *body = w->bodies[i];
        if (!body)
            continue;
        if (body->id == 0 || body->id > BANANA_MAX_PHYSICS_BODIES)
            continue;
        body_by_id[body->id] = body;
    }

    for (i = 0; i < cols->count; i++)
    {
        PhysicsBodyId a_id = cols->items[i].body_a;
        PhysicsBodyId b_id = cols->items[i].body_b;
        PhysicsBody *a = NULL;
        PhysicsBody *b = NULL;

        if (a_id == 0 || a_id > BANANA_MAX_PHYSICS_BODIES ||
            b_id == 0 || b_id > BANANA_MAX_PHYSICS_BODIES)
        {
            continue;
        }

        a = body_by_id[a_id];
        b = body_by_id[b_id];
        if (a && b)
            collider_resolve(&cols->items[i], a, b);
    }
}
