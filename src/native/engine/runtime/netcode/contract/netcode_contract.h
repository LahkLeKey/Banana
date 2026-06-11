#ifndef BANANA_ENGINE_RUNTIME_NETCODE_CONTRACT_H
#define BANANA_ENGINE_RUNTIME_NETCODE_CONTRACT_H

#include "../model/netcode_model.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_netcode_contract_reset(void);

    void runtime_netcode_contract_record_node_tap(RuntimeNetcodeNode node);

    void runtime_netcode_contract_record_action(RuntimeNetcodeAction action);

    int runtime_netcode_contract_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger);

    RuntimeNetcodeNode runtime_netcode_contract_get_focus_node(void);

#ifdef __cplusplus
}
#endif

#endif
