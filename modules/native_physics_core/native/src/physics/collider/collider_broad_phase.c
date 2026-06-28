#include "collider.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP) && !defined(__INTELLISENSE__)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

CollisionList collider_broad_phase(PhysicsBody **bodies, int count)
{
    CollisionList list;
    list.count = 0;

    if (!bodies || count <= 1)
        return list;

#pragma omp parallel
    {
        Collision local_items[BANANA_MAX_COLLISIONS];
        int local_count = 0;
        int i = 0;

#pragma omp for schedule(guided, 1) nowait
        for (i = 0; i < count; i++)
        {
            for (int j = i + 1; j < count; j++)
            {
                Collision c;
                int hit = 0;
                PhysicsBody *a = bodies[i];
                PhysicsBody *b = bodies[j];

                if (a->shape_type == SHAPE_BOX && b->shape_type == SHAPE_BOX)
                    hit = collider_box_vs_box(a, b, &c);
                else if (a->shape_type == SHAPE_SPHERE && b->shape_type == SHAPE_SPHERE)
                    hit = collider_sphere_vs_sphere(a, b, &c);
                else
                    hit = collider_box_vs_sphere(a, b, &c);

                if (hit && local_count < BANANA_MAX_COLLISIONS)
                    local_items[local_count++] = c;
            }
        }

#pragma omp critical
        {
            for (int k = 0; k < local_count && list.count < BANANA_MAX_COLLISIONS; k++)
                list.items[list.count++] = local_items[k];
        }
    }

    return list;
}
