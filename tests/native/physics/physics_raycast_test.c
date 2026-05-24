#include "physics/body.h"
#include "physics/raycast.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    PhysicsBody *bodies[2] = {0};
    PhysicsBody *ptrs[2] = {0};
    PhysicsBodyId hit_id = 0;
    float hit_t = 0.0f;

    float origin[3] = {-2.0f, 0.0f, 0.0f};
    float direction[3] = {1.0f, 0.0f, 0.0f};

    bodies[0] = physics_body_create(101, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    bodies[1] = physics_body_create(102, 1.0f, 3.0f, 0.0f, 0.0f, SHAPE_BOX);

    ptrs[0] = bodies[0];
    ptrs[1] = bodies[1];

    if (!expect_int("raycast hit",
                    physics_raycast_bodies(ptrs, 2, origin, direction, &hit_id, &hit_t),
                    1))
        return 1;

    if (!expect_int("nearest body", (int)hit_id, 101))
        return 1;

    if (!expect_int("positive t", hit_t > 0.0f ? 1 : 0, 1))
        return 1;

    for (int i = 0; i < 2; i++)
        physics_body_destroy(bodies[i]);

    return 0;
}
