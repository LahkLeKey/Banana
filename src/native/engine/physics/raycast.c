#include "raycast.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP) && !defined(__INTELLISENSE__)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

int physics_raycast_bodies(PhysicsBody **bodies,
                           int body_count,
                           const float *origin,
                           const float *direction,
                           PhysicsBodyId *hit_body_out,
                           float *t_out)
{
    float best_t = 1e9f;
    PhysicsBodyId best_id = 0;
    int hit = 0;

    if (!bodies || body_count <= 0 || !origin || !direction)
        return 0;

#pragma omp parallel
    {
        float local_best_t = 1e9f;
        PhysicsBodyId local_best_id = 0;
        int local_hit = 0;
        int i = 0;

#pragma omp for schedule(guided, 8) nowait
        for (i = 0; i < body_count; i++)
        {
            PhysicsBody *b = bodies[i];
            if (!b || b->shape_type != SHAPE_BOX)
                continue;

            float *p = b->position;
            float *he = b->shape.box.half_extents;
            float tmin = -1e9f, tmax = 1e9f;

            for (int axis = 0; axis < 3; axis++)
            {
                float d = direction[axis];
                float mn = p[axis] - he[axis];
                float mx = p[axis] + he[axis];

                if (d != 0.f)
                {
                    float t1 = (mn - origin[axis]) / d;
                    float t2 = (mx - origin[axis]) / d;
                    if (t1 > t2)
                    {
                        float tmp = t1;
                        t1 = t2;
                        t2 = tmp;
                    }
                    if (t1 > tmin)
                        tmin = t1;
                    if (t2 < tmax)
                        tmax = t2;
                }
                else if (origin[axis] < mn || origin[axis] > mx)
                {
                    tmin = 1e9f;
                    break;
                }
            }

            if (tmin < tmax && tmin > 0.f && tmin < local_best_t)
            {
                local_best_t = tmin;
                local_best_id = b->id;
                local_hit = 1;
            }
        }

#pragma omp critical
        {
            if (local_hit && local_best_t < best_t)
            {
                best_t = local_best_t;
                best_id = local_best_id;
                hit = 1;
            }
        }
    }

    if (hit)
    {
        if (hit_body_out)
            *hit_body_out = best_id;
        if (t_out)
            *t_out = best_t;
    }

    return hit;
}
