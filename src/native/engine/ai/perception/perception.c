#include "perception.h"
#include "../world/world.h"
#include <math.h>

PerceptionResult perception_query_nearby(const PerceptionQuery *q)
{
    PerceptionResult result;
    result.count = 0;
    World *w = world_get();
    if (!w)
        return result;

    for (int i = 0; i < w->entity_count && result.count < 16; i++)
    {
        Entity *e = w->entities[i];
        if (!e || !e->active || e->id == q->exclude_id)
            continue;
        float dx = e->position[0] - q->origin[0];
        float dy = e->position[1] - q->origin[1];
        float dz = e->position[2] - q->origin[2];
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);
        if (dist <= q->radius)
        {
            result.entity_ids[result.count] = e->id;
            result.distances[result.count] = dist;
            result.count++;
        }
    }
    return result;
}

int perception_line_of_sight(const float *from, const float *to)
{
    /* Phase 1 stub: always returns 1 (no occlusion geometry yet) */
    (void)from;
    (void)to;
    return 1;
}

int perception_sense(const float *position, float radius, uint32_t exclude_id)
{
    PerceptionQuery q;
    q.origin[0] = position[0];
    q.origin[1] = position[1];
    q.origin[2] = position[2];
    q.radius = radius;
    q.exclude_id = exclude_id;
    PerceptionResult r = perception_query_nearby(&q);
    return r.count > 0;
}
