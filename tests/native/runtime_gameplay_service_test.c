#include "runtime/gameplay_service.h"

#include <stdio.h>
#include <string.h>

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
    EntityId player_id = 0;
    EntityId pickup_id = 0;
    int pickup_collected = 0;

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
    pickup_id = world_spawn_entity(world, ENTITY_TYPE_STATIC, 0.5f, 0.f, 0.5f);

    {
        Entity *pickup = world_get_entity(world, pickup_id);
        if (!expect_int("pickup exists", pickup != NULL ? 1 : 0, 1))
            return 1;
        strncpy(pickup->controller_kind, "pbj_pickup", sizeof(pickup->controller_kind) - 1);
        pickup->controller_kind[sizeof(pickup->controller_kind) - 1] = '\0';
    }

    runtime_gameplay_service_tick(world,
                                  NULL,
                                  0,
                                  player_id,
                                  &pickup_collected,
                                  6.0f,
                                  1.55f);

    if (!expect_int("pickup collected flag", pickup_collected, 1))
        return 1;

    {
        Entity *pickup = world_get_entity(world, pickup_id);
        if (!expect_int("pickup disabled", pickup && pickup->active == 0 ? 1 : 0, 1))
            return 1;
    }

    world_destroy(world);
    return 0;
}
