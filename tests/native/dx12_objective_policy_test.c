#include "../../../src/native/engine/win32_dx12_poc/objective_policy.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    BananaPocObjectivePolicy policy = {0};

    banana_poc_objective_policy_init(&policy, 30);
    if (!expect_int("init base timeout", policy.base_timeout_seconds, 30))
        return 1;
    if (!expect_int("init active timeout", policy.active_timeout_seconds, 30))
        return 1;

    banana_poc_objective_policy_apply_world_variant(&policy, 1);
    if (!expect_int("variant 1 has no timeout", policy.active_timeout_seconds, 0))
        return 1;

    banana_poc_objective_policy_apply_world_variant(&policy, 2);
    if (!expect_int("variant 2 restores base timeout", policy.active_timeout_seconds, 30))
        return 1;

    if (!expect_int("no timeout fail in startup smoke",
                    banana_poc_objective_policy_should_fail_timeout(&policy,
                                                                    1,
                                                                    BANANA_POC_SCENE_WORLD,
                                                                    0,
                                                                    31),
                    0))
        return 1;

    if (!expect_int("timeout triggers in world",
                    banana_poc_objective_policy_should_fail_timeout(&policy,
                                                                    0,
                                                                    BANANA_POC_SCENE_WORLD,
                                                                    0,
                                                                    31),
                    1))
        return 1;

    if (!expect_int("timeout ignored after objective",
                    banana_poc_objective_policy_should_fail_timeout(&policy,
                                                                    0,
                                                                    BANANA_POC_SCENE_WORLD,
                                                                    1,
                                                                    31),
                    0))
        return 1;

    if (!expect_int("first completion announces",
                    banana_poc_objective_policy_on_objective_collected(&policy,
                                                                        BANANA_POC_SCENE_WORLD,
                                                                        1),
                    1))
        return 1;

    if (!expect_int("completion clears timeout",
                    policy.active_timeout_seconds,
                    0))
        return 1;

    if (!expect_int("second completion does not announce",
                    banana_poc_objective_policy_on_objective_collected(&policy,
                                                                        BANANA_POC_SCENE_WORLD,
                                                                        1),
                    0))
        return 1;

    banana_poc_objective_policy_on_entered_world(&policy, 0);
    if (!expect_int("enter world resets timeout from base",
                    policy.active_timeout_seconds,
                    30))
        return 1;

    return 0;
}
