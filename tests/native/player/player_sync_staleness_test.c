#include "runtime/engine/engine_aux_abi.h"

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

int main(void)
{
    banana_launch_gate_policy policy;
    banana_launch_gate_reason_code reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;
    int allow = 0;

    memset(&policy, 0, sizeof(policy));
    if (!expect_int("resolve production policy",
                    runtime_engine_aux_launch_gate_policy_resolve("production-steam-package", &policy),
                    0))
        return 1;
    if (!expect_int("fresh production verification",
                    runtime_engine_aux_launch_gate_decide(&policy,
                                                          1,
                                                          1,
                                                          1,
                                                          1,
                                                          1,
                                                          &reason_code,
                                                          &allow),
                    0))
        return 1;
    if (!expect_int("fresh production allow", allow, 1))
        return 1;
    if (!expect_reason("fresh production reason ok", reason_code, BANANA_LAUNCH_GATE_REASON_OK))
        return 1;

    if (!expect_int("stale production verification",
                    runtime_engine_aux_launch_gate_decide(&policy,
                                                          1,
                                                          1,
                                                          1,
                                                          0,
                                                          1,
                                                          &reason_code,
                                                          &allow),
                    0))
        return 1;
    if (!expect_int("stale production deny", allow, 0))
        return 1;
    if (!expect_reason("stale production reason", reason_code, BANANA_LAUNCH_GATE_REASON_STALE_SESSION))
        return 1;
    if (!expect_int("stale production remediable",
                    runtime_engine_aux_launch_gate_reason_is_remediable(reason_code),
                    1))
        return 1;

    if (!expect_int("fresh verification recovers after remediation",
                    runtime_engine_aux_launch_gate_decide(&policy,
                                                          1,
                                                          1,
                                                          1,
                                                          1,
                                                          1,
                                                          &reason_code,
                                                          &allow),
                    0))
        return 1;
    if (!expect_int("fresh after remediation allow", allow, 1))
        return 1;
    if (!expect_reason("fresh after remediation reason", reason_code, BANANA_LAUNCH_GATE_REASON_OK))
        return 1;

    printf("player_sync_staleness_test: pass\n");
    return 0;
}