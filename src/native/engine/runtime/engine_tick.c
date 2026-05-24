#include "engine_tick.h"

#include "orchestration/runtime_tick_orchestration.h"

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
    return runtime_tick_orchestration_execute(window,
                                              renderer,
                                              physics_world,
                                              world,
                                              viewport_width,
                                              viewport_height,
                                              terrain_rebuild,
                                              controllers,
                                              controller_count,
                                              dt,
                                              context,
                                              update_player_motion,
                                              follow_player_camera,
                                              apply_click_input,
                                              run_gameplay,
                                              render_scene);
}
