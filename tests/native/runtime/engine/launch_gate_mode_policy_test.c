#include "runtime/engine/engine_aux_abi.h"
#include "runtime/engine/engine_host.h"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_str(const char *label, const char *actual, const char *expected)
{
    if (actual && expected && strcmp(actual, expected) == 0)
        return 1;

    fprintf(stderr,
            "%s expected='%s' actual='%s'\n",
            label,
            expected ? expected : "<null>",
            actual ? actual : "<null>");
    return 0;
}

int main(void)
{
    banana_launch_gate_policy policy;

    unsetenv("BANANA_LAUNCH_GATE_MODE");
    unsetenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE");

    if (!expect_str("default trusted mode",
                    runtime_engine_host_launch_gate_mode_label_for(NULL),
                    "development"))
        return 1;

    setenv("BANANA_LAUNCH_GATE_MODE", "production-steam-package", 1);
    setenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE", "development", 1);
    if (!expect_str("production ignores override",
                    runtime_engine_host_launch_gate_mode_label_for(NULL),
                    "production-steam-package"))
        return 1;

    setenv("BANANA_LAUNCH_GATE_MODE", "staging-uat", 1);
    setenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE", "development", 1);
    if (!expect_str("staging allows override",
                    runtime_engine_host_launch_gate_mode_label_for(NULL),
                    "development"))
        return 1;

    setenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE", "bogus", 1);
    if (!expect_str("invalid override ignored",
                    runtime_engine_host_launch_gate_mode_label_for(NULL),
                    "staging-uat"))
        return 1;

    setenv("BANANA_LAUNCH_GATE_MODE", "production-steam-package", 1);
    setenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE", "development", 1);
    if (!expect_int("production policy no override",
                    runtime_engine_aux_launch_gate_policy_resolve(
                        runtime_engine_host_launch_gate_mode_label_for(NULL),
                        &policy),
                    0))
        return 1;

    if (!expect_int("production override context off",
                    policy.allow_override_context,
                    0))
        return 1;

    unsetenv("BANANA_LAUNCH_GATE_MODE");
    unsetenv("BANANA_LAUNCH_GATE_MODE_OVERRIDE");

    printf("launch_gate_mode_policy_test: pass\n");
    return 0;
}
