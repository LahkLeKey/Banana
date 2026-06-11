#ifndef BANANA_ENGINE_RUNTIME_NETCODE_ABI_H
#define BANANA_ENGINE_RUNTIME_NETCODE_ABI_H

#include "../../netcode/model/netcode_model.h"
#include "../../netcode/reward/netcode_reward.h"
#include "../../netcode/link/netcode_link.h"
#include "../../netcode/vector/netcode_vector.h"
#include "../../netcode/hypersphere/netcode_hypersphere.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeSignalInput
    {
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
    } RuntimeNetcodeSignalInput;

    typedef struct RuntimeNetcodeLinkSignalInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int interaction_signal;
    } RuntimeNetcodeLinkSignalInput;

    typedef struct RuntimeNetcodeVectorSignalInput
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
    } RuntimeNetcodeVectorSignalInput;

    void runtime_netcode_abi_reset(void);

    void runtime_netcode_abi_record_node_tap(RuntimeNetcodeNode node);

    void runtime_netcode_abi_record_action(RuntimeNetcodeAction action);

    RuntimeNetcodeNode runtime_netcode_abi_get_focus_node(void);

    int runtime_netcode_abi_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger);

    int runtime_netcode_abi_build_learning(RuntimeNetcodeSignalInput signal_input,
                                           RuntimeNetcodeLearningOutput *out_output);

    int runtime_netcode_abi_build_reward(RuntimeNetcodeSignalInput signal_input,
                                         int interaction_signal,
                                         RuntimeNetcodeRewardOutput *out_output);

    int runtime_netcode_abi_build_link(RuntimeNetcodeLinkSignalInput signal_input,
                                       RuntimeNetcodeLinkOutput *out_output);

    int runtime_netcode_abi_build_vector(RuntimeNetcodeVectorSignalInput signal_input,
                                         RuntimeNetcodeVectorOutput *out_output);

    int runtime_netcode_abi_build_hypersphere(RuntimeNetcodeVectorSignalInput signal_input,
                                              RuntimeNetcodeHypersphereOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
