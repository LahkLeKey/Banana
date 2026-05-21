#include "physics/body.h"
#include "physics/collider.h"

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
    PhysicsBody *bodies[3] = {0};
    PhysicsBody *ptrs[3] = {0};

    bodies[0] = physics_body_create(1, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    bodies[1] = physics_body_create(2, 1.0f, 0.4f, 0.0f, 0.0f, SHAPE_BOX);
    bodies[2] = physics_body_create(3, 1.0f, 4.0f, 0.0f, 0.0f, SHAPE_BOX);

    ptrs[0] = bodies[0];
    ptrs[1] = bodies[1];
    ptrs[2] = bodies[2];

    CollisionList list = collider_broad_phase(ptrs, 3);

    if (!expect_int("collision count", list.count, 1))
        return 1;

    {
        PhysicsBodyId a = list.items[0].body_a;
        PhysicsBodyId b = list.items[0].body_b;
        int pair_ok = (a == 1 && b == 2) || (a == 2 && b == 1);
        if (!expect_int("first pair", pair_ok, 1))
            return 1;
    }

    for (int i = 0; i < 3; i++)
        physics_body_destroy(bodies[i]);

    return 0;
}
