#include "controller_sync.h"
#include "interlock.h"

#include <stdio.h>
#include <stdlib.h>

int runtime_controller_sync_parallel_available(void)
{
    return 1;
}

void runtime_sync_controller_positions(World *world,
                                       ControllerInstance **controllers,
                                       int controller_count,
                                       float dt)
{
    RuntimeInterlockContext interlock;
    ControllerInstance **controller_by_id = NULL;
    int map_size = 0;
    int max_id = 0;

    if (!world || !controllers || controller_count <= 0)
        return;
    if (!runtime_interlock_init(&interlock))
    {
        fprintf(stderr,
                "[engine/runtime] failed to initialize interlock for controller sync\n");
        abort();
    }

    int i = 0;
    int j = 0;

    for (i = 0; i < controller_count; ++i)
    {
        ControllerInstance *controller = controllers[i];
        if (controller && (int)controller->id > max_id)
            max_id = (int)controller->id;
    }

    map_size = max_id + 1;
    if (map_size < 2)
        map_size = 2;

    controller_by_id = (ControllerInstance **)calloc((size_t)map_size, sizeof(ControllerInstance *));
    if (!controller_by_id)
    {
        fprintf(stderr,
                "[engine/runtime] failed to allocate controller id map for parallel sync\n");
        runtime_interlock_destroy(&interlock);
        abort();
    }

#pragma omp parallel for schedule(dynamic, 4)
    for (i = 0; i < controller_count; ++i)
    {
        ControllerInstance *controller = controllers[i];
        int id = 0;
        if (!controller)
            continue;

        controller_update(controller, dt);

        id = (int)controller->id;
        if (id <= 0 || id >= map_size)
            continue;

        runtime_interlock_enter(&interlock);
        if (controller_by_id[id] == NULL)
            controller_by_id[id] = controller;
        runtime_interlock_leave(&interlock);
    }

#pragma omp parallel for schedule(guided, 4)
    for (j = 0; j < world->entity_count; ++j)
    {
        Entity *entity = world->entities[j];
        ControllerInstance *controller = NULL;
        int controller_id = 0;

        if (!entity || !entity->active || entity->controller_id == 0)
            continue;

        controller_id = (int)entity->controller_id;
        if (controller_id > 0 && controller_id < map_size)
            controller = controller_by_id[controller_id];

        if (!controller)
            continue;

        entity->position[0] = controller->position[0];
        entity->position[1] = controller->position[1];
        entity->position[2] = controller->position[2];
    }

    free(controller_by_id);

    runtime_interlock_destroy(&interlock);
}