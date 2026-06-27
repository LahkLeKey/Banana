#include "banana_native_v3.h"

#include "runtime/support/test_support.h"

#include <string.h>

static void fill_turn_input(
    banana_native_v3_k3h4_dialogue_turn_input *out_input,
    const char *intent_id,
    const char *policy_category,
    const char *confidence_band,
    const char *severity)
{
    memset(out_input, 0, sizeof(*out_input));

    strncpy(out_input->npc_id, "edda_gatekeeper", sizeof(out_input->npc_id) - 1);
    strncpy(out_input->quest_state_id, "castle_entry_writ_pending", sizeof(out_input->quest_state_id) - 1);
    strncpy(out_input->region_id, "south_gate", sizeof(out_input->region_id) - 1);
    strncpy(out_input->intent_id, intent_id, sizeof(out_input->intent_id) - 1);

    strncpy(out_input->policy_category, policy_category, sizeof(out_input->policy_category) - 1);
    strncpy(out_input->policy_confidence_band, confidence_band, sizeof(out_input->policy_confidence_band) - 1);
    strncpy(out_input->policy_severity, severity, sizeof(out_input->policy_severity) - 1);

    out_input->prior_memory_trust_delta_q16 = 2048;
    out_input->prior_memory_hostility_delta_q16 = 0;
    out_input->prior_memory_helpfulness_delta_q16 = 1024;
    out_input->sequence_tick = 11;
}

static void test_dialogue_turn_executes_and_emits_contract(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output;
    int status;

    (void)state;

    fill_turn_input(
        &input,
        "ASK_CASTLE_ENTRY",
        "game_world_violence",
        "low",
        "moderate");
    memset(&output, 0, sizeof(output));

    status = banana_native_v3_k3h4_run_dialogue_turn(&input, &output);

    BANANA_TEST_ASSERT_INT_EQ(status, BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK, "dialogue turn should execute");
    BANANA_TEST_ASSERT_INT_EQ(output.schema_version, 1, "schema version should be 1");
    BANANA_TEST_ASSERT_INT_EQ(output.abi_status, 0, "ABI status should report success");
    BANANA_TEST_ASSERT_INT_EQ(output.abi_contract_version, 1, "ABI contract version should be 1");
    BANANA_TEST_ASSERT_TRUE(output.route_cluster_id > 0, "route cluster id should be populated");
    BANANA_TEST_ASSERT_TRUE(output.transition_id > 0, "transition id should be populated");
    BANANA_TEST_ASSERT_TRUE(strlen(output.npc_line_candidate) > 0, "npc line candidate should be emitted");
    BANANA_TEST_ASSERT_TRUE(strlen(output.response_policy_label) > 0, "response policy label should be emitted");
}

static void test_dialogue_turn_is_deterministic_for_identical_inputs(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output_a;
    banana_native_v3_k3h4_dialogue_turn_output output_b;

    (void)state;

    fill_turn_input(
        &input,
        "ASK_DIRECTIONS",
        "game_world_violence",
        "low",
        "moderate");

    memset(&output_a, 0, sizeof(output_a));
    memset(&output_b, 0, sizeof(output_b));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output_a),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "first deterministic run should succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output_b),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "second deterministic run should succeed");

    BANANA_TEST_ASSERT_TRUE(
        memcmp(&output_a, &output_b, sizeof(output_a)) == 0,
        "identical input should produce byte-identical output");
}

static void test_dialogue_turn_hard_block_sets_mutation_guards(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output;

    (void)state;

    fill_turn_input(
        &input,
        "REQUEST_SELF_HARM_INSTRUCTIONS",
        "self_harm",
        "high",
        "critical");
    memset(&output, 0, sizeof(output));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "hard-block run should still return deterministic success envelope");

    BANANA_TEST_ASSERT_STR_EQ(output.response_policy_label, "hard_block", "hard-block label should be emitted");
    BANANA_TEST_ASSERT_INT_EQ(output.state_mutation_blocked, 1, "hard-block should set state mutation guard");
    BANANA_TEST_ASSERT_INT_EQ(output.memory_write_blocked, 1, "hard-block should set memory write guard");
    BANANA_TEST_ASSERT_INT_EQ(output.memory_delta_applied, 0, "hard-block should prevent memory delta write");
    BANANA_TEST_ASSERT_TRUE(strlen(output.npc_line_candidate) > 0, "hard-block should emit deterministic fallback line");
}

static void test_dialogue_turn_boundary_confidence_maps_to_safe_redirect(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output;

    (void)state;

    fill_turn_input(
        &input,
        "REQUEST_SELF_HARM_INSTRUCTIONS",
        "self_harm",
        "boundary",
        "critical");
    memset(&output, 0, sizeof(output));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "boundary confidence run should succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        output.policy_action_selected,
        BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_SAFE_REDIRECT,
        "boundary confidence hard-block category should map to safe redirect");
    BANANA_TEST_ASSERT_STR_EQ(output.response_policy_label, "safe_redirect", "safe-redirect label should be emitted");
    BANANA_TEST_ASSERT_INT_EQ(output.state_mutation_blocked, 1, "safe-redirect should set state mutation guard");
    BANANA_TEST_ASSERT_INT_EQ(output.memory_write_blocked, 1, "safe-redirect should set memory write guard");
    BANANA_TEST_ASSERT_INT_EQ(output.memory_delta_applied, 0, "safe-redirect should prevent memory delta write");
}

static void test_dialogue_turn_jailbreak_category_maps_to_safe_redirect(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output;

    (void)state;

    fill_turn_input(
        &input,
        "ASK_DIRECTIONS",
        "jailbreak_or_prompt_attack",
        "low",
        "moderate");
    memset(&output, 0, sizeof(output));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "jailbreak category run should succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        output.policy_action_selected,
        BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_SAFE_REDIRECT,
        "jailbreak category should map to safe redirect regardless of confidence");
    BANANA_TEST_ASSERT_STR_EQ(output.response_policy_label, "safe_redirect", "safe-redirect label should be emitted");
    BANANA_TEST_ASSERT_INT_EQ(output.state_mutation_blocked, 1, "safe-redirect should set state mutation guard");
    BANANA_TEST_ASSERT_INT_EQ(output.memory_write_blocked, 1, "safe-redirect should set memory write guard");
}

static void test_dialogue_turn_hard_block_category_low_confidence_allows(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output;

    (void)state;

    fill_turn_input(
        &input,
        "ASK_CASTLE_ENTRY",
        "self_harm",
        "low",
        "critical");
    memset(&output, 0, sizeof(output));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "low confidence hard-block category run should succeed");

    BANANA_TEST_ASSERT_INT_EQ(
        output.policy_action_selected,
        BANANA_NATIVE_V3_K3H4_DIALOGUE_POLICY_ACTION_ALLOW,
        "low confidence hard-block category should map to allow");
    BANANA_TEST_ASSERT_INT_EQ(output.state_mutation_blocked, 0, "allow branch should not block state mutation");
    BANANA_TEST_ASSERT_INT_EQ(output.memory_write_blocked, 0, "allow branch should not block memory write");
    BANANA_TEST_ASSERT_TRUE(
        strcmp(output.response_policy_label, "hard_block") != 0 && strcmp(output.response_policy_label, "safe_redirect") != 0,
        "allow branch should not emit hard-block or safe-redirect label");
}

static void test_dialogue_turn_hard_block_uses_reserved_reason_code_range(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input_a;
    banana_native_v3_k3h4_dialogue_turn_input input_b;
    banana_native_v3_k3h4_dialogue_turn_output output_a;
    banana_native_v3_k3h4_dialogue_turn_output output_b;

    (void)state;

    fill_turn_input(
        &input_a,
        "REQUEST_SELF_HARM_INSTRUCTIONS",
        "self_harm",
        "high",
        "critical");
    fill_turn_input(
        &input_b,
        "ASK_DIRECTIONS",
        "self_harm",
        "high",
        "critical");
    memset(&output_a, 0, sizeof(output_a));
    memset(&output_b, 0, sizeof(output_b));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input_a, &output_a),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "hard-block run A should succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input_b, &output_b),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "hard-block run B should succeed");

    BANANA_TEST_ASSERT_STR_EQ(output_a.response_policy_label, "hard_block", "hard-block label should be emitted");
    BANANA_TEST_ASSERT_TRUE(
        output_a.deny_reason_code >= 9100 && output_a.deny_reason_code <= 9199,
        "hard-block deny reason code should be in reserved 9100-9199 range");
    BANANA_TEST_ASSERT_TRUE(
        output_b.deny_reason_code >= 9100 && output_b.deny_reason_code <= 9199,
        "hard-block deny reason code should be in reserved 9100-9199 range for repeated category");
    BANANA_TEST_ASSERT_INT_EQ(
        output_a.deny_reason_code,
        output_b.deny_reason_code,
        "hard-block deny reason code should be category-stable regardless of intent for same category");
}

static void test_dialogue_turn_safe_redirect_does_not_use_reserved_hard_block_range(void **state)
{
    banana_native_v3_k3h4_dialogue_turn_input input;
    banana_native_v3_k3h4_dialogue_turn_output output;

    (void)state;

    fill_turn_input(
        &input,
        "REQUEST_SELF_HARM_INSTRUCTIONS",
        "self_harm",
        "boundary",
        "critical");
    memset(&output, 0, sizeof(output));

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_v3_k3h4_run_dialogue_turn(&input, &output),
        BANANA_NATIVE_V3_K3H4_DIALOGUE_TURN_OK,
        "safe-redirect boundary run should succeed");

    BANANA_TEST_ASSERT_STR_EQ(output.response_policy_label, "safe_redirect", "safe-redirect label should be emitted");
    BANANA_TEST_ASSERT_TRUE(
        output.deny_reason_code < 9100 || output.deny_reason_code > 9199,
        "safe-redirect deny reason code should not collide with hard-block reserved range");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_dialogue_turn_executes_and_emits_contract),
    BANANA_TEST_CASE(test_dialogue_turn_is_deterministic_for_identical_inputs),
    BANANA_TEST_CASE(test_dialogue_turn_hard_block_sets_mutation_guards),
    BANANA_TEST_CASE(test_dialogue_turn_boundary_confidence_maps_to_safe_redirect),
    BANANA_TEST_CASE(test_dialogue_turn_jailbreak_category_maps_to_safe_redirect),
    BANANA_TEST_CASE(test_dialogue_turn_hard_block_category_low_confidence_allows),
    BANANA_TEST_CASE(test_dialogue_turn_hard_block_uses_reserved_reason_code_range),
    BANANA_TEST_CASE(test_dialogue_turn_safe_redirect_does_not_use_reserved_hard_block_range))
