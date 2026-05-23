#include "player_builds.h"

#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT_EQ(actual, expected, label)                                                     \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((actual) == (expected))                                                            \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected %d got %d)\n", label, (expected), (actual));         \
        }                                                                                      \
    } while (0)

static void test_base_class_stats(void)
{
    BuildStats stats;
    player_builds_init();

    ASSERT_EQ(player_builds_upsert("p1", BUILD_CLASS_VANGUARD), 0, "upsert vanguard");
    ASSERT_EQ(player_builds_get_stats("p1", &stats), 0, "get stats");
    ASSERT_EQ(stats.health, 120, "vanguard health");
    ASSERT_EQ(stats.attack, 18, "vanguard attack");
    ASSERT_EQ(stats.defense, 16, "vanguard defense");
    ASSERT_EQ(stats.utility, 8, "vanguard utility");

    player_builds_cleanup();
}

static void test_allocations_and_gear_tradeoffs(void)
{
    BuildStats stats;
    GearModifier weapon = {2, 6, 0, 0};
    GearModifier armor = {3, -1, 5, 0};
    GearModifier trinket = {1, 0, 0, 4};

    player_builds_init();
    ASSERT_EQ(player_builds_upsert("p2", BUILD_CLASS_RANGER), 0, "upsert ranger");
    ASSERT_EQ(player_builds_set_allocations("p2", 4, 2, 1), 0, "set allocations");
    ASSERT_EQ(player_builds_equip("p2", GEAR_SLOT_WEAPON, &weapon), 0, "equip weapon");
    ASSERT_EQ(player_builds_equip("p2", GEAR_SLOT_ARMOR, &armor), 0, "equip armor");
    ASSERT_EQ(player_builds_equip("p2", GEAR_SLOT_TRINKET, &trinket), 0, "equip trinket");
    ASSERT_EQ(player_builds_get_stats("p2", &stats), 0, "get geared stats");

    ASSERT_EQ(stats.health, 106, "ranger health with defense points");
    ASSERT_EQ(stats.attack, 33, "ranger attack with offense and gear");
    ASSERT_EQ(stats.defense, 21, "ranger defense with armor and points");
    ASSERT_EQ(stats.utility, 20, "ranger utility with trinket and points");

    player_builds_cleanup();
}

static void test_combo_solo_and_party(void)
{
    ComboOutcome outcome;

    player_builds_init();

    ASSERT_EQ(player_builds_upsert("solo-vg", BUILD_CLASS_VANGUARD), 0, "upsert vanguard combo");
    ASSERT_EQ(player_builds_evaluate_combo("solo-vg", "guard_break", "heavy_slash", 900, 1, &outcome), 1,
              "solo vanguard combo triggered");
    ASSERT_EQ(outcome.triggered, 1, "solo combo triggered flag");
    ASSERT_EQ(outcome.damage_bonus_pct, 22, "solo damage bonus");
    ASSERT_EQ(outcome.mitigation_bonus_pct, 8, "solo mitigation bonus");
    ASSERT_EQ(outcome.party_synergy_bonus_pct, 0, "solo no party synergy");

    ASSERT_EQ(player_builds_upsert("party-arc", BUILD_CLASS_ARCANIST), 0, "upsert arcanist combo");
    ASSERT_EQ(player_builds_evaluate_combo("party-arc", "mana_mark", "arc_burst", 1000, 4, &outcome), 1,
              "party arcanist combo triggered");
    ASSERT_EQ(outcome.triggered, 1, "party combo triggered flag");
    ASSERT_EQ(outcome.damage_bonus_pct, 18, "party damage bonus");
    ASSERT_EQ(outcome.party_synergy_bonus_pct, 12, "party synergy bonus");

    player_builds_cleanup();
}

static void test_combo_timeout_and_repeat_guard(void)
{
    ComboOutcome outcome;

    player_builds_init();
    ASSERT_EQ(player_builds_upsert("p3", BUILD_CLASS_RANGER), 0, "upsert ranger combo");

    ASSERT_EQ(player_builds_evaluate_combo("p3", "snare_shot", "piercing_volley", 1100, 2, &outcome), 1,
              "first ranger combo");
    ASSERT_EQ(player_builds_evaluate_combo("p3", "snare_shot", "piercing_volley", 300, 2, &outcome), 0,
              "repeat guard blocks immediate spam");
    ASSERT_EQ(player_builds_evaluate_combo("p3", "snare_shot", "piercing_volley", 1500, 2, &outcome), 0,
              "timeout blocks combo");

    player_builds_cleanup();
}

static void test_validation_guards(void)
{
    char long_guid[96];
    ComboOutcome outcome;
    GearModifier bad_tier = {11, 1, 0, 0};

    memset(long_guid, 'a', sizeof(long_guid));
    long_guid[sizeof(long_guid) - 1] = '\0';

    player_builds_init();

    ASSERT_EQ(player_builds_upsert("", BUILD_CLASS_VANGUARD), -1, "empty guid rejected");
    ASSERT_EQ(player_builds_upsert(long_guid, BUILD_CLASS_VANGUARD), -1, "oversized guid rejected");
    ASSERT_EQ(player_builds_upsert("p4", BUILD_CLASS_RANGER), 0, "valid upsert for validation checks");
    ASSERT_EQ(player_builds_set_allocations("p4", 8, 8, 8), -1, "allocation cap enforced");
    ASSERT_EQ(player_builds_equip("p4", GEAR_SLOT_WEAPON, &bad_tier), -1, "gear tier range enforced");
    ASSERT_EQ(player_builds_evaluate_combo("p4", "", "piercing_volley", 1000, 2, &outcome), -1,
              "empty first skill rejected");
    ASSERT_EQ(player_builds_evaluate_combo("p4", "snare_shot", "piercing_volley", 1000, 0, &outcome), -1,
              "non-positive party size rejected");

    player_builds_cleanup();
}

static void test_typed_domain_parsing(void)
{
    BuildClass build_class;
    GearSlot slot;
    BuildStat stat;
    BuildStats stats = {150, 24, 18, 11};
    int value = 0;

    ASSERT_EQ(player_builds_parse_class(BUILD_CLASS_ARCANIST, &build_class), 0, "parse build class");
    ASSERT_EQ((int)build_class, BUILD_CLASS_ARCANIST, "parsed build class value");
    ASSERT_EQ(player_builds_parse_class(99, &build_class), -1, "reject invalid class");

    ASSERT_EQ(player_builds_parse_gear_slot(GEAR_SLOT_TRINKET, &slot), 0, "parse gear slot");
    ASSERT_EQ((int)slot, GEAR_SLOT_TRINKET, "parsed gear slot value");
    ASSERT_EQ(player_builds_parse_gear_slot(-1, &slot), -1, "reject invalid gear slot");

    ASSERT_EQ(player_builds_parse_stat_name("defense", &stat), 0, "parse defense stat");
    ASSERT_EQ((int)stat, BUILD_STAT_DEFENSE, "parsed stat enum");
    ASSERT_EQ(player_builds_parse_stat_name("unknown", &stat), -1, "reject unknown stat");

    ASSERT_EQ(player_builds_stat_value(&stats, BUILD_STAT_ATTACK, &value), 0, "read attack stat");
    ASSERT_EQ(value, 24, "attack stat value");
    ASSERT_EQ(player_builds_stat_value(&stats, BUILD_STAT_COUNT, &value), -1, "reject out-of-range stat enum");
}

int main(void)
{
    printf("=== Player Builds Tests ===\n\n");

    test_base_class_stats();
    test_allocations_and_gear_tradeoffs();
    test_combo_solo_and_party();
    test_combo_timeout_and_repeat_guard();
    test_validation_guards();
    test_typed_domain_parsing();

    printf("\n=== Results ===\n");
    printf("Passed: %d / %d\n", tests_passed, tests_run);
    return (tests_run == tests_passed) ? 0 : 1;
}