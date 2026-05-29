#ifndef BANANA_RUNTIME_GAMEPLAY_TEST_FACTORY_H
#define BANANA_RUNTIME_GAMEPLAY_TEST_FACTORY_H

#include "runtime/engine/gameplay_service.h"
#include "runtime/engine/engine_state.h"
#include "runtime/controller/attach/controller_attach.h"
#include "ai/wildlife_controller.h"
#include "ai/combat_controller.h"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

typedef struct RuntimeGameplayTestFactoryContext
{
    World *world;
    ControllerInstance **controllers;
    int max_controllers;
    int controller_count;
    EntityId player_id;
    int pickup_collected;
    EngineRuntimeState telemetry;
} RuntimeGameplayTestFactoryContext;

static int runtime_gameplay_test_expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int runtime_gameplay_test_expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s expected=true actual=false\n", label);
    return 0;
}

static int runtime_gameplay_test_factory_init(RuntimeGameplayTestFactoryContext *context,
                                              ControllerInstance **controllers,
                                              int max_controllers,
                                              float player_x,
                                              float player_z)
{
    if (!context || !controllers || max_controllers <= 0)
        return 0;

    memset(context, 0, sizeof(*context));

    context->world = world_create();
    if (!context->world)
        return 0;

    context->controllers = controllers;
    context->max_controllers = max_controllers;
    context->controller_count = 0;
    context->pickup_collected = 1;

    wildlife_controller_register();
    combat_controller_register();

    context->player_id = world_spawn_entity(context->world, ENTITY_TYPE_PLAYER, player_x, 0.0f, player_z);
    if (context->player_id == 0)
        return 0;

    return 1;
}

static int runtime_gameplay_test_factory_spawn_team_controller(RuntimeGameplayTestFactoryContext *context,
                                                               ControllerTeam team,
                                                               const char *controller_kind,
                                                               float x,
                                                               float z,
                                                               EntityId *out_entity_id)
{
    EntityId entity_id = 0;
    Entity *entity = NULL;

    if (!context || !context->world || !context->controllers || !controller_kind)
        return 0;

    entity_id = world_spawn_entity(context->world, ENTITY_TYPE_NPC, x, 0.0f, z);
    if (entity_id == 0)
        return 0;

    if (runtime_controller_attach_to_entity_with_team(context->world,
                                                      context->controllers,
                                                      context->max_controllers,
                                                      &context->controller_count,
                                                      entity_id,
                                                      controller_kind,
                                                      team) == 0u)
    {
        return 0;
    }

    entity = world_get_entity(context->world, entity_id);
    if (entity)
    {
        strncpy(entity->controller_kind, controller_kind, sizeof(entity->controller_kind) - 1);
        entity->controller_kind[sizeof(entity->controller_kind) - 1] = '\0';
    }

    if (out_entity_id)
        *out_entity_id = entity_id;

    return 1;
}

static void runtime_gameplay_test_factory_tick(RuntimeGameplayTestFactoryContext *context,
                                               float player_dx,
                                               float player_dz,
                                               float controller_war_radius,
                                               int controller_war_reinforcements_per_tick,
                                               int controller_war_population_cap,
                                               int war_escalation_tier,
                                               int war_intelligence_stage,
                                               int war_biome_stage_index)
{
    if (!context || !context->world)
        return;

    runtime_gameplay_service_tick(context->world,
                                  &context->telemetry,
                                  context->controllers,
                                  context->max_controllers,
                                  &context->controller_count,
                                  context->player_id,
                                  &context->pickup_collected,
                                  player_dx,
                                  player_dz,
                                  controller_war_radius,
                                  controller_war_reinforcements_per_tick,
                                  controller_war_population_cap,
                                  war_escalation_tier,
                                  war_intelligence_stage,
                                  war_biome_stage_index);
}

static void runtime_gameplay_test_factory_destroy(RuntimeGameplayTestFactoryContext *context)
{
    if (!context)
        return;

    if (context->world)
        world_destroy(context->world);

    if (context->controllers)
    {
        for (int i = 0; i < context->controller_count; i++)
            controller_destroy(context->controllers[i]);
    }

    memset(context, 0, sizeof(*context));
}

#endif
