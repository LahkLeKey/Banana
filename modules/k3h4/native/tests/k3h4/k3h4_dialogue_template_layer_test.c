#include "k3h4/k3h4_dialogue_cluster_router.h"
#include "k3h4/k3h4_dialogue_template_layer.h"

#include "runtime/support/test_support.h"

#include <string.h>

static int build_route(BananaK3h4DialogueIntent intent,
                       int has_entry_writ,
                       int mentions_secret_tunnel,
                       int ambiguity_basis_points,
                       BananaK3h4DialogueRouteOutput *out_output)
{
    BananaK3h4DialogueRouteInput route_input = {
        .intent = intent,
        .has_entry_writ = has_entry_writ,
        .mentions_secret_tunnel = mentions_secret_tunnel,
        .ambiguity_basis_points = ambiguity_basis_points,
    };

    return banana_native_k3h4_route_dialogue_cluster(&route_input, out_output);
}

static void test_template_policy_uses_deterministic_cluster_stack(void **state)
{
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueTemplateInput input;
    BananaK3h4DialogueTemplateOutput output = {0};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_route(BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS, 1, 0, 0, &route_output),
        0,
        "route generation must succeed");

    input.route_output = route_output;
    input.proposed_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_resolve_dialogue_template(&input, &output),
        0,
        "template resolver must succeed");

    BANANA_TEST_ASSERT_INT_EQ(output.selected_cluster_stack_count,
                              BANANA_K3H4_DIALOGUE_TEMPLATE_STACK_MAX,
                              "cluster stack count must be fixed deterministically");
    BANANA_TEST_ASSERT_INT_EQ(output.selected_cluster_stack[0],
                              route_output.cluster_id,
                              "stack must begin with routed cluster");
    BANANA_TEST_ASSERT_INT_EQ(output.selected_cluster_stack[2],
                              1099,
                              "stack must end with global deterministic fallback cluster");
}

static void test_fact_validator_blocks_unauthorized_leaks(void **state)
{
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueTemplateInput input;
    BananaK3h4DialogueTemplateOutput output = {0};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_route(BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS, 1, 0, 0, &route_output),
        0,
        "route generation must succeed");

    input.route_output = route_output;
    input.proposed_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_SECRET_TUNNEL_LOCATION;

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_resolve_dialogue_template(&input, &output),
        0,
        "template resolver must succeed for forbidden fact case");

    BANANA_TEST_ASSERT_INT_EQ(output.fact_validation.has_unauthorized_facts,
                              1,
                              "forbidden fact must be flagged as unauthorized");
    BANANA_TEST_ASSERT_TRUE((output.fact_validation.unauthorized_fact_mask &
                             BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_SECRET_TUNNEL_LOCATION) != 0u,
                            "unauthorized mask must include leaked secret tunnel fact");
    BANANA_TEST_ASSERT_INT_EQ(output.fallback_applied,
                              1,
                              "forbidden leaks must trigger deterministic fallback");
    BANANA_TEST_ASSERT_STR_EQ(output.fallback_template_key,
                              "k3h4.template.edda.south_gate.fallback.safe",
                              "fallback metadata must identify safe template selection");
}

static void test_validation_golden_scenarios_have_zero_unauthorized_leaks(void **state)
{
    struct
    {
        BananaK3h4DialogueIntent intent;
        int has_entry_writ;
        int mentions_secret_tunnel;
        int ambiguity_basis_points;
        uint32_t proposed_fact_mask;
    } golden_cases[] = {
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_CASTLE_ENTRY,
            .has_entry_writ = 1,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 0,
            .proposed_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ENTRY_WRIT_STATUS,
        },
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS,
            .has_entry_writ = 1,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 70,
            .proposed_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS,
        },
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_INSULT,
            .has_entry_writ = 0,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 0,
            .proposed_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_ESCALATION_WARNING,
        },
        {
            .intent = BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP,
            .has_entry_writ = 1,
            .mentions_secret_tunnel = 0,
            .ambiguity_basis_points = 120,
            .proposed_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE,
        },
    };
    size_t index;

    (void)state;

    for (index = 0; index < sizeof(golden_cases) / sizeof(golden_cases[0]); ++index)
    {
        BananaK3h4DialogueRouteOutput route_output = {0};
        BananaK3h4DialogueTemplateInput input;
        BananaK3h4DialogueTemplateOutput output = {0};

        BANANA_TEST_ASSERT_INT_EQ(
            build_route(golden_cases[index].intent,
                        golden_cases[index].has_entry_writ,
                        golden_cases[index].mentions_secret_tunnel,
                        golden_cases[index].ambiguity_basis_points,
                        &route_output),
            0,
            "route generation must succeed for golden case");

        input.route_output = route_output;
        input.proposed_fact_mask = golden_cases[index].proposed_fact_mask;

        BANANA_TEST_ASSERT_INT_EQ(
            banana_native_k3h4_resolve_dialogue_template(&input, &output),
            0,
            "template resolution must succeed for golden case");

        BANANA_TEST_ASSERT_INT_EQ(output.fact_validation.has_unauthorized_facts,
                                  0,
                                  "golden scenarios must not leak unauthorized facts");
        BANANA_TEST_ASSERT_INT_EQ((int)output.fact_validation.unauthorized_fact_mask,
                                  0,
                                  "unauthorized fact mask must remain zero on golden scenarios");
    }
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_template_policy_uses_deterministic_cluster_stack),
    BANANA_TEST_CASE(test_fact_validator_blocks_unauthorized_leaks),
    BANANA_TEST_CASE(test_validation_golden_scenarios_have_zero_unauthorized_leaks))
