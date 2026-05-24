#ifndef BANANA_ENGINE_RUNTIME_PHYSICS_ABI_H
#define BANANA_ENGINE_RUNTIME_PHYSICS_ABI_H

#include "../../../physics/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_physics_abi_step(PhysicsWorld *physics_world, float dt);

    void runtime_physics_abi_add_body(PhysicsWorld *physics_world,
                                      uint32_t body_id,
                                      float mass,
                                      float x,
                                      float y,
                                      float z);

    void runtime_physics_abi_update_body(PhysicsWorld *physics_world,
                                         uint32_t body_id,
                                         const float *position,
                                         const float *velocity);

#ifdef __cplusplus
}
#endif

#endif
