#include "wildlife_gameplay.h"

#include <string.h>

int runtime_wildlife_signal_player_nearby(World *world,
                                          ControllerInstance **controllers,
                                          int controller_count,
                                          EntityId player_id,
                                          float trigger_radius)
{
    Entity *player = NULL;
    float radius_sq = 0.0f;
    int signaled = 0;
    int i = 0;

    if (!world || !controllers || controller_count <= 0 || player_id == 0 || trigger_radius <= 0.0f)
        return 0;

    player = world_get_entity(world, player_id);
    if (!player || !player->active)
        return 0;

    radius_sq = trigger_radius * trigger_radius;

#pragma omp parallel for schedule(static) reduction(+ : signaled)
    for (i = 0; i < controller_count; i++)
    {
        ControllerInstance *controller = controllers[i];
        float dx = 0.0f;
        float dz = 0.0f;
        float dist_sq = 0.0f;

        if (!controller)
            continue;
        if (strncmp(controller->type_name, "wildlife", sizeof(controller->type_name)) != 0)
            continue;

        dx = controller->position[0] - player->position[0];
        dz = controller->position[2] - player->position[2];
        dist_sq = dx * dx + dz * dz;
        if (dist_sq > radius_sq)
            continue;

        controller_signal(controller, "player_nearby", player->position);
        signaled += 1;
    }

    return signaled;
}
