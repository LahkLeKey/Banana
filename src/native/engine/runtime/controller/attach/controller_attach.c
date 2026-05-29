#include "controller_attach.h"

#include <stddef.h>
#include <string.h>

static ControllerTeam runtime_controller_default_team_for_type(const char *type)
{
    if (!type || type[0] == '\0')
        return CONTROLLER_TEAM_NEUTRAL;

    if (strcmp(type, "combat") == 0)
        return CONTROLLER_TEAM_BANANA;

    if (strcmp(type, "wildlife") == 0)
        return CONTROLLER_TEAM_BEAN;

    return CONTROLLER_TEAM_NEUTRAL;
}

uint32_t runtime_controller_create_and_register(ControllerInstance **controllers,
                                                int max_controllers,
                                                int *inout_controller_count,
                                                const char *type,
                                                float x,
                                                float y,
                                                float z)
{
    return runtime_controller_create_and_register_with_team(controllers,
                                                            max_controllers,
                                                            inout_controller_count,
                                                            type,
                                                            x,
                                                            y,
                                                            z,
                                                            runtime_controller_default_team_for_type(type));
}

uint32_t runtime_controller_create_and_register_with_team(ControllerInstance **controllers,
                                                          int max_controllers,
                                                          int *inout_controller_count,
                                                          const char *type,
                                                          float x,
                                                          float y,
                                                          float z,
                                                          ControllerTeam team)
{
    ControllerInstance *controller = NULL;
    int count = 0;

    if (!controllers || !inout_controller_count || !type)
        return 0;

    count = *inout_controller_count;
    if (count < 0 || count >= max_controllers)
        return 0;

    controller = controller_create(type, x, y, z);
    if (!controller)
        return 0;

    controller->id = (uint32_t)(count + 1);
    controller->team = team;
    controllers[count] = controller;
    *inout_controller_count = count + 1;

    return controller->id;
}

uint32_t runtime_controller_attach_to_entity(World *world,
                                             ControllerInstance **controllers,
                                             int max_controllers,
                                             int *inout_controller_count,
                                             uint32_t entity_id,
                                             const char *type)
{
    return runtime_controller_attach_to_entity_with_team(world,
                                                         controllers,
                                                         max_controllers,
                                                         inout_controller_count,
                                                         entity_id,
                                                         type,
                                                         runtime_controller_default_team_for_type(type));
}

uint32_t runtime_controller_attach_to_entity_with_team(World *world,
                                                       ControllerInstance **controllers,
                                                       int max_controllers,
                                                       int *inout_controller_count,
                                                       uint32_t entity_id,
                                                       const char *type,
                                                       ControllerTeam team)
{
    Entity *entity = NULL;
    uint32_t controller_id = 0;

    if (!world)
        return 0;

    entity = world_get_entity(world, entity_id);
    if (!entity)
        return 0;

    controller_id = runtime_controller_create_and_register_with_team(controllers,
                                                                     max_controllers,
                                                                     inout_controller_count,
                                                                     type,
                                                                     entity->position[0],
                                                                     entity->position[1],
                                                                     entity->position[2],
                                                                     team);
    entity->controller_id = controller_id;
    return controller_id;
}
