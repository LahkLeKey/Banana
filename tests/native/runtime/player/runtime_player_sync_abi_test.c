#include "runtime/player/player_registry.h"
#include "runtime/player/player_sync_abi.h"

#include <stdio.h>

#include "world/world.h"

static float sample_height_fn(float x, float z)
{
    (void)x;
    (void)z;
    return 10.0f;
}

static uint32_t attach_controller_noop(uint32_t entity_id, const char *type)
{
    (void)entity_id;
    (void)type;
    return 0;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s failed: got %d, expected %d\n", label, actual, expected);
    return 0;
}

int main(void)
{
    World *world = NULL;
    NativePlayerBinding *binding = NULL;
    EntityId primary_id = 0;
    RuntimePlayerSyncWindow window;

    world = world_create();
    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        goto cleanup;

    runtime_player_registry_reset();

    if (!expect_int("upsert p1", runtime_player_registry_upsert(world,
                                                                  "p1",
                                                                  "Player1",
                                                                  "human",
                                                                  1,
                                                                  sample_height_fn,
                                                                  attach_controller_noop,
                                                                  &primary_id) != NULL ? 1 : 0,
                    1))
    {
        goto cleanup;
    }

    if (!expect_int("upsert p2", runtime_player_registry_upsert(world,
                                                                  "p2",
                                                                  "Player2",
                                                                  "human",
                                                                  1,
                                                                  sample_height_fn,
                                                                  attach_controller_noop,
                                                                  &primary_id) != NULL ? 1 : 0,
                    1))
    {
        goto cleanup;
    }

    runtime_player_sync_abi_mark_seen("p1", 1400);
    runtime_player_sync_abi_mark_seen("p2", 900);

    window.current_time_ms = 2500;
    window.stale_threshold_ms = 1200;
    runtime_player_sync_abi_update_staleness(window);

    binding = runtime_player_registry_find("p1");
    if (!expect_int("p1 fresh", binding ? binding->is_stale : -1, 0))
        goto cleanup;

    binding = runtime_player_registry_find("p2");
    if (!expect_int("p2 stale", binding ? binding->is_stale : -1, 1))
        goto cleanup;

    if (!expect_int("active count before deactivate",
                    runtime_player_sync_abi_count_active(),
                    1))
    {
        goto cleanup;
    }

    runtime_player_sync_abi_deactivate_stale();

    binding = runtime_player_registry_find("p2");
    if (!expect_int("p2 deactivated", binding ? binding->active : -1, 0))
        goto cleanup;

    if (!expect_int("active count after deactivate",
                    runtime_player_sync_abi_count_active(),
                    1))
    {
        goto cleanup;
    }

    runtime_player_sync_abi_mark_seen("unknown-player", 9999);
    if (!expect_int("unknown player mark seen no effect",
                    runtime_player_sync_abi_count_active(),
                    1))
    {
        goto cleanup;
    }

    binding = runtime_player_registry_find("p1");
    if (!expect_int("p1 binding exists for threshold test", binding != NULL ? 1 : 0, 1))
        goto cleanup;

    runtime_player_sync_abi_mark_seen("p1", 3000);
    window.current_time_ms = 4200;
    window.stale_threshold_ms = 1200;
    runtime_player_sync_abi_update_staleness(window);

    binding = runtime_player_registry_find("p1");
    if (!expect_int("threshold boundary not stale", binding ? binding->is_stale : -1, 0))
        goto cleanup;

    world_destroy(world);
    return 0;

cleanup:
    if (world)
        world_destroy(world);
    return 1;
}
