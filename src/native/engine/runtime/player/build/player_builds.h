#ifndef BANANA_ENGINE_PLAYER_BUILDS_H
#define BANANA_ENGINE_PLAYER_BUILDS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BUILD_CLASS_VANGUARD = 0,
    BUILD_CLASS_ARCANIST = 1,
    BUILD_CLASS_RANGER = 2,
    BUILD_CLASS_COUNT = 3
} BuildClass;

typedef enum
{
    GEAR_SLOT_WEAPON = 0,
    GEAR_SLOT_ARMOR = 1,
    GEAR_SLOT_TRINKET = 2,
    GEAR_SLOT_COUNT = 3
} GearSlot;

typedef struct
{
    int tier;
    int attack_bonus;
    int defense_bonus;
    int utility_bonus;
} GearModifier;

typedef struct
{
    int health;
    int attack;
    int defense;
    int utility;
} BuildStats;

typedef enum
{
    BUILD_STAT_HEALTH = 0,
    BUILD_STAT_ATTACK = 1,
    BUILD_STAT_DEFENSE = 2,
    BUILD_STAT_UTILITY = 3,
    BUILD_STAT_COUNT = 4
} BuildStat;

typedef struct
{
    int triggered;
    int damage_bonus_pct;
    int mitigation_bonus_pct;
    int party_synergy_bonus_pct;
} ComboOutcome;

int player_builds_init(void);
void player_builds_cleanup(void);

int player_builds_upsert(const char *player_guid, BuildClass class_type);
int player_builds_set_allocations(const char *player_guid, int offense_points, int defense_points, int utility_points);
int player_builds_equip(const char *player_guid, GearSlot slot, const GearModifier *gear);
int player_builds_get_stats(const char *player_guid, BuildStats *out_stats);

int player_builds_parse_class(int class_type, BuildClass *out_class);
int player_builds_parse_gear_slot(int slot, GearSlot *out_slot);
int player_builds_parse_stat_name(const char *stat_name, BuildStat *out_stat);
int player_builds_stat_value(const BuildStats *stats, BuildStat stat, int *out_value);

int player_builds_evaluate_combo(const char *player_guid,
                                 const char *first_skill,
                                 const char *second_skill,
                                 int elapsed_ms,
                                 int party_size,
                                 ComboOutcome *out_outcome);

#ifdef __cplusplus
}
#endif

#endif