#ifndef BANANA_ENGINE_RAYCAST_H
#define BANANA_ENGINE_RAYCAST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "body.h"

    int physics_raycast_bodies(PhysicsBody **bodies,
                               int body_count,
                               const float *origin,
                               const float *direction,
                               PhysicsBodyId *hit_body_out,
                               float *t_out);

#ifdef __cplusplus
}
#endif

#endif
