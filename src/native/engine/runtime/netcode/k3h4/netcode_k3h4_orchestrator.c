#include "netcode_k3h4_orchestrator.h"

#include "netcode_k3h4_metrics.h"

#include <string.h>

/*
 * This file is the native bridge between scalar request surfaces and the
 * lower-level learning/reward/link/vector/k3h4 builders. It does three things:
 *   1. reshape a combined request into stage-specific input structs,
 *   2. run the stages in canonical order, and
 *   3. keep default-vs-explicit k3h4 configuration choices centralized.
 */
int runtime_netcode_k3h4_orchestrate_full(const RuntimeNetcodeK3h4Request *request,
                                          RuntimeNetcodeK3h4FullOutput *out_output)
{
    RuntimeNetcodeK3h4SignalInput signal_input;
    RuntimeNetcodeK3h4LinkSignalInput link_input;
    RuntimeNetcodeK3h4VectorSignalInput vector_input;

    if (!request || !out_output)
        return -1;

    /* The orchestration output is fully zeroed first so partial failures never
     * leak stale data from previous stack contents.
     */
    memset(out_output, 0, sizeof(*out_output));

    /* Reshape the aggregate request into the narrower structs expected by each
     * stage-specific builder.
     */
    signal_input.call_density = request->call_density;
    signal_input.quest_percent = request->quest_percent;
    signal_input.combo_streak = request->combo_streak;
    signal_input.branch_pressure = request->branch_pressure;
    signal_input.workflow_depth = request->workflow_depth;

    link_input.call_density = request->call_density;
    link_input.quest_percent = request->quest_percent;
    link_input.player_level = request->player_level;
    link_input.combo_streak = request->combo_streak;
    link_input.branch_pressure = request->branch_pressure;
    link_input.dependency_pulse = request->dependency_pulse;
    link_input.interaction_signal = request->interaction_signal;

    vector_input.call_density = request->call_density;
    vector_input.quest_percent = request->quest_percent;
    vector_input.player_level = request->player_level;
    vector_input.combo_streak = request->combo_streak;
    vector_input.branch_pressure = request->branch_pressure;
    vector_input.dependency_pulse = request->dependency_pulse;
    vector_input.workflow_depth = request->workflow_depth;
    vector_input.neural_relevance_score = request->neural_relevance_score;
    vector_input.network_dimensions = request->network_dimensions;
    vector_input.model_confidence = request->model_confidence;
    vector_input.policy_momentum = request->policy_momentum;
    vector_input.assignment_family = request->assignment_family;
    vector_input.spectral_mode = request->spectral_mode;
    vector_input.hardware_byte_order_tag = request->hardware_byte_order_tag;
    vector_input.hardware_dtype_tag = request->hardware_dtype_tag;
    vector_input.hardware_alignment_bytes = request->hardware_alignment_bytes;

    /* Run the layers in the same order exposed to higher API consumers. */
    if (runtime_netcode_k3h4_build_learning(&request->ledger, &signal_input, &out_output->learning) != 0)
        return -1;

    if (runtime_netcode_k3h4_build_reward(&signal_input, request->interaction_signal, &out_output->reward) != 0)
        return -1;

    if (runtime_netcode_k3h4_build_link(&link_input, &out_output->link) != 0)
        return -1;

    if (runtime_netcode_k3h4_build_vector(&vector_input, &out_output->vector) != 0)
        return -1;

    if (runtime_netcode_k3h4_build_with_config(
            &out_output->vector,
            &out_output->k3h4,
            vector_input.assignment_family,
            vector_input.spectral_mode) != 0)
        return -1;

    return 0;
}

int runtime_netcode_k3h4_build_learning(const RuntimeNetcodeInteractionLedger *ledger,
                                        const RuntimeNetcodeK3h4SignalInput *input,
                                        RuntimeNetcodeLearningOutput *out_output)
{
    RuntimeNetcodeLearningInput learning_input;

    /* Learning is the only stage that consumes the interaction ledger. */
    if (!ledger || !input || !out_output)
        return -1;

    memset(&learning_input, 0, sizeof(learning_input));
    learning_input.ledger = *ledger;
    learning_input.call_density = input->call_density;
    learning_input.quest_percent = input->quest_percent;
    learning_input.combo_streak = input->combo_streak;
    learning_input.branch_pressure = input->branch_pressure;
    learning_input.workflow_depth = input->workflow_depth;

    return runtime_netcode_model_build(&learning_input, out_output);
}

int runtime_netcode_k3h4_build_reward(const RuntimeNetcodeK3h4SignalInput *input,
                                      int interaction_signal,
                                      RuntimeNetcodeRewardOutput *out_output)
{
    RuntimeNetcodeRewardInput reward_input;

    /* Reward derives only from scalar signals plus the chosen interaction term. */
    if (!input || !out_output)
        return -1;

    memset(&reward_input, 0, sizeof(reward_input));
    reward_input.call_density = input->call_density;
    reward_input.quest_percent = input->quest_percent;
    reward_input.combo_streak = input->combo_streak;
    reward_input.branch_pressure = input->branch_pressure;
    reward_input.workflow_depth = input->workflow_depth;
    reward_input.interaction_signal = interaction_signal;

    return runtime_netcode_reward_build(&reward_input, out_output);
}

int runtime_netcode_k3h4_build_link(const RuntimeNetcodeK3h4LinkSignalInput *input,
                                    RuntimeNetcodeLinkOutput *out_output)
{
    RuntimeNetcodeLinkInput link_input;

    /* Link computation repacks the caller-facing struct into the core link ABI. */
    if (!input || !out_output)
        return -1;

    memset(&link_input, 0, sizeof(link_input));
    link_input.call_density = input->call_density;
    link_input.quest_percent = input->quest_percent;
    link_input.player_level = input->player_level;
    link_input.combo_streak = input->combo_streak;
    link_input.branch_pressure = input->branch_pressure;
    link_input.dependency_pulse = input->dependency_pulse;
    link_input.interaction_signal = input->interaction_signal;

    return runtime_netcode_link_build(&link_input, out_output);
}

int runtime_netcode_k3h4_build_vector(const RuntimeNetcodeK3h4VectorSignalInput *input,
                                      RuntimeNetcodeVectorOutput *out_output)
{
    RuntimeNetcodeVectorInput vector_input;

    /* The vector builder consumes the pre-k3h4 scalar contract only; assignment
     * family and spectral mode are applied later by the k3h4 projection stage.
     */
    if (!input || !out_output)
        return -1;

    memset(&vector_input, 0, sizeof(vector_input));
    vector_input.call_density = input->call_density;
    vector_input.quest_percent = input->quest_percent;
    vector_input.player_level = input->player_level;
    vector_input.combo_streak = input->combo_streak;
    vector_input.branch_pressure = input->branch_pressure;
    vector_input.dependency_pulse = input->dependency_pulse;
    vector_input.workflow_depth = input->workflow_depth;
    vector_input.neural_relevance_score = input->neural_relevance_score;
    vector_input.network_dimensions = input->network_dimensions;
    vector_input.model_confidence = input->model_confidence;
    vector_input.policy_momentum = input->policy_momentum;

    return runtime_netcode_vector_build(&vector_input, out_output);
}

int runtime_netcode_k3h4_orchestrate(const RuntimeNetcodeVectorInput *input,
                                     RuntimeNetcodeK3h4OrchestrationOutput *out_output)
{
    /* This convenience path hard-codes the default policy:
     *   assignment_family = multiplicative
     *   spectral_mode     = disabled
     */
    if (!input || !out_output)
        return -1;

    memset(out_output, 0, sizeof(*out_output));

    if (runtime_netcode_vector_build(input, &out_output->vector) != 0)
        return -1;

    if (runtime_netcode_k3h4_build_with_config(
            &out_output->vector,
            &out_output->k3h4,
            RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE,
            RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED) != 0)
        return -1;

    return 0;
}

int runtime_netcode_k3h4_build_k3h4(const RuntimeNetcodeK3h4VectorSignalInput *input,
                                           RuntimeNetcodeK3h4Output *out_output)
{
    RuntimeNetcodeVectorInput vector_input;
    RuntimeNetcodeK3h4OrchestrationOutput orchestration_output;

    /* Build the vector contract first, then feed it into the explicit k3h4
     * projection path using the caller-provided assignment and spectral modes.
     */
    if (!input || !out_output)
        return -1;

    memset(&vector_input, 0, sizeof(vector_input));
    vector_input.call_density = input->call_density;
    vector_input.quest_percent = input->quest_percent;
    vector_input.player_level = input->player_level;
    vector_input.combo_streak = input->combo_streak;
    vector_input.branch_pressure = input->branch_pressure;
    vector_input.dependency_pulse = input->dependency_pulse;
    vector_input.workflow_depth = input->workflow_depth;
    vector_input.neural_relevance_score = input->neural_relevance_score;
    vector_input.network_dimensions = input->network_dimensions;
    vector_input.model_confidence = input->model_confidence;
    vector_input.policy_momentum = input->policy_momentum;

    if (runtime_netcode_vector_build(&vector_input, &orchestration_output.vector) != 0)
        return -1;

    if (runtime_netcode_k3h4_build_with_config(
            &orchestration_output.vector,
            &orchestration_output.k3h4,
            input->assignment_family,
            input->spectral_mode) != 0)
        return -1;

    /* Only the final k3h4 payload is surfaced back to the caller here. */
    *out_output = orchestration_output.k3h4;
    return 0;
}
