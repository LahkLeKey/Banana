#include "runtime/player_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int64_t base_time_ms = 1000000;
    int64_t stale_threshold_ms = 2000;  /* 2-second staleness window */

    /* Create a world for entity management */
    world = world_create();
    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        goto cleanup;

    /* Reset the registry for this test */
    runtime_player_registry_reset();

    /* Upsert a player */
    binding = runtime_player_registry_upsert(
        world,
        "player-1",
        "TestPlayer1",
        "human",
        1,
        sample_height_fn,
        attach_controller_noop,
        &primary_id);
    if (!expect_int("player upserted", binding != NULL ? 1 : 0, 1))
        goto cleanup;

    /* ── Test 1: Fresh player should not be stale ── */
    runtime_player_registry_mark_seen("player-1", base_time_ms);
    runtime_player_registry_update_staleness(base_time_ms + 1000, stale_threshold_ms);
    binding = runtime_player_registry_find("player-1");
    if (!expect_int("fresh player not stale (1s)", binding->is_stale, 0))
        goto cleanup;

    /* ── Test 2: Player at threshold should not be stale ── */
    runtime_player_registry_update_staleness(base_time_ms + stale_threshold_ms, stale_threshold_ms);
    binding = runtime_player_registry_find("player-1");
    if (!expect_int("player at threshold not stale (2s exactly)", binding->is_stale, 0))
        goto cleanup;

    /* ── Test 3: Player beyond threshold should be stale ── */
    runtime_player_registry_update_staleness(base_time_ms + stale_threshold_ms + 1, stale_threshold_ms);
    binding = runtime_player_registry_find("player-1");
    if (!expect_int("player beyond threshold is stale (2001ms)", binding->is_stale, 1))
        goto cleanup;

    /* ── Test 4: Marking player seen again clears stale flag ── */
    runtime_player_registry_mark_seen("player-1", base_time_ms + stale_threshold_ms + 1);
    runtime_player_registry_update_staleness(base_time_ms + stale_threshold_ms + 2, stale_threshold_ms);
    binding = runtime_player_registry_find("player-1");
    if (!expect_int("player marked seen clears stale", binding->is_stale, 0))
        goto cleanup;

    /* ── Test 5: Deactivate stale should clear active flag ── */
    binding->active = 1;
    runtime_player_registry_mark_seen("player-1", base_time_ms);
    runtime_player_registry_update_staleness(base_time_ms + stale_threshold_ms + 10, stale_threshold_ms);
    if (!expect_int("player marked stale", binding->is_stale, 1))
        goto cleanup;

    runtime_player_registry_deactivate_stale();
    binding = runtime_player_registry_find("player-1");
    if (!expect_int("stale player deactivated", binding->active, 0))
        goto cleanup;

    /* ── Test 6: Deactivate stale only affects stale players ── */
    runtime_player_registry_reset();
    world_destroy(world);

    /* Create a fresh world for this sub-test */
    world = world_create();
    if (!expect_int("world created for deactivate test", world != NULL ? 1 : 0, 1))
        goto cleanup;

    /* Add 2 players */
    runtime_player_registry_upsert(
        world,
        "p1",
        "Player1",
        "human",
        1,
        sample_height_fn,
        attach_controller_noop,
        &primary_id);
    runtime_player_registry_upsert(
        world,
        "p2",
        "Player2",
        "human",
        1,
        sample_height_fn,
        attach_controller_noop,
        &primary_id);

    /* Mark p1 as fresh, p2 as old */
    runtime_player_registry_mark_seen("p1", base_time_ms + 100);
    runtime_player_registry_mark_seen("p2", base_time_ms - 1000);

    /* Update staleness: p1 fresh, p2 stale */
    runtime_player_registry_update_staleness(base_time_ms + stale_threshold_ms + 10, stale_threshold_ms);
    binding = runtime_player_registry_find("p1");
    if (!expect_int("p1 fresh", binding->is_stale, 0))
        goto cleanup;
    binding = runtime_player_registry_find("p2");
    if (!expect_int("p2 stale", binding->is_stale, 1))
        goto cleanup;

    /* Deactivate stale - should only affect p2 */
    runtime_player_registry_deactivate_stale();

    binding = runtime_player_registry_find("p1");
    if (!expect_int("p1 still active after deactivate", binding->active, 1))
        goto cleanup;
    binding = runtime_player_registry_find("p2");
    if (!expect_int("p2 deactivated", binding->active, 0))
        goto cleanup;

    world_destroy(world);
    return 0;

cleanup:
    if (world)
        world_destroy(world);
    return 1;
}
