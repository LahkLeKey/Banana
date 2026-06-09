#include "gameplay_service_warfront.h"

#include <string.h>

int runtime_gameplay_count_active_war_npcs(const World *world)
{
    int count = 0;

    if (!world)
        return 0;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];
        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strcmp(entity->controller_kind, "combat") == 0 ||
            strcmp(entity->controller_kind, "wildlife") == 0)
        {
            count += 1;
        }
    }

    return count;
}

int runtime_gameplay_count_team_controllers(ControllerInstance **controllers,
                                           int controller_count,
                                           ControllerTeam team)
{
    int count = 0;

    if (!controllers || controller_count <= 0)
        return 0;

    for (int i = 0; i < controller_count; i++)
    {
        const ControllerInstance *controller = controllers[i];
        if (!controller)
            continue;
        if (controller->team == team)
            count += 1;
    }

    return count;
}

int runtime_gameplay_find_active_warfront(ControllerInstance **controllers,
                                         int controller_count,
                                         float trigger_radius,
                                         ControllerInstance **out_banana,
                                         ControllerInstance **out_bean)
{
    int found = 0;
    float best_dist_sq = 0.0f;
    float radius_sq = 0.0f;

    if (!controllers || controller_count <= 1 || !out_banana || !out_bean || trigger_radius <= 0.0f)
        return 0;

    *out_banana = NULL;
    *out_bean = NULL;
    radius_sq = trigger_radius * trigger_radius;

    for (int i = 0; i < controller_count; i++)
    {
        ControllerInstance *first = controllers[i];
        if (!first)
            continue;

        for (int j = i + 1; j < controller_count; j++)
        {
            ControllerInstance *second = controllers[j];
            float dx = 0.0f;
            float dz = 0.0f;
            float dist_sq = 0.0f;

            if (!second)
                continue;
            if (!controller_teams_are_hostile(first->team, second->team))
                continue;

            dx = first->position[0] - second->position[0];
            dz = first->position[2] - second->position[2];
            dist_sq = (dx * dx) + (dz * dz);
            if (dist_sq > radius_sq)
                continue;

            if (!found || dist_sq < best_dist_sq)
            {
                found = 1;
                best_dist_sq = dist_sq;

                if (first->team == CONTROLLER_TEAM_BANANA)
                {
                    *out_banana = first;
                    *out_bean = second;
                }
                else
                {
                    *out_banana = second;
                    *out_bean = first;
                }
            }
        }
    }

    return found;
}