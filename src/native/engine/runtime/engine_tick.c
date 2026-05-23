#include "engine_tick.h"

#include "controller_sync.h"
#include "input_contract.h"

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
                                RuntimeTickClickInputFn apply_click_input,
                                RuntimeTickGameplayFn run_gameplay,
                                RuntimeTickVoidFn render_scene)
{
    if (!window)
        return -1;

    window_poll_events(window);
    {
        float click_x = 0.f;
        float click_y = 0.f;
        if (window_take_right_click(window, &click_x, &click_y))
        {
            int input_width = 0;
            int input_height = 0;
            float normalized_x = 0.f;
            float normalized_y = 0.f;

            runtime_input_contract_handle_right_click(click_x, click_y);

            window_get_input_size(window, &input_width, &input_height);
            if (input_width > 0 && input_height > 0)
            {
                normalized_x = (click_x / (float)input_width) * 2.f - 1.f;
                normalized_y = 1.f - (click_y / (float)input_height) * 2.f;
                runtime_input_contract_handle_right_click_normalized(normalized_x, normalized_y);
                if (apply_click_input)
                    apply_click_input(normalized_x, normalized_y);
            }
        }
    }
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
