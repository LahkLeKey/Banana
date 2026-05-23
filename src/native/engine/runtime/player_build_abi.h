#ifndef BANANA_ENGINE_RUNTIME_PLAYER_BUILD_ABI_H
#define BANANA_ENGINE_RUNTIME_PLAYER_BUILD_ABI_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_player_build_abi_set_class(const char *player_guid,
                                           int class_type);

    int runtime_player_build_abi_set_allocations(const char *player_guid,
                                                 int offense_points,
                                                 int defense_points,
                                                 int utility_points);

    int runtime_player_build_abi_equip(const char *player_guid,
                                       int slot,
                                       int tier,
                                       int attack_bonus,
                                       int defense_bonus,
                                       int utility_bonus);

    int runtime_player_build_abi_get_stat(const char *player_guid,
                                          const char *stat_name);

    int runtime_player_build_abi_evaluate_combo(const char *player_guid,
                                                const char *first_skill,
                                                const char *second_skill,
                                                int elapsed_ms,
                                                int party_size,
                                                int *out_damage_bonus_pct,
                                                int *out_mitigation_bonus_pct,
                                                int *out_party_synergy_bonus_pct);

#ifdef __cplusplus
}
#endif

#endif