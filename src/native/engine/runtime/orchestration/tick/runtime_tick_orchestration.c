#include "runtime_tick_orchestration.h"

#include "../../tick/tick_budget_policy.h"
#include "../../tick/tick_input_phase.h"
#include "../../tick/tick_post_phase.h"
#include "../../tick/tick_phases.h"

int runtime_tick_orchestration_execute(Window *window,
                                       Renderer *renderer,
                                       PhysicsWorld *physics_world,
                                       World *world,
                                       int *viewport_width,
                                       int *viewport_height,
                                       RuntimeTerrainRebuildFn terrain_rebuild,
                                       ControllerInstance **controllers,
                                       int controller_count,
                                       float dt,
                                       void *context,
                                       RuntimeTickUpdateFn update_player_motion,
                                       RuntimeTickVoidFn follow_player_camera,
                                       RuntimeTickClickInputFn apply_click_input,
                                       RuntimeTickGameplayFn run_gameplay,
                                       RuntimeTickVoidFn render_scene)
{
    if (!window)
        return -1;

    window_poll_events(window);
    if (!window_is_open(window))
        return 1;

    runtime_tick_input_phase_process(window, context, apply_click_input);
    physics_world_step(physics_world, dt);
    world_tick(world, dt);

    if (update_player_motion)
        update_player_motion(context, dt);

    runtime_phase_viewport_resize(window, renderer, viewport_width, viewport_height);
    if (runtime_phase_terrain_budget(context,
                                     terrain_rebuild,
                                     runtime_tick_budget_policy_terrain_chunks_per_tick()) != 0)
        return -1;

    if (run_gameplay)
        run_gameplay(context);

    runtime_tick_post_phase_execute(world,
                                    controllers,
                                    controller_count,
                                    dt,
                                    context,
                                    follow_player_camera,
                                    render_scene);

    return 0;
}
