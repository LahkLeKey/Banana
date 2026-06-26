#include "k3h4/k3h4_dialogue_cluster_router.h"

#include "runtime/support/test_support.h"

#include <string.h>

static void assert_same_bytes(const void *left,
                              const void *right,
                              size_t size,
                              const char *message)
{
    BANANA_TEST_ASSERT_TRUE(memcmp(left, right, size) == 0, message);
}

static void test_replay_is_byte_identical_for_identical_input_streams(void **state)
{
    BananaK3h4DialogueRouteInput inputs[] = {
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY,
            .has_entry_writ = 0,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 90,
        },
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS,
            .has_entry_writ = 1,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 120,
        },
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_INSULT,
            .has_entry_writ = 0,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 30,
        },
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP,
            .has_entry_writ = 1,
            .mentions_secret_tunnel = 1,
            .ambiguity_basis_points = 250,
        },
    };
    BananaK3h4DialogueRouteOutput replay_a[sizeof(inputs) / sizeof(inputs[0])];
    BananaK3h4DialogueRouteOutput replay_b[sizeof(inputs) / sizeof(inputs[0])];
    size_t index;

    (void)state;

    memset(replay_a, 0, sizeof(replay_a));
    memset(replay_b, 0, sizeof(replay_b));

    for (index = 0; index < sizeof(inputs) / sizeof(inputs[0]); ++index)
    {
        BANANA_TEST_ASSERT_INT_EQ(
            banana_native_k3h4_route_dialogue_cluster(&inputs[index], &replay_a[index]),
            0,
            "first replay routing pass must succeed");
        BANANA_TEST_ASSERT_INT_EQ(
            banana_native_k3h4_route_dialogue_cluster(&inputs[index], &replay_b[index]),
            0,
            "second replay routing pass must succeed");

        BANANA_TEST_ASSERT_INT_EQ(
            replay_a[index].transition_id,
            replay_b[index].transition_id,
            "transition ids must match for identical inputs");
    }

    assert_same_bytes(replay_a,
                      replay_b,
                      sizeof(replay_a),
                      "routing outputs must be byte-identical across deterministic replay");
}

static void test_gate_precedence_emits_tie_break_metadata(void **state)
{
    BananaK3h4DialogueRouteInput input = {
        .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY,
        .has_entry_writ = 0,
        .mentions_secret_tunnel = 1,
        .ambiguity_basis_points = 50,
    };
    BananaK3h4DialogueRouteOutput output = {0};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_route_dialogue_cluster(&input, &output),
        0,
        "routing must succeed for tie-break case");

    BANANA_TEST_ASSERT_INT_EQ(
        output.resolved_gate,
        BANANA_K3H4_DIALOGUE_GATE_DO_NOT_REVEAL_SECRET_TUNNEL,
        "secret tunnel gate must win precedence tie-break");
    BANANA_TEST_ASSERT_INT_EQ(output.gate_precedence_rank,
                              1,
                              "winning gate must report precedence rank");
    BANANA_TEST_ASSERT_INT_EQ(output.tie_break_applied,
                              1,
                              "tie-break flag must indicate resolver tie use");
    BANANA_TEST_ASSERT_INT_EQ((int)output.triggered_gate_mask,
                              BANANA_K3H4_DIALOGUE_GATE_MASK_SECRET_TUNNEL |
                                  BANANA_K3H4_DIALOGUE_GATE_MASK_WRIT_REQUIRED,
                              "both gates must be represented in triggered mask");
    BANANA_TEST_ASSERT_STR_EQ(output.tie_break_rule,
                              "precedence-order",
                              "tie-break metadata must preserve deterministic rule id");
}

static void test_response_contract_is_skeleton_without_phrasing_authority(void **state)
{
    BananaK3h4DialogueRouteInput input = {
        .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS,
        .has_entry_writ = 1,
        .mentions_secret_tunnel = 0,
        .ambiguity_basis_points = 0,
    };
    BananaK3h4DialogueRouteOutput output = {0};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_route_dialogue_cluster(&input, &output),
        0,
        "routing must succeed for response skeleton checks");

    BANANA_TEST_ASSERT_INT_EQ(output.response_contract.contract_version,
                              1,
                              "response contract version must be set");
    BANANA_TEST_ASSERT_INT_EQ(output.response_contract.response_mode,
                              BANANA_K3H4_DIALOGUE_RESPONSE_REDIRECT,
                              "directions intent should emit redirect mode skeleton");
    BANANA_TEST_ASSERT_INT_EQ(output.response_contract.target_cluster_id,
                              output.cluster_id,
                              "response contract must reference routed cluster");
    BANANA_TEST_ASSERT_STR_EQ(output.response_contract.response_key,
                              "k3h4.edda.south_gate.directions",
                              "response contract must emit deterministic key only");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_replay_is_byte_identical_for_identical_input_streams),
    BANANA_TEST_CASE(test_gate_precedence_emits_tie_break_metadata),
    BANANA_TEST_CASE(test_response_contract_is_skeleton_without_phrasing_authority))
