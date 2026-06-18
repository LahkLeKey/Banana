#include "runtime/abi/netcode/netcode_abi.h"
#include "runtime/netcode/model/netcode_model.h"
#include "../support/test_support.h"

static void test_netcode_model(void **state)
{
    (void)state;
    RuntimeNetcodeLearningOutput output;
    RuntimeNetcodeRewardOutput reward_output;
    RuntimeNetcodeLinkOutput link_output;
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeK3h4Output k3h4_output;
    RuntimeNetcodeInteractionLedger ledger;
    RuntimeK3h4SignalInput input = {
        .call_density = 54,
        .quest_percent = 62,
        .combo_streak = 3,
        .branch_pressure = 28,
        .workflow_depth = 2,
    };
    RuntimeK3h4LinkSignalInput link_input = {
        .call_density = 54,
        .quest_percent = 62,
        .player_level = 4,
        .combo_streak = 3,
        .branch_pressure = 28,
        .dependency_pulse = 36,
        .interaction_signal = 41,
    };
    RuntimeK3h4VectorSignalInput vector_input = {
        .call_density = 54,
        .quest_percent = 62,
        .player_level = 4,
        .combo_streak = 3,
        .branch_pressure = 28,
        .dependency_pulse = 36,
        .workflow_depth = 2,
        .neural_relevance_score = 58,
        .network_dimensions = 10,
        .model_confidence = 46,
        .policy_momentum = 52,
        .assignment_family = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
        .spectral_mode = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED,
        .hardware_byte_order_tag = RUNTIME_K3H4_BYTE_ORDER_TAG,
        .hardware_dtype_tag = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED,
        .hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4,
    };

    runtime_k3h4_abi_reset();
    runtime_k3h4_abi_record_node_tap(RUNTIME_NETCODE_NODE_INTEL);
    runtime_k3h4_abi_record_node_tap(RUNTIME_NETCODE_NODE_OBJECTIVES);
    runtime_k3h4_abi_record_action(RUNTIME_NETCODE_ACTION_SNAPSHOT);
    runtime_k3h4_abi_record_action(RUNTIME_NETCODE_ACTION_INSPECT);
    runtime_k3h4_abi_record_action(RUNTIME_NETCODE_ACTION_TRAIN);

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_get_ledger(&ledger), 0,
                              "failed to fetch netcode ledger");
    BANANA_TEST_ASSERT_INT_EQ(ledger.snapshots, 1, "snapshot counter mismatch");
    BANANA_TEST_ASSERT_INT_EQ(ledger.inspections, 1, "inspect counter mismatch");
    BANANA_TEST_ASSERT_INT_EQ(ledger.trainings, 1, "train counter mismatch");
    BANANA_TEST_ASSERT_INT_EQ(ledger.routes, 0, "route counter mismatch");
    BANANA_TEST_ASSERT_INT_EQ(ledger.node_taps, 2, "node tap counter mismatch");

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_learning(input, &output), 0,
                              "failed to build netcode learning output");
    BANANA_TEST_ASSERT_TRUE(output.model_confidence >= 0 && output.model_confidence <= 100,
                            "model confidence out of range");
    BANANA_TEST_ASSERT_TRUE(output.training_accuracy >= 0 && output.training_accuracy <= 100,
                            "training accuracy out of range");
    BANANA_TEST_ASSERT_TRUE(output.policy_momentum >= 0 && output.policy_momentum <= 100,
                            "policy momentum out of range");
    BANANA_TEST_ASSERT_TRUE(output.recommended_node >= RUNTIME_NETCODE_NODE_INTEL &&
                            output.recommended_node < RUNTIME_NETCODE_NODE_COUNT,
                            "recommended node out of range");
    BANANA_TEST_ASSERT_TRUE(output.recommended_action >= RUNTIME_NETCODE_ACTION_SNAPSHOT &&
                            output.recommended_action < RUNTIME_NETCODE_ACTION_COUNT,
                            "recommended action out of range");
    BANANA_TEST_ASSERT_TRUE(output.xp_by_action[RUNTIME_NETCODE_ACTION_SNAPSHOT] > 0,
                            "snapshot xp should be positive");
    BANANA_TEST_ASSERT_TRUE(output.xp_by_action[RUNTIME_NETCODE_ACTION_ROUTE] > 0,
                            "route xp should be positive");

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_reward(input, 41, &reward_output), 0,
                              "failed to build reward output");
    BANANA_TEST_ASSERT_TRUE(reward_output.neural_relevance_score >= 0 &&
                            reward_output.neural_relevance_score <= 100,
                            "reward neural relevance out of range");
    BANANA_TEST_ASSERT_TRUE(reward_output.projected_reward_xp >= 5,
                            "projected reward xp should be >= 5");

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_link(link_input, &link_output), 0,
                              "failed to build link output");
    BANANA_TEST_ASSERT_TRUE(link_output.intel >= 0 && link_output.intel <= 100,
                            "link intel out of range");
    BANANA_TEST_ASSERT_TRUE(link_output.objectives >= 0 && link_output.objectives <= 100,
                            "link objectives out of range");
    BANANA_TEST_ASSERT_TRUE(link_output.player >= 0 && link_output.player <= 100,
                            "link player out of range");
    BANANA_TEST_ASSERT_TRUE(link_output.ops >= 0 && link_output.ops <= 100,
                            "link ops out of range");

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_vector(vector_input, &vector_output), 0,
                              "failed to build vector output");
    BANANA_TEST_ASSERT_INT_EQ(vector_output.dimensions, 10, "vector dimensions mismatch");
    BANANA_TEST_ASSERT_TRUE(vector_output.contract_strength[0] >= 0 &&
                            vector_output.contract_strength[0] <= 100,
                            "vector contract strength out of range");

    BANANA_TEST_ASSERT_INT_EQ(runtime_k3h4_abi_build_k3h4(vector_input, &k3h4_output), 0,
                              "failed to build k3h4 output");
    BANANA_TEST_ASSERT_INT_EQ(k3h4_output.dimensions, 10, "k3h4 dimensions mismatch");
    BANANA_TEST_ASSERT_TRUE(k3h4_output.alignment >= 0 && k3h4_output.alignment <= 100,
                            "k3h4 alignment out of range");
    BANANA_TEST_ASSERT_TRUE(k3h4_output.radial_stability >= 0 &&
                            k3h4_output.radial_stability <= 100,
                            "k3h4 radial stability out of range");
    BANANA_TEST_ASSERT_TRUE(k3h4_output.nodes[0].inradius >= 0.0f,
                            "k3h4 inradius should be non-negative");
    BANANA_TEST_ASSERT_TRUE(k3h4_output.nodes[0].nearest_neighbor_distance >= 0.0f,
                            "k3h4 nearest-neighbor distance should be non-negative");
    BANANA_TEST_ASSERT_TRUE(k3h4_output.nodes[0].nearest_neighbor_distance >=
                            k3h4_output.nodes[0].inradius,
                            "k3h4 inradius should be bounded by nearest-neighbor distance");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_netcode_model)
)
