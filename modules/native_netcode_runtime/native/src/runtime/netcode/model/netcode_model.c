#include "netcode_model.h"

#include <math.h>

static int clamp_percent(int value)
{
    if (value < 0)
        return 0;
    if (value > 100)
        return 100;
    return value;
}

static int score_from_weighted_sum(float a,
                                   float b,
                                   float c,
                                   float d,
                                   float e,
                                   float f,
                                   float wa,
                                   float wb,
                                   float wc,
                                   float wd,
                                   float we,
                                   float wf)
{
    float value = a * wa + b * wb + c * wc + d * wd + e * we + f * wf;
    return clamp_percent((int)lroundf(value));
}

static RuntimeNetcodeAction action_from_node(RuntimeNetcodeNode node)
{
    if (node == RUNTIME_NETCODE_NODE_OPS)
        return RUNTIME_NETCODE_ACTION_ROUTE;
    if (node == RUNTIME_NETCODE_NODE_PLAYER)
        return RUNTIME_NETCODE_ACTION_TRAIN;
    if (node == RUNTIME_NETCODE_NODE_OBJECTIVES)
        return RUNTIME_NETCODE_ACTION_INSPECT;
    return RUNTIME_NETCODE_ACTION_SNAPSHOT;
}

int runtime_netcode_model_build(const RuntimeNetcodeLearningInput *input,
                                RuntimeNetcodeLearningOutput *out_output)
{
    int total_actions;
    int action_load;
    int exploration_balance;
    int exploitation_balance;
    int branch_stability;
    int combo_signal;
    int best_index;
    int index;

    if (!input || !out_output)
        return -1;

    total_actions = input->ledger.snapshots + input->ledger.inspections +
                    input->ledger.trainings + input->ledger.routes +
                    input->ledger.node_taps;
    action_load = clamp_percent(total_actions * 6);

    exploration_balance = clamp_percent(input->ledger.snapshots * 20 +
                                        input->ledger.inspections * 18 +
                                        input->ledger.node_taps * 12);
    exploitation_balance = clamp_percent(input->ledger.trainings * 24 +
                                         input->ledger.routes * 22);
    branch_stability = clamp_percent(100 - input->branch_pressure);
    combo_signal = clamp_percent((input->combo_streak < 1 ? 1 : input->combo_streak) * 15);

    out_output->model_confidence = score_from_weighted_sum(
        (float)input->call_density,
        (float)input->quest_percent,
        (float)branch_stability,
        (float)exploration_balance,
        (float)exploitation_balance,
        0.0f,
        0.22f,
        0.22f,
        0.18f,
        0.20f,
        0.18f,
        0.0f);

    out_output->training_accuracy = score_from_weighted_sum(
        (float)out_output->model_confidence,
        (float)exploitation_balance,
        (float)combo_signal,
        0.0f,
        0.0f,
        0.0f,
        0.62f,
        0.24f,
        0.14f,
        0.0f,
        0.0f,
        0.0f);

    out_output->policy_momentum = score_from_weighted_sum(
        (float)action_load,
        (float)out_output->training_accuracy,
        (float)(input->workflow_depth * 8),
        (float)combo_signal,
        0.0f,
        0.0f,
        0.45f,
        0.35f,
        1.0f,
        0.20f,
        0.0f,
        0.0f);

    out_output->node_weights[RUNTIME_NETCODE_NODE_INTEL] =
        clamp_percent((int)lroundf(input->call_density * 0.62f + exploration_balance * 0.38f));
    out_output->node_weights[RUNTIME_NETCODE_NODE_OBJECTIVES] =
        clamp_percent((int)lroundf(input->quest_percent * 0.64f + exploration_balance * 0.36f));
    out_output->node_weights[RUNTIME_NETCODE_NODE_PLAYER] =
        clamp_percent((int)lroundf(combo_signal * 0.40f + out_output->model_confidence * 0.60f));
    out_output->node_weights[RUNTIME_NETCODE_NODE_OPS] =
        clamp_percent((int)lroundf(exploitation_balance * 0.52f + out_output->policy_momentum * 0.48f));

    best_index = RUNTIME_NETCODE_NODE_INTEL;
    for (index = 1; index < RUNTIME_NETCODE_NODE_COUNT; index++)
    {
        if (out_output->node_weights[index] > out_output->node_weights[best_index])
            best_index = index;
    }

    out_output->recommended_node = (RuntimeNetcodeNode)best_index;
    out_output->recommended_action = action_from_node(out_output->recommended_node);

    out_output->xp_by_action[RUNTIME_NETCODE_ACTION_SNAPSHOT] =
        (4 + out_output->model_confidence / 30) < 4 ? 4 : (4 + out_output->model_confidence / 30);
    out_output->xp_by_action[RUNTIME_NETCODE_ACTION_INSPECT] =
        (5 + out_output->training_accuracy / 28) < 5 ? 5 : (5 + out_output->training_accuracy / 28);
    out_output->xp_by_action[RUNTIME_NETCODE_ACTION_TRAIN] =
        (7 + out_output->policy_momentum / 24) < 7 ? 7 : (7 + out_output->policy_momentum / 24);
    out_output->xp_by_action[RUNTIME_NETCODE_ACTION_ROUTE] =
        (8 + out_output->model_confidence / 26 + out_output->policy_momentum / 40) < 8
            ? 8
            : (8 + out_output->model_confidence / 26 + out_output->policy_momentum / 40);

    return 0;
}
