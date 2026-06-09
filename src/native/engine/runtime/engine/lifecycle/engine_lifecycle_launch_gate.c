#include "engine_lifecycle.h"

#include "../engine_aux_abi.h"
#include "../engine_host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int runtime_launch_gate_env_flag(const char *name)
{
    const char *value = getenv(name);

    if (!value || value[0] == '\0')
        return 0;

    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0 || strcmp(value, "TRUE") == 0 || strcmp(value, "yes") == 0 || strcmp(value, "YES") == 0)
        return 1;

    return 0;
}

static const char *runtime_launch_gate_mode_env(void)
{
    return runtime_engine_host_launch_gate_mode_label_for(NULL);
}

int runtime_engine_lifecycle_preflight_launch_gate(EngineRuntimeState *state)
{
    banana_launch_gate_policy policy;
    banana_launch_gate_reason_code reason_code = BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE;
    int allow = 0;
    int has_steam_identity = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_HAS_STEAM_IDENTITY");
    int account_linked = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_ACCOUNT_LINKED");
    int account_in_good_standing = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING");
    int verification_fresh = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_VERIFICATION_FRESH");
    int verification_available = runtime_launch_gate_env_flag("BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE");

    if (!state)
        return -1;

    if (runtime_engine_aux_launch_gate_policy_resolve(runtime_launch_gate_mode_env(), &policy) != 0)
    {
        runtime_engine_state_apply_launch_gate_decision(state, 0, BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE);
        fprintf(stderr, "[engine] launch gate blocked: unknown mode\n");
        return -1;
    }

    if (runtime_engine_aux_launch_gate_decide(&policy,
                                              has_steam_identity,
                                              account_linked,
                                              account_in_good_standing,
                                              verification_fresh,
                                              verification_available,
                                              &reason_code,
                                              &allow) != 0)
    {
        runtime_engine_state_apply_launch_gate_decision(state, 0, BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE);
        fprintf(stderr, "[engine] launch gate blocked: decision evaluation failed\n");
        return -1;
    }

    runtime_engine_state_apply_launch_gate_decision(state, allow, reason_code);
    if (!allow)
    {
        fprintf(stderr, "[engine] launch gate blocked: reason=%d state=%d\n",
                (int)reason_code,
                (int)state->launch_gate_blocked_state);
        return -1;
    }

    return 0;
}