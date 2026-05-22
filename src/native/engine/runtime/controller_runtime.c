#include "controller_runtime.h"

#include <stddef.h>

ControllerInstance *runtime_controller_find_by_id(ControllerInstance **controllers,
                                                  int controller_count,
                                                  uint32_t controller_id)
{
    int i = 0;

    if (!controllers || controller_count <= 0 || controller_id == 0)
        return NULL;

    for (i = 0; i < controller_count; i++)
    {
        ControllerInstance *controller = controllers[i];
        if (!controller)
            continue;
        if (controller->id == controller_id)
            return controller;
    }

    return NULL;
}

int runtime_controller_update_by_id(ControllerInstance **controllers,
                                    int controller_count,
                                    uint32_t controller_id,
                                    float dt)
{
    ControllerInstance *controller = runtime_controller_find_by_id(controllers,
                                                                   controller_count,
                                                                   controller_id);

    if (!controller)
        return 0;

    controller_update(controller, dt);
    return 1;
}

int runtime_controller_signal_by_id(ControllerInstance **controllers,
                                    int controller_count,
                                    uint32_t controller_id,
                                    const char *signal_name,
                                    const void *data)
{
    ControllerInstance *controller = runtime_controller_find_by_id(controllers,
                                                                   controller_count,
                                                                   controller_id);

    if (!controller)
        return 0;

    controller_signal(controller, signal_name, data);
    return 1;
}
