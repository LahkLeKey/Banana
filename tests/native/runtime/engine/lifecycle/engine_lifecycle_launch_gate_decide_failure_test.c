#include "runtime/engine/lifecycle/engine_lifecycle.h"
#include "runtime/engine/state/engine_state.h"

#include "runtime/support/test_support.h"

#include <string.h>

const char *runtime_engine_host_launch_gate_mode_label_for(const char *trusted_mode_label)
{
    (void)trusted_mode_label;
    return "development";
}

int runtime_engine_aux_launch_gate_policy_resolve(const char *mode_label,
                                                  banana_launch_gate_policy *out_policy)
{
    if (!mode_label || !out_policy)
        return -1;

    memset(out_policy, 0, sizeof(*out_policy));
    out_policy->mode = BANANA_LAUNCH_GATE_MODE_DEVELOPMENT;
    out_policy->allow_non_steam_startup = 1;
    out_policy->allow_cached_verification = 1;
    out_policy->allow_override_context = 1;
    return 0;
}

int runtime_engine_aux_launch_gate_decide(const banana_launch_gate_policy *policy,
                                          int has_steam_identity,
                                          int account_linked,
                                          int account_in_good_standing,
                                          int verification_fresh,
                                          int verification_available,
                                          banana_launch_gate_reason_code *out_reason_code,
                                          int *out_allow)
{
    (void)policy;
    (void)has_steam_identity;
    (void)account_linked;
    (void)account_in_good_standing;
    (void)verification_fresh;
    (void)verification_available;
    (void)out_reason_code;
    (void)out_allow;
    return -1;
}

void runtime_engine_state_apply_launch_gate_decision(EngineRuntimeState *state,
                                                     int allow,
                                                     banana_launch_gate_reason_code reason_code)
{
    if (!state)
        return;

    state->launch_gate_allowed = allow;
    state->launch_gate_reason_code = reason_code;
    state->launch_gate_blocked_state =
        runtime_engine_state_launch_gate_blocked_state_from_reason_code(reason_code);
}

RuntimeEngineLaunchGateBlockedState
runtime_engine_state_launch_gate_blocked_state_from_reason_code(banana_launch_gate_reason_code reason_code)
{
    if (reason_code == BANANA_LAUNCH_GATE_REASON_OK)
        return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE;

    return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STEAM_UNAVAILABLE;
}

static void test_launch_gate_preflight_blocks_when_decision_evaluation_fails(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_preflight_launch_gate(&runtime_state),
                              -1,
                              "launch gate preflight must fail when decision evaluation returns an error");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_reason_code,
                              (int)BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE,
                              "decision-evaluation errors must map to UNKNOWN_MODE in runtime state");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.launch_gate_allowed,
                              0,
                              "decision-evaluation errors must keep launch_gate_allowed disabled");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_launch_gate_preflight_blocks_when_decision_evaluation_fails)
)
