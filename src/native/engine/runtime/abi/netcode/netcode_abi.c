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

int runtime_netcode_abi_build_reward(RuntimeNetcodeSignalInput signal_input,
                                     int interaction_signal,
                                     RuntimeNetcodeRewardOutput *out_output)
{
    RuntimeNetcodeRewardInput input;

    if (!out_output)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.workflow_depth = signal_input.workflow_depth;
    input.interaction_signal = interaction_signal;

    return runtime_netcode_reward_build(&input, out_output);
}

int runtime_netcode_abi_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                   RuntimeNetcodeLinkOutput *out_output)
{
    RuntimeNetcodeLinkInput input;

    if (!out_output)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.player_level = signal_input.player_level;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.dependency_pulse = signal_input.dependency_pulse;
    input.interaction_signal = signal_input.interaction_signal;

    return runtime_netcode_link_build(&input, out_output);
}

int runtime_netcode_abi_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                     RuntimeNetcodeVectorOutput *out_output)
{
    RuntimeNetcodeVectorInput input;

    if (!out_output)
        return -1;

    input.call_density = signal_input.call_density;
    input.quest_percent = signal_input.quest_percent;
    input.player_level = signal_input.player_level;
    input.combo_streak = signal_input.combo_streak;
    input.branch_pressure = signal_input.branch_pressure;
    input.dependency_pulse = signal_input.dependency_pulse;
    input.workflow_depth = signal_input.workflow_depth;
    input.neural_relevance_score = signal_input.neural_relevance_score;
    input.network_dimensions = signal_input.network_dimensions;
    input.model_confidence = signal_input.model_confidence;
    input.policy_momentum = signal_input.policy_momentum;

    return runtime_netcode_vector_build(&input, out_output);
}

int runtime_netcode_abi_build_hypersphere(RuntimeNetcodeVectorSignalInput signal_input,
                                          RuntimeNetcodeHypersphereOutput *out_output)
{
    RuntimeNetcodeVectorOutput vector_output;

    if (!out_output)
        return -1;

    if (runtime_netcode_abi_build_vector(signal_input, &vector_output) != 0)
        return -1;

    return runtime_netcode_hypersphere_build(&vector_output, out_output);
}
