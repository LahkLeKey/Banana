#include "runtime/physics_abi.h"
#include "runtime/world_abi.h"

#include <math.h>
#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_float_near(const char *label, float actual, float expected)
{
    if (fabsf(actual - expected) < 0.0001f)
        return 1;

    fprintf(stderr, "%s expected=%.4f actual=%.4f\n", label, expected, actual);
    return 0;
}

int main(void)
{
    PhysicsWorld *physics = physics_world_create();
    World *world = world_create();
    float position[3] = {3.0f, 4.0f, 5.0f};
    float velocity[3] = {1.0f, 0.5f, -2.0f};

    if (!expect_int("physics created", physics != NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    runtime_physics_abi_add_body(physics, 77, 1.0f, 0.0f, 0.0f, 0.0f);
    if (!expect_int("body count", physics->body_count, 1))
        return 1;
    if (!expect_int("body id", (int)physics->bodies[0]->id, 77))
        return 1;

    runtime_physics_abi_update_body(physics, 77, position, velocity);
    if (!expect_float_near("position x", physics->bodies[0]->position[0], 3.0f))
        return 1;
    if (!expect_float_near("position y", physics->bodies[0]->position[1], 4.0f))
        return 1;
    if (!expect_float_near("velocity z", physics->bodies[0]->velocity[2], -2.0f))
        return 1;

    runtime_physics_abi_step(physics, physics->fixed_dt);

    {
        uint32_t spawned = runtime_world_abi_spawn(world, ENTITY_TYPE_NPC, 1.5f, 2.5f, 3.5f);
        if (!expect_int("spawned id", spawned != 0 ? 1 : 0, 1))
            return 1;
        if (!expect_int("world count", world->entity_count, 1))
            return 1;
        if (!expect_float_near("spawn x", world->entities[0]->position[0], 1.5f))
            return 1;
    }

    runtime_world_abi_tick(world, 1.0f / 60.0f);

    physics_world_destroy(physics);
    world_destroy(world);
    return 0;
}
