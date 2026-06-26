#include "k3h4/k3h4_dialogue_memory_delta_store.h"

#include "runtime/support/test_support.h"

static BananaK3h4DialogueMemoryPolicy default_policy(void)
{
    BananaK3h4DialogueMemoryPolicy policy = {
        .ttl_ticks = 5,
        .delta_min = -100,
        .delta_max = 100,
    };
    return policy;
}

static void test_delta_store_applies_cap_and_range_policy(void **state)
{
    BananaK3h4DialogueMemoryStore store;
    int delta = 0;

    (void)state;

    banana_native_k3h4_dialogue_memory_init(&store, default_policy());

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_upsert_delta(
            &store,
            1001,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA,
            1000,
            10),
        0,
        "upsert should succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_get_delta(
            &store,
            1001,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA,
            11,
            &delta),
        0,
        "stored delta should be retrievable before TTL expiry");
    BANANA_TEST_ASSERT_INT_EQ(delta,
                              100,
                              "delta must clamp to configured upper range boundary");
}

static void test_ttl_prunes_expired_entries_deterministically(void **state)
{
    BananaK3h4DialogueMemoryStore store;
    int delta = 0;

    (void)state;

    banana_native_k3h4_dialogue_memory_init(&store, default_policy());

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_upsert_delta(
            &store,
            2001,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_HELPFULNESS_DELTA,
            20,
            0),
        0,
        "initial upsert should succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_get_delta(
            &store,
            2001,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_HELPFULNESS_DELTA,
            4,
            &delta),
        0,
        "entry should remain active before TTL");

    banana_native_k3h4_dialogue_memory_prune_expired(&store, 5);

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_get_delta(
            &store,
            2001,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_HELPFULNESS_DELTA,
            5,
            &delta),
        -2,
        "entry should be unavailable at or after expiry tick");
}

static void test_capacity_pressure_uses_deterministic_eviction_order(void **state)
{
    BananaK3h4DialogueMemoryStore store;
    int expected_capacity = BANANA_K3H4_DIALOGUE_MEMORY_NPC_CAP * BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_CAP;
    int index;
    int delta = 0;

    (void)state;

    banana_native_k3h4_dialogue_memory_init(&store, default_policy());

    for (index = 0; index < expected_capacity; ++index)
    {
        uint32_t npc_id = (uint32_t)(3000 + (index / BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_CAP));
        BananaK3h4DialogueMemoryFeature feature =
            (BananaK3h4DialogueMemoryFeature)(index % BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_CAP);

        BANANA_TEST_ASSERT_INT_EQ(
            banana_native_k3h4_dialogue_memory_upsert_delta(
                &store,
                npc_id,
                feature,
                index,
                100),
            0,
            "filling capacity entries should succeed");
    }

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_count_active(&store, 101),
        expected_capacity,
        "active count must be capped at configured store capacity");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_upsert_delta(
            &store,
            9999,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA,
            77,
            100),
        0,
        "over-capacity insert should succeed by deterministic eviction");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_get_delta(
            &store,
            3000,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA,
            101,
            &delta),
        -2,
        "first inserted entry should be evicted first under equal-expiry pressure");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_dialogue_memory_get_delta(
            &store,
            9999,
            BANANA_K3H4_DIALOGUE_MEMORY_FEATURE_TRUST_DELTA,
            101,
            &delta),
        0,
        "new entry should be retained after deterministic eviction");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_delta_store_applies_cap_and_range_policy),
    BANANA_TEST_CASE(test_ttl_prunes_expired_entries_deterministically),
    BANANA_TEST_CASE(test_capacity_pressure_uses_deterministic_eviction_order))
