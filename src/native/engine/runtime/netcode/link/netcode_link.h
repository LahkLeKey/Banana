#ifndef BANANA_ENGINE_RUNTIME_NETCODE_LINK_H
#define BANANA_ENGINE_RUNTIME_NETCODE_LINK_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeNetcodeLinkInput
    {
        int call_density;
        int quest_percent;
        int player_level;
        int combo_streak;
        int branch_pressure;
        int dependency_pulse;
        int interaction_signal;
    } RuntimeNetcodeLinkInput;

    typedef struct RuntimeNetcodeLinkOutput
    {
        int intel;
        int objectives;
        int player;
        int ops;
    } RuntimeNetcodeLinkOutput;

    int runtime_netcode_link_build(const RuntimeNetcodeLinkInput *input,
                                   RuntimeNetcodeLinkOutput *out_output);

#ifdef __cplusplus
}
#endif

#endif
