#include "runtime/player_api.h"

#include <stdio.h>
#include <string.h>

static float sample_height(float x, float z)
{
    return (x * 0.02f) + (z * 0.01f);
}

static uint32_t attach_controller_stub(uint32_t entity_id, const char *type)
{
    (void)type;
    return entity_id + 1000u;
}

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
    EntityId primary_player = 0;
    uint32_t entity_id = 0;

    runtime_player_registry_reset();

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    entity_id = runtime_player_api_upsert(world,
                                          "player-1",
                                          "One",
                                          "ai",
                                          1,
                                          sample_height,
                                          attach_controller_stub,
                                          &primary_player);
    if (!expect_int("entity id assigned", entity_id != 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("primary set", (int)primary_player, (int)entity_id))
        return 1;

    runtime_player_api_apply_input("player-1", 2.0f, -2.0f);
    {
        NativePlayerBinding *binding = runtime_player_registry_find("player-1");
        if (!expect_int("binding exists", binding != NULL ? 1 : 0, 1))
            return 1;
        if (!expect_int("input clamp x", binding->pending_move_x == 1.0f ? 1 : 0, 1))
            return 1;
        if (!expect_int("input clamp z", binding->pending_move_z == -1.0f ? 1 : 0, 1))
            return 1;
    }

        runtime_player_registry_add_resource("player-1", "gold", 100);
        runtime_player_registry_add_resource("player-1", "wood", 3);
        runtime_player_registry_add_resource("player-1", "ore", 2);
        runtime_player_registry_add_resource("player-1", "gold", -25);
        if (!expect_int("gold resource", runtime_player_registry_get_resource("player-1", "gold"), 75))
            return 1;
        if (!expect_int("wood resource", runtime_player_registry_get_resource("player-1", "wood"), 3))
            return 1;
        if (!expect_int("ore resource", runtime_player_registry_get_resource("player-1", "ore"), 2))
            return 1;

    runtime_player_api_set_transform(world, "player-1", 99.0f, 5.0f, -99.0f, 1, 8.0f, sample_height);
    {
        Entity *entity = world_get_entity(world, entity_id);
        if (!expect_int("entity exists", entity != NULL ? 1 : 0, 1))
            return 1;
        if (!expect_int("x clamped", entity->position[0] == 8.0f ? 1 : 0, 1))
            return 1;
        if (!expect_int("z clamped", entity->position[2] == -8.0f ? 1 : 0, 1))
            return 1;
    }

    world_destroy(world);
    runtime_player_registry_reset();
    return 0;
}
