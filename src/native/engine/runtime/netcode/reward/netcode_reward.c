#include "netcode_reward.h"

#include <math.h>

static int clamp_percent(int value)
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

int runtime_netcode_reward_build(const RuntimeNetcodeRewardInput *input,
                                 RuntimeNetcodeRewardOutput *out_output)
{
    int combo_signal;
    int stability_signal;
    int interaction_signal;
    float neural;

    if (!input || !out_output)
        return -1;

    combo_signal = clamp_percent(
        (input->combo_streak < 1 ? 1 : input->combo_streak) * 18);
    stability_signal = clamp_percent(100 - input->branch_pressure);
    interaction_signal = clamp_percent(input->interaction_signal);

    neural = (float)input->call_density * 0.36f
           + (float)input->quest_percent * 0.26f
           + (float)combo_signal         * 0.20f
           + (float)stability_signal     * 0.14f
           + (float)interaction_signal   * 0.04f;

    out_output->neural_relevance_score = clamp_percent((int)lroundf(neural));

    out_output->projected_reward_xp =
        out_output->neural_relevance_score / 4 + input->workflow_depth * 3;
    if (out_output->projected_reward_xp < 5)
        out_output->projected_reward_xp = 5;

    if (out_output->neural_relevance_score >= 80)
        out_output->reward_tier = RUNTIME_NETCODE_REWARD_ELITE;
    else if (out_output->neural_relevance_score >= 55)
        out_output->reward_tier = RUNTIME_NETCODE_REWARD_RELEVANT;
    else
        out_output->reward_tier = RUNTIME_NETCODE_REWARD_NEEDS_LABELING;

    return 0;
}
