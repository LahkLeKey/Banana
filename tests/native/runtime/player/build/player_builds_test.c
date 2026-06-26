#include "runtime/player/build/player_builds.h"
#include "runtime/support/test_support.h"

static int player_builds_test_setup(void **state)
{
    (void)state;
    return player_builds_init();
}

static int player_builds_test_teardown(void **state)
{
    (void)state;
    player_builds_cleanup();
    return 0;
}

static void test_player_builds_parse_and_stat_helpers(void **state)
{
    (void)state;
    BuildClass class_type = BUILD_CLASS_COUNT;
    GearSlot slot = GEAR_SLOT_COUNT;
    BuildStat stat = BUILD_STAT_COUNT;
    BuildStats stats = {12, 34, 56, 78};
    int value = 0;

    BANANA_TEST_ASSERT_INT_EQ(player_builds_parse_class(BUILD_CLASS_VANGUARD, &class_type), 0,
                              "parse_class must accept valid build classes");
    BANANA_TEST_ASSERT_INT_EQ(class_type, BUILD_CLASS_VANGUARD,
                              "parse_class must echo the requested class");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_parse_class(BUILD_CLASS_COUNT, &class_type), -1,
                              "parse_class must reject out-of-range classes");

    BANANA_TEST_ASSERT_INT_EQ(player_builds_parse_gear_slot(GEAR_SLOT_ARMOR, &slot), 0,
                              "parse_gear_slot must accept valid gear slots");
    BANANA_TEST_ASSERT_INT_EQ(slot, GEAR_SLOT_ARMOR,
                              "parse_gear_slot must echo the requested slot");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_parse_gear_slot(GEAR_SLOT_COUNT, &slot), -1,
                              "parse_gear_slot must reject out-of-range slots");

    BANANA_TEST_ASSERT_INT_EQ(player_builds_parse_stat_name("attack", &stat), 0,
                              "parse_stat_name must accept known stat names");
    BANANA_TEST_ASSERT_INT_EQ(stat, BUILD_STAT_ATTACK,
                              "parse_stat_name must map attack to the attack stat");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_parse_stat_name("invalid", &stat), -1,
                              "parse_stat_name must reject unknown stat names");

    BANANA_TEST_ASSERT_INT_EQ(player_builds_stat_value(&stats, BUILD_STAT_DEFENSE, &value), 0,
                              "stat_value must read the requested field");
    BANANA_TEST_ASSERT_INT_EQ(value, 56,
                              "stat_value must return the stored defense value");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_stat_value(&stats, BUILD_STAT_COUNT, &value), -1,
                              "stat_value must reject out-of-range stat selectors");
}

static void test_player_builds_progression_and_combo(void **state)
{
    (void)state;
    BuildStats stats = {0, 0, 0, 0};
    ComboOutcome outcome = {0, 0, 0, 0};
    GearModifier weapon = {3, 4, 1, 2};

    BANANA_TEST_ASSERT_INT_EQ(player_builds_upsert("player-1", BUILD_CLASS_VANGUARD), 0,
                              "upsert must create a build entry");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_set_allocations("player-1", 4, 3, 2), 0,
                              "set_allocations must accept valid point totals");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_equip("player-1", GEAR_SLOT_WEAPON, &weapon), 0,
                              "equip must store a valid gear modifier");
    BANANA_TEST_ASSERT_INT_EQ(player_builds_get_stats("player-1", &stats), 0,
                              "get_stats must return computed stats for a valid build");
    BANANA_TEST_ASSERT_INT_EQ(stats.health, 129,
                              "health must include the defense allocation bonus");
    BANANA_TEST_ASSERT_INT_EQ(stats.attack, 30,
                              "attack must include the offense and gear bonuses");
    BANANA_TEST_ASSERT_INT_EQ(stats.defense, 23,
                              "defense must include the defense allocation and gear bonuses");
    BANANA_TEST_ASSERT_INT_EQ(stats.utility, 14,
                              "utility must include the utility allocation and gear bonuses");

    BANANA_TEST_ASSERT_INT_EQ(player_builds_evaluate_combo("player-1",
                                                           "guard_break",
                                                           "heavy_slash",
                                                           1200,
                                                           1,
                                                           &outcome),
                              1,
                              "evaluate_combo must trigger the configured vanguard combo");
    BANANA_TEST_ASSERT_INT_EQ(outcome.triggered, 1,
                              "evaluate_combo must mark the outcome as triggered");
    BANANA_TEST_ASSERT_INT_EQ(outcome.damage_bonus_pct, 22,
                              "solo combos must use the solo damage bonus");
    BANANA_TEST_ASSERT_INT_EQ(outcome.mitigation_bonus_pct, 8,
                              "combo outcome must surface the mitigation bonus");
    BANANA_TEST_ASSERT_INT_EQ(outcome.party_synergy_bonus_pct, 0,
                              "solo combos must not grant party synergy");

    outcome.triggered = 0;
    BANANA_TEST_ASSERT_INT_EQ(player_builds_evaluate_combo("player-1",
                                                           "guard_break",
                                                           "heavy_slash",
                                                           300,
                                                           1,
                                                           &outcome),
                              0,
                              "evaluate_combo must suppress repeated frame-to-frame spam");
    BANANA_TEST_ASSERT_INT_EQ(outcome.triggered, 0,
                              "suppressed combos must keep the outcome cleared");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_player_builds_parse_and_stat_helpers, player_builds_test_setup, player_builds_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_player_builds_progression_and_combo, player_builds_test_setup, player_builds_test_teardown)
)