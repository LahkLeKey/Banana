#include "controller_runtime.h"

#include <stddef.h>

ControllerInstance *runtime_controller_find_by_id(ControllerInstance **controllers,
                                                  int controller_count,
                                                  uint32_t controller_id)
{
    int i = 0;

    if (!controllers || controller_count <= 0 || controller_id == 0)
        return NULL;

    for (i = 0; i < controller_count; i++)
    {
        ControllerInstance *controller = controllers[i];
        if (!controller)
            continue;
        if (controller->id == controller_id)
            return controller;
    }

    return NULL;
}

int runtime_controller_update_by_id(ControllerInstance **controllers,
                                    int controller_count,
                                    uint32_t controller_id,
                                    float dt)
{
    ControllerInstance *controller = runtime_controller_find_by_id(controllers,
                                                                   controller_count,
                                                                   controller_id);

    if (!controller)
        return 0;

    controller_update(controller, dt);
    return 1;
}

int runtime_controller_signal_by_id(ControllerInstance **controllers,
                                    int controller_count,
                                    uint32_t controller_id,
                                    const char *signal_name,
                                    const void *data)
{
    ControllerInstance *controller = runtime_controller_find_by_id(controllers,
                                                                   controller_count,
                                                                   controller_id);

    if (!controller)
        return 0;

    controller_signal(controller, signal_name, data);
    return 1;
}

int runtime_controller_assign_team_by_id(ControllerInstance **controllers,
                                         int controller_count,
                                         uint32_t controller_id,
                                         ControllerTeam team)
{
    ControllerInstance *controller = runtime_controller_find_by_id(controllers,
                                                                   controller_count,
                                                                   controller_id);

    if (!controller)
        return 0;

    controller->team = team;
    return 1;
}

int runtime_controller_signal_team_war(ControllerInstance **controllers,
                                       int controller_count,
                                       float trigger_radius,
                                       const char *signal_name)
{
    int i = 0;
    int j = 0;
    int signaled = 0;
    float radius_sq = 0.0f;
    const char *resolved_signal = signal_name ? signal_name : "enemy_spotted";

    if (!controllers || controller_count <= 1 || trigger_radius <= 0.0f)
        return 0;

    radius_sq = trigger_radius * trigger_radius;

    for (i = 0; i < controller_count; i++)
    {
        ControllerInstance *first = controllers[i];

        if (!first)
            continue;

        for (j = i + 1; j < controller_count; j++)
        {
            ControllerInstance *second = controllers[j];
            float dx = 0.0f;
            float dz = 0.0f;
            float dist_sq = 0.0f;
            float second_position[3];
            float first_position[3];

            if (!second)
                continue;

            if (!controller_teams_are_hostile(first->team, second->team))
                continue;

            dx = first->position[0] - second->position[0];
            dz = first->position[2] - second->position[2];
            dist_sq = dx * dx + dz * dz;
            if (dist_sq > radius_sq)
                continue;

            second_position[0] = second->position[0];
            second_position[1] = second->position[1];
            second_position[2] = second->position[2];
            first_position[0] = first->position[0];
            first_position[1] = first->position[1];
            first_position[2] = first->position[2];

            controller_signal(first, resolved_signal, second_position);
            controller_signal(second, resolved_signal, first_position);
            signaled += 2;
        }
    }

    return signaled;
}
