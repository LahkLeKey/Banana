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

    /* Shared signal shape for the learning and reward stages. */
    typedef struct RuntimeNetcodeK3h4SignalInput
    {
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
    } RuntimeNetcodeK3h4SignalInput;

    /* Signal shape for the link stage, including interaction metadata. */
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

    /*
     * Signal shape for vector and k3h4 projection builds.
     * Hardware fields are forwarded into envelope validation at the ABI edge.
     */
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
        int assignment_family;
        int spectral_mode;
        int hardware_byte_order_tag;
        int hardware_dtype_tag;
        int hardware_alignment_bytes;
    } RuntimeNetcodeK3h4VectorSignalInput;

    /* Combined vector + k3h4 result used by direct native callers. */
    typedef struct RuntimeNetcodeK3h4OrchestrationOutput
    {
        RuntimeNetcodeVectorOutput vector;
        RuntimeNetcodeK3h4Output k3h4;
    } RuntimeNetcodeK3h4OrchestrationOutput;

    /* End-to-end native request for the authoritative k3h4 pipeline. */
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
        int assignment_family;
        int spectral_mode;
        int hardware_byte_order_tag;
        int hardware_dtype_tag;
        int hardware_alignment_bytes;
    } RuntimeNetcodeK3h4Request;

    /* Full native pipeline output before API shaping trims the payload. */
    typedef struct RuntimeNetcodeK3h4FullOutput
    {
        RuntimeNetcodeLearningOutput learning;
        RuntimeNetcodeRewardOutput reward;
        RuntimeNetcodeLinkOutput link;
        RuntimeNetcodeVectorOutput vector;
        RuntimeNetcodeK3h4Output k3h4;
    } RuntimeNetcodeK3h4FullOutput;

    /* Runs the full native learning -> reward -> link -> vector -> k3h4 flow. */
    int runtime_netcode_k3h4_orchestrate_full(const RuntimeNetcodeK3h4Request *request,
                                              RuntimeNetcodeK3h4FullOutput *out_output);

    /* Rebuilds the learning layer from a ledger snapshot plus scalar signals. */
    int runtime_netcode_k3h4_build_learning(const RuntimeNetcodeInteractionLedger *ledger,
                                            const RuntimeNetcodeK3h4SignalInput *input,
                                            RuntimeNetcodeLearningOutput *out_output);

    /* Rebuilds the reward layer from learning-stage scalar inputs. */
    int runtime_netcode_k3h4_build_reward(const RuntimeNetcodeK3h4SignalInput *input,
                                          int interaction_signal,
                                          RuntimeNetcodeRewardOutput *out_output);

    /* Rebuilds the link layer from graph-related scalar inputs. */
    int runtime_netcode_k3h4_build_link(const RuntimeNetcodeK3h4LinkSignalInput *input,
                                        RuntimeNetcodeLinkOutput *out_output);

    /* Rebuilds the vector layer that seeds the k3h4 projection stages. */
    int runtime_netcode_k3h4_build_vector(const RuntimeNetcodeK3h4VectorSignalInput *input,
                                          RuntimeNetcodeVectorOutput *out_output);

    /*
     * Convenience path: vector build followed by default k3h4 projection
     * config. This hard-codes the same score/radius math as the default
     * pipeline entry point while keeping the wrapper surface minimal.
     */
    int runtime_netcode_k3h4_orchestrate(const RuntimeNetcodeVectorInput *input,
                                         RuntimeNetcodeK3h4OrchestrationOutput *out_output);

    /*
     * Builds only the final k3h4 projection layer from vector-stage inputs.
     * The caller still controls assignment-family and spectral mode, but the
     * upstream vector contract is always rebuilt first to preserve Q16 parity.
     */
    int runtime_netcode_k3h4_build_k3h4(const RuntimeNetcodeK3h4VectorSignalInput *input,
                                               RuntimeNetcodeK3h4Output *out_output);

#ifdef __cplusplus
}
#endif

#endif
