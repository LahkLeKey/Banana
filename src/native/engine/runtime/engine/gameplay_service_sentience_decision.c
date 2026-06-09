#include "gameplay_service_sentience_decision.h"

#include "../tick/tick_budget_policy.h"

RuntimeWarSentienceBehaviorMode runtime_gameplay_resolve_sentience_behavior_mode(
    const EngineRuntimeState *runtime_state,
    int team_count,
    int opposing_team_count)
{
    int pressure_gap = opposing_team_count - team_count;
    int parity_gap = team_count - opposing_team_count;
    int humanoid_index = 0;
    int coordination = 0;
    int empathy = 0;

    if (parity_gap < 0)
        parity_gap = -parity_gap;

    if (runtime_state)
    {
        humanoid_index = runtime_state->war_sentience_humanoid_index;
        coordination = runtime_state->war_sentience_coordination_level;
        empathy = runtime_state->war_sentience_empathy_level;
    }

    if (humanoid_index >= 8 && empathy >= 6 && parity_gap <= 1)
        return RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;

    if (pressure_gap >= 2)
        return RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP;

    if (coordination >= 6 && pressure_gap <= 0)
        return RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK;

    return RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
}

int runtime_gameplay_humanoid_coordination_comeback_bonus(const EngineRuntimeState *runtime_state,
                                                         int banana_team_count,
                                                         int bean_team_count)
{
    int per_coordination = 0;
    int bonus = 0;

    if (!runtime_state)
        return 0;
    if (bean_team_count <= banana_team_count)
        return 0;

    per_coordination = runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination();
    if (per_coordination <= 0)
        return 0;

    bonus = runtime_state->war_sentience_coordination_level / per_coordination;
    bonus += runtime_state->war_sentience_empathy_level / ((per_coordination * 2) + 1);

    if (bonus < 0)
        bonus = 0;
    if (bonus > 4)
        bonus = 4;

    return bonus;
}