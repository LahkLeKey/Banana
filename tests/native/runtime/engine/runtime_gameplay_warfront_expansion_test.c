#include "runtime/engine/gameplay_service.h"
#include "runtime/engine/engine_state.h"
#include "runtime/controller/attach/controller_attach.h"
#include "ai/wildlife_controller.h"
#include "ai/combat_controller.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int count_team(ControllerInstance **controllers, int controller_count, ControllerTeam team)
{
    int count = 0;

    for (int i = 0; i < controller_count; i++)
    {
        if (!controllers[i])
            continue;
        if (controllers[i]->team == team)
            count += 1;
    }

    return count;
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

static float max_abs_z_for_model_token(const World *world, const char *token)
{
    float max_abs = 0.0f;

    if (!world || !token)
        return 0.0f;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];
        float abs_z = 0.0f;

        if (!entity || !entity->active || entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strstr(entity->gameplay_model_id, token) == NULL)
            continue;

        abs_z = fabsf(entity->position[2]);
        if (abs_z > max_abs)
            max_abs = abs_z;
    }

    return max_abs;
}

static float min_x_for_model_token(const World *world, const char *token)
{
    float min_x = 0.0f;
    int found = 0;

    if (!world || !token)
        return 0.0f;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];

        if (!entity || !entity->active || entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strstr(entity->gameplay_model_id, token) == NULL)
            continue;

        if (!found || entity->position[0] < min_x)
            min_x = entity->position[0];
        found = 1;
    }

    return found ? min_x : 0.0f;
}

int main(void)
{
    World *world = world_create();
    ControllerInstance *controllers[64] = {0};
    const int max_controllers = (int)(sizeof(controllers) / sizeof(controllers[0]));
    int controller_count = 0;
    int pickup_collected = 1;
    EntityId player_id = 0;
    EntityId banana_id = 0;
    EntityId bean_id = 0;
    uint32_t banana_controller = 0;
    uint32_t bean_controller = 0;
    int base_entities = 0;
    EngineRuntimeState telemetry_state;

    memset(&telemetry_state, 0, sizeof(telemetry_state));

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    wildlife_controller_register();
    combat_controller_register();

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, -16.0f, 0.0f, -16.0f);
    banana_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    bean_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.0f, 0.0f, 0.0f);

    banana_controller = runtime_controller_attach_to_entity_with_team(world,
                                                                      controllers,
                                                                      max_controllers,
                                                                      &controller_count,
                                                                      banana_id,
                                                                      "combat",
                                                                      CONTROLLER_TEAM_BANANA);
    bean_controller = runtime_controller_attach_to_entity_with_team(world,
                                                                    controllers,
                                                                    max_controllers,
                                                                    &controller_count,
                                                                    bean_id,
                                                                    "wildlife",
                                                                    CONTROLLER_TEAM_BEAN);

    if (!expect_int("banana controller attached", banana_controller != 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("bean controller attached", bean_controller != 0 ? 1 : 0, 1))
        return 1;

    {
        Entity *banana = world_get_entity(world, banana_id);
        Entity *bean = world_get_entity(world, bean_id);
        if (banana)
            strncpy(banana->controller_kind, "combat", sizeof(banana->controller_kind) - 1);
        if (bean)
            strncpy(bean->controller_kind, "wildlife", sizeof(bean->controller_kind) - 1);
    }

    base_entities = world->entity_count;

    telemetry_state.war_sentience_coordination_level = 6;

    runtime_gameplay_service_tick(world,
                                  &telemetry_state,
                                  controllers,
                                  max_controllers,
                                  &controller_count,
                                  player_id,
                                  &pickup_collected,
                                  0.0f,
                                  1.0f,
                                  5.0f,
                                  2,
                                  12,
                                  1,
                                  1,
                                  2);

    if (!expect_int("world expanded by reinforcements", world->entity_count, base_entities + 2))
        return 1;
    if (!expect_int("controllers expanded by reinforcements", controller_count, 4))
        return 1;
    if (!expect_int("banana team count", count_team(controllers, controller_count, CONTROLLER_TEAM_BANANA), 2))
        return 1;
    if (!expect_int("bean team count", count_team(controllers, controller_count, CONTROLLER_TEAM_BEAN), 2))
        return 1;
    if (!expect_int("banana skirmish model scaffolded", count_model_token(world, "scout") > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("bean skirmish model scaffolded", count_model_token(world, "raider") > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("urban biome variant scaffolded", count_model_token(world, "urban") >= 2 ? 1 : 0, 1))
        return 1;
    if (!expect_int("banana behavior mode flank",
                    telemetry_state.war_sentience_behavior_banana,
                    RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK))
    {
        return 1;
    }
    if (!expect_int("bean behavior mode flank",
                    telemetry_state.war_sentience_behavior_bean,
                    RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK))
    {
        return 1;
    }
    if (!expect_int("negotiate streak remains zero in flank scenario",
                    telemetry_state.war_sentience_negotiate_streak_ticks,
                    0))
    {
        return 1;
    }
    if (!expect_int("negotiate trim remains zero in flank scenario",
                    telemetry_state.war_sentience_negotiate_deescalation_trim_last_tick,
                    0))
    {
        return 1;
    }
    if (!expect_int("banana flank asset scaffolded",
                    count_model_token(world, "banana-scout-flank") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_int("bean flank asset scaffolded",
                    count_model_token(world, "bean-raider-flank") > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_int("banana flank channel telemetry recorded",
                    telemetry_state.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK] > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_int("bean flank channel telemetry recorded",
                    telemetry_state.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK] > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_int("flank behavior widens lateral spawn geometry",
                    max_abs_z_for_model_token(world, "war/") >= 1.30f ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_int("flank heading rotation drives wrap toward enemy lane",
                    min_x_for_model_token(world, "bean-raider") < 1.10f ? 1 : 0,
                    1))
    {
        return 1;
    }

    base_entities = world->entity_count;
    runtime_gameplay_service_tick(world,
                                  &telemetry_state,
                                  controllers,
                                  max_controllers,
                                  &controller_count,
                                  player_id,
                                  &pickup_collected,
                                  0.0f,
                                  1.0f,
                                  5.0f,
                                  2,
                                  4,
                                  1,
                                  1,
                                  2);

    if (!expect_int("population cap blocks additional spawn", world->entity_count, base_entities))
        return 1;

    {
        int cycle_base_entities = world->entity_count;

        for (int biome_stage = 0; biome_stage < 4; biome_stage++)
        {
            runtime_gameplay_service_tick(world,
                                          &telemetry_state,
                                          controllers,
                                          max_controllers,
                                          &controller_count,
                                          player_id,
                                          &pickup_collected,
                                          0.0f,
                                          1.0f,
                                          5.0f,
                                          2,
                                          64,
                                          1,
                                          2,
                                          biome_stage);
        }

        if (!expect_int("all-biome cycle expanded world",
                        world->entity_count >= cycle_base_entities + 8 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("banana tropical siege model scaffolded",
                        count_model_token(world, "banana-siege-commander-tropical") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("banana glacier siege model scaffolded",
                        count_model_token(world, "banana-siege-commander-glacier") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("banana urban siege model scaffolded",
                        count_model_token(world, "banana-siege-commander-urban") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("banana volcanic siege model scaffolded",
                        count_model_token(world, "banana-siege-commander-volcanic") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("bean tropical warbrute model scaffolded",
                        count_model_token(world, "bean-warbrute-tropical") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("bean glacier warbrute model scaffolded",
                        count_model_token(world, "bean-warbrute-glacier") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("bean urban warbrute model scaffolded",
                        count_model_token(world, "bean-warbrute-urban") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("bean volcanic warbrute model scaffolded",
                        count_model_token(world, "bean-warbrute-volcanic") > 0 ? 1 : 0,
                        1))
        {
            return 1;
        }

        if (!expect_int("telemetry total reinforcement hits",
                        telemetry_state.war_reinforcement_spawns_total,
                        10))
        {
            return 1;
        }

        if (!expect_int("telemetry biome tropical hits",
                        telemetry_state.war_reinforcement_biome_hits[0],
                        2))
        {
            return 1;
        }

        if (!expect_int("telemetry biome glacier hits",
                        telemetry_state.war_reinforcement_biome_hits[1],
                        2))
        {
            return 1;
        }

        if (!expect_int("telemetry biome urban hits",
                        telemetry_state.war_reinforcement_biome_hits[2],
                        4))
        {
            return 1;
        }

        if (!expect_int("telemetry biome volcanic hits",
                        telemetry_state.war_reinforcement_biome_hits[3],
                        2))
        {
            return 1;
        }

        if (!expect_int("telemetry banana scout urban hit",
                        telemetry_state.war_reinforcement_banana_scout_hits[2],
                        1))
        {
            return 1;
        }

        if (!expect_int("telemetry bean raider urban hit",
                        telemetry_state.war_reinforcement_bean_raider_hits[2],
                        1))
        {
            return 1;
        }

        if (!expect_int("telemetry banana siege volcanic hit",
                        telemetry_state.war_reinforcement_banana_siege_hits[3],
                        1))
        {
            return 1;
        }

        if (!expect_int("telemetry bean warbrute tropical hit",
                        telemetry_state.war_reinforcement_bean_warbrute_hits[0],
                        1))
        {
            return 1;
        }
    }

    world_destroy(world);
    for (int i = 0; i < controller_count; i++)
        controller_destroy(controllers[i]);

    return 0;
}
