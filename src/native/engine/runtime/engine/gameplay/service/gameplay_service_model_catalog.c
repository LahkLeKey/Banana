#include "gameplay_service_model_catalog.h"

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

int runtime_gameplay_reinforcement_model_catalog_count(void)
{
    return (int)(sizeof(k_banana_skirmish_models) / sizeof(k_banana_skirmish_models[0]));
}

const char *runtime_gameplay_reinforcement_skirmish_model(RuntimeWarReinforcementFamily family,
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

const char *runtime_gameplay_reinforcement_siege_model(RuntimeWarReinforcementFamily family,
                                                       int biome_index)
{
    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
            return k_banana_siege_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
            return k_bean_siege_models[biome_index];
        default:
            return NULL;
    }
}

const char *runtime_gameplay_reinforcement_apex_model(RuntimeWarReinforcementFamily family,
                                                      int biome_index)
{
    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            return k_banana_apex_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            return k_bean_apex_models[biome_index];
        default:
            return NULL;
    }
}

const char *runtime_gameplay_reinforcement_mythic_model(RuntimeWarReinforcementFamily family,
                                                        int biome_index)
{
    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            return k_banana_mythic_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            return k_bean_mythic_models[biome_index];
        default:
            return NULL;
    }
}