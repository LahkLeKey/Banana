#include "engine_state.h"

#include <string.h>

RuntimeEngineLaunchGateBlockedState
runtime_engine_state_launch_gate_blocked_state_from_reason_code(banana_launch_gate_reason_code reason_code)
{
    switch (reason_code)
    {
        case BANANA_LAUNCH_GATE_REASON_OK:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE;
        case BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNLINKED_ACCOUNT;
        case BANANA_LAUNCH_GATE_REASON_SUSPENDED_OR_RESTRICTED:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_SUSPENDED_OR_RESTRICTED;
        case BANANA_LAUNCH_GATE_REASON_STALE_SESSION:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STALE_SESSION;
        case BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STEAM_UNAVAILABLE;
        case BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_API_UNAVAILABLE;
        case BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_OFFLINE_UNVERIFIABLE;
        case BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE:
        default:
            return RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNKNOWN_MODE;
    }
}

void runtime_engine_state_apply_launch_gate_decision(EngineRuntimeState *state,
                                                     int allow,
                                                     banana_launch_gate_reason_code reason_code)
{
    if (!state)
        return;

    state->launch_gate_allowed = allow ? 1 : 0;
    state->launch_gate_reason_code = reason_code;
    state->launch_gate_blocked_state = allow ? RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE
                                             : runtime_engine_state_launch_gate_blocked_state_from_reason_code(reason_code);
}

void runtime_engine_state_reset(EngineRuntimeState *state)
{
    if (!state)
        return;

    memset(state, 0, sizeof(*state));
}
