#include "engine_tick.h"

#include "controller_sync.h"
#include "input_click_policy.h"
#include "input_contract.h"
#include "tick_budget_policy.h"

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
            if (runtime_input_click_policy_normalize(click_x,
                                                     click_y,
                                                     input_width,
                                                     input_height,
                                                     &normalized_x,
                                                     &normalized_y))
            {
                runtime_input_contract_handle_right_click_normalized(normalized_x, normalized_y);
                if (apply_click_input)
                    apply_click_input(context, normalized_x, normalized_y);
            }
        }
    }
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

    runtime_sync_controller_positions(world, controllers, controller_count, dt);

    if (follow_player_camera)
        follow_player_camera(context);

    if (render_scene)
        render_scene(context);

    return 0;
}
