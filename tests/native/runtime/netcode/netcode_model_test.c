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
    RuntimeNetcodeInteractionLedger ledger;
    RuntimeNetcodeSignalInput input = {
        .call_density = 54,
        .quest_percent = 62,
        .combo_streak = 3,
        .branch_pressure = 28,
        .workflow_depth = 2,
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

    return 0;
}
