#include "runtime/player_api.h"
#include "runtime/player_builds.h"

#include <stdio.h>

static float sample_height(float x, float z)
{
    return (x + z) * 0.01f;
}

static uint32_t attach_controller_stub(uint32_t entity_id, const char *type)
{
    (void)type;
    return entity_id + 33u;
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
    BuildStats stats;
    GearModifier weapon = {2, 5, 0, 0};
    ComboOutcome outcome;

    runtime_player_registry_reset();
    player_builds_init();

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    if (!expect_int("upsert player with AI controller",
                    runtime_player_api_upsert(world,
                                              "build-player",
                                              "Builder",
                                              "ai",
                                              1,
                                              sample_height,
                                              attach_controller_stub,
                                              &primary_player) != 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    /* AI defaults to ranger via runtime_player_api upsert integration. */
    if (!expect_int("default ranger attack",
                    player_builds_get_stats("build-player", &stats) == 0 ? stats.attack : -1,
                    20))
        return 1;

    if (!expect_int("set allocations", player_builds_set_allocations("build-player", 3, 1, 2), 0))
        return 1;

    if (!expect_int("equip weapon", player_builds_equip("build-player", GEAR_SLOT_WEAPON, &weapon), 0))
        return 1;

    if (!expect_int("stats reflect build+gear",
                    player_builds_get_stats("build-player", &stats) == 0 ? stats.attack : -1,
                    31))
        return 1;

    if (!expect_int("combo trigger",
                    player_builds_evaluate_combo("build-player",
                                                 "snare_shot",
                                                 "piercing_volley",
                                                 1000,
                                                 3,
                                                 &outcome),
                    1))
    {
        return 1;
    }

    if (!expect_int("party combo synergy", outcome.party_synergy_bonus_pct, 9))
        return 1;

    world_destroy(world);
    player_builds_cleanup();
    runtime_player_registry_reset();
    return 0;
}