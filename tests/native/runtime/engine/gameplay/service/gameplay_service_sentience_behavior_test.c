#include "runtime/engine/gameplay/service/gameplay_service_sentience_behavior.h"

#include "../../../support/test_support.h"

static void test_mode_labels_cover_all_paths(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_STR_EQ(
        runtime_gameplay_sentience_behavior_mode_label(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE),
        "hold",
        "hold-line mode label must be hold");
    BANANA_TEST_ASSERT_STR_EQ(
        runtime_gameplay_sentience_behavior_mode_label(RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK),
        "flank",
        "flank mode label must be flank");
    BANANA_TEST_ASSERT_STR_EQ(
        runtime_gameplay_sentience_behavior_mode_label(RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP),
        "regroup",
        "regroup mode label must be regroup");
    BANANA_TEST_ASSERT_STR_EQ(
        runtime_gameplay_sentience_behavior_mode_label(RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE),
        "negotiate",
        "negotiate mode label must be negotiate");
    BANANA_TEST_ASSERT_STR_EQ(
        runtime_gameplay_sentience_behavior_mode_label((RuntimeWarSentienceBehaviorMode)99),
        "hold",
        "unknown mode should fall back to hold label");
}

static void test_comeback_bonus_and_deescalation_trim(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_sentience_behavior_comeback_bonus(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE),
        0,
        "hold-line mode has no comeback bonus");
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_sentience_behavior_comeback_bonus(RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP),
        1,
        "regroup mode grants comeback bonus");

    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(0),
        0,
        "non-positive streak should trim zero");
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(1),
        1,
        "first streak tick should trim one");
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(5),
        3,
        "streak five should trim three");
    BANANA_TEST_ASSERT_INT_EQ(
        runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(100),
        4,
        "trim should cap at four");
}

static void test_spawn_offsets_cover_each_mode_and_null_outputs(void **state)
{
    float forward = 0.0f;
    float lateral = 0.0f;

    (void)state;

    runtime_gameplay_sentience_behavior_spawn_offsets(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                      4,
                                                      &forward,
                                                      &lateral);
    BANANA_TEST_ASSERT_FLOAT_EQ(forward, 2.20f, 0.0001f, "hold-line forward offset must use base formula");
    BANANA_TEST_ASSERT_FLOAT_EQ(lateral, 0.60f, 0.0001f, "hold-line lateral offset must use base formula");

    runtime_gameplay_sentience_behavior_spawn_offsets(RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                      1,
                                                      &forward,
                                                      &lateral);
    BANANA_TEST_ASSERT_FLOAT_EQ(forward, 2.45f, 0.0001f, "flank forward offset must include flank bonus");
    BANANA_TEST_ASSERT_FLOAT_EQ(lateral, 1.60f, 0.0001f, "flank lateral offset must include flank bonus");

    runtime_gameplay_sentience_behavior_spawn_offsets(RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP,
                                                      2,
                                                      &forward,
                                                      &lateral);
    BANANA_TEST_ASSERT_FLOAT_EQ(forward, -1.60f, 0.0001f, "regroup forward offset must invert with regroup formula");
    BANANA_TEST_ASSERT_FLOAT_EQ(lateral, 0.18f, 0.0001f, "regroup lateral offset must use regroup formula");

    runtime_gameplay_sentience_behavior_spawn_offsets(RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE,
                                                      5,
                                                      &forward,
                                                      &lateral);
    BANANA_TEST_ASSERT_FLOAT_EQ(forward, 1.25f, 0.0001f, "negotiate forward offset must use negotiate formula");
    BANANA_TEST_ASSERT_FLOAT_EQ(lateral, 0.14f, 0.0001f, "negotiate lateral offset must use negotiate formula");

    runtime_gameplay_sentience_behavior_spawn_offsets(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                      0,
                                                      NULL,
                                                      &lateral);
    runtime_gameplay_sentience_behavior_spawn_offsets(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                      0,
                                                      &forward,
                                                      NULL);
}

static void test_apply_directionality_flank_rotation_and_guards(void **state)
{
    float dir_x = 0.0f;
    float dir_z = 1.0f;

    (void)state;

    runtime_gameplay_sentience_behavior_apply_directionality(RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                             1.0f,
                                                             &dir_x,
                                                             &dir_z);
    BANANA_TEST_ASSERT_FLOAT_EQ(dir_x, 0.0f, 0.0001f, "non-flank directionality should not rotate x");
    BANANA_TEST_ASSERT_FLOAT_EQ(dir_z, 1.0f, 0.0001f, "non-flank directionality should not rotate z");

    runtime_gameplay_sentience_behavior_apply_directionality(RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                             1.0f,
                                                             &dir_x,
                                                             &dir_z);
    BANANA_TEST_ASSERT_FLOAT_EQ(dir_x, -0.529919f, 0.0002f, "positive flank sign should rotate x to negative value");
    BANANA_TEST_ASSERT_FLOAT_EQ(dir_z, 0.848048f, 0.0002f, "positive flank sign should rotate z forward");

    runtime_gameplay_sentience_behavior_apply_directionality(RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                             -1.0f,
                                                             &dir_x,
                                                             &dir_z);
    BANANA_TEST_ASSERT_FLOAT_EQ(dir_x, 0.0f, 0.0002f, "opposite flank sign should rotate back near original x");
    BANANA_TEST_ASSERT_FLOAT_EQ(dir_z, 1.0f, 0.0002f, "opposite flank sign should rotate back near original z");

    runtime_gameplay_sentience_behavior_apply_directionality(RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                             1.0f,
                                                             NULL,
                                                             &dir_z);
    runtime_gameplay_sentience_behavior_apply_directionality(RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                             1.0f,
                                                             &dir_x,
                                                             NULL);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_mode_labels_cover_all_paths),
    BANANA_TEST_CASE(test_comeback_bonus_and_deescalation_trim),
    BANANA_TEST_CASE(test_spawn_offsets_cover_each_mode_and_null_outputs),
    BANANA_TEST_CASE(test_apply_directionality_flank_rotation_and_guards)
)
