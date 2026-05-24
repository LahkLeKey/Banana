#include "../objective_policy.h"

void banana_poc_objective_policy_init(BananaPocObjectivePolicy *policy,
                                      int base_timeout_seconds)
{
    if (!policy)
        return;

    if (base_timeout_seconds < 0)
        base_timeout_seconds = 0;

    policy->base_timeout_seconds = base_timeout_seconds;
    policy->active_timeout_seconds = base_timeout_seconds;
    policy->completion_announced = 0;
}

void banana_poc_objective_policy_on_entered_world(BananaPocObjectivePolicy *policy,
                                                   int entered_scene_browser_scene)
{
    if (!policy)
        return;

    policy->completion_announced = 0;

    if (!entered_scene_browser_scene)
        policy->active_timeout_seconds = policy->base_timeout_seconds;
}

void banana_poc_objective_policy_apply_world_variant(BananaPocObjectivePolicy *policy,
                                                     int world_variant)
{
    if (!policy)
        return;

    if (world_variant == 0)
        policy->active_timeout_seconds = policy->base_timeout_seconds;
    else if (world_variant == 2)
        policy->active_timeout_seconds = policy->base_timeout_seconds;
    else
        policy->active_timeout_seconds = 0;
}

int banana_poc_objective_policy_should_fail_timeout(const BananaPocObjectivePolicy *policy,
                                                    int startup_smoke_mode,
                                                    BananaPocScene scene,
                                                    int objective_collected,
                                                    int elapsed_seconds)
{
    int timeout_seconds = 0;

    if (!policy)
        return 0;

    timeout_seconds = policy->active_timeout_seconds;

    return (!startup_smoke_mode &&
            scene == BANANA_POC_SCENE_WORLD &&
            !objective_collected &&
            timeout_seconds > 0 &&
            elapsed_seconds >= timeout_seconds)
               ? 1
               : 0;
}

int banana_poc_objective_policy_on_objective_collected(BananaPocObjectivePolicy *policy,
                                                        BananaPocScene scene,
                                                        int objective_collected)
{
    int announce = 0;

    if (!policy)
        return 0;

    if (scene != BANANA_POC_SCENE_WORLD || !objective_collected)
        return 0;

    announce = policy->completion_announced ? 0 : 1;
    policy->completion_announced = 1;
    policy->active_timeout_seconds = 0;
    return announce;
}

int banana_poc_objective_policy_timeout_seconds(const BananaPocObjectivePolicy *policy)
{
    if (!policy)
        return 0;

    return policy->active_timeout_seconds;
}
