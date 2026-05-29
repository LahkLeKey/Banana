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

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s expected=true actual=false\n", label);
    return 0;
}

static int count_model_token(const World *world, const char *token)
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

int main(void)
{
    World *world = world_create();
    ControllerInstance *controllers[128] = {0};
    const int max_controllers = (int)(sizeof(controllers) / sizeof(controllers[0]));
    int controller_count = 0;
    int pickup_collected = 1;
    EngineRuntimeState telemetry;
    EntityId player_id = 0;
    EntityId banana_id = 0;
    EntityId bean_id = 0;
    int spawned_per_tick[3] = {0};
    int trim_per_tick[3] = {0};

    memset(&telemetry, 0, sizeof(telemetry));

    if (!expect_true("world created", world != NULL))
        return 1;

    wildlife_controller_register();
    combat_controller_register();

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, -12.0f, 0.0f, -12.0f);
    banana_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    bean_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.0f, 0.0f, 0.0f);

    if (!expect_true("banana controller attached",
                     runtime_controller_attach_to_entity_with_team(world,
                                                                   controllers,
                                                                   max_controllers,
                                                                   &controller_count,
                                                                   banana_id,
                                                                   "combat",
                                                                   CONTROLLER_TEAM_BANANA) != 0u))
    {
        return 1;
    }

    if (!expect_true("bean controller attached",
                     runtime_controller_attach_to_entity_with_team(world,
                                                                   controllers,
                                                                   max_controllers,
                                                                   &controller_count,
                                                                   bean_id,
                                                                   "wildlife",
                                                                   CONTROLLER_TEAM_BEAN) != 0u))
    {
        return 1;
    }

    {
        Entity *banana = world_get_entity(world, banana_id);
        Entity *bean = world_get_entity(world, bean_id);

        if (banana)
        {
            strncpy(banana->controller_kind, "combat", sizeof(banana->controller_kind) - 1);
            banana->controller_kind[sizeof(banana->controller_kind) - 1] = '\0';
        }

        if (bean)
        {
            strncpy(bean->controller_kind, "wildlife", sizeof(bean->controller_kind) - 1);
            bean->controller_kind[sizeof(bean->controller_kind) - 1] = '\0';
        }
    }

    telemetry.war_sentience_humanoid_index = 12;
    telemetry.war_sentience_coordination_level = 8;
    telemetry.war_sentience_empathy_level = 8;

    for (int tick = 0; tick < 3; tick++)
    {
        int entities_before = world->entity_count;

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
                                      7,
                                      128,
                                      1,
                                      1,
                                      0);

        spawned_per_tick[tick] = world->entity_count - entities_before;
        trim_per_tick[tick] = telemetry.war_sentience_negotiate_deescalation_trim_last_tick;

        if (!expect_int("banana behavior negotiate",
                        telemetry.war_sentience_behavior_banana,
                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
        {
            return 1;
        }

        if (!expect_int("bean behavior negotiate",
                        telemetry.war_sentience_behavior_bean,
                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
        {
            return 1;
        }

        if (!expect_int("negotiate streak increments",
                        telemetry.war_sentience_negotiate_streak_ticks,
                        tick + 1))
        {
            return 1;
        }
    }

    if (!expect_int("deescalation trim tick1", trim_per_tick[0], 1))
        return 1;
    if (!expect_int("deescalation trim tick2", trim_per_tick[1], 1))
        return 1;
    if (!expect_int("deescalation trim tick3", trim_per_tick[2], 2))
        return 1;

    if (!expect_true("banana negotiate envoy asset scaffolded",
                     count_model_token(world, "banana-scout-envoy") > 0))
    {
        return 1;
    }
    if (!expect_true("bean negotiate envoy asset scaffolded",
                     count_model_token(world, "bean-raider-envoy") > 0))
    {
        return 1;
    }
    if (!expect_true("banana negotiate channel telemetry recorded",
                     telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE] > 0))
    {
        return 1;
    }
    if (!expect_true("bean negotiate channel telemetry recorded",
                     telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE] > 0))
    {
        return 1;
    }

    if (!expect_int("spawned count tick1 trimmed", spawned_per_tick[0], 6))
        return 1;
    if (!expect_int("spawned count tick2 trimmed", spawned_per_tick[1], 6))
        return 1;
    if (!expect_int("spawned count tick3 trimmed harder", spawned_per_tick[2], 5))
        return 1;
    if (!expect_true("later negotiate ticks reduce pressure",
                     spawned_per_tick[2] < spawned_per_tick[0]))
    {
        return 1;
    }

    telemetry.war_sentience_empathy_level = 0;

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
                                  7,
                                  128,
                                  1,
                                  1,
                                  0);

    if (!expect_true("breaking negotiate resets mode pair",
                     telemetry.war_sentience_behavior_banana != RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE ||
                         telemetry.war_sentience_behavior_bean != RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE))
    {
        return 1;
    }

    if (!expect_int("negotiate streak reset after break",
                    telemetry.war_sentience_negotiate_streak_ticks,
                    0))
    {
        return 1;
    }

    if (!expect_int("deescalation trim reset after break",
                    telemetry.war_sentience_negotiate_deescalation_trim_last_tick,
                    0))
    {
        return 1;
    }

    world_destroy(world);
    for (int i = 0; i < controller_count; i++)
        controller_destroy(controllers[i]);

    return 0;
}
