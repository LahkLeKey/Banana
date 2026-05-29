#include "runtime/engine/gameplay_service.h"
#include "runtime/engine/engine_state.h"
#include "runtime/controller/attach/controller_attach.h"
#include "ai/wildlife_controller.h"
#include "ai/combat_controller.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_true(const char *label, int value)
{
    if (value)
        return 1;

    fprintf(stderr, "%s expected=true actual=false\n", label);
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

static int sum_hits(const int *hits)
{
    int total = 0;

    if (!hits)
        return 0;

    for (int i = 0; i < 4; i++)
        total += hits[i];

    return total;
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

static float max_x_for_model_token(const World *world, const char *token)
{
    float max_x = 0.0f;
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

        if (!found || entity->position[0] > max_x)
            max_x = entity->position[0];
        found = 1;
    }

    return found ? max_x : 0.0f;
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
    EntityId banana_id = 0;
    EntityId bean_a_id = 0;
    EntityId bean_b_id = 0;
    EntityId bean_c_id = 0;
    int banana_before = 0;
    int bean_before = 0;
    int banana_after = 0;
    int bean_after = 0;
    int banana_delta = 0;
    int bean_delta = 0;
    int banana_hits = 0;
    int bean_hits = 0;

    memset(&telemetry, 0, sizeof(telemetry));

    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    wildlife_controller_register();
    combat_controller_register();

    player_id = world_spawn_entity(world, ENTITY_TYPE_PLAYER, -10.0f, 0.0f, -10.0f);
    banana_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    bean_a_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.0f, 0.0f, 0.0f);
    bean_b_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 2.5f, 0.0f, 0.0f);
    bean_c_id = world_spawn_entity(world, ENTITY_TYPE_NPC, 3.0f, 0.0f, 0.0f);

    if (!expect_true("player spawned", player_id != 0))
        return 1;

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

    if (!expect_true("bean a controller attached",
                     runtime_controller_attach_to_entity_with_team(world,
                                                                   controllers,
                                                                   max_controllers,
                                                                   &controller_count,
                                                                   bean_a_id,
                                                                   "wildlife",
                                                                   CONTROLLER_TEAM_BEAN) != 0u))
    {
        return 1;
    }

    if (!expect_true("bean b controller attached",
                     runtime_controller_attach_to_entity_with_team(world,
                                                                   controllers,
                                                                   max_controllers,
                                                                   &controller_count,
                                                                   bean_b_id,
                                                                   "wildlife",
                                                                   CONTROLLER_TEAM_BEAN) != 0u))
    {
        return 1;
    }

    if (!expect_true("bean c controller attached",
                     runtime_controller_attach_to_entity_with_team(world,
                                                                   controllers,
                                                                   max_controllers,
                                                                   &controller_count,
                                                                   bean_c_id,
                                                                   "wildlife",
                                                                   CONTROLLER_TEAM_BEAN) != 0u))
    {
        return 1;
    }

    {
        Entity *banana = world_get_entity(world, banana_id);
        Entity *bean_a = world_get_entity(world, bean_a_id);
        Entity *bean_b = world_get_entity(world, bean_b_id);
        Entity *bean_c = world_get_entity(world, bean_c_id);

        if (banana)
        {
            strncpy(banana->controller_kind, "combat", sizeof(banana->controller_kind) - 1);
            banana->controller_kind[sizeof(banana->controller_kind) - 1] = '\0';
        }

        if (bean_a)
        {
            strncpy(bean_a->controller_kind, "wildlife", sizeof(bean_a->controller_kind) - 1);
            bean_a->controller_kind[sizeof(bean_a->controller_kind) - 1] = '\0';
        }

        if (bean_b)
        {
            strncpy(bean_b->controller_kind, "wildlife", sizeof(bean_b->controller_kind) - 1);
            bean_b->controller_kind[sizeof(bean_b->controller_kind) - 1] = '\0';
        }

        if (bean_c)
        {
            strncpy(bean_c->controller_kind, "wildlife", sizeof(bean_c->controller_kind) - 1);
            bean_c->controller_kind[sizeof(bean_c->controller_kind) - 1] = '\0';
        }
    }

    banana_before = count_team(controllers, controller_count, CONTROLLER_TEAM_BANANA);
    bean_before = count_team(controllers, controller_count, CONTROLLER_TEAM_BEAN);

    setenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION", "4", 1);
    telemetry.war_sentience_coordination_level = 6;
    telemetry.war_sentience_empathy_level = 4;

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
                                  10,
                                  64,
                                  1,
                                  1,
                                  0);

    banana_after = count_team(controllers, controller_count, CONTROLLER_TEAM_BANANA);
    bean_after = count_team(controllers, controller_count, CONTROLLER_TEAM_BEAN);

    banana_delta = banana_after - banana_before;
    bean_delta = bean_after - bean_before;

    banana_hits = sum_hits(telemetry.war_reinforcement_banana_scout_hits) +
                  sum_hits(telemetry.war_reinforcement_banana_siege_hits);
    bean_hits = sum_hits(telemetry.war_reinforcement_bean_raider_hits) +
                sum_hits(telemetry.war_reinforcement_bean_warbrute_hits);

    if (!expect_true("banana comeback bonus favors banana spawns", banana_delta > bean_delta))
        return 1;
    if (!expect_true("banana comeback bonus adds multiple banana reinforcements", banana_delta >= 2))
        return 1;
    if (!expect_int("telemetry tracks reinforcement totals",
                    telemetry.war_reinforcement_spawns_total,
                    banana_delta + bean_delta))
    {
        return 1;
    }
    if (!expect_true("telemetry records stronger banana reinforcement pressure", banana_hits > bean_hits))
        return 1;
    if (!expect_true("sentient humanoid comeback bonus applied",
                     telemetry.war_sentience_comeback_bonus_last_tick > 0))
    {
        return 1;
    }
    if (!expect_int("banana sentience behavior mode regroup",
                    telemetry.war_sentience_behavior_banana,
                    RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP))
    {
        return 1;
    }
    if (!expect_int("bean sentience behavior mode flank",
                    telemetry.war_sentience_behavior_bean,
                    RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK))
    {
        return 1;
    }
    if (!expect_int("negotiate streak remains zero",
                    telemetry.war_sentience_negotiate_streak_ticks,
                    0))
    {
        return 1;
    }
    if (!expect_int("negotiate trim remains zero",
                    telemetry.war_sentience_negotiate_deescalation_trim_last_tick,
                    0))
    {
        return 1;
    }
    if (!expect_true("regroup banana asset scaffolded",
                     count_model_token(world, "banana-scout-regroup") > 0))
    {
        return 1;
    }
    if (!expect_true("flank bean asset scaffolded",
                     count_model_token(world, "bean-raider-flank") > 0))
    {
        return 1;
    }
    if (!expect_int("banana regroup channel telemetry recorded",
                    telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP] > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_int("bean flank channel telemetry recorded",
                    telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK] > 0 ? 1 : 0,
                    1))
    {
        return 1;
    }
    if (!expect_true("regroup mode keeps banana reinforcements tight",
                     max_abs_z_for_model_token(world, "banana-scout") <= 0.40f))
    {
        return 1;
    }
    if (!expect_true("regroup mode forms rear echelon behind banana anchor",
                     max_x_for_model_token(world, "banana-scout") <= -0.85f))
    {
        return 1;
    }
    if (!expect_true("flank mode keeps bean reinforcements wide",
                     max_abs_z_for_model_token(world, "bean-raider") >= 1.20f))
    {
        return 1;
    }
    if (!expect_true("flank mode still pressures bean advance toward banana",
                     min_x_for_model_token(world, "bean-raider") < 1.25f))
    {
        return 1;
    }
    if (!expect_true("flank spread exceeds regroup spread",
                     max_abs_z_for_model_token(world, "bean-raider") >
                         (max_abs_z_for_model_token(world, "banana-scout") + 0.70f)))
    {
        return 1;
    }

    world_destroy(world);
    for (int i = 0; i < controller_count; i++)
        controller_destroy(controllers[i]);

    unsetenv("BANANA_CONTROLLER_WAR_SENTIENCE_COMEBACK_BONUS_PER_COORDINATION");

    return 0;
}
