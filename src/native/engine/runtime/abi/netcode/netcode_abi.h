#ifndef BANANA_ENGINE_RUNTIME_NETCODE_ABI_H
#define BANANA_ENGINE_RUNTIME_NETCODE_ABI_H

#include "../../netcode/model/netcode_model.h"

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

    void runtime_netcode_abi_reset(void);

    void runtime_netcode_abi_record_node_tap(RuntimeNetcodeNode node);

    void runtime_netcode_abi_record_action(RuntimeNetcodeAction action);

    RuntimeNetcodeNode runtime_netcode_abi_get_focus_node(void);

    int runtime_netcode_abi_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger);

    int runtime_netcode_abi_build_learning(RuntimeNetcodeSignalInput signal_input,
                                           RuntimeNetcodeLearningOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
