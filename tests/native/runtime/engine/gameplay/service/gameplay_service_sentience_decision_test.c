#include "runtime/engine/gameplay/service/gameplay_service_sentience_decision.h"

#include "runtime/support/test_support.h"

#include <string.h>

static int g_test_per_coordination = 2;

int runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(void)
{
    return g_test_per_coordination;
}

static EngineRuntimeState make_runtime_state(int humanoid_index, int coordination, int empathy)
{
    EngineRuntimeState state;

    memset(&state, 0, sizeof(state));
    state.war_sentience_humanoid_index = humanoid_index;
    state.war_sentience_coordination_level = coordination;
    state.war_sentience_empathy_level = empathy;
    return state;
}

static void test_resolve_sentience_behavior_prefers_negotiate(void **state)
{
    EngineRuntimeState runtime_state = make_runtime_state(8, 3, 6);

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_resolve_sentience_behavior_mode(&runtime_state, 6, 5),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE,
        "high humanoid index and empathy with near parity must negotiate");
}

static void test_resolve_sentience_behavior_regroups_under_pressure(void **state)
{
    EngineRuntimeState runtime_state = make_runtime_state(1, 7, 1);

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_resolve_sentience_behavior_mode(&runtime_state, 3, 5),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP,
        "pressure gap >= 2 must regroup");
}

static void test_resolve_sentience_behavior_flanks_when_coordinated(void **state)
{
    EngineRuntimeState runtime_state = make_runtime_state(2, 6, 2);

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_resolve_sentience_behavior_mode(&runtime_state, 5, 4),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
        "coordinated teams with non-positive pressure should flank");
}

static void test_resolve_sentience_behavior_holds_line_for_null_state(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_resolve_sentience_behavior_mode(NULL, 5, 5),
        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
        "null runtime state with no pressure should hold line");
}

static void test_coordination_bonus_returns_zero_for_invalid_inputs(void **state)
{
    EngineRuntimeState runtime_state = make_runtime_state(0, 8, 7);

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_humanoid_coordination_comeback_bonus(NULL, 3, 5),
        0,
        "null runtime state must return zero comeback bonus");

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_humanoid_coordination_comeback_bonus(&runtime_state, 5, 5),
        0,
        "no trailing team condition must return zero comeback bonus");
}

static void test_coordination_bonus_returns_zero_when_policy_is_non_positive(void **state)
{
    EngineRuntimeState runtime_state = make_runtime_state(0, 8, 7);

    (void)state;

    g_test_per_coordination = 0;
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_humanoid_coordination_comeback_bonus(&runtime_state, 2, 4),
        0,
        "non-positive policy budget must disable comeback bonus");

    g_test_per_coordination = 2;
}

static void test_coordination_bonus_clamps_negative_and_high_values(void **state)
{
    EngineRuntimeState runtime_state = make_runtime_state(0, -3, -4);

    (void)state;

    g_test_per_coordination = 2;
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_humanoid_coordination_comeback_bonus(&runtime_state, 1, 4),
        0,
        "negative computed bonus must clamp to zero");

    runtime_state.war_sentience_coordination_level = 20;
    runtime_state.war_sentience_empathy_level = 20;
    g_test_per_coordination = 1;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_humanoid_coordination_comeback_bonus(&runtime_state, 1, 4),
        4,
        "computed bonus above cap must clamp to four");

    g_test_per_coordination = 2;
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_resolve_sentience_behavior_prefers_negotiate),
    BANANA_TEST_CASE(test_resolve_sentience_behavior_regroups_under_pressure),
    BANANA_TEST_CASE(test_resolve_sentience_behavior_flanks_when_coordinated),
    BANANA_TEST_CASE(test_resolve_sentience_behavior_holds_line_for_null_state),
    BANANA_TEST_CASE(test_coordination_bonus_returns_zero_for_invalid_inputs),
    BANANA_TEST_CASE(test_coordination_bonus_returns_zero_when_policy_is_non_positive),
    BANANA_TEST_CASE(test_coordination_bonus_clamps_negative_and_high_values)
)
