#include "gameplay_service.h"

#include "../wildlife/wildlife_gameplay.h"

#include <math.h>
#include <string.h>

void runtime_gameplay_service_tick(World *world,
                                   ControllerInstance **controllers,
                                   int controller_count,
                                   EntityId player_id,
                                   int *inout_pbj_pickup_collected,
                                   float wildlife_signal_radius,
                                   float collect_radius)
{
    Entity *player = NULL;

    if (!world || !player_id || !inout_pbj_pickup_collected)
        return;

    runtime_wildlife_signal_player_nearby(world,
                                          controllers,
                                          controller_count,
                                          player_id,
                                          wildlife_signal_radius);

    if (*inout_pbj_pickup_collected)
        return;

    player = world_get_entity(world, player_id);
    if (!player || !player->active)
        return;

    for (int i = 0; i < world->entity_count; i++)
    {
        Entity *entity = world->entities[i];
        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_STATIC)
            continue;
        if (strcmp(entity->controller_kind, "pbj_pickup") != 0)
            continue;

        {
            float dx = player->position[0] - entity->position[0];
            float dz = player->position[2] - entity->position[2];
            float distance = sqrtf(dx * dx + dz * dz);
            if (distance <= collect_radius)
            {
                entity->active = 0;
                *inout_pbj_pickup_collected = 1;
                break;
            }
        }
    }
}
