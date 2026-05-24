#ifndef BANANA_ENGINE_RUNTIME_TICK_POST_PHASE_H
#define BANANA_ENGINE_RUNTIME_TICK_POST_PHASE_H

#include "../ai/controller.h"
#include "../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*RuntimeTickPostVoidFn)(void *context);

    void runtime_tick_post_phase_execute(World *world,
                                         ControllerInstance **controllers,
                                         int controller_count,
                                         float dt,
                                         void *context,
                                         RuntimeTickPostVoidFn follow_player_camera,
                                         RuntimeTickPostVoidFn render_scene);

#ifdef __cplusplus
}
#endif

#endif
