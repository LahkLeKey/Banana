#include "world/entity.h"
#include <stdlib.h>
#include <string.h>

Entity *entity_create(EntityId id, EntityType type, float x, float y, float z)
{
    Entity *e = calloc(1, sizeof(Entity));
    e->id = id;
    e->type = type;
    e->position[0] = x;
    e->position[1] = y;
    e->position[2] = z;
    e->rotation[3] = 1.f; /* identity quaternion */
    e->scale[0] = 1.f;
    e->scale[1] = 1.f;
    e->scale[2] = 1.f;
    e->active = 1;
    return e;
}

void entity_destroy(Entity *e)
{
    free(e);
}
