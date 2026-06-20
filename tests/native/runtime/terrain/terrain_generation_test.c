#include "runtime/terrain/terrain_generation.h"
#include "../support/test_support.h"

#include <stddef.h>
#include <string.h>

static unsigned int hash_string_reference(unsigned int seed, const char *text)
{
    const unsigned char *cursor = (const unsigned char *)(text ? text : "");
    unsigned int hash = seed;

    while (*cursor != '\0')
    {
        hash ^= (unsigned int)(*cursor);
        hash *= 16777619u;
        cursor++;
    }

    return hash;
}

static unsigned int terrain_seed_reference(const char *world_id,
                                           const char *lane_id,
                                           unsigned int partition_epoch)
{
    unsigned int seed = 2166136261u;

    seed = hash_string_reference(seed, (world_id && world_id[0] != '\0') ? world_id : "default-world");
    seed ^= 0x9e3779b9u;
    seed *= 16777619u;

    seed = hash_string_reference(seed, (lane_id && lane_id[0] != '\0') ? lane_id : "default-lane");
    seed ^= partition_epoch;
    seed *= 16777619u;

    if (seed == 0u)
        seed = 1u;

    return seed;
}

static unsigned int terrain_fingerprint_reference(const char *world_id,
                                                  const char *lane_id,
                                                  unsigned int partition_epoch,
                                                  int chunk_x,
                                                  int chunk_z,
                                                  const char *generation_contract_version,
                                                  unsigned int retry_attempt)
{
    unsigned int hash = 2166136261u;
    const char *contract_version = generation_contract_version ? generation_contract_version
                                                               : RUNTIME_TERRAIN_GENERATION_CONTRACT_V1;

    hash = hash_string_reference(hash, world_id ? world_id : "");
    hash ^= (unsigned int)chunk_x;
    hash *= 16777619u;
    hash = hash_string_reference(hash, lane_id ? lane_id : "");
    hash ^= (unsigned int)chunk_z;
    hash *= 16777619u;
    hash ^= partition_epoch;
    hash *= 16777619u;
    hash = hash_string_reference(hash, contract_version);
    hash ^= retry_attempt;
    hash *= 16777619u;

    if (hash == 0u)
        hash = 1u;

    return hash;
}

static void test_terrain_derive_world_seed(void **state)
{
    (void)state;

    unsigned int first = runtime_terrain_derive_world_seed("world-a", "lane-1", 42u);
    unsigned int second = runtime_terrain_derive_world_seed("world-a", "lane-1", 42u);
    unsigned int third = runtime_terrain_derive_world_seed("world-b", "lane-1", 42u);

    BANANA_TEST_ASSERT_INT_EQ((int)first, (int)second, "terrain seed must be deterministic for identical inputs");
    BANANA_TEST_ASSERT_TRUE(first != 0u, "terrain seed must never be zero");
    BANANA_TEST_ASSERT_TRUE(first != third, "terrain seed should change when the world id changes");
    BANANA_TEST_ASSERT_INT_EQ((int)first,
                              (int)terrain_seed_reference("world-a", "lane-1", 42u),
                              "terrain seed must match the reference implementation");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_terrain_derive_world_seed(NULL, NULL, 0u),
                              (int)terrain_seed_reference(NULL, NULL, 0u),
                              "terrain seed must apply default world and lane ids");
}

static void test_terrain_generation_input_fingerprint(void **state)
{
    (void)state;

    unsigned int first = runtime_terrain_generation_input_fingerprint("world-a",
                                                                       "lane-1",
                                                                       42u,
                                                                       3,
                                                                       -7,
                                                                       NULL,
                                                                       2u);
    unsigned int second = runtime_terrain_generation_input_fingerprint("world-a",
                                                                        "lane-1",
                                                                        42u,
                                                                        3,
                                                                        -7,
                                                                        NULL,
                                                                        2u);
    unsigned int changed_retry = runtime_terrain_generation_input_fingerprint("world-a",
                                                                              "lane-1",
                                                                              42u,
                                                                              3,
                                                                              -7,
                                                                              NULL,
                                                                              3u);
    unsigned int explicit_contract = runtime_terrain_generation_input_fingerprint("world-a",
                                                                                   "lane-1",
                                                                                   42u,
                                                                                   3,
                                                                                   -7,
                                                                                   "custom-contract",
                                                                                   2u);

    BANANA_TEST_ASSERT_INT_EQ((int)first, (int)second, "terrain fingerprint must be deterministic for identical inputs");
    BANANA_TEST_ASSERT_TRUE(first != 0u, "terrain fingerprint must never be zero");
    BANANA_TEST_ASSERT_TRUE(first != changed_retry, "terrain fingerprint should change when retry changes");
    BANANA_TEST_ASSERT_TRUE(first != explicit_contract, "terrain fingerprint should change when the contract changes");
    BANANA_TEST_ASSERT_INT_EQ((int)first,
                              (int)terrain_fingerprint_reference("world-a", "lane-1", 42u, 3, -7, NULL, 2u),
                              "terrain fingerprint must match the reference implementation");
}

static void test_terrain_generate_unexplored_contract_validation(void **state)
{
    (void)state;

    unsigned char height_grid[4] = {0};
    unsigned int fingerprint = 0u;

    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_generate_unexplored_contract(NULL,
                                                                           2,
                                                                           2,
                                                                           "world-a",
                                                                           "lane-1",
                                                                           1u,
                                                                           0,
                                                                           0,
                                                                           NULL,
                                                                           0u,
                                                                           0,
                                                                           &fingerprint),
                              RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY,
                              "terrain generation must reject a missing grid");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_generate_unexplored_contract(height_grid,
                                                                           2,
                                                                           2,
                                                                           NULL,
                                                                           "lane-1",
                                                                           1u,
                                                                           0,
                                                                           0,
                                                                           NULL,
                                                                           0u,
                                                                           0,
                                                                           &fingerprint),
                              RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY,
                              "terrain generation must reject a missing world id");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_generate_unexplored_contract(height_grid,
                                                                           2,
                                                                           2,
                                                                           "world-a",
                                                                           "lane-1",
                                                                           1u,
                                                                           0,
                                                                           0,
                                                                           "unsupported-contract",
                                                                           0u,
                                                                           0,
                                                                           &fingerprint),
                              RUNTIME_TERRAIN_GENERATION_STATUS_CONTRACT_VERSION_UNSUPPORTED,
                              "terrain generation must reject unsupported contracts");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_generate_unexplored_contract(height_grid,
                                                                           2,
                                                                           2,
                                                                           "world-a",
                                                                           "lane-1",
                                                                           1u,
                                                                           0,
                                                                           0,
                                                                           NULL,
                                                                           7u,
                                                                           -123,
                                                                           &fingerprint),
                              -123,
                              "terrain generation must return injected failures after fingerprinting");
    BANANA_TEST_ASSERT_TRUE(fingerprint != 0u, "terrain generation must populate the fingerprint before injected failures");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_terrain_derive_world_seed),
    BANANA_TEST_CASE(test_terrain_generation_input_fingerprint),
    BANANA_TEST_CASE(test_terrain_generate_unexplored_contract_validation)
)