#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_ORCHESTRATOR_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_ORCHESTRATOR_H

#include "../link/netcode_link.h"
#include "../model/netcode_model.h"
#include "../reward/netcode_reward.h"
#include "netcode_k3h4_metrics.h"
#include "../vector/netcode_vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeK3h4SignalInput
    {
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
    } RuntimeNetcodeK3h4SignalInput;

    typedef struct RuntimeNetcodeK3h4LinkSignalInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int interaction_signal;
    } RuntimeNetcodeK3h4LinkSignalInput;

    typedef struct RuntimeNetcodeK3h4VectorSignalInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int workflow_depth;
        int neural_relevance_score;
        int network_dimensions;
        int model_confidence;
        int policy_momentum;
    } RuntimeNetcodeK3h4VectorSignalInput;

    typedef struct RuntimeNetcodeK3h4OrchestrationOutput
    {
        RuntimeNetcodeVectorOutput vector;
        RuntimeNetcodeK3h4Output k3h4;
    } RuntimeNetcodeK3h4OrchestrationOutput;

    typedef struct RuntimeNetcodeK3h4Request
    {
        RuntimeNetcodeInteractionLedger ledger;
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int workflow_depth;
        int interaction_signal;
        int neural_relevance_score;
        int network_dimensions;
        int model_confidence;
        int policy_momentum;
    } RuntimeNetcodeK3h4Request;

    typedef struct RuntimeNetcodeK3h4FullOutput
    {
        RuntimeNetcodeLearningOutput learning;
        RuntimeNetcodeRewardOutput reward;
        RuntimeNetcodeLinkOutput link;
        RuntimeNetcodeVectorOutput vector;
        RuntimeNetcodeK3h4Output k3h4;
    } RuntimeNetcodeK3h4FullOutput;

    int runtime_netcode_k3h4_orchestrate_full(const RuntimeNetcodeK3h4Request *request,
                                              RuntimeNetcodeK3h4FullOutput *out_output);

    int runtime_netcode_k3h4_build_learning(const RuntimeNetcodeInteractionLedger *ledger,
                                            const RuntimeNetcodeK3h4SignalInput *input,
                                            RuntimeNetcodeLearningOutput *out_output);

    int runtime_netcode_k3h4_build_reward(const RuntimeNetcodeK3h4SignalInput *input,
                                          int interaction_signal,
                                          RuntimeNetcodeRewardOutput *out_output);

    int runtime_netcode_k3h4_build_link(const RuntimeNetcodeK3h4LinkSignalInput *input,
                                        RuntimeNetcodeLinkOutput *out_output);

    int runtime_netcode_k3h4_build_vector(const RuntimeNetcodeK3h4VectorSignalInput *input,
                                          RuntimeNetcodeVectorOutput *out_output);

    int runtime_netcode_k3h4_orchestrate(const RuntimeNetcodeVectorInput *input,
                                         RuntimeNetcodeK3h4OrchestrationOutput *out_output);

    int runtime_netcode_k3h4_build_k3h4(const RuntimeNetcodeK3h4VectorSignalInput *input,
                                               RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
