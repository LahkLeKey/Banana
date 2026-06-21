#include "k3h4/k3h4_metrics_orchestration_layer.h"
#include "k3h4/k3h4_native_orchestrator.h"

#include "../runtime/support/test_support.h"

#include <string.h>

static void assert_same_bytes(const void *left,
                              const void *right,
                              size_t size,
                              const char *message)
{
    BANANA_TEST_ASSERT_TRUE(memcmp(left, right, size) == 0, message);
}

static void test_k3h4_layer_builders_match_direct_facade(void **state)
{
    (void)state;
    RuntimeK3h4SignalInput signal_input = {
        .call_density = 42,
        .quest_percent = 64,
        .combo_streak = 7,
        .branch_pressure = 27,
        .workflow_depth = 3,
    };
    RuntimeK3h4LinkSignalInput link_input = {
        .call_density = 42,
        .quest_percent = 64,
        .player_level = 5,
        .combo_streak = 7,
        .branch_pressure = 27,
        .dependency_pulse = 3,
        .interaction_signal = 9,
    };
    RuntimeK3h4VectorSignalInput vector_input = {
        .call_density = 42,
        .quest_percent = 64,
        .player_level = 5,
        .combo_streak = 7,
        .branch_pressure = 27,
        .dependency_pulse = 3,
        .workflow_depth = 3,
        .neural_relevance_score = 18,
        .network_dimensions = 3,
        .model_confidence = 74,
        .policy_momentum = 9,
        .assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
        .spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED,
        .hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG,
        .hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED,
        .hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4,
    };
    RuntimeNetcodeLearningOutput direct_learning_output = {0};
    RuntimeNetcodeLearningOutput layer_learning_output = {0};
    RuntimeNetcodeRewardOutput direct_reward_output = {0};
    RuntimeNetcodeRewardOutput layer_reward_output = {0};
    RuntimeNetcodeLinkOutput direct_link_output = {0};
    RuntimeNetcodeLinkOutput layer_link_output = {0};
    RuntimeNetcodeVectorOutput direct_vector_output = {0};
    RuntimeNetcodeVectorOutput layer_vector_output = {0};
    RuntimeNetcodeK3h4Output direct_k3h4_output = {0};
    RuntimeNetcodeK3h4Output layer_k3h4_output = {0};

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_build_learning(signal_input, &direct_learning_output),
        0,
        "direct learning build must succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_layer_build_learning(signal_input, &layer_learning_output),
        0,
        "layer learning build must succeed");
    assert_same_bytes(&direct_learning_output,
                      &layer_learning_output,
                      sizeof(direct_learning_output),
                      "layer learning output must match direct facade output");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_build_reward(signal_input, 9, &direct_reward_output),
        0,
        "direct reward build must succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_layer_build_reward(signal_input, 9, &layer_reward_output),
        0,
        "layer reward build must succeed");
    assert_same_bytes(&direct_reward_output,
                      &layer_reward_output,
                      sizeof(direct_reward_output),
                      "layer reward output must match direct facade output");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_build_link(link_input, &direct_link_output),
        0,
        "direct link build must succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_layer_build_link(link_input, &layer_link_output),
        0,
        "layer link build must succeed");
    assert_same_bytes(&direct_link_output,
                      &layer_link_output,
                      sizeof(direct_link_output),
                      "layer link output must match direct facade output");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_build_vector(vector_input, &direct_vector_output),
        0,
        "direct vector build must succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_layer_build_vector(vector_input, &layer_vector_output),
        0,
        "layer vector build must succeed");
    assert_same_bytes(&direct_vector_output,
                      &layer_vector_output,
                      sizeof(direct_vector_output),
                      "layer vector output must match direct facade output");

    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_build_k3h4(vector_input, &direct_k3h4_output),
        0,
        "direct k3h4 build must succeed");
    BANANA_TEST_ASSERT_INT_EQ(
        banana_native_k3h4_layer_build_k3h4(vector_input, &layer_k3h4_output),
        0,
        "layer k3h4 build must succeed");
    assert_same_bytes(&direct_k3h4_output,
                      &layer_k3h4_output,
                      sizeof(direct_k3h4_output),
                      "layer k3h4 output must match direct facade output");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_k3h4_layer_builders_match_direct_facade)
)
