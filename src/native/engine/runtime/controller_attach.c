#include "controller_attach.h"

uint32_t runtime_controller_create_and_register(ControllerInstance **controllers,
                                                int max_controllers,
                                                int *inout_controller_count,
                                                const char *type,
                                                float x,
                                                float y,
                                                float z)
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
    Entity *entity = NULL;
    uint32_t controller_id = 0;

    if (!world)
        return 0;

    entity = world_get_entity(world, entity_id);
    if (!entity)
        return 0;

    controller_id = runtime_controller_create_and_register(controllers,
                                                           max_controllers,
                                                           inout_controller_count,
                                                           type,
                                                           entity->position[0],
                                                           entity->position[1],
                                                           entity->position[2]);
    entity->controller_id = controller_id;
    return controller_id;
}
