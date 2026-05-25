#include "runtime/engine/engine_aux_abi.h"
#include "runtime/engine/engine_state.h"

#include <stdio.h>
#include <string.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_reason(const char *label,
                         banana_launch_gate_reason_code actual,
                         banana_launch_gate_reason_code expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr,
            "%s expected=%d actual=%d\n",
            label,
            (int)expected,
            (int)actual);
    return 0;
}

static int expect_blocked_state(const char *label,
                                RuntimeEngineLaunchGateBlockedState actual,
                                RuntimeEngineLaunchGateBlockedState expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, (int)expected, (int)actual);
    return 0;
}

static int run_case(const char *label,
                    const char *mode_label,
                    int has_steam_identity,
                    int account_linked,
                    int account_in_good_standing,
                    int verification_fresh,
                    int verification_available,
                    int expected_allow,
                    banana_launch_gate_reason_code expected_reason)
{
    banana_launch_gate_policy policy;
    banana_launch_gate_reason_code reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;
    int allow = 0;

    memset(&policy, 0, sizeof(policy));
    if (!expect_int(label,
                    runtime_engine_aux_launch_gate_policy_resolve(mode_label, &policy),
                    0))
        return 0;

    if (!expect_int(label,
                    runtime_engine_aux_launch_gate_decide(&policy,
                                                          has_steam_identity,
                                                          account_linked,
                                                          account_in_good_standing,
                                                          verification_fresh,
                                                          verification_available,
                                                          &reason_code,
                                                          &allow),
                    0))
    {
        return 0;
    }

    if (!expect_int(label, allow, expected_allow))
        return 0;

    if (!expect_reason(label, reason_code, expected_reason))
        return 0;

    if (!expect_int(label,
                    runtime_engine_aux_launch_gate_reason_is_remediable(reason_code),
                    expected_reason == BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT ||
                        expected_reason == BANANA_LAUNCH_GATE_REASON_STALE_SESSION ||
                        expected_reason == BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE ||
                        expected_reason == BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE
                        ? 1
                        : 0))
    {
        return 0;
    }

    return 1;
}

int main(void)
{
    banana_launch_gate_policy policy;

    memset(&policy, 0, sizeof(policy));
    if (!expect_int("policy resolve production",
                    runtime_engine_aux_launch_gate_policy_resolve("production-steam-package", &policy),
                    0))
        return 1;
    if (!expect_int("production mode", policy.mode, BANANA_LAUNCH_GATE_MODE_PRODUCTION_STEAM_PACKAGE))
        return 1;
    if (!expect_int("production disallows non-steam startup", policy.allow_non_steam_startup, 0))
        return 1;
    if (!expect_int("production disallows cached verification", policy.allow_cached_verification, 0))
        return 1;
    if (!expect_int("production disallows override context", policy.allow_override_context, 0))
        return 1;

    if (!run_case("production allow",
                  "production-steam-package",
                  1,
                  1,
                  1,
                  1,
                  1,
                  1,
                  BANANA_LAUNCH_GATE_REASON_OK))
        return 1;

    if (!run_case("unlinked deny",
                  "production-steam-package",
                  1,
                  0,
                  1,
                  1,
                  1,
                  0,
                  BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT))
        return 1;

    if (!run_case("restricted deny",
                  "production-steam-package",
                  1,
                  1,
                  0,
                  1,
                  1,
                  0,
                  BANANA_LAUNCH_GATE_REASON_SUSPENDED_OR_RESTRICTED))
        return 1;

    if (!run_case("stale deny",
                  "production-steam-package",
                  1,
                  1,
                  1,
                  0,
                  1,
                  0,
                  BANANA_LAUNCH_GATE_REASON_STALE_SESSION))
        return 1;

    if (!run_case("offline deny",
                  "production-steam-package",
                  1,
                  1,
                  1,
                  1,
                  0,
                  0,
                  BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE))
        return 1;

    if (!run_case("steam unavailable deny",
                  "production-steam-package",
                  0,
                  1,
                  1,
                  1,
                  1,
                  0,
                  BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE))
        return 1;

    if (!run_case("development fallback",
                  "development",
                  0,
                  0,
                  1,
                  1,
                  1,
                  1,
                  BANANA_LAUNCH_GATE_REASON_OK))
        return 1;

    if (!expect_int("unknown mode policy rejected",
                    runtime_engine_aux_launch_gate_policy_resolve("bogus", &policy),
                    -1))
        return 1;

    if (!expect_blocked_state("blocked state ok",
                              runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_OK),
                              RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE))
        return 1;

    if (!expect_blocked_state("blocked state offline",
                              runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE),
                              RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_OFFLINE_UNVERIFIABLE))
        return 1;

    if (!expect_blocked_state("blocked state unknown",
                              runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE),
                              RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNKNOWN_MODE))
        return 1;

    printf("launch_gate_matrix_test: pass\n");
    return 0;
}
