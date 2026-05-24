#ifndef BANANA_ENGINE_WIN32_DX12_POC_OBJECTIVE_POLICY_H
#define BANANA_ENGINE_WIN32_DX12_POC_OBJECTIVE_POLICY_H

#include "scene_overlay.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct BananaPocObjectivePolicy
    {
        int base_timeout_seconds;
        int active_timeout_seconds;
        int completion_announced;
    } BananaPocObjectivePolicy;

    void banana_poc_objective_policy_init(BananaPocObjectivePolicy *policy,
                                          int base_timeout_seconds);

    void banana_poc_objective_policy_on_entered_world(BananaPocObjectivePolicy *policy,
                                                       int entered_scene_browser_scene);

    void banana_poc_objective_policy_apply_world_variant(BananaPocObjectivePolicy *policy,
                                                         int world_variant);

    int banana_poc_objective_policy_should_fail_timeout(const BananaPocObjectivePolicy *policy,
                                                        int startup_smoke_mode,
                                                        BananaPocScene scene,
                                                        int objective_collected,
                                                        int elapsed_seconds);

    int banana_poc_objective_policy_on_objective_collected(BananaPocObjectivePolicy *policy,
                                                            BananaPocScene scene,
                                                            int objective_collected);

    int banana_poc_objective_policy_timeout_seconds(const BananaPocObjectivePolicy *policy);

#ifdef __cplusplus
}
#endif

#endif
