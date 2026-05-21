#include "engine_tick.h"

#include "controller_sync.h"

int runtime_engine_tick_execute(Window *window,
                                Renderer *renderer,
                                PhysicsWorld *physics_world,
                                World *world,
                                int *viewport_width,
                                int *viewport_height,
                                RuntimeTerrainRebuildFn terrain_rebuild,
                                ControllerInstance **controllers,
                                int controller_count,
                                float dt,
                                RuntimeTickUpdateFn update_player_motion,
                                RuntimeTickVoidFn follow_player_camera,
                                RuntimeTickGameplayFn run_gameplay,
                                RuntimeTickVoidFn render_scene)
{
    if (!window)
        return -1;

    window_poll_events(window);
    physics_world_step(physics_world, dt);
    world_tick(world, dt);

    if (update_player_motion)
        update_player_motion(dt);
    if (follow_player_camera)
        follow_player_camera();

    runtime_phase_viewport_resize(window, renderer, viewport_width, viewport_height);
    if (runtime_phase_terrain_budget(terrain_rebuild, 2) != 0)
        return -1;

    if (run_gameplay)
        run_gameplay();

    runtime_sync_controller_positions(world, controllers, controller_count, dt);

    if (render_scene)
        render_scene();

    return 0;
}
