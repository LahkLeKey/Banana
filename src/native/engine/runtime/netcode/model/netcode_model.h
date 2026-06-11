#ifndef BANANA_ENGINE_RUNTIME_NETCODE_MODEL_H
#define BANANA_ENGINE_RUNTIME_NETCODE_MODEL_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum RuntimeNetcodeNode
    {
        RUNTIME_NETCODE_NODE_INTEL = 0,
        RUNTIME_NETCODE_NODE_OBJECTIVES = 1,
        RUNTIME_NETCODE_NODE_PLAYER = 2,
        RUNTIME_NETCODE_NODE_OPS = 3,
        RUNTIME_NETCODE_NODE_COUNT = 4
    } RuntimeNetcodeNode;

    typedef enum RuntimeNetcodeAction
    {
        RUNTIME_NETCODE_ACTION_SNAPSHOT = 0,
        RUNTIME_NETCODE_ACTION_INSPECT = 1,
        RUNTIME_NETCODE_ACTION_TRAIN = 2,
        RUNTIME_NETCODE_ACTION_ROUTE = 3,
        RUNTIME_NETCODE_ACTION_COUNT = 4
    } RuntimeNetcodeAction;

    typedef struct RuntimeNetcodeInteractionLedger
    {
        int snapshots;
        int inspections;
        int trainings;
        int routes;
        int node_taps;
    } RuntimeNetcodeInteractionLedger;

    typedef struct RuntimeNetcodeLearningInput
    {
        RuntimeNetcodeInteractionLedger ledger;
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
    } RuntimeNetcodeLearningInput;

    typedef struct RuntimeNetcodeLearningOutput
    {
        int model_confidence;
        int training_accuracy;
        int policy_momentum;
        int node_weights[RUNTIME_NETCODE_NODE_COUNT];
        RuntimeNetcodeNode recommended_node;
        RuntimeNetcodeAction recommended_action;
        int xp_by_action[RUNTIME_NETCODE_ACTION_COUNT];
    } RuntimeNetcodeLearningOutput;

    int runtime_netcode_model_build(const RuntimeNetcodeLearningInput *input,
                                    RuntimeNetcodeLearningOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
