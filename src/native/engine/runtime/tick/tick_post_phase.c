#include "tick_post_phase.h"

#include "../controller/sync/controller_sync.h"

void runtime_tick_post_phase_execute(World *world,
                                     ControllerInstance **controllers,
                                     int controller_count,
                                     float dt,
                                     void *context,
                                     RuntimeTickPostVoidFn follow_player_camera,
                                     RuntimeTickPostVoidFn render_scene)
{
    runtime_sync_controller_positions(world, controllers, controller_count, dt);

    if (follow_player_camera)
        follow_player_camera(context);

    if (render_scene)
        render_scene(context);
}
