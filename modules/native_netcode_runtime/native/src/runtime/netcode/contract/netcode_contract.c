#include "netcode_contract.h"

static RuntimeNetcodeInteractionLedger s_ledger = {0, 0, 0, 0, 0};
static RuntimeNetcodeNode s_focus_node = RUNTIME_NETCODE_NODE_INTEL;

void runtime_netcode_contract_reset(void)
{
    s_ledger.snapshots = 0;
    s_ledger.inspections = 0;
    s_ledger.trainings = 0;
    s_ledger.routes = 0;
    s_ledger.node_taps = 0;
    s_focus_node = RUNTIME_NETCODE_NODE_INTEL;
}

void runtime_netcode_contract_record_node_tap(RuntimeNetcodeNode node)
{
    if (node < RUNTIME_NETCODE_NODE_INTEL || node >= RUNTIME_NETCODE_NODE_COUNT)
        return;

    s_focus_node = node;
    s_ledger.node_taps += 1;
}

void runtime_netcode_contract_record_action(RuntimeNetcodeAction action)
{
    if (action < RUNTIME_NETCODE_ACTION_SNAPSHOT || action >= RUNTIME_NETCODE_ACTION_COUNT)
        return;

    if (action == RUNTIME_NETCODE_ACTION_SNAPSHOT)
        s_ledger.snapshots += 1;
    else if (action == RUNTIME_NETCODE_ACTION_INSPECT)
        s_ledger.inspections += 1;
    else if (action == RUNTIME_NETCODE_ACTION_TRAIN)
        s_ledger.trainings += 1;
    else if (action == RUNTIME_NETCODE_ACTION_ROUTE)
        s_ledger.routes += 1;
}

int runtime_netcode_contract_get_ledger(RuntimeNetcodeInteractionLedger *out_ledger)
{
    if (!out_ledger)
        return -1;

    *out_ledger = s_ledger;
    return 0;
}

RuntimeNetcodeNode runtime_netcode_contract_get_focus_node(void)
{
    return s_focus_node;
}
