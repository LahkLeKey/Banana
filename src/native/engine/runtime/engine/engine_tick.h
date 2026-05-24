#ifndef BANANA_ENGINE_RUNTIME_ENGINE_TICK_H
#define BANANA_ENGINE_RUNTIME_ENGINE_TICK_H

#include "../../ai/controller.h"
#include "../../physics/world.h"
#include "../../render/renderer.h"
#include "../../render/window.h"
#include "../../world/world.h"
#include "../tick/tick_phases.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*RuntimeTickUpdateFn)(void *context, float dt);
    typedef void (*RuntimeTickVoidFn)(void *context);
    typedef void (*RuntimeTickGameplayFn)(void *context);
    typedef void (*RuntimeTickClickInputFn)(void *context, float normalized_x, float normalized_y);

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
                                    RuntimeTickVoidFn render_scene);

#ifdef __cplusplus
}
#endif

#endif
