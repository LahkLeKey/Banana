#include "runtime/abi/netcode/netcode_abi.h"
#include "runtime/netcode/model/netcode_model.h"

#include <stdio.h>

static int assert_true(int condition, const char *message)
{
    if (!condition)
    {
        fprintf(stderr, "[netcode-test] %s\n", message);
        return 1;
    }
    return 0;
}

int main(void)
{
    RuntimeNetcodeLearningOutput output;
    RuntimeNetcodeRewardOutput reward_output;
    RuntimeNetcodeLinkOutput link_output;
    RuntimeNetcodeVectorOutput vector_output;
    RuntimeNetcodeHypersphereOutput hypersphere_output;
    RuntimeNetcodeInteractionLedger ledger;
    RuntimeNetcodeSignalInput input = {
        .call_density = 54,
        .quest_percent = 62,
        .combo_streak = 3,
        .branch_pressure = 28,
        .workflow_depth = 2,
    };
    RuntimeNetcodeLinkSignalInput link_input = {
        .call_density = 54,
        .quest_percent = 62,
        .player_level = 4,
        .combo_streak = 3,
        .branch_pressure = 28,
        .dependency_pulse = 36,
        .interaction_signal = 41,
    };
    RuntimeNetcodeVectorSignalInput vector_input = {
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
    };

    runtime_netcode_abi_reset();
    runtime_netcode_abi_record_node_tap(RUNTIME_NETCODE_NODE_INTEL);
    runtime_netcode_abi_record_node_tap(RUNTIME_NETCODE_NODE_OBJECTIVES);
    runtime_netcode_abi_record_action(RUNTIME_NETCODE_ACTION_SNAPSHOT);
    runtime_netcode_abi_record_action(RUNTIME_NETCODE_ACTION_INSPECT);
    runtime_netcode_abi_record_action(RUNTIME_NETCODE_ACTION_TRAIN);

    if (runtime_netcode_abi_get_ledger(&ledger) != 0)
        return assert_true(0, "failed to fetch netcode ledger");

    if (assert_true(ledger.snapshots == 1, "snapshot counter mismatch") != 0)
        return 1;
    if (assert_true(ledger.inspections == 1, "inspect counter mismatch") != 0)
        return 1;
    if (assert_true(ledger.trainings == 1, "train counter mismatch") != 0)
        return 1;
    if (assert_true(ledger.routes == 0, "route counter mismatch") != 0)
        return 1;
    if (assert_true(ledger.node_taps == 2, "node tap counter mismatch") != 0)
        return 1;

    if (runtime_netcode_abi_build_learning(input, &output) != 0)
        return assert_true(0, "failed to build netcode learning output");

    if (assert_true(output.model_confidence >= 0 && output.model_confidence <= 100,
                    "model confidence out of range") != 0)
        return 1;
    if (assert_true(output.training_accuracy >= 0 && output.training_accuracy <= 100,
                    "training accuracy out of range") != 0)
        return 1;
    if (assert_true(output.policy_momentum >= 0 && output.policy_momentum <= 100,
                    "policy momentum out of range") != 0)
        return 1;

    if (assert_true(output.recommended_node >= RUNTIME_NETCODE_NODE_INTEL &&
                        output.recommended_node < RUNTIME_NETCODE_NODE_COUNT,
                    "recommended node out of range") != 0)
        return 1;

    if (assert_true(output.recommended_action >= RUNTIME_NETCODE_ACTION_SNAPSHOT &&
                        output.recommended_action < RUNTIME_NETCODE_ACTION_COUNT,
                    "recommended action out of range") != 0)
        return 1;

    if (assert_true(output.xp_by_action[RUNTIME_NETCODE_ACTION_SNAPSHOT] > 0,
                    "snapshot xp should be positive") != 0)
        return 1;
    if (assert_true(output.xp_by_action[RUNTIME_NETCODE_ACTION_ROUTE] > 0,
                    "route xp should be positive") != 0)
        return 1;

    if (runtime_netcode_abi_build_reward(input, 41, &reward_output) != 0)
        return assert_true(0, "failed to build reward output");

    if (assert_true(reward_output.neural_relevance_score >= 0 &&
                        reward_output.neural_relevance_score <= 100,
                    "reward neural relevance out of range") != 0)
        return 1;
    if (assert_true(reward_output.projected_reward_xp >= 5,
                    "projected reward xp should be >= 5") != 0)
        return 1;

    if (runtime_netcode_abi_build_link(link_input, &link_output) != 0)
        return assert_true(0, "failed to build link output");

    if (assert_true(link_output.intel >= 0 && link_output.intel <= 100,
                    "link intel out of range") != 0)
        return 1;
    if (assert_true(link_output.objectives >= 0 &&
                        link_output.objectives <= 100,
                    "link objectives out of range") != 0)
        return 1;
    if (assert_true(link_output.player >= 0 && link_output.player <= 100,
                    "link player out of range") != 0)
        return 1;
    if (assert_true(link_output.ops >= 0 && link_output.ops <= 100,
                    "link ops out of range") != 0)
        return 1;

    if (runtime_netcode_abi_build_vector(vector_input, &vector_output) != 0)
        return assert_true(0, "failed to build vector output");

    if (assert_true(vector_output.dimensions == 10,
                    "vector dimensions mismatch") != 0)
        return 1;
    if (assert_true(vector_output.contract_strength[0] >= 0 &&
                        vector_output.contract_strength[0] <= 100,
                    "vector contract strength out of range") != 0)
        return 1;

    if (runtime_netcode_abi_build_hypersphere(vector_input, &hypersphere_output) != 0)
        return assert_true(0, "failed to build hypersphere output");

    if (assert_true(hypersphere_output.dimensions == 10,
                    "hypersphere dimensions mismatch") != 0)
        return 1;
    if (assert_true(hypersphere_output.alignment >= 0 &&
                        hypersphere_output.alignment <= 100,
                    "hypersphere alignment out of range") != 0)
        return 1;
    if (assert_true(hypersphere_output.radial_stability >= 0 &&
                        hypersphere_output.radial_stability <= 100,
                    "hypersphere radial stability out of range") != 0)
        return 1;

    if (assert_true(hypersphere_output.nodes[0].inradius >= 0.0f,
                    "hypersphere inradius should be non-negative") != 0)
        return 1;
    if (assert_true(hypersphere_output.nodes[0].nearest_neighbor_distance >= 0.0f,
                    "hypersphere nearest-neighbor distance should be non-negative") != 0)
        return 1;
    if (assert_true(hypersphere_output.nodes[0].nearest_neighbor_distance >=
                        hypersphere_output.nodes[0].inradius,
                    "hypersphere inradius should be bounded by nearest-neighbor distance") != 0)
        return 1;

    return 0;
}
