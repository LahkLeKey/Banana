#include "world.h"

#include <string.h>

int world_query_nearby(const World *w, const float *position, float radius, EntityId *out_ids,
                       int max_results)
{
    unsigned char nearby_mask[BANANA_MAX_ENTITIES];
    float radius_sq = 0.0f;
    int entity_count = 0;
    int found = 0;
    int i = 0;

    if (!w || !position || !out_ids || max_results <= 0 || radius < 0.0f)
        return 0;

    radius_sq = radius * radius;
    entity_count = w->entity_count;
    if (entity_count < 0)
        entity_count = 0;
    if (entity_count > BANANA_MAX_ENTITIES)
        entity_count = BANANA_MAX_ENTITIES;

    memset(nearby_mask, 0, (size_t)entity_count);

#pragma omp parallel for schedule(static)
    for (i = 0; i < entity_count; i++)
    {
        const Entity *e = w->entities[i];
        float dx = 0.0f;
        float dy = 0.0f;
        float dz = 0.0f;
        float dist_sq = 0.0f;

        if (!e || !e->active)
            continue;

        dx = e->position[0] - position[0];
        dy = e->position[1] - position[1];
        dz = e->position[2] - position[2];
        dist_sq = dx * dx + dy * dy + dz * dz;
        if (dist_sq <= radius_sq)
            nearby_mask[i] = 1u;
    }

    for (i = 0; i < entity_count && found < max_results; i++)
    {
        if (!nearby_mask[i])
            continue;
        out_ids[found++] = w->entities[i]->id;
    }

    return found;
}
