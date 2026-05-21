#ifndef BANANA_ENGINE_PERCEPTION_H
#define BANANA_ENGINE_PERCEPTION_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef struct PerceptionQuery
    {
        float origin[3];
        float radius;
        uint32_t exclude_id; /* entity ID to exclude (self) */
    } PerceptionQuery;

    typedef struct PerceptionResult
    {
        uint32_t entity_ids[16];
        float distances[16];
        int count;
    } PerceptionResult;

    /* Query nearby entity IDs within a sphere radius.
     * Requires the world module — implemented in perception.c which references world.h */
    PerceptionResult perception_query_nearby(const PerceptionQuery *q);

    /* Raycast to check line-of-sight.  Returns 1 if unobstructed. */
    int perception_line_of_sight(const float *from, const float *to);

    /* Returns 1 if any entity matching category mask is within radius. */
    int perception_sense(const float *position, float radius, uint32_t exclude_id);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_PERCEPTION_H */
