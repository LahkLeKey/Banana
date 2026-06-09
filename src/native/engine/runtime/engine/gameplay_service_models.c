#include "gameplay_service_models.h"

static const char *k_banana_skirmish_models[] = {
    "gameplay/war/banana-scout-tropical-v1",
    "gameplay/war/banana-scout-glacier-v1",
    "gameplay/war/banana-scout-urban-v1",
    "gameplay/war/banana-scout-volcanic-v1",
};

static const char *k_banana_skirmish_flank_models[] = {
    "gameplay/war/banana-scout-flank-tropical-v1",
    "gameplay/war/banana-scout-flank-glacier-v1",
    "gameplay/war/banana-scout-flank-urban-v1",
    "gameplay/war/banana-scout-flank-volcanic-v1",
};

static const char *k_banana_skirmish_regroup_models[] = {
    "gameplay/war/banana-scout-regroup-tropical-v1",
    "gameplay/war/banana-scout-regroup-glacier-v1",
    "gameplay/war/banana-scout-regroup-urban-v1",
    "gameplay/war/banana-scout-regroup-volcanic-v1",
};

static const char *k_banana_skirmish_negotiate_models[] = {
    "gameplay/war/banana-scout-envoy-tropical-v1",
    "gameplay/war/banana-scout-envoy-glacier-v1",
    "gameplay/war/banana-scout-envoy-urban-v1",
    "gameplay/war/banana-scout-envoy-volcanic-v1",
};

static const char *k_banana_siege_models[] = {
    "gameplay/war/banana-siege-commander-tropical-v1",
    "gameplay/war/banana-siege-commander-glacier-v1",
    "gameplay/war/banana-siege-commander-urban-v1",
    "gameplay/war/banana-siege-commander-volcanic-v1",
};

static const char *k_bean_skirmish_models[] = {
    "gameplay/war/bean-raider-tropical-v1",
    "gameplay/war/bean-raider-glacier-v1",
    "gameplay/war/bean-raider-urban-v1",
    "gameplay/war/bean-raider-volcanic-v1",
};

static const char *k_bean_skirmish_flank_models[] = {
    "gameplay/war/bean-raider-flank-tropical-v1",
    "gameplay/war/bean-raider-flank-glacier-v1",
    "gameplay/war/bean-raider-flank-urban-v1",
    "gameplay/war/bean-raider-flank-volcanic-v1",
};

static const char *k_bean_skirmish_regroup_models[] = {
    "gameplay/war/bean-raider-regroup-tropical-v1",
    "gameplay/war/bean-raider-regroup-glacier-v1",
    "gameplay/war/bean-raider-regroup-urban-v1",
    "gameplay/war/bean-raider-regroup-volcanic-v1",
};

static const char *k_bean_skirmish_negotiate_models[] = {
    "gameplay/war/bean-raider-envoy-tropical-v1",
    "gameplay/war/bean-raider-envoy-glacier-v1",
    "gameplay/war/bean-raider-envoy-urban-v1",
    "gameplay/war/bean-raider-envoy-volcanic-v1",
};

static const char *k_bean_siege_models[] = {
    "gameplay/war/bean-warbrute-tropical-v1",
    "gameplay/war/bean-warbrute-glacier-v1",
    "gameplay/war/bean-warbrute-urban-v1",
    "gameplay/war/bean-warbrute-volcanic-v1",
};

static const char *k_banana_apex_models[] = {
    "gameplay/war/banana-phalanx-tropical-v1",
    "gameplay/war/banana-phalanx-glacier-v1",
    "gameplay/war/banana-phalanx-urban-v1",
    "gameplay/war/banana-phalanx-volcanic-v1",
};

static const char *k_bean_apex_models[] = {
    "gameplay/war/bean-colossus-tropical-v1",
    "gameplay/war/bean-colossus-glacier-v1",
    "gameplay/war/bean-colossus-urban-v1",
    "gameplay/war/bean-colossus-volcanic-v1",
};

static const char *k_banana_mythic_models[] = {
    "gameplay/war/banana-archon-tropical-v1",
    "gameplay/war/banana-archon-glacier-v1",
    "gameplay/war/banana-archon-urban-v1",
    "gameplay/war/banana-archon-volcanic-v1",
};

static const char *k_bean_mythic_models[] = {
    "gameplay/war/bean-leviathan-tropical-v1",
    "gameplay/war/bean-leviathan-glacier-v1",
    "gameplay/war/bean-leviathan-urban-v1",
    "gameplay/war/bean-leviathan-volcanic-v1",
};

int runtime_gameplay_clamp_biome_index(int war_biome_stage_index)
{
    int model_count = (int)(sizeof(k_banana_skirmish_models) / sizeof(k_banana_skirmish_models[0]));

    if (model_count <= 0)
        return 0;

    if (war_biome_stage_index < 0)
        war_biome_stage_index = -war_biome_stage_index;

    return war_biome_stage_index % model_count;
}

int runtime_gameplay_clamp_sentience_mode_index(RuntimeWarSentienceBehaviorMode behavior_mode)
{
    int mode_index = (int)behavior_mode;

    if (mode_index < 0 || mode_index >= BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT)
        return (int)RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;

    return mode_index;
}

RuntimeWarReinforcementFamily runtime_gameplay_reinforcement_family_for_team(ControllerTeam team,
                                                                             int war_escalation_tier,
                                                                             int war_intelligence_stage)
{
    int effective_tier = war_escalation_tier;

    if (war_intelligence_stage >= 2 && effective_tier < 2)
        effective_tier += 1;

    if (team == CONTROLLER_TEAM_BANANA)
    {
        if (effective_tier >= 2)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE;
        if (effective_tier == 1)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT;
        return RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
    }

    if (team == CONTROLLER_TEAM_BEAN)
    {
        if (effective_tier >= 2)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE;
        if (effective_tier == 1)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER;
        return RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
    }

    return RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
}

RuntimeWarReinforcementVisualTier runtime_gameplay_reinforcement_visual_tier_for_family(
    RuntimeWarReinforcementFamily family,
    int war_intelligence_stage)
{
    if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT ||
        family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE ||
        family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER ||
        family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE)
    {
        if (war_intelligence_stage >= 5)
            return RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_MYTHIC;
        if (war_intelligence_stage >= 3)
            return RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_APEX;
    }

    return RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_BASE;
}

static int runtime_gameplay_sentience_mode_prefers_behavioral_art(RuntimeWarSentienceBehaviorMode behavior_mode)
{
    return behavior_mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;
}

static const char *runtime_gameplay_behavioral_skirmish_model_id_for_family(
    RuntimeWarReinforcementFamily family,
    RuntimeWarSentienceBehaviorMode behavior_mode,
    int biome_index)
{
    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            switch (behavior_mode)
            {
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
                    return k_banana_skirmish_flank_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
                    return k_banana_skirmish_regroup_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
                    return k_banana_skirmish_negotiate_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
                default:
                    return k_banana_skirmish_models[biome_index];
            }

        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            switch (behavior_mode)
            {
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
                    return k_bean_skirmish_flank_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
                    return k_bean_skirmish_regroup_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
                    return k_bean_skirmish_negotiate_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
                default:
                    return k_bean_skirmish_models[biome_index];
            }

        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE:
        default:
            return NULL;
    }
}

const char *runtime_gameplay_reinforcement_model_id_for_family(RuntimeWarReinforcementFamily family,
                                                               RuntimeWarSentienceBehaviorMode behavior_mode,
                                                               int war_intelligence_stage,
                                                               int biome_index)
{
    RuntimeWarReinforcementVisualTier visual_tier =
        runtime_gameplay_reinforcement_visual_tier_for_family(family, war_intelligence_stage);

    if ((family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT ||
         family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER) &&
        runtime_gameplay_sentience_mode_prefers_behavioral_art(behavior_mode))
    {
        const char *behavioral_skirmish_model_id =
            runtime_gameplay_behavioral_skirmish_model_id_for_family(family,
                                                                     behavior_mode,
                                                                     biome_index);

        if (behavioral_skirmish_model_id)
            return behavioral_skirmish_model_id;
    }

    if (visual_tier == RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_MYTHIC)
    {
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT)
            return k_banana_mythic_models[biome_index];
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER)
            return k_bean_mythic_models[biome_index];
    }

    if (visual_tier == RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_APEX)
    {
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT)
            return k_banana_apex_models[biome_index];
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER)
            return k_bean_apex_models[biome_index];
    }

    if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT ||
        family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER)
    {
        const char *behavioral_skirmish_model_id =
            runtime_gameplay_behavioral_skirmish_model_id_for_family(family,
                                                                     behavior_mode,
                                                                     biome_index);

        if (behavioral_skirmish_model_id)
            return behavioral_skirmish_model_id;
    }

    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
            return k_banana_siege_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
            return k_bean_siege_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            return k_banana_skirmish_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            return k_bean_skirmish_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE:
        default:
            return "gameplay/reference/banana-basic-v1";
    }
}