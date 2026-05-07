#ifndef BANANA_ENGINE_DYNAMICS_H
#define BANANA_ENGINE_DYNAMICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "body.h"

/* Gravity constant (m/s²). Override via dynamics_set_gravity(). */
#define BANANA_GRAVITY_DEFAULT -9.81f

void dynamics_set_gravity(float gy);

/* Semi-implicit (symplectic) Euler integration.
 * Integrates a single body's velocity and position by dt seconds. */
void dynamics_integrate(PhysicsBody *b, float dt);

/* Integrate all bodies in an array. */
void dynamics_integrate_all(PhysicsBody **bodies, int count, float dt);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_DYNAMICS_H */
