#include "runtime/engine/gameplay_service.h"
#include "runtime/engine/engine_state.h"
#include "runtime/controller/attach/controller_attach.h"
#include "ai/wildlife_controller.h"
#include "ai/combat_controller.h"

#include <stdio.h>
#include <string.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int count_model_token(World *world, const char *token)
{
    int count = 0;

    if (!world || !token)
        return 0;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];

        if (!entity || !entity->active || entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strstr(entity->gameplay_model_id, token) != NULL)
            count += 1;
    }

    return count;
}

static int sum_hits(const int *hits)
{
    int total = 0;

    if (!hits)
        return 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
        total += hits[i];

    return total;
}

int main(void)
{
    World *world = world_create();
    ControllerInstance *controllers[64] = {0};
    const int max_controllers = (int)(sizeof(controllers) / sizeof(controllers[0]));
    int controller_count = 0;
    int pickup_collected = 1;
    EngineRuntimeState telemetry;
    EntityId player_id = 0;
    EntityId banana_front_id = 0;
    EntityId bean_front_id = 0;

    memset(&telemetry, 0, sizeof(telemetry));

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    wildlife_controller_register();
    combat_controller_register();

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, -4.0f, 0.0f, -4.0f);
    banana_front_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    bean_front_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.0f, 0.0f, 0.0f);

    if (!expect_int("banana front controller attached",
                    runtime_controller_attach_to_entity_with_team(world,
                                                                  controllers,
                                                                  max_controllers,
                                                                  &controller_count,
                                                                  banana_front_id,
                                                                  "combat",
                                                                  CONTROLLER_TEAM_BANANA) != 0u ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean front controller attached",
                    runtime_controller_attach_to_entity_with_team(world,
                                                                  controllers,
                                                                  max_controllers,
                                                                  &controller_count,
                                                                  bean_front_id,
                                                                  "wildlife",
                                                                  CONTROLLER_TEAM_BEAN) != 0u ? 1 : 0,
                    1))
    {
        return 1;
    }

    {
        Entity *banana_front = world_get_entity(world, banana_front_id);
        Entity *bean_front = world_get_entity(world, bean_front_id);

        if (banana_front)
        {
            strncpy(banana_front->controller_kind, "combat", sizeof(banana_front->controller_kind) - 1);
            banana_front->controller_kind[sizeof(banana_front->controller_kind) - 1] = '\0';
        }

        if (bean_front)
        {
            strncpy(bean_front->controller_kind, "wildlife", sizeof(bean_front->controller_kind) - 1);
            bean_front->controller_kind[sizeof(bean_front->controller_kind) - 1] = '\0';
        }
    }

    for (int biome_stage = 0; biome_stage < BANANA_ENGINE_TERRAIN_MAX_LAYERS; biome_stage++)
    {
        runtime_gameplay_service_tick(world,
                                      &telemetry,
                                      controllers,
                                      max_controllers,
                                      &controller_count,
                                      player_id,
                                      &pickup_collected,
                                      0.0f,
                                      1.0f,
                                      6.0f,
                                      2,
                                      96,
                                      2,
                                      3,
                                      biome_stage);
    }

    for (int biome_stage = 0; biome_stage < BANANA_ENGINE_TERRAIN_MAX_LAYERS; biome_stage++)
    {
        runtime_gameplay_service_tick(world,
                                      &telemetry,
                                      controllers,
                                      max_controllers,
                                      &controller_count,
                                      player_id,
                                      &pickup_collected,
                                      0.0f,
                                      1.0f,
                                      6.0f,
                                      2,
                                      96,
                                      2,
                                      5,
                                      biome_stage);
    }

    if (!expect_int("banana apex tropical scaffolded",
                    count_model_token(world, "banana-phalanx-tropical") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana apex glacier scaffolded",
                    count_model_token(world, "banana-phalanx-glacier") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana apex urban scaffolded",
                    count_model_token(world, "banana-phalanx-urban") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana apex volcanic scaffolded",
                    count_model_token(world, "banana-phalanx-volcanic") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean apex tropical scaffolded",
                    count_model_token(world, "bean-colossus-tropical") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean apex glacier scaffolded",
                    count_model_token(world, "bean-colossus-glacier") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean apex urban scaffolded",
                    count_model_token(world, "bean-colossus-urban") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean apex volcanic scaffolded",
                    count_model_token(world, "bean-colossus-volcanic") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana mythic tropical scaffolded",
                    count_model_token(world, "banana-archon-tropical") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana mythic glacier scaffolded",
                    count_model_token(world, "banana-archon-glacier") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana mythic urban scaffolded",
                    count_model_token(world, "banana-archon-urban") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("banana mythic volcanic scaffolded",
                    count_model_token(world, "banana-archon-volcanic") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean mythic tropical scaffolded",
                    count_model_token(world, "bean-leviathan-tropical") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean mythic glacier scaffolded",
                    count_model_token(world, "bean-leviathan-glacier") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean mythic urban scaffolded",
                    count_model_token(world, "bean-leviathan-urban") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("bean mythic volcanic scaffolded",
                    count_model_token(world, "bean-leviathan-volcanic") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }

    if (!expect_int("telemetry tracks apex+mythic reinforcement total",
                    telemetry.war_reinforcement_spawns_total,
                    16))
    {
        return 1;
    }

    if (!expect_int("telemetry banana siege channel tracks apex+mythic family",
                    sum_hits(telemetry.war_reinforcement_banana_siege_hits),
                    8))
    {
        return 1;
    }

    if (!expect_int("telemetry bean warbrute channel tracks apex+mythic family",
                    sum_hits(telemetry.war_reinforcement_bean_warbrute_hits),
                    8))
    {
        return 1;
    }

    if (!expect_int("telemetry banana apex channel total",
                    sum_hits(telemetry.war_reinforcement_banana_apex_hits),
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry bean apex channel total",
                    sum_hits(telemetry.war_reinforcement_bean_apex_hits),
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry banana mythic channel total",
                    sum_hits(telemetry.war_reinforcement_banana_mythic_hits),
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry bean mythic channel total",
                    sum_hits(telemetry.war_reinforcement_bean_mythic_hits),
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry banana apex stage-3 channel total",
                    telemetry.war_reinforcement_banana_apex_stage_hits[3],
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry bean apex stage-3 channel total",
                    telemetry.war_reinforcement_bean_apex_stage_hits[3],
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry banana mythic stage-5 channel total",
                    telemetry.war_reinforcement_banana_mythic_stage_hits[5],
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry bean mythic stage-5 channel total",
                    telemetry.war_reinforcement_bean_mythic_stage_hits[5],
                    4))
    {
        return 1;
    }

    if (!expect_int("telemetry stage-4 apex channel remains empty",
                    telemetry.war_reinforcement_banana_apex_stage_hits[4] +
                        telemetry.war_reinforcement_bean_apex_stage_hits[4],
                    0))
    {
        return 1;
    }

    if (!expect_int("telemetry stage-4 mythic channel remains empty",
                    telemetry.war_reinforcement_banana_mythic_stage_hits[4] +
                        telemetry.war_reinforcement_bean_mythic_stage_hits[4],
                    0))
    {
        return 1;
    }

    world_destroy(world);
    for (int i = 0; i < controller_count; i++)
    {
        if (controllers[i])
            controller_destroy(controllers[i]);
    }

    return 0;
}
