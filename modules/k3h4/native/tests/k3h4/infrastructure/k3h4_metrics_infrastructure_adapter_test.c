#include "k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.h"
#include "k3h4/k3h4_native_orchestrator.h"
#include "runtime/support/test_support.h"

#include <string.h>

static void assert_same_bytes(const void *left,
                              const void *right,
                              size_t size,
                              const char *message)
{
    BANANA_TEST_ASSERT_TRUE(memcmp(left, right, size) == 0, message);
}

static void test_k3h4_metrics_infrastructure_adapter_maps_facade_functions(void **state)
{
    (void)state;
        BananaNativeK3h4DomainPort port = banana_native_k3h4_infrastructure_create_domain_port();
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
    RuntimeNetcodeLearningOutput adapter_learning_output = {0};
    RuntimeNetcodeRewardOutput direct_reward_output = {0};
    RuntimeNetcodeRewardOutput adapter_reward_output = {0};
    RuntimeNetcodeLinkOutput direct_link_output = {0};
    RuntimeNetcodeLinkOutput adapter_link_output = {0};
    RuntimeNetcodeVectorOutput direct_vector_output = {0};
    RuntimeNetcodeVectorOutput adapter_vector_output = {0};
    RuntimeNetcodeK3h4Output direct_k3h4_output = {0};
    RuntimeNetcodeK3h4Output adapter_k3h4_output = {0};

    BANANA_TEST_ASSERT_TRUE(port.build_learning != NULL, "adapter must provide a learning builder");
    BANANA_TEST_ASSERT_TRUE(port.build_reward != NULL, "adapter must provide a reward builder");
    BANANA_TEST_ASSERT_TRUE(port.build_link != NULL, "adapter must provide a link builder");
    BANANA_TEST_ASSERT_TRUE(port.build_vector != NULL, "adapter must provide a vector builder");
    BANANA_TEST_ASSERT_TRUE(port.build_k3h4 != NULL, "adapter must provide a k3h4 builder");

    BANANA_TEST_ASSERT_TRUE(banana_native_k3h4_build_learning(signal_input, &direct_learning_output) == 0,
                            "direct learning builder must succeed");
    BANANA_TEST_ASSERT_TRUE(port.build_learning(signal_input, &adapter_learning_output) == 0,
                            "adapter learning builder must succeed");
    assert_same_bytes(&direct_learning_output,
                      &adapter_learning_output,
                      sizeof(direct_learning_output),
                      "learning builder must match direct runtime output");

    BANANA_TEST_ASSERT_TRUE(banana_native_k3h4_build_reward(signal_input, 9, &direct_reward_output) == 0,
                            "direct reward builder must succeed");
    BANANA_TEST_ASSERT_TRUE(port.build_reward(signal_input, 9, &adapter_reward_output) == 0,
                            "adapter reward builder must succeed");
    assert_same_bytes(&direct_reward_output,
                      &adapter_reward_output,
                      sizeof(direct_reward_output),
                      "reward builder must match direct runtime output");

    BANANA_TEST_ASSERT_TRUE(banana_native_k3h4_build_link(link_input, &direct_link_output) == 0,
                            "direct link builder must succeed");
    BANANA_TEST_ASSERT_TRUE(port.build_link(link_input, &adapter_link_output) == 0,
                            "adapter link builder must succeed");
    assert_same_bytes(&direct_link_output,
                      &adapter_link_output,
                      sizeof(direct_link_output),
                      "link builder must match direct runtime output");

    BANANA_TEST_ASSERT_TRUE(banana_native_k3h4_build_vector(vector_input, &direct_vector_output) == 0,
                            "direct vector builder must succeed");
    BANANA_TEST_ASSERT_TRUE(port.build_vector(vector_input, &adapter_vector_output) == 0,
                            "adapter vector builder must succeed");
    assert_same_bytes(&direct_vector_output,
                      &adapter_vector_output,
                      sizeof(direct_vector_output),
                      "vector builder must match direct runtime output");

    BANANA_TEST_ASSERT_TRUE(banana_native_k3h4_build_k3h4(vector_input, &direct_k3h4_output) == 0,
                            "direct k3h4 builder must succeed");
    BANANA_TEST_ASSERT_TRUE(port.build_k3h4(vector_input, &adapter_k3h4_output) == 0,
                            "adapter k3h4 builder must succeed");
    assert_same_bytes(&direct_k3h4_output,
                      &adapter_k3h4_output,
                      sizeof(direct_k3h4_output),
                      "k3h4 builder must match direct runtime output");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_k3h4_metrics_infrastructure_adapter_maps_facade_functions)
)