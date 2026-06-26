#include "k3h4/k3h4_dialogue_cluster_router.h"
#include "k3h4/k3h4_dialogue_generative_surface_layer.h"
#include "k3h4/k3h4_dialogue_template_layer.h"

#include "runtime/support/test_support.h"

static int build_template(BananaK3h4DialogueIntent intent,
                          int has_entry_writ,
                          int mentions_secret_tunnel,
                          int ambiguity_basis_points,
                          uint32_t proposed_fact_mask,
                          BananaK3h4DialogueTemplateOutput *out_template_output)
{
    BananaK3h4DialogueRouteInput route_input = {
        .intent = intent,
        .has_entry_writ = has_entry_writ,
        .mentions_secret_tunnel = mentions_secret_tunnel,
        .ambiguity_basis_points = ambiguity_basis_points,
    };
    BananaK3h4DialogueRouteOutput route_output = {0};
    BananaK3h4DialogueTemplateInput template_input;

    if (banana_native_k3h4_route_dialogue_cluster(&route_input, &route_output) != 0)
        return -1;

    template_input.route_output = route_output;
    template_input.proposed_fact_mask = proposed_fact_mask;

    return banana_native_k3h4_resolve_dialogue_template(&template_input, out_template_output);
}

static void test_assembler_builds_generator_contract_from_stack_and_facts(void **state)
{
    BananaK3h4DialogueTemplateOutput template_output = {0};
    BananaK3h4DialogueGeneratorInput generator_input;
    BananaK3h4DialogueGeneratorOutput generator_output = {0};
    int status;

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_template(BANANA_K3H4_DIALOGUE_INTENT_REQUEST_HELP,
                       1,
                       0,
                       40,
                       BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE,
                       &template_output),
        0,
        "template output generation must succeed");

    generator_input.template_output = template_output;
    generator_input.selected_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE;
    generator_input.canon_violation_mask = 0u;

    status = banana_native_k3h4_assemble_dialogue_generator_contract(&generator_input, &generator_output);

    BANANA_TEST_ASSERT_INT_EQ(status, 0, "contract assembly must succeed for valid facts");
    BANANA_TEST_ASSERT_INT_EQ(generator_output.fail_closed, 0, "valid contract must not fail closed");
    BANANA_TEST_ASSERT_INT_EQ(generator_output.selected_cluster_stack_count,
                              template_output.selected_cluster_stack_count,
                              "generator contract must include selected cluster stack");
    BANANA_TEST_ASSERT_TRUE((generator_output.validated_fact_mask &
                             BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_HELP_GUIDANCE) != 0u,
                            "validated fact mask must preserve allowed selected facts");
}

static void test_validator_rejects_violations_with_deterministic_deny_metadata(void **state)
{
    BananaK3h4DialogueTemplateOutput template_output = {0};
    BananaK3h4DialogueGeneratorInput generator_input;
    BananaK3h4DialogueGeneratorOutput first_output = {0};
    BananaK3h4DialogueGeneratorOutput second_output = {0};
    int first_status;
    int second_status;

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        build_template(BANANA_K3H4_DIALOGUE_INTENT_ASK_DIRECTIONS,
                       1,
                       0,
                       0,
                       BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS,
                       &template_output),
        0,
        "template output generation must succeed");

    generator_input.template_output = template_output;
    generator_input.selected_fact_mask = BANANA_K3H4_DIALOGUE_TEMPLATE_FACT_MASK_PUBLIC_GATE_DIRECTIONS;
    generator_input.canon_violation_mask = 1u;

    first_status = banana_native_k3h4_assemble_dialogue_generator_contract(&generator_input, &first_output);
    second_status = banana_native_k3h4_assemble_dialogue_generator_contract(&generator_input, &second_output);

    BANANA_TEST_ASSERT_TRUE(first_status != 0,
                            "canon violations must be rejected with non-zero status");
    BANANA_TEST_ASSERT_TRUE(second_status != 0,
                            "repeat canon violations must remain non-zero");
    BANANA_TEST_ASSERT_INT_EQ(first_output.fail_closed,
                              1,
                              "violation must trigger fail-closed output");
    BANANA_TEST_ASSERT_INT_EQ(first_output.deny_reason,
                              BANANA_K3H4_DIALOGUE_GENERATOR_DENY_REASON_CANON_VIOLATION,
                              "deny reason must encode canon violation");
    BANANA_TEST_ASSERT_STR_EQ(first_output.deny_template_key,
                              "k3h4.template.edda.south_gate.deny.fail_closed",
                              "deny metadata must provide deterministic deny template key");
    BANANA_TEST_ASSERT_INT_EQ(first_output.deny_code,
                              second_output.deny_code,
                              "deny code must be deterministic for identical violating input");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_assembler_builds_generator_contract_from_stack_and_facts),
    BANANA_TEST_CASE(test_validator_rejects_violations_with_deterministic_deny_metadata))
