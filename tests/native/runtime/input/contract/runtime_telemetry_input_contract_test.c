#include "runtime/input/contract/input_contract.h"
#include "runtime/world/world_telemetry.h"
#include "world/world.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

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
    World *world = world_create();
    TerrainChunk telemetry_chunk;
    uint64_t area_hash_a = 0ULL;
    uint64_t area_hash_b = 0ULL;
    uint64_t area_hash_c = 0ULL;
    float out_x = 0.f;
    float out_z = 0.f;

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    world_spawn_entity(world, ENTITY_TYPE_PLAYER, 3.5f, 0.f, -2.25f);
    world_spawn_entity(world, ENTITY_TYPE_NPC, -1.0f, 0.f, 4.75f);

    if (!expect_int("entity count", runtime_world_entity_count(world), 2))
        return 1;
    if (!expect_float_near("entity 0 x", runtime_world_entity_x(world, 0), 3.5f))
        return 1;
    if (!expect_float_near("entity 1 z", runtime_world_entity_z(world, 1), 4.75f))
        return 1;
    if (!expect_float_near("out of range x", runtime_world_entity_x(world, 99), 0.f))
        return 1;

    area_hash_a = runtime_world_telemetry_area_identity_hash("world-a", "lane-main", 4, 8, 2u);
    area_hash_b = runtime_world_telemetry_area_identity_hash("world-a", "lane-main", 4, 8, 2u);
    area_hash_c = runtime_world_telemetry_area_identity_hash("world-a", "lane-main", 5, 8, 2u);
    if (!expect_int("deterministic area hash", area_hash_a == area_hash_b ? 1 : 0, 1))
        return 1;
    if (!expect_int("area hash changes with chunk", area_hash_a != area_hash_c ? 1 : 0, 1))
        return 1;

    memset(&telemetry_chunk, 0, sizeof(telemetry_chunk));
    telemetry_chunk.generation_fingerprint = 0x1234ULL;
    telemetry_chunk.boundary_hash = 0x5678ULL;
    if (!expect_int("generation fingerprint telemetry",
                    runtime_world_telemetry_chunk_generation_fingerprint(&telemetry_chunk) == 0x1234ULL ? 1 : 0,
                    1))
        return 1;
    if (!expect_int("boundary hash telemetry",
                    runtime_world_telemetry_chunk_boundary_hash(&telemetry_chunk) == 0x5678ULL ? 1 : 0,
                    1))
        return 1;

    runtime_input_contract_sanitize_move_input(2.5f, -3.5f, &out_x, &out_z);
    if (!expect_float_near("clamp x", out_x, 1.f))
        return 1;
    if (!expect_float_near("clamp z", out_z, -1.f))
        return 1;

    runtime_input_contract_sanitize_move_input(NAN, INFINITY, &out_x, &out_z);
    if (!expect_float_near("nan x", out_x, 0.f))
        return 1;
    if (!expect_float_near("inf z", out_z, 0.f))
        return 1;

    if (!expect_int("click count", runtime_input_contract_get_click_count(), 0))
        return 1;
    if (!expect_int("target reached", runtime_input_contract_get_target_reached_count(), 0))
        return 1;
    if (!expect_int("has move target", runtime_input_contract_get_has_move_target(), 0))
        return 1;

    if (!expect_int("right click accepted", runtime_input_contract_handle_right_click(10.f, 20.f), 1))
        return 1;
    if (!expect_int("click count incremented", runtime_input_contract_get_click_count(), 1))
        return 1;
    if (!expect_int("target flagged", runtime_input_contract_get_has_move_target(), 1))
        return 1;

    if (!expect_int("normalized accepted", runtime_input_contract_handle_right_click_normalized(0.2f, 0.8f), 1))
        return 1;
    if (!expect_int("normalized does not increment click count", runtime_input_contract_get_click_count(), 1))
        return 1;
    if (!expect_int("normalized keeps target flag", runtime_input_contract_get_has_move_target(), 1))
        return 1;

    if (!expect_int("normalized rejects out of range",
                    runtime_input_contract_handle_right_click_normalized(2.0f, 0.0f),
                    0))
        return 1;

    runtime_input_contract_mark_target_reached();
    if (!expect_int("target reached incremented", runtime_input_contract_get_target_reached_count(), 1))
        return 1;
    if (!expect_int("target cleared after reached", runtime_input_contract_get_has_move_target(), 0))
        return 1;

    runtime_input_contract_set_has_move_target(1);
    if (!expect_int("explicit target set", runtime_input_contract_get_has_move_target(), 1))
        return 1;

    runtime_input_contract_reset();
    if (!expect_int("reset click count", runtime_input_contract_get_click_count(), 0))
        return 1;
    if (!expect_int("reset target reached", runtime_input_contract_get_target_reached_count(), 0))
        return 1;
    if (!expect_int("reset has move target", runtime_input_contract_get_has_move_target(), 0))
        return 1;

    world_destroy(world);
    return 0;
}
