#ifndef BANANA_ENGINE_RUNTIME_NETCODE_REWARD_H
#define BANANA_ENGINE_RUNTIME_NETCODE_REWARD_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum RuntimeNetcodeRewardTier
    {
        RUNTIME_NETCODE_REWARD_ELITE  = 0,
        RUNTIME_NETCODE_REWARD_RELEVANT = 1,
        RUNTIME_NETCODE_REWARD_NEEDS_LABELING = 2
    } RuntimeNetcodeRewardTier;

    typedef struct RuntimeNetcodeRewardInput
    {
        int call_density;
        int quest_percent;
        int combo_streak;
        int branch_pressure;
        int workflow_depth;
        int interaction_signal;
    } RuntimeNetcodeRewardInput;

    typedef struct RuntimeNetcodeRewardOutput
    {
        int neural_relevance_score;
        int projected_reward_xp;
        RuntimeNetcodeRewardTier reward_tier;
    } RuntimeNetcodeRewardOutput;

    int runtime_netcode_reward_build(const RuntimeNetcodeRewardInput *input,
                                     RuntimeNetcodeRewardOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
