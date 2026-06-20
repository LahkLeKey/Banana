#include "runtime/engine/lifecycle/engine_lifecycle.h"

#include "../../support/test_support.h"

#include <string.h>

#if defined(_WIN32)
static void set_env_value(const char *name, const char *value)
{
    if (!name || !value)
        return;
    _putenv_s(name, value);
}
#else
#include <stdlib.h>
static void set_env_value(const char *name, const char *value)
{
    if (!name || !value)
        return;
    setenv(name, value, 1);
}
#endif

static void set_launch_gate_env(const char *mode,
                                const char *has_steam,
                                const char *linked,
                                const char *good_standing,
                                const char *fresh,
                                const char *available)
{
    set_env_value("BANANA_LAUNCH_GATE_MODE", mode);
    set_env_value("BANANA_LAUNCH_GATE_MODE_OVERRIDE", "");
    set_env_value("BANANA_LAUNCH_GATE_HAS_STEAM_IDENTITY", has_steam);
    set_env_value("BANANA_LAUNCH_GATE_ACCOUNT_LINKED", linked);
    set_env_value("BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING", good_standing);
    set_env_value("BANANA_LAUNCH_GATE_VERIFICATION_FRESH", fresh);
    set_env_value("BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE", available);
}

static void test_launch_gate_preflight_rejects_null_state(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_preflight_launch_gate(NULL),
                              -1,
                              "launch gate preflight must reject null engine runtime state");
}

static void test_launch_gate_preflight_blocks_unknown_mode(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));
    set_launch_gate_env("invalid-mode", "1", "1", "1", "1", "1");

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_preflight_launch_gate(&runtime_state),
                              -1,
                              "launch gate preflight must block unresolved mode labels");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_reason_code,
                              (int)BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE,
                              "unknown mode must map to UNKNOWN_MODE launch gate reason");
}

static void test_launch_gate_preflight_allows_development_mode_with_cached_session(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));
    set_launch_gate_env("development", "0", "0", "1", "0", "1");

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_preflight_launch_gate(&runtime_state),
                              0,
                              "development mode should allow startup without steam identity when verification is available");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.launch_gate_allowed,
                              1,
                              "allowed preflight must set launch_gate_allowed to 1");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_blocked_state,
                              (int)RUNTIME_ENGINE_LAUNCH_GATE_BLOCKED_STATE_NONE,
                              "allowed preflight must clear blocked-state output");
}

static void test_launch_gate_preflight_blocks_production_without_steam_identity(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));
    set_launch_gate_env("production-steam-package", "0", "1", "1", "1", "1");

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_preflight_launch_gate(&runtime_state),
                              -1,
                              "production launch gate must block startup without steam identity");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_reason_code,
                              (int)BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE,
                              "missing steam identity must map to STEAM_UNAVAILABLE reason");
}

static void test_launch_gate_preflight_blocks_when_verification_unavailable(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));
    set_launch_gate_env("staging-uat", "1", "1", "1", "1", "0");

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_preflight_launch_gate(&runtime_state),
                              -1,
                              "launch gate preflight must block when verification is unavailable");
    BANANA_TEST_ASSERT_INT_EQ((int)runtime_state.launch_gate_reason_code,
                              (int)BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE,
                              "unavailable verification must map to OFFLINE_UNVERIFIABLE reason");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_launch_gate_preflight_rejects_null_state),
    BANANA_TEST_CASE(test_launch_gate_preflight_blocks_unknown_mode),
    BANANA_TEST_CASE(test_launch_gate_preflight_allows_development_mode_with_cached_session),
    BANANA_TEST_CASE(test_launch_gate_preflight_blocks_production_without_steam_identity),
    BANANA_TEST_CASE(test_launch_gate_preflight_blocks_when_verification_unavailable)
)
