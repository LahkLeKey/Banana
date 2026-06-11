#include "netcode_abi.h"

#include "../../netcode/contract/netcode_contract.h"

void runtime_netcode_abi_reset(void)
{
    runtime_netcode_contract_reset();
}

void runtime_netcode_abi_record_node_tap(RuntimeNetcodeNode node)
{
    runtime_netcode_contract_record_node_tap(node);
}

void runtime_netcode_abi_record_action(RuntimeNetcodeAction action)
{
    runtime_netcode_contract_record_action(action);
}

RuntimeNetcodeNode runtime_netcode_abi_get_focus_node(void)
{
    return runtime_netcode_contract_get_focus_node();
}

int runtime_netcode_abi_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger)
{
    return runtime_netcode_contract_get_ledger(out_ledger);
}

int runtime_netcode_abi_build_learning(RuntimeNetcodeSignalInput signal_input,
                                       RuntimeNetcodeLearningOutput *out_output)
{
    RuntimeNetcodeLearningInput input;

    if (!out_output)
        return -1;

    if (runtime_netcode_contract_get_ledger(&input.ledger) != 0)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.workflow_depth = signal_input.workflow_depth;

    return runtime_netcode_model_build(&input, out_output);
}
