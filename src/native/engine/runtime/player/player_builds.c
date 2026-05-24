#include "player_builds.h"

#include <stddef.h>
#include <string.h>

#define MAX_BUILD_PLAYERS 256
#define MAX_GUID_LEN 64
#define MAX_ALLOCATION_POINTS 20

typedef struct
{
    BuildClass class_type;
    int offense_points;
    int defense_points;
    int utility_points;
    int has_gear[GEAR_SLOT_COUNT];
    GearModifier gear[GEAR_SLOT_COUNT];
    int last_combo_rule;
} PlayerBuildEntry;

typedef struct
{
    char guid[MAX_GUID_LEN];
    PlayerBuildEntry build;
} BuildRegistryEntry;

typedef struct
{
    BuildClass class_type;
    int health;
    int attack;
    int defense;
    int utility;
} ClassBaseStats;

typedef struct
{
    BuildClass class_type;
    const char *first_skill;
    const char *second_skill;
    int max_window_ms;
    int solo_damage_bonus_pct;
    int party_damage_bonus_pct;
    int party_synergy_bonus_pct;
    int mitigation_bonus_pct;
} ComboRule;

static const ClassBaseStats k_base_stats[BUILD_CLASS_COUNT] = {
    {BUILD_CLASS_VANGUARD, 120, 18, 16, 8},
    {BUILD_CLASS_ARCANIST, 90, 24, 8, 18},
    {BUILD_CLASS_RANGER, 100, 20, 12, 14},
};

static const ComboRule k_combo_rules[] = {
    {BUILD_CLASS_VANGUARD, "guard_break", "heavy_slash", 1400, 22, 16, 10, 8},
    {BUILD_CLASS_ARCANIST, "mana_mark", "arc_burst", 1300, 24, 18, 12, 0},
    {BUILD_CLASS_RANGER, "snare_shot", "piercing_volley", 1200, 20, 17, 9, 0},
};

static struct
{
    int count;
    BuildRegistryEntry entries[MAX_BUILD_PLAYERS];
} g_build_registry;

static int is_valid_guid(const char *player_guid)
{
    size_t len;
    if (player_guid == NULL)
    {
        return 0;
    }

    len = strlen(player_guid);
    return len > 0 && len < MAX_GUID_LEN;
}

static int is_valid_class(BuildClass class_type)
{
    return class_type >= 0 && class_type < BUILD_CLASS_COUNT;
}

static int find_index(const char *player_guid)
{
    int i;
    for (i = 0; i < g_build_registry.count; ++i)
    {
        if (strcmp(g_build_registry.entries[i].guid, player_guid) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int get_or_create_index(const char *player_guid)
{
    int idx = find_index(player_guid);
    if (idx >= 0)
    {
        return idx;
    }

    if (g_build_registry.count >= MAX_BUILD_PLAYERS)
    {
        return -1;
    }

    idx = g_build_registry.count++;
    memset(&g_build_registry.entries[idx], 0, sizeof(g_build_registry.entries[idx]));
    strncpy(g_build_registry.entries[idx].guid, player_guid, MAX_GUID_LEN - 1);
    g_build_registry.entries[idx].build.last_combo_rule = -1;
    return idx;
}

int player_builds_init(void)
{
    memset(&g_build_registry, 0, sizeof(g_build_registry));
    return 0;
}

void player_builds_cleanup(void)
{
    memset(&g_build_registry, 0, sizeof(g_build_registry));
}

int player_builds_upsert(const char *player_guid, BuildClass class_type)
{
    int idx;
    if (!is_valid_guid(player_guid) || !is_valid_class(class_type))
    {
        return -1;
    }

    idx = get_or_create_index(player_guid);
    if (idx < 0)
    {
        return -1;
    }

    /* Preserve existing allocations/gear when class is unchanged (reconnect/upsert). */
    if (is_valid_class(g_build_registry.entries[idx].build.class_type) &&
        g_build_registry.entries[idx].build.class_type == class_type)
    {
        return 0;
    }

    g_build_registry.entries[idx].build.class_type = class_type;
    g_build_registry.entries[idx].build.offense_points = 0;
    g_build_registry.entries[idx].build.defense_points = 0;
    g_build_registry.entries[idx].build.utility_points = 0;
    memset(g_build_registry.entries[idx].build.has_gear, 0, sizeof(g_build_registry.entries[idx].build.has_gear));
    memset(g_build_registry.entries[idx].build.gear, 0, sizeof(g_build_registry.entries[idx].build.gear));
    g_build_registry.entries[idx].build.last_combo_rule = -1;
    return 0;
}

int player_builds_set_allocations(const char *player_guid, int offense_points, int defense_points, int utility_points)
{
    int idx;
    int total;
    if (!is_valid_guid(player_guid) || offense_points < 0 || defense_points < 0 || utility_points < 0)
    {
        return -1;
    }

    total = offense_points + defense_points + utility_points;
    if (total > MAX_ALLOCATION_POINTS)
    {
        return -1;
    }

    idx = find_index(player_guid);
    if (idx < 0)
    {
        return -1;
    }

    g_build_registry.entries[idx].build.offense_points = offense_points;
    g_build_registry.entries[idx].build.defense_points = defense_points;
    g_build_registry.entries[idx].build.utility_points = utility_points;
    return 0;
}

int player_builds_equip(const char *player_guid, GearSlot slot, const GearModifier *gear)
{
    int idx;
    if (!is_valid_guid(player_guid) || !gear || slot < 0 || slot >= GEAR_SLOT_COUNT)
    {
        return -1;
    }
    if (gear->tier < 0 || gear->tier > 10)
    {
        return -1;
    }

    idx = find_index(player_guid);
    if (idx < 0)
    {
        return -1;
    }

    g_build_registry.entries[idx].build.gear[slot] = *gear;
    g_build_registry.entries[idx].build.has_gear[slot] = 1;
    return 0;
}

int player_builds_get_stats(const char *player_guid, BuildStats *out_stats)
{
    int idx;
    int slot;
    PlayerBuildEntry *build;
    const ClassBaseStats *base;

    if (!is_valid_guid(player_guid) || !out_stats)
    {
        return -1;
    }

    idx = find_index(player_guid);
    if (idx < 0)
    {
        return -1;
    }

    build = &g_build_registry.entries[idx].build;
    if (!is_valid_class(build->class_type))
    {
        return -1;
    }
    base = &k_base_stats[build->class_type];

    out_stats->health = base->health + (build->defense_points * 3);
    out_stats->attack = base->attack + (build->offense_points * 2);
    out_stats->defense = base->defense + (build->defense_points * 2);
    out_stats->utility = base->utility + (build->utility_points * 2);

    for (slot = 0; slot < GEAR_SLOT_COUNT; ++slot)
    {
        if (!build->has_gear[slot])
        {
            continue;
        }
        out_stats->attack += build->gear[slot].attack_bonus;
        out_stats->defense += build->gear[slot].defense_bonus;
        out_stats->utility += build->gear[slot].utility_bonus;
    }

    return 0;
}

int player_builds_parse_class(int class_type, BuildClass *out_class)
{
    BuildClass parsed = (BuildClass)class_type;

    if (!out_class || !is_valid_class(parsed))
    {
        return -1;
    }

    *out_class = parsed;
    return 0;
}

int player_builds_parse_gear_slot(int slot, GearSlot *out_slot)
{
    GearSlot parsed = (GearSlot)slot;

    if (!out_slot || parsed < 0 || parsed >= GEAR_SLOT_COUNT)
    {
        return -1;
    }

    *out_slot = parsed;
    return 0;
}

int player_builds_parse_stat_name(const char *stat_name, BuildStat *out_stat)
{
    if (!stat_name || !out_stat)
    {
        return -1;
    }

    if (strcmp(stat_name, "health") == 0)
    {
        *out_stat = BUILD_STAT_HEALTH;
        return 0;
    }
    if (strcmp(stat_name, "attack") == 0)
    {
        *out_stat = BUILD_STAT_ATTACK;
        return 0;
    }
    if (strcmp(stat_name, "defense") == 0)
    {
        *out_stat = BUILD_STAT_DEFENSE;
        return 0;
    }
    if (strcmp(stat_name, "utility") == 0)
    {
        *out_stat = BUILD_STAT_UTILITY;
        return 0;
    }

    return -1;
}

int player_builds_stat_value(const BuildStats *stats, BuildStat stat, int *out_value)
{
    if (!stats || !out_value || stat < 0 || stat >= BUILD_STAT_COUNT)
    {
        return -1;
    }

    switch (stat)
    {
    case BUILD_STAT_HEALTH:
        *out_value = stats->health;
        return 0;
    case BUILD_STAT_ATTACK:
        *out_value = stats->attack;
        return 0;
    case BUILD_STAT_DEFENSE:
        *out_value = stats->defense;
        return 0;
    case BUILD_STAT_UTILITY:
        *out_value = stats->utility;
        return 0;
    default:
        return -1;
    }
}

int player_builds_evaluate_combo(const char *player_guid,
                                 const char *first_skill,
                                 const char *second_skill,
                                 int elapsed_ms,
                                 int party_size,
                                 ComboOutcome *out_outcome)
{
    int idx;
    int rule_idx;
    PlayerBuildEntry *build;

    if (!is_valid_guid(player_guid) || !first_skill || !second_skill || !out_outcome)
    {
        return -1;
    }

    if (first_skill[0] == '\0' || second_skill[0] == '\0' || party_size <= 0)
    {
        return -1;
    }

    memset(out_outcome, 0, sizeof(*out_outcome));

    idx = find_index(player_guid);
    if (idx < 0)
    {
        return -1;
    }
    build = &g_build_registry.entries[idx].build;

    for (rule_idx = 0; rule_idx < (int)(sizeof(k_combo_rules) / sizeof(k_combo_rules[0])); ++rule_idx)
    {
        const ComboRule *rule = &k_combo_rules[rule_idx];
        if (rule->class_type != build->class_type)
        {
            continue;
        }
        if (strcmp(rule->first_skill, first_skill) != 0 || strcmp(rule->second_skill, second_skill) != 0)
        {
            continue;
        }
        if (elapsed_ms <= 0 || elapsed_ms > rule->max_window_ms)
        {
            return 0;
        }

        /* Guard against low-effort spam of the same combo frame-to-frame. */
        if (build->last_combo_rule == rule_idx && elapsed_ms <= 450)
        {
            return 0;
        }

        build->last_combo_rule = rule_idx;
        out_outcome->triggered = 1;
        out_outcome->damage_bonus_pct = (party_size > 1) ? rule->party_damage_bonus_pct : rule->solo_damage_bonus_pct;
        out_outcome->mitigation_bonus_pct = rule->mitigation_bonus_pct;
        out_outcome->party_synergy_bonus_pct = (party_size > 1) ? rule->party_synergy_bonus_pct : 0;
        return 1;
    }

    return 0;
}