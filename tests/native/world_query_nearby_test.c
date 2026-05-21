#include "world/world.h"

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
    World *world = world_create();
    float probe[3] = {0.0f, 0.0f, 0.0f};
    EntityId found[8] = {0};

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    EntityId near_a = world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 0.0f);
    EntityId far_b = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 12.0f, 0.0f, 0.0f);
    EntityId near_c = world_spawn_entity(world, ENTITY_TYPE_PLAYER, -1.2f, 0.0f, 0.3f);
    EntityId near_inactive = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.8f, 0.0f, -0.2f);

    if (!expect_int("spawn ids", near_a != 0 && far_b != 0 && near_c != 0 && near_inactive != 0, 1))
        return 1;

    {
        Entity *inactive = world_get_entity(world, near_inactive);
        if (!expect_int("inactive found", inactive != NULL ? 1 : 0, 1))
            return 1;
        inactive->active = 0;
    }

    {
        int count = world_query_nearby(world, probe, 2.0f, found, 8);
        if (!expect_int("count full", count, 2))
            return 1;
        if (!expect_int("first id order", (int)found[0], (int)near_a))
            return 1;
        if (!expect_int("second id order", (int)found[1], (int)near_c))
            return 1;
    }

    {
        int count = world_query_nearby(world, probe, 2.0f, found, 1);
        if (!expect_int("count capped", count, 1))
            return 1;
        if (!expect_int("capped id", (int)found[0], (int)near_a))
            return 1;
    }

    if (!expect_int("null world guard", world_query_nearby(NULL, probe, 1.0f, found, 1), 0))
        return 1;

    world_destroy(world);
    return 0;
}
