#include "gameplay_service.h"
#include "gameplay_service_warfront_expansion.h"

#include "../wildlife/wildlife_gameplay.h"

#include <math.h>
#include <string.h>
void runtime_gameplay_service_tick(World *world,
                                   EngineRuntimeState *runtime_state,
                                   ControllerInstance **controllers,
                                   int max_controllers,
                                   int *inout_controller_count,
                                   EntityId player_id,
                                   int *inout_pbj_pickup_collected,
                                   float wildlife_signal_radius,
                                   float collect_radius,
                                   float controller_war_radius,
                                   int controller_war_reinforcements_per_tick,
                                   int controller_war_population_cap,
                                   int war_escalation_tier,
                                   int war_intelligence_stage,
                                   int war_biome_stage_index)
{
    Entity *player = NULL;
    int controller_count = 0;

    if (!world || !player_id || !inout_pbj_pickup_collected)
        return;

    if (inout_controller_count)
        controller_count = *inout_controller_count;

    runtime_wildlife_signal_player_nearby(world,
                                          controllers,
                                          controller_count,
                                          player_id,
                                          wildlife_signal_radius);

    if (!(*inout_pbj_pickup_collected))
    {
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

    runtime_gameplay_service_expand_warfront(world,
                                             runtime_state,
                                             controllers,
                                             max_controllers,
                                             inout_controller_count,
                                             controller_war_radius,
                                             controller_war_reinforcements_per_tick,
                                             controller_war_population_cap,
                                             war_escalation_tier,
                                             war_intelligence_stage,
                                             war_biome_stage_index);
}
