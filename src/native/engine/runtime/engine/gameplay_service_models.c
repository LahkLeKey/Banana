#include "gameplay_service_models.h"
#include "gameplay_service_model_catalog.h"

int runtime_gameplay_clamp_biome_index(int war_biome_stage_index)
{
    int model_count = runtime_gameplay_reinforcement_model_catalog_count();

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
            runtime_gameplay_reinforcement_skirmish_model(family,
                                                          behavior_mode,
                                                          biome_index);

        if (behavioral_skirmish_model_id)
            return behavioral_skirmish_model_id;
    }

    if (visual_tier == RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_MYTHIC)
    {
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT)
            return runtime_gameplay_reinforcement_mythic_model(family, biome_index);
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER)
            return runtime_gameplay_reinforcement_mythic_model(family, biome_index);
    }

    if (visual_tier == RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_APEX)
    {
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT)
            return runtime_gameplay_reinforcement_apex_model(family, biome_index);
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE ||
            family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER)
            return runtime_gameplay_reinforcement_apex_model(family, biome_index);
    }

    if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT ||
        family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER)
    {
        const char *behavioral_skirmish_model_id =
            runtime_gameplay_reinforcement_skirmish_model(family,
                                                          behavior_mode,
                                                          biome_index);

        if (behavioral_skirmish_model_id)
            return behavioral_skirmish_model_id;
    }

    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
            return runtime_gameplay_reinforcement_siege_model(family, biome_index);
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
            return runtime_gameplay_reinforcement_siege_model(family, biome_index);
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            return runtime_gameplay_reinforcement_skirmish_model(family,
                                                                 RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                 biome_index);
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            return runtime_gameplay_reinforcement_skirmish_model(family,
                                                                 RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                 biome_index);
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE:
        default:
            return "gameplay/reference/banana-basic-v1";
    }
}