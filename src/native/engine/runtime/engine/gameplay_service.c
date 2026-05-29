#include "gameplay_service.h"

#include "../controller/attach/controller_attach.h"
#include "../wildlife/wildlife_gameplay.h"

#include <math.h>
#include <string.h>

static int runtime_gameplay_count_active_war_npcs(const World *world)
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

static int runtime_gameplay_find_active_warfront(ControllerInstance **controllers,
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

static int runtime_gameplay_spawn_war_reinforcement(World *world,
                                                    ControllerInstance **controllers,
                                                    int max_controllers,
                                                    int *inout_controller_count,
                                                    ControllerTeam team,
                                                    float anchor_x,
                                                    float anchor_y,
                                                    float anchor_z,
                                                    float direction_x,
                                                    float direction_z,
                                                    float lateral_sign,
                                                    int ordinal)
{
    float forward = 1.75f + (float)(ordinal % 3) * 0.45f;
    float lateral = (0.60f + (float)(ordinal % 2) * 0.25f) * lateral_sign;
    float x = anchor_x + (direction_x * forward) + ((-direction_z) * lateral);
    float z = anchor_z + (direction_z * forward) + (direction_x * lateral);
    float y = anchor_y + 0.35f;
    const char *controller_type = (team == CONTROLLER_TEAM_BANANA) ? "combat" : "wildlife";
    const char *gameplay_model_id = (team == CONTROLLER_TEAM_BANANA)
                                        ? "gameplay/reference/banana-basic-v1"
                                        : "gameplay/reference/banana-bean-green-v1";
    EntityId entity_id = 0;
    Entity *entity = NULL;
    uint32_t controller_id = 0;

    if (!world || !controllers || !inout_controller_count || max_controllers <= 0)
        return 0;

    if (*inout_controller_count >= max_controllers)
        return 0;

    entity_id = world_spawn_entity(world, ENTITY_TYPE_NPC, x, y, z);
    if (!entity_id)
        return 0;

    entity = world_get_entity(world, entity_id);
    if (!entity)
        return 0;

    strncpy(entity->controller_kind, controller_type, sizeof(entity->controller_kind) - 1);
    entity->controller_kind[sizeof(entity->controller_kind) - 1] = '\0';
    strncpy(entity->gameplay_model_id, gameplay_model_id, sizeof(entity->gameplay_model_id) - 1);
    entity->gameplay_model_id[sizeof(entity->gameplay_model_id) - 1] = '\0';

    controller_id = runtime_controller_attach_to_entity_with_team(world,
                                                                  controllers,
                                                                  max_controllers,
                                                                  inout_controller_count,
                                                                  entity_id,
                                                                  controller_type,
                                                                  team);
    if (!controller_id)
    {
        world_despawn_entity(world, entity_id);
        return 0;
    }

    entity->controller_id = controller_id;
    return 1;
}

static void runtime_gameplay_service_expand_warfront(World *world,
                                                     ControllerInstance **controllers,
                                                     int max_controllers,
                                                     int *inout_controller_count,
                                                     float controller_war_radius,
                                                     int controller_war_reinforcements_per_tick,
                                                     int controller_war_population_cap)
{
    ControllerInstance *banana_front = NULL;
    ControllerInstance *bean_front = NULL;
    int active_war_npc_count = 0;
    int spawn_budget = 0;
    float dx = 0.0f;
    float dz = 0.0f;
    float distance = 0.0f;
    float direction_x = 1.0f;
    float direction_z = 0.0f;

    if (!world || !controllers || !inout_controller_count)
        return;
    if (controller_war_reinforcements_per_tick <= 0 || controller_war_population_cap <= 0)
        return;

    if (!runtime_gameplay_find_active_warfront(controllers,
                                               *inout_controller_count,
                                               controller_war_radius,
                                               &banana_front,
                                               &bean_front))
    {
        return;
    }

    active_war_npc_count = runtime_gameplay_count_active_war_npcs(world);
    if (active_war_npc_count >= controller_war_population_cap)
        return;

    spawn_budget = controller_war_population_cap - active_war_npc_count;
    if (spawn_budget > controller_war_reinforcements_per_tick)
        spawn_budget = controller_war_reinforcements_per_tick;

    dx = bean_front->position[0] - banana_front->position[0];
    dz = bean_front->position[2] - banana_front->position[2];
    distance = sqrtf((dx * dx) + (dz * dz));
    if (distance > 0.001f)
    {
        direction_x = dx / distance;
        direction_z = dz / distance;
    }

    for (int i = 0; i < spawn_budget; i++)
    {
        ControllerTeam team = (i % 2 == 0) ? CONTROLLER_TEAM_BANANA : CONTROLLER_TEAM_BEAN;
        ControllerInstance *anchor = (team == CONTROLLER_TEAM_BANANA) ? banana_front : bean_front;
        float team_direction_x = (team == CONTROLLER_TEAM_BANANA) ? direction_x : -direction_x;
        float team_direction_z = (team == CONTROLLER_TEAM_BANANA) ? direction_z : -direction_z;
        float lateral_sign = (((i / 2) % 2) == 0) ? 1.0f : -1.0f;

        runtime_gameplay_spawn_war_reinforcement(world,
                                                 controllers,
                                                 max_controllers,
                                                 inout_controller_count,
                                                 team,
                                                 anchor->position[0],
                                                 anchor->position[1],
                                                 anchor->position[2],
                                                 team_direction_x,
                                                 team_direction_z,
                                                 lateral_sign,
                                                 i);
    }
}

void runtime_gameplay_service_tick(World *world,
                                   ControllerInstance **controllers,
                                   int max_controllers,
                                   int *inout_controller_count,
                                   EntityId player_id,
                                   int *inout_pbj_pickup_collected,
                                   float wildlife_signal_radius,
                                   float collect_radius,
                                   float controller_war_radius,
                                   int controller_war_reinforcements_per_tick,
                                   int controller_war_population_cap)
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
                                             controllers,
                                             max_controllers,
                                             inout_controller_count,
                                             controller_war_radius,
                                             controller_war_reinforcements_per_tick,
                                             controller_war_population_cap);
}
