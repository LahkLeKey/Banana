#include "physics_abi.h"

#include <stddef.h>

void runtime_physics_abi_step(PhysicsWorld *physics_world, float dt)
{
    if (!physics_world)
        return;

    physics_world_step(physics_world, dt);
}

void runtime_physics_abi_add_body(PhysicsWorld *physics_world,
                                  uint32_t body_id,
                                  float mass,
                                  float x,
                                  float y,
                                  float z)
{
    PhysicsBody *body = NULL;

    if (!physics_world)
        return;

    body = physics_world_add_body(physics_world, mass, x, y, z, SHAPE_BOX);
    if (body)
        body->id = body_id;
}

void runtime_physics_abi_update_body(PhysicsWorld *physics_world,
                                     uint32_t body_id,
                                     const float *position,
                                     const float *velocity)
{
    int i = 0;
    int found_index = -1;

    if (!physics_world)
        return;

#pragma omp parallel for schedule(static)
    for (i = 0; i < physics_world->body_count; i++)
    {
        PhysicsBody *body = physics_world->bodies[i];
        if (!body || body->id != body_id)
            continue;

#pragma omp critical
        {
            if (found_index < 0 || i < found_index)
                found_index = i;
        }
    }

    if (found_index < 0)
        return;

    {
        PhysicsBody *body = physics_world->bodies[found_index];
        if (!body)
            return;

        if (position)
        {
            body->position[0] = position[0];
            body->position[1] = position[1];
            body->position[2] = position[2];
        }

        if (velocity)
        {
            body->velocity[0] = velocity[0];
            body->velocity[1] = velocity[1];
            body->velocity[2] = velocity[2];
        }
    }
}
