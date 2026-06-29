#include "dynamics.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP) && !defined(__INTELLISENSE__)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <string.h>

static float s_gravity = BANANA_GRAVITY_DEFAULT;

void dynamics_set_gravity(float gy)
{
    s_gravity = gy;
}

void dynamics_integrate(PhysicsBody *b, float dt)
{
    if (b->is_static || b->mass <= 0.f)
        return;

    /* Apply gravity to force accumulator */
    b->force_accum[1] += s_gravity * b->mass;

    /* Acceleration = F / m */
    float ax = b->force_accum[0] / b->mass;
    float ay = b->force_accum[1] / b->mass;
    float az = b->force_accum[2] / b->mass;

    /* Semi-implicit Euler: update velocity first, then position */
    b->velocity[0] += ax * dt;
    b->velocity[1] += ay * dt;
    b->velocity[2] += az * dt;

    /* Linear damping (reduces energy to prevent drift) */
    float damping = 0.99f;
    b->velocity[0] *= damping;
    b->velocity[1] *= damping;
    b->velocity[2] *= damping;

    b->position[0] += b->velocity[0] * dt;
    b->position[1] += b->velocity[1] * dt;
    b->position[2] += b->velocity[2] * dt;

    /* Reset force accumulator */
    b->force_accum[0] = 0.f;
    b->force_accum[1] = 0.f;
    b->force_accum[2] = 0.f;
}

void dynamics_integrate_all(PhysicsBody **bodies, int count, float dt)
{
    int i = 0;

#pragma omp parallel for schedule(static)
    for (i = 0; i < count; i++)
        dynamics_integrate(bodies[i], dt);
}
