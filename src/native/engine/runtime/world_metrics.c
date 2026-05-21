#include "world_metrics.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include "controller_runtime.h"

int runtime_world_active_player_count(const World *world)
{
    int count = 0;
    int i = 0;

    if (!world)
        return 0;

#pragma omp parallel for schedule(static) reduction(+ : count)
    for (i = 0; i < world->entity_count; i++)
    {
        Entity *entity = world->entities[i];
        if (!entity)
            continue;
        if (entity->type == ENTITY_TYPE_PLAYER && entity->active)
            count += 1;
    }

    return count;
}

int runtime_world_entity_state(const World *world,
                               int entity_index,
                               ControllerInstance **controllers,
                               int controller_count)
{
    Entity *entity = NULL;
    ControllerInstance *controller = NULL;

    if (!world || entity_index < 0 || entity_index >= world->entity_count)
        return 0;

    entity = world->entities[entity_index];
    if (!entity || entity->controller_id == 0)
        return 0;

    controller = runtime_controller_find_by_id(controllers, controller_count, entity->controller_id);
    if (!controller)
        return 0;

    return 1;
}
