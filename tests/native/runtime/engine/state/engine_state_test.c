#include "runtime/engine/state/engine_state.h"

#include "runtime/support/test_support.h"

#include <string.h>

static void test_engine_state_reason_mapping_covers_all_launch_gate_codes(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_OK),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE,
                              "OK reason must map to NONE blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNLINKED_ACCOUNT,
                              "UNLINKED_ACCOUNT reason must map to matching blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_SUSPENDED_OR_RESTRICTED),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_SUSPENDED_OR_RESTRICTED,
                              "SUSPENDED_OR_RESTRICTED reason must map to matching blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_STALE_SESSION),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STALE_SESSION,
                              "STALE_SESSION reason must map to matching blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_STEAM_UNAVAILABLE,
                              "STEAM_UNAVAILABLE reason must map to matching blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_API_UNAVAILABLE,
                              "API_UNAVAILABLE reason must map to matching blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code(BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_OFFLINE_UNVERIFIABLE,
                              "OFFLINE_UNVERIFIABLE reason must map to matching blocked state");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_engine_state_launch_gate_blocked_state_from_reason_code((banana_launch_gate_reason_code)999),
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_UNKNOWN_MODE,
                              "unknown reason code must map to UNKNOWN blocked state");
}

static void test_engine_state_apply_launch_gate_decision_handles_null_state(void **state)
{
    (void)state;

    runtime_engine_state_apply_launch_gate_decision(NULL, 1, BANANA_LAUNCH_GATE_REASON_OK);

    BANANA_TEST_ASSERT_TRUE(1,
                            "apply launch gate decision must tolerate null state");
}

static void test_engine_state_apply_launch_gate_decision_sets_allow_and_block_fields(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));

    runtime_engine_state_apply_launch_gate_decision(&runtime_state,
                                                    1,
                                                    BANANA_LAUNCH_GATE_REASON_STALE_SESSION);

    BANANA_TEST_ASSERT_INT_EQ(runtime_state.launch_gate_allowed,
                              1,
                              "allow decision must set launch_gate_allowed to 1");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_reason_code,
                              (int)BANANA_LAUNCH_GATE_REASON_STALE_SESSION,
                              "allow decision must preserve reason code");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_blocked_state,
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE,
                              "allow decision must clear blocked state");

    runtime_engine_state_apply_launch_gate_decision(&runtime_state,
                                                    0,
                                                    BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE);

    BANANA_TEST_ASSERT_INT_EQ(runtime_state.launch_gate_allowed,
                              0,
                              "deny decision must clear launch_gate_allowed");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_reason_code,
                              (int)BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE,
                              "deny decision must preserve reason code");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_blocked_state,
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_API_UNAVAILABLE,
                              "deny decision must map reason to blocked state");
}

static void test_engine_state_reset_zeroes_runtime_state(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0x7F, sizeof(runtime_state));
    runtime_state.viewport_width = 123;
    runtime_state.viewport_height = 456;
    runtime_state.launch_gate_allowed = 1;

    runtime_engine_state_reset(&runtime_state);

    BANANA_TEST_ASSERT_INT_EQ(runtime_state.viewport_width,
                              0,
                              "engine state reset must zero viewport width");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.viewport_height,
                              0,
                              "engine state reset must zero viewport height");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.launch_gate_allowed,
                              0,
                              "engine state reset must clear launch gate allow flag");

    runtime_engine_state_reset(NULL);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_state_reason_mapping_covers_all_launch_gate_codes),
    BANANA_TEST_CASE(test_engine_state_apply_launch_gate_decision_handles_null_state),
    BANANA_TEST_CASE(test_engine_state_apply_launch_gate_decision_sets_allow_and_block_fields),
    BANANA_TEST_CASE(test_engine_state_reset_zeroes_runtime_state)
)
