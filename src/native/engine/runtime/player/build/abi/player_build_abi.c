#include "player_build_abi.h"

#include "../../player_builds.h"

int runtime_player_build_abi_set_class(const char *player_guid,
                                       int class_type)
{
    BuildClass parsed_class;

    if (player_builds_parse_class(class_type, &parsed_class) != 0)
        return -1;

    return player_builds_upsert(player_guid, parsed_class);
}

int runtime_player_build_abi_set_allocations(const char *player_guid,
                                             int offense_points,
                                             int defense_points,
                                             int utility_points)
{
    return player_builds_set_allocations(player_guid,
                                         offense_points,
                                         defense_points,
                                         utility_points);
}

int runtime_player_build_abi_equip(const char *player_guid,
                                   int slot,
                                   int tier,
                                   int attack_bonus,
                                   int defense_bonus,
                                   int utility_bonus)
{
    GearSlot parsed_slot;
    GearModifier gear;

    if (player_builds_parse_gear_slot(slot, &parsed_slot) != 0)
        return -1;

    gear.tier = tier;
    gear.attack_bonus = attack_bonus;
    gear.defense_bonus = defense_bonus;
    gear.utility_bonus = utility_bonus;
    return player_builds_equip(player_guid, parsed_slot, &gear);
}

int runtime_player_build_abi_get_stat(const char *player_guid,
                                      const char *stat_name)
{
    BuildStat stat;
    BuildStats stats;
    int stat_value = -1;

    if (player_builds_parse_stat_name(stat_name, &stat) != 0 ||
        player_builds_get_stats(player_guid, &stats) != 0)
    {
        return -1;
    }

    if (player_builds_stat_value(&stats, stat, &stat_value) != 0)
        return -1;

    return stat_value;
}

int runtime_player_build_abi_evaluate_combo(const char *player_guid,
                                            const char *first_skill,
                                            const char *second_skill,
                                            int elapsed_ms,
                                            int party_size,
                                            int *out_damage_bonus_pct,
                                            int *out_mitigation_bonus_pct,
                                            int *out_party_synergy_bonus_pct)
{
    ComboOutcome outcome;
    int result = player_builds_evaluate_combo(player_guid,
                                              first_skill,
                                              second_skill,
                                              elapsed_ms,
                                              party_size,
                                              &outcome);
    if (result < 0)
        return -1;

    if (out_damage_bonus_pct)
        *out_damage_bonus_pct = outcome.damage_bonus_pct;
    if (out_mitigation_bonus_pct)
        *out_mitigation_bonus_pct = outcome.mitigation_bonus_pct;
    if (out_party_synergy_bonus_pct)
        *out_party_synergy_bonus_pct = outcome.party_synergy_bonus_pct;
    return outcome.triggered;
}