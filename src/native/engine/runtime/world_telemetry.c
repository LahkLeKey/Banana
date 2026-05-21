#include "world_telemetry.h"

int runtime_world_entity_count(const World *world)
{
    if (!world)
        return 0;

    return world->entity_count;
}

float runtime_world_entity_x(const World *world, int entity_index)
{
    if (!world || entity_index < 0 || entity_index >= world->entity_count)
        return 0.f;

    return world->entities[entity_index] ? world->entities[entity_index]->position[0] : 0.f;
}

float runtime_world_entity_z(const World *world, int entity_index)
{
    if (!world || entity_index < 0 || entity_index >= world->entity_count)
        return 0.f;

    return world->entities[entity_index] ? world->entities[entity_index]->position[2] : 0.f;
}
