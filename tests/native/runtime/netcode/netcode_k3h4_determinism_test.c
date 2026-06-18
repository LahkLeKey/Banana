#include "runtime/abi/netcode/netcode_abi.h"
#include "../support/test_support.h"
#include <string.h>

static void test_netcode_k3h4_determinism(void **state)
{
    (void)state;
    RuntimeK3h4VectorSignalInput input = {
        .call_density = 48,
        .quest_percent = 55,
        .player_level = 6,
        .combo_streak = 3,
        .branch_pressure = 22,
        .dependency_pulse = 37,
        .workflow_depth = 2,
        .neural_relevance_score = 61,
        .network_dimensions = 10,
        .model_confidence = 72,
        .policy_momentum = 64,
        .assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
        .spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED,
        .hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG,
        .hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED,
        .hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4,
    };
    RuntimeNetcodeK3h4Output first_output;
    RuntimeNetcodeK3h4Output second_output;

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_k3h4(input, &first_output), 0,
                              "failed to build first k3h4 output");
    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_k3h4(input, &second_output), 0,
                              "failed to build second k3h4 output");
    BANANA_TEST_ASSERT_INT_EQ(memcmp(&first_output, &second_output, sizeof(RuntimeNetcodeK3h4Output)), 0,
                              "repeated deterministic output mismatch");
    BANANA_TEST_ASSERT_INT_EQ(first_output.vector_count, 4, "vector count mismatch");
    BANANA_TEST_ASSERT_TRUE(first_output.observability.deterministic_hash != 0,
                            "deterministic hash should be non-zero");
    BANANA_TEST_ASSERT_TRUE(first_output.observability.iteration_count > 0,
                            "iteration count should be positive");
    BANANA_TEST_ASSERT_INT_EQ(first_output.weighted_voronoi_scores[0].score_validity,
                              RUNTIME_NETCODE_SCORE_VALID,
                              "expected first weighted score to be valid");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_netcode_k3h4_determinism)
)
