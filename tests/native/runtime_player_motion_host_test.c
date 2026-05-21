#include "runtime/player_motion_host.h"

#include <math.h>
#include <stdio.h>

static float sample_height(float x, float z)
{
    (void)x;
    (void)z;
    return 0.0f;
}

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    World *world = world_create();
    EntityId player_id = 0;
    float eye[3] = {0.f, 4.f, 6.f};
    float target[3] = {0.f, 0.f, 0.f};

    runtime_player_registry_reset();

    if (!expect_true("world", world != NULL))
        return 1;

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, 0.f, 0.f, 0.f);
    runtime_player_registry_add_default(player_id,
                                        "motion-host-player",
                                        "Motion Host",
                                        "human",
                                        1);

    runtime_player_motion_tick(world,
                               player_id,
                               0.f,
                               1.f,
                               eye,
                               target,
                               0,
                               5.5f,
                               1.0f,
                               8.0f,
                               sample_height);

    {
        Entity *player = world_get_entity(world, player_id);
        if (!expect_true("player exists", player != NULL))
            return 1;
        if (!expect_true("moved forward fallback basis", player->position[2] < -1.0f))
            return 1;
        if (!expect_true("height applied", fabsf(player->position[1] - 0.55f) < 0.0001f))
            return 1;
    }

    world_destroy(world);
    runtime_player_registry_reset();
    return 0;
}
