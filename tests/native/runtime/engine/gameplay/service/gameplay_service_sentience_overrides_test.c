#include "runtime/engine/gameplay/service/gameplay_service_sentience_overrides.h"
#include "runtime/engine/gameplay/service/gameplay_service_sentience_override_config.h"
#include "runtime/engine/state/engine_state.h"
#include "runtime/support/test_support.h"

#include <string.h>

static RuntimeWarSentienceOverrides g_overrides;

const RuntimeWarSentienceOverrides *runtime_gameplay_sentience_overrides_get(void)
{
    return &g_overrides;
}

static void reset_overrides(void)
{
    memset(&g_overrides, 0, sizeof(g_overrides));
}

static void test_apply_sentience_overrides_respects_null_state_and_updates_fields(void **state)
{
    EngineRuntimeState runtime_state;

    (void)state;

    memset(&runtime_state, 0, sizeof(runtime_state));
    runtime_state.war_sentience_negotiate_streak_ticks = 2;

    reset_overrides();
    g_overrides.has_humanoid_index = 1;
    g_overrides.humanoid_index = 7;
    g_overrides.has_coordination_level = 1;
    g_overrides.coordination_level = 3;
    g_overrides.has_empathy_level = 1;
    g_overrides.empathy_level = 5;
    g_overrides.has_negotiate_streak_seed = 1;
    g_overrides.negotiate_streak_seed = 6;

    runtime_gameplay_apply_sentience_overrides(NULL);

    runtime_gameplay_apply_sentience_overrides(&runtime_state);
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.war_sentience_humanoid_index, 7,
                              "humanoid index override must be applied");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.war_sentience_coordination_level, 3,
                              "coordination override must be applied");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.war_sentience_empathy_level, 5,
                              "empathy override must be applied");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.war_sentience_negotiate_streak_ticks, 6,
                              "streak seed must lift low streak values");

    runtime_state.war_sentience_negotiate_streak_ticks = 9;
    runtime_gameplay_apply_sentience_overrides(&runtime_state);
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.war_sentience_negotiate_streak_ticks, 9,
                              "streak seed must not lower existing streak values");
}

static void test_apply_sentience_mode_override_prefers_side_specific_and_global_modes(void **state)
{
    (void)state;

    reset_overrides();
    g_overrides.has_banana_mode_override = 1;
    g_overrides.banana_mode_override = RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;
    g_overrides.has_bean_mode_override = 1;
    g_overrides.bean_mode_override = RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP;
    g_overrides.has_mode_override = 1;
    g_overrides.mode_override = RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_apply_sentience_mode_override(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE, 1),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE,
        "banana overrides must win for banana actors");

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_apply_sentience_mode_override(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE, 0),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP,
        "bean overrides must win for bean actors");

    g_overrides.has_banana_mode_override = 0;
    g_overrides.has_bean_mode_override = 0;
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_apply_sentience_mode_override(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE, 1),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
        "global override must be used when side-specific overrides are absent");

    g_overrides.has_mode_override = 0;
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_apply_sentience_mode_override(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE, 0),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
        "mode must pass through when no override is set");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_apply_sentience_overrides_respects_null_state_and_updates_fields),
    BANANA_TEST_CASE(test_apply_sentience_mode_override_prefers_side_specific_and_global_modes)
)